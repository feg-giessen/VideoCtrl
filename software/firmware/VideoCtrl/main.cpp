#include <string.h>
#include <stdio.h>
#include "ch.h"
#include "ch.hpp"
#include "hal.h"
#include "halconf.h"
#include "chconf.h"
#include "board_hw.h"
#include "lwip/init.h"
#include "lwipthread.h"
#include "lwip/ip_addr.h"
#include "net/web/WebServer.h"
#include "lib/MatrixSwitch.h"
#include "lib/ATEM.h"
#include "lib/hw/PCA9685.h"
#include "lib/SkaarhojBI8.h"
#include "lib/Display.h"
#include "app/MessageWriter.h"
#include "app/ReaderThread.h"
#include "app/HwModules.h"
#include "app/Memory.h"
#include "app/Videoswitcher.h"
#include "app/VideoMatrix.h"
#include "app/ButtonMapper.h"
#include "app/OutputDisplays.h"
#include "app/ScalerAndSwitchModule.h"
#include "app/PtzCamera.h"
#include "lib/AdcChannel.h"

// ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

static bool blink_enable;

static Videoswitcher* s_atem;
static OutputDisplays* s_displays;
static PtzCamera* s_camera;

static WebServer webServerThread;
static ReaderThread readerThread;
static MessageWriter messager;
static HwModules hwModules;
static Memory memory;
static adcsample_t adc_buffer[ADC3_CH_NUM * ADC3_SMP_DEPTH];

Videoswitcher atem;
ButtonMapper mapper;
VideoMatrix matrix;
PtzCamera camera;
AdcChannel channelX;
AdcChannel channelY;
AdcChannel channelZ;
OutputDisplays displays;
ScalerAndSwitchModule scalerAndSwitch;

// netif options
struct lwipthread_opts net_opts;

// ----- ----- ----- ----- ----- ----- ----- ----- ----- -----
// LED blinker thread, times are in milliseconds.

static WORKING_AREA(waBlinkThread, 256);
static msg_t BlinkThread(void *arg) {
    (void)arg;
    chRegSetThreadName("blinker");
    while (TRUE) {
        palClearPad(GPIOC, GPIOC_LED);
        if (s_atem != NULL && blink_enable)
            s_atem->doBlink();
        if (s_displays != NULL && blink_enable)
            s_displays->doBlink();

        chThdSleepMilliseconds(300);

        palSetPad(GPIOC, GPIOC_LED);
        if (s_atem != NULL && blink_enable)
            s_atem->doBlink();
        if (s_displays != NULL && blink_enable)
            s_displays->doBlink();

        chThdSleepMilliseconds(300);
    }
    return 0;
}

static WORKING_AREA(waTimeCriticalThread, 1024);
static msg_t TimeCriticalThread(void *arg) {
    (void)arg;
    chRegSetThreadName("atem");
    while (TRUE) {

        if (s_atem != NULL) {
            s_atem->run();
        }
        if (s_camera != NULL) {
            s_camera->run();
        }

        chThdSleepMilliseconds(5);
    }
    return 0;
}

// ----- ----- ----- ----- ----- ----- ----- ----- ----- -----
// Application entry point.

