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
static uint8_t c_reset[5];

/*
 * Application entry point.
 */
int main(void) {

	  palSetPadMode(GPIOB, GPIOB_I2C1_SCL, PAL_MODE_ALTERNATE(4) | PAL_STM32_OTYPE_OPENDRAIN);   /* SCL */
	  palSetPadMode(GPIOB, GPIOB_I2C1_SDA, PAL_MODE_ALTERNATE(4) | PAL_STM32_OTYPE_OPENDRAIN);   /* SDA */

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

  /*
  ip_addr_t* ip_addr = NULL;
  IP4_ADDR(ip_addr, 192, 168, 0, 124);
  MatrixSwitch* t = new MatrixSwitch(*ip_addr, 101);
  t->setOutput(1, 4);
*/

  I2CConfig i2c1conf;
  i2c1conf.op_mode = OPMODE_I2C;
  i2c1conf.duty_cycle = STD_DUTY_CYCLE;
  i2c1conf.clock_speed = 100000;

  I2CD1.addr = 0b1111000;

  uint8_t recv_b[4];

  chThdSleepMilliseconds(10);
  systime_t tmo = MS2ST(4);

  i2cStart(&I2CD1, &i2c1conf);

  c_reset[0] = 0;
  c_reset[1] = 1;
  // reset

  chThdSleepMilliseconds(10);

  i2cAcquireBus(&I2CD1);
  i2cMasterTransmitTimeout(&I2CD1, 0b1111000, c_reset, 2, recv_b, 0, tmo);
  i2cReleaseBus(&I2CD1);

  c_reset[1] = 0b10100000;
  // auto increment
  chThdSleepMilliseconds(10);
  i2cAcquireBus(&I2CD1);
  i2cMasterTransmitTimeout(&I2CD1, 0b1111000, c_reset, 2, recv_b, 0, tmo);
  i2cReleaseBus(&I2CD1);

  c_reset[0] = 0x01;
  c_reset[1] = 0x10;
  // output
  chThdSleepMilliseconds(10);
  i2cAcquireBus(&I2CD1);
  i2cMasterTransmitTimeout(&I2CD1, 0b1111000, c_reset, 2, recv_b, 0, tmo);
  i2cReleaseBus(&I2CD1);


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
