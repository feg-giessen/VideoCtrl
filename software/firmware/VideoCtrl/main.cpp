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
#include "lwip/init.h"

#include "lwipthread.h"
#include "lwip/ip_addr.h"

#include "net/web/WebServer.h"
#include "lib/MatrixSwitch.h"
#include "lib/ATEM.h"

using namespace chibios_rt;

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
  System::init();


  /*
   * Activates the serial driver 6 using the driver default configuration.
   */
  sdStart(&SD6, NULL);

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

  //*
  ip_addr_t* ip_addr = NULL;
  IP4_ADDR(ip_addr, 192, 168, 0, 239);
  MatrixSwitch* t = new MatrixSwitch(*ip_addr, 101);
  t->setOutput(1, 4);
//*/

  ip_addr_t atem_ip_addr;
  IP4_ADDR(&atem_ip_addr, 192, 168, 40, 21);
  ATEM atem;
  atem.begin(atem_ip_addr);
  atem.connect();

  /*
   * Normal main() thread activity, in this demo it does nothing except
   * sleeping in a loop and check the button state.
   */
  while (TRUE) {
    if (palReadPad(GPIOA, GPIOA_BUTTON_WKUP) == 0) {

    }

    atem.runLoop();

    if (atem.isConnectionTimedOut()) {
    	atem.connect();
    }
    else if (atem.hasInitialized()) {
    	if (atem.getPreviewInput() != 1) {
    		atem.changePreviewInput(1);
    	}
    }

    chThdSleepMilliseconds(50);
  }
}