int main(void) {

    //
    // Static pointers for blink thread

    blink_enable = false;

    s_atem = &atem;
    s_displays = &displays;
    s_camera = &camera;

    /*
    * System initializations.
    * - HAL initialization, this also initializes the configured device drivers
    *   and performs the board-specific initializations.
    * - Kernel initialization, the main() function becomes a thread and the
    *   RTOS is active.
    */
    halInit();
    chibios_rt::System::init();
    init_board_hal();

    // create alive blinker
    chThdCreateStatic(waBlinkThread, sizeof(waBlinkThread), NORMALPRIO, BlinkThread, NULL);

    chThdSleepMilliseconds(500);

    hwModules.init();

    messager.begin(hwModules.getDisplay());
    messager.write((char*)"HwModules initialized");

    // initialize memory (includes data migration)
    memory.init(hwModules.getEeprom());

    if (memory.hasMigrated()) {
        char migMsg[22];
        sprintf(migMsg, "Mem. migrated to v%d", memory.getDataVersion());
        messager.write(migMsg);
    }

    messager.write((char*)"Memory initialized");

    enable_adc(adc_buffer);
    messager.write((char*)"ADC enabled");

    hwModules.setScheduler(&readerThread);
    messager.write((char*)"ReaderThread configured.");
    readerThread.start(NORMALPRIO);
    messager.write((char*)"ReaderThread started.");

    messager.reset();

    uint8_t mac[] = {
        LWIP_ETHADDR_0,
        LWIP_ETHADDR_1,
        LWIP_ETHADDR_2,
        LWIP_ETHADDR_3,
        LWIP_ETHADDR_4,
        LWIP_ETHADDR_5
    };

    net_opts.macaddress = &mac[0];
    net_opts.address = memory.getIpAddress();
    net_opts.gateway = (net_opts.address & 0x00FFFFFF) | (0x01 << 24); // xxx.xxx.xxx.1
    net_opts.netmask = 0x00FFFFFF; // 255.255.255.0

    char ipmsg[20];
    sprintf(
        ipmsg,
        "IP: %d.%d.%d.%d",
        (uint8_t)(net_opts.address & 0xff),
        (uint8_t)((net_opts.address >> 8)  & 0xFF),
        (uint8_t)((net_opts.address >> 16) & 0xFF),
        (uint8_t)((net_opts.address >> 24) & 0xFF)
    );
    messager.write(ipmsg);

    // Creates the LWIP threads (it changes priority internally)
    chThdCreateStatic(wa_lwip_thread, LWIP_THREAD_STACK_SIZE, NORMALPRIO + 1,lwip_thread, &net_opts);

    // Creates the HTTP thread (it changes priority internally).
    webServerThread.start(NORMALPRIO);

    messager.write((char*)"Network initialized.");
    chThdSleepMilliseconds(500);
    messager.reset();

    // ---------------------------------------------------------------------------

    channelX.begin(adc_buffer, 1, ADC3_CH_NUM, ADC3_SMP_DEPTH, false);
    channelY.begin(adc_buffer, 2, ADC3_CH_NUM, ADC3_SMP_DEPTH, false);
    channelZ.begin(adc_buffer, 3, ADC3_CH_NUM, ADC3_SMP_DEPTH, false);

    camera.begin(&SD2, &channelX, &channelY, &channelZ);

    camera.setButton(PTZ_None, hwModules.getBi8(3), 7);
    camera.setLed(PTZ_None, hwModules.getBi8(3), 7);
    camera.setButton(PTZ_MEM_1, hwModules.getBi8(3), 5);
    camera.setLed(PTZ_MEM_1, hwModules.getBi8(3), 5);
    camera.setButton(PTZ_MEM_2, hwModules.getBi8(3), 6);
    camera.setLed(PTZ_MEM_2, hwModules.getBi8(3), 6);
    camera.setButton(PTZ_MEM_3, hwModules.getBi8(3), 3);
    camera.setLed(PTZ_MEM_3, hwModules.getBi8(3), 3);
    camera.setButton(PTZ_MEM_4, hwModules.getBi8(3), 8);
    camera.setLed(PTZ_MEM_4, hwModules.getBi8(3), 8);
    camera.setButton(PTZ_MEM_Store, hwModules.getBi8(3), 4);
    camera.setLed(PTZ_MEM_Store, hwModules.getBi8(3), 4);

    messager.write((char*)"Camera configured");

    // ---------------------------------------------------------------------------

    ip_addr_t addr_proj_li;
    ip_addr_t addr_proj_re;
    ip_addr_t addr_klSaal_li;
    ip_addr_t addr_klSaal_re;
    ip_addr_t addr_stage;
    uint16_t port_proj = 5000;
    IP4_ADDR(&addr_proj_li, 192, 168, 40, 33);
    IP4_ADDR(& addr_proj_re, 192, 168, 40, 32);
    IP4_ADDR(&addr_klSaal_li, 192, 168, 40, 34);
    IP4_ADDR(&addr_klSaal_re, 192, 168, 40, 35);
    IP4_ADDR(&addr_stage, 192, 168, 40, 36);

//*
    displays.begin(
            addr_proj_li, port_proj,
            addr_proj_re, port_proj,
            addr_klSaal_li, port_proj,
            addr_klSaal_re, port_proj,
            addr_stage, port_proj,
            hwModules.getBi8(0));
//*/

    // ---------------------------------------------------------------------------

    ip_addr_t addr_hdmi_switch;
    ip_addr_t addr_scaler;
    IP4_ADDR(&addr_hdmi_switch, 192, 168, 40, 31);
    IP4_ADDR(&addr_scaler, 192, 168, 40, 31);

//*
    scalerAndSwitch.begin(
            addr_hdmi_switch, 101,
            addr_scaler, 102,
            hwModules.getBi8(4));
//*/

    // ---------------------------------------------------------------------------

    ip_addr_t matrix_ip_addr;
    IP4_ADDR(&matrix_ip_addr, 192, 168, 40, 31);

    matrix.begin(matrix_ip_addr, 100);

    SkaarhojBI8* current_bi8;

    current_bi8 = hwModules.getBi8(1);
    matrix.setButton(1, 1, current_bi8, 4);
    matrix.setButton(2, 1, current_bi8, 3);
    matrix.setButton(3, 1, current_bi8, 2);
    matrix.setButton(4, 1, current_bi8, 1);
    matrix.setButton(1, 2, current_bi8, 8);
    matrix.setButton(2, 2, current_bi8, 7);
    matrix.setButton(3, 2, current_bi8, 6);
    matrix.setButton(4, 2, current_bi8, 5);
    current_bi8 = hwModules.getBi8(2);
    matrix.setButton(1, 3, current_bi8, 4);
    matrix.setButton(2, 3, current_bi8, 3);
    matrix.setButton(3, 3, current_bi8, 2);
    matrix.setButton(4, 3, current_bi8, 1);
    matrix.setButton(1, 4, current_bi8, 8);
    matrix.setButton(2, 4, current_bi8, 7);
    matrix.setButton(3, 4, current_bi8, 6);
    matrix.setButton(4, 4, current_bi8, 5);

    messager.write((char*)"Matrix configured");

    // ---------------------------------------------------------------------------

    ip_addr_t atem_ip_addr;
    IP4_ADDR(&atem_ip_addr, 192, 168, 40, 21);

    messager.write((char*)"ATEM init...");
    atem.begin(atem_ip_addr);

    // init button mapper
    mapper.begin(
        &memory,
        hwModules.getBi8(5),
        hwModules.getBi8(6)
    );

    // load mapping 0 and apply to video switcher
    mapper.load(0);
    mapper.apply(&atem);
    atem.setButton(ATEM_Cut, hwModules.getBi8(3), 1);
    atem.setLed(ATEM_Cut, hwModules.getBi8(3), 1);
    atem.setButton(ATEM_Auto, hwModules.getBi8(3), 2);
    atem.setLed(ATEM_Auto, hwModules.getBi8(3), 2);
    messager.write((char*)"ATEM Buttons mapped.");

    atem.deactivate();

    Display* display = hwModules.getDisplay();
    uint8_t blink_count = 0;
    char button_msg[12];
    char xmes[30];

    // ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

    messager.write((char*)"Performing tests...");

    char buttonColor[6] = {BI8_COLOR_OFF,BI8_COLOR_ON,BI8_COLOR_RED,BI8_COLOR_GREEN,BI8_COLOR_YELLOW,BI8_COLOR_BACKLIGHT};

    for (int i = 0; i < 6; i++) {
        for (int j = 0; j < 7; j++) {
            for (int k = 0; k <= NUMBER_BI8; k++) {
                hwModules.getBi8(j)->setButtonColor(k + 1, buttonColor[i]);
            }
        }
        chThdSleepMilliseconds(200);
    }

    messager.write((char*)"Tests performed");

    //
    // Everything is initialized -> start blink handling
    blink_enable = true;

    messager.reset();

    messager.write((char*)"Starting run loop...");
    messager.reset();

    // ----- ----- ----- ----- ----- ----- ----- ----- ----- -----
    // Main Loop.

    bool atem_online = false;
    messager.write("ATEM offline.");

    atem.connect();

    // create atem/cammera run thread
    chThdCreateStatic(waTimeCriticalThread, sizeof(waTimeCriticalThread), NORMALPRIO, TimeCriticalThread, NULL);

    scalerAndSwitch.update(); // read initial values from devices

    while (TRUE) {

        matrix.run();
        scalerAndSwitch.run();
        displays.run();

        bool online_temp = atem.online();
        if (atem_online != online_temp) {
            if (atem_online) {
                messager.write("ATEM offline.");
            } else {
                messager.write("ATEM online.");
            }
            atem_online = online_temp;
        }

        /*

        if (!palReadPad(GPIOD, GPIOD_PIN14))
            messager.write("IT WORKS!");


        int i;
        for (i = 1; i <= 4; i++)
        if (display.buttonDown(i)) display.setButtonLed(i, !display.getButtonLed(i));
        bi8.setButtonColor(enc1val + 1, BI8_COLOR_BACKLIGHT);
        bi8.setButtonColor(enc2val + 1, BI8_COLOR_BACKLIGHT);
        enc1val = (enc1val + display.getEncoder1(true)) % 8;
        enc2val = (enc2val + display.getEncoder2(true)) % 8;
        bi8.setButtonColor(enc1val + 1, BI8_COLOR_GREEN);
        bi8.setButtonColor(enc2val + 1, BI8_COLOR_RED);
        */

        //messager.write("RUN");
        /*
        for (int i = 0; i < NUMBER_BI8; i++) {
            SkaarhojBI8* bi8 = hwModules.getBi8(i);
            for (uint8_t j = 1; j <= 8; j++) {
                if (bi8->buttonDown(j)) {
                    sprintf(button_msg, "Button %d-%d", i+1, j);
                    messager.write(button_msg);
                }
            }
        }
        //*/

        if (display->buttonDown(1)) {
            messager.reset();
            display->clear();
        }

        if (display->buttonDown(2)) {
            for (int i = 0; i < NUMBER_BI8; i++) {
                hwModules.getBi8(i)->setButtonColor(i + 1, BI8_COLOR_RED);
            }
        } else if (display->buttonDown(3)) {
            camera.power(true);
        } else if (display->buttonDown(4)) {
            camera.power(false);
        }

        blink_count++;

        if (blink_count == 200) {

            scalerAndSwitch.update();   // read status from devices

            /*if (atem_online) {
                // adjust TvOne Scaler video format to ATEM video format
                scalerAndSwitch.setFormatFromAtem(atem.getVideoFormat());
            }*/

            sprintf(xmes, "X:%d Y:%d Z:%d", channelX.getValue(), channelY.getValue(), channelZ.getValue());
            messager.write(xmes);

            blink_count = 0;
        }

        chThdSleepMilliseconds(5);
    }
}
