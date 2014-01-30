/*
    ChibiOS/RT - Copyright (C) 2006-2013 Giovanni Di Sirio

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
*/

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

#include "app/ReaderThread.h"
#include "app/HwModules.h"
#include "app/Memory.h"
#include "app/Videoswitcher.h"
#include "app/ButtonMapper.h"

/*
* Green LED blinker thread, times are in milliseconds.
*/
static WORKING_AREA(waThread1, 128);
static msg_t Thread1(void *arg) {
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

static HwModules hwModules;
static Memory memory;
static Videoswitcher atem;
static ButtonMapper mapper;

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

  chThdSleepMilliseconds(10);

  /*
   * Creates the blinker thread.
   */
  chThdCreateStatic(waThread1, sizeof(waThread1), NORMALPRIO, Thread1, NULL);


  /*
   * Creates the LWIP threads (it changes priority internally).
   */
  chThdCreateStatic(wa_lwip_thread, LWIP_THREAD_STACK_SIZE, NORMALPRIO + 1,
                    lwip_thread, NULL);

  /*
   * Creates the HTTP thread (it changes priority internally).
   */
  webServerThread.start(NORMALPRIO);

  readerThread.start(NORMALPRIO);


  hwModules.init();

  // initialize memory (includes data migration)
  memory.init(hwModules.getEeprom());

  /*
  ip_addr_t* ip_addr = NULL;
  IP4_ADDR(ip_addr, 192, 168, 0, 239);
  MatrixSwitch* t = new MatrixSwitch(*ip_addr, 101);
  t->setOutput(1, 4);
  //*/

  ip_addr_t atem_ip_addr;
  IP4_ADDR(&atem_ip_addr, 192, 168, 40, 21);

  atem.begin(atem_ip_addr);

  // init button mapper
  mapper.begin(
          &memory,
          hwModules.getBi8(5),
          hwModules.getBi8(6));

  // load mapping 0 and appy to video switcher
  mapper.load(0);
  mapper.apply(&atem);

  // uint8_t enc1val, enc2val;
  // enc1val = enc2val = 0;

  /*
   * Normal main() thread activity, in this demo it does nothing except
   * sleeping in a loop and check the button state.
   */
  uint8_t blink_count = 0;
  while (TRUE) {

      atem.run();
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

    blink_count++;

    if (blink_count % 10 == 0) {
        atem.doBlink();
    }

    chThdSleepMilliseconds(50);
  }
}
