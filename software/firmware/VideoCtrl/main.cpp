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

  I2cBus i2cBus1(&I2CD1);
  I2cBus i2cBus2(&I2CD2);

  Display display(&SPID1, &i2cBus2);

  SkaarhojBI8 bi8;
  bi8.begin(&i2cBus1, 7);
  bi8.setButtonType(1);

  readerThread.add(&bi8, 3);
  readerThread.add(&display, 5);

  display.init();
  bi8.testSequence(50);

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

  /*
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

	atem.runLoop();

	if (atem.isConnectionTimedOut()) {
		atem.connect();
	}
	else if (atem.hasInitialized()) {
		uint8_t preview = atem.getPreviewInput();
		uint8_t program = atem.getProgramInput();

		bi8.setButtonColor(3, program == 1 ? 2 : (preview == 1 ? 3 : 5));
		bi8.setButtonColor(4, program == 5 ? 2 : (preview == 5 ? 3 : 5));

		uint16_t buttonDown = bi8.buttonDownAll();

		if (buttonDown & 0b1) {
			atem.doCut();
		} else if ((buttonDown >> 1) & 0b1) {
			atem.doAuto();
		}

		if ((buttonDown >> 2) & 0b1) {
			atem.changePreviewInput(1);
		} else if ((buttonDown >> 3) & 0b1) {
			atem.changePreviewInput(5);
		}

		if ((buttonDown >> 4) & 0b1) {
			uint8_t visca[] = { 0x81, 0x01, 0x04, 0x3F, 0x02, 0x03, 0xFF };
			sdWrite(&SD2, visca, sizeof(visca));
		}
	}

    int i;
    for (i = 1; i <= 4; i++)
        if (display.buttonDown(i)) display.setButtonLed(i, !display.getButtonLed(i));

    chThdSleepMilliseconds(50);
  }
}
