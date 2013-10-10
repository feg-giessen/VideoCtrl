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

using namespace chibios_rt;

static uint8_t c_reset[5];

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

  I2CConfig i2c1conf;
  i2c1conf.op_mode = OPMODE_I2C;
  i2c1conf.duty_cycle = STD_DUTY_CYCLE;
  i2c1conf.clock_speed = 100000;

  I2CD1.addr = 0b1111000;

  uint8_t recv_b[4];

  chThdSleepMilliseconds(10);
  systime_t tmo = MS2ST(20);

  i2cStart(&I2CD1, &i2c1conf);

  palSetPadMode(GPIOB, GPIOB_I2C1_SCL, PAL_MODE_ALTERNATE(4) | PAL_STM32_OTYPE_OPENDRAIN);   /* SCL */
  palSetPadMode(GPIOB, GPIOB_I2C1_SDA, PAL_MODE_ALTERNATE(4) | PAL_STM32_OTYPE_OPENDRAIN);   /* SDA */

  c_reset[0] = 0;
  c_reset[1] = 1;
  // reset

  chThdSleepMilliseconds(10);

  i2cAcquireBus(&I2CD1);
  i2cMasterTransmitTimeout(&I2CD1, 0b1111000, c_reset, 2, recv_b, 0, -1);
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

  /*
   * Normal main() thread activity, in this demo it does nothing except
   * sleeping in a loop and check the button state.
   */
  while (TRUE) {
    if (palReadPad(GPIOA, GPIOA_BUTTON_WKUP) == 0) {
    }

    chThdSleepMilliseconds(50);
  }
}
