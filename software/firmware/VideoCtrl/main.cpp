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
#include "app/ButtonMapper.h"

/*
* Green LED blinker thread, times are in milliseconds.
*/
static WORKING_AREA(waBlinkThread, 128);
static msg_t BlinkThread(void *arg) {
  (void)arg;
  chRegSetThreadName("blinker");
  while (TRUE) {
    palClearPad(GPIOC, GPIOC_LED);
    chThdSleepMilliseconds(500);
    palSetPad(GPIOC, GPIOC_LED);
    chThdSleepMilliseconds(500);
  }

  return 0;
}

static WebServer webServerThread;
static ReaderThread readerThread;

static MessageWriter messager;

static HwModules hwModules;
static Memory memory;

static adcsample_t adc_buffer[ADC3_CH_NUM * ADC3_SMP_DEPTH];

Videoswitcher atem;
ButtonMapper mapper;

// netif options
struct lwipthread_opts net_opts;

/*
 * Application entry point.
 */
int main(void) {

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

  hwModules.init();

  messager.begin(hwModules.getDisplay());
  messager.write((char*)"Hello :-)");
  messager.write((char*)"HwModules initialized");

  messager.write((char*)"Memory initialization");
  //*
  // initialize memory (includes data migration)
  memory.init(hwModules.getEeprom());

  if (memory.hasMigrated()) {
	  char migMsg[22];
	  sprintf(migMsg, "Mem. migrated to v%d", memory.getDataVersion());
	  messager.write(migMsg);
  }

  //*/
  messager.write((char*)"Memory initialized");

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
  net_opts.gateway = (net_opts.address & 0xFFFFFF00) | 0x01;
  net_opts.netmask = 0xFFFFFF00; // 255.255.255.0

  char ipmsg[20];
  sprintf(
	  ipmsg,
	  "IP: %d.%d.%d.%d",
	  (uint8_t)(net_opts.address & 0xff),
	  (uint8_t)((net_opts.address >> 8)  & 0xFF),
	  (uint8_t)((net_opts.address >> 16) & 0xFF),
	  (uint8_t)((net_opts.address >> 24) & 0xFF));
  messager.write(ipmsg);


  /*
   * Creates the LWIP threads (it changes priority internally).
   */
  chThdCreateStatic(wa_lwip_thread, LWIP_THREAD_STACK_SIZE, NORMALPRIO + 1,
                    lwip_thread, &net_opts);

  // Creates the HTTP thread (it changes priority internally).
  webServerThread.start(NORMALPRIO);
  messager.write((char*)"Webserver started.");

  hwModules.setScheduler(&readerThread);
  messager.write((char*)"ReaderThread configured.");
  readerThread.start(NORMALPRIO);
  messager.write((char*)"ReaderThread started.");


  messager.write((char*)"Performing tests...");
  uint8_t i;
  for (i = 0; i < NUMBER_BI8; i++) {
	  hwModules.getBi8(i)->testSequence();
	  hwModules.getBi8(i)->setButtonColor(i + 1, BI8_COLOR_RED);
  }

  messager.write((char*)"Tests performed");

  enable_adc(adc_buffer);
  messager.write((char*)"ADC enabled");

  /*
  ip_addr_t* ip_addr = NULL;
  IP4_ADDR(ip_addr, 192, 168, 0, 239);
  MatrixSwitch* t = new MatrixSwitch(*ip_addr, 101);
  t->setOutput(1, 4);
  //*/

  //*
  ip_addr_t atem_ip_addr;
  IP4_ADDR(&atem_ip_addr, 192, 168, 40, 21);

  messager.write((char*)"ATEM init...");
  atem.begin(atem_ip_addr);

  // init button mapper
  mapper.begin(
          &memory,
          hwModules.getBi8(5),
          hwModules.getBi8(6));

  // load mapping 0 and apply to video switcher
  mapper.load(0);
  mapper.apply(&atem);
  messager.write((char*)"ATEM Buttons mapped.");
//*/
  atem.deactivate();

  bool atem_online = false;
  messager.write("ATEM offline.");

  messager.write((char*)"Starting run loop...");

  Display* display = hwModules.getDisplay();
  uint8_t blink_count = 0;
  char button_msg[12];
  char xmes[30];

  while (TRUE) {

    atem.run();

    if (atem_online != atem.online()) {
    	if (atem_online) {
    		messager.write("ATEM offline.");
    	} else {
    		messager.write("ATEM online.");
    	}

        atem_online = atem.online();
    }
/*
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

    for (i = 0; i < NUMBER_BI8; i++) {
      SkaarhojBI8* bi8 = hwModules.getBi8(i);
  	  for (uint8_t j = 1; j <= 8; j++) {
  		  if (bi8->buttonDown(j)) {
  			  sprintf(button_msg, "Button %d-%d", i+1, j);
  			  messager.write(button_msg);
  		  }
  	  }
    }

    if (display->buttonDown(1)) {
    	messager.reset();
    	display->clear();
    }

    if (display->buttonDown(2)) {
        for (i = 0; i < NUMBER_BI8; i++) {
        	hwModules.getBi8(i)->setButtonColor(i + 1, BI8_COLOR_RED);
        }
    } else if (display->buttonDown(3)) {
        for (i = 0; i < NUMBER_BI8; i++) {
        	hwModules.getBi8(i)->setButtonColor(i + 1, BI8_COLOR_GREEN);
        }
    } else if (display->buttonDown(4)) {
        for (i = 0; i < NUMBER_BI8; i++) {
        	hwModules.getBi8(i)->setButtonColor(i + 1, BI8_COLOR_YELLOW);
        }
    }

    blink_count++;

    if (blink_count % 10 == 0) {
        atem.doBlink();
    }

    if (blink_count % 40 == 0) {
        sprintf(xmes, "Slider %d", adc_buffer[0]);
        messager.write(xmes);
        sprintf(xmes, "Joystick %d,%d,%d", adc_buffer[1], adc_buffer[2], adc_buffer[3]);
        messager.write(xmes);
    }

    chThdSleepMilliseconds(50);
  }
}
