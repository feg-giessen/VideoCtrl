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
#include "lib/SkaarhojBI8.h"
#include "lib/PCA9685.h"

using namespace chibios_rt;

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

  uint16_t pca_addr = 0b1111000;

  I2CD1.addr = pca_addr;

  i2cStart(&I2CD1, &i2c1conf);

  palSetPadMode(GPIOB, GPIOB_I2C1_SCL, PAL_MODE_ALTERNATE(4) | PAL_STM32_OTYPE_OPENDRAIN);   /* SCL */
  palSetPadMode(GPIOB, GPIOB_I2C1_SDA, PAL_MODE_ALTERNATE(4) | PAL_STM32_OTYPE_OPENDRAIN);   /* SDA */

  chThdSleepMilliseconds(10);

  i2cAcquireBus(&I2CD1);

  I2cBus i2cBus1(&I2CD1);

  SkaarhojBI8 bi8;
  bi8.begin(&i2cBus1, 7);
  bi8.usingB1alt();
  bi8.setButtonType(1);
  bi8.testSequence(100);

  /*
  ip_addr_t* ip_addr = NULL;
  IP4_ADDR(ip_addr, 192, 168, 0, 239);
  MatrixSwitch* t = new MatrixSwitch(*ip_addr, 101);
  t->setOutput(1, 4);
//*/


  /*
   * Normal main() thread activity, in this demo it does nothing except
   * sleeping in a loop and check the button state.
   */
  while (TRUE) {
    if (bi8.buttonDown(3) == 1) {
      bi8.setButtonColor(5, 5);
    } else if (bi8.buttonDown(4) == 1) {
      bi8.setButtonColor(5, 3);
    }

    chThdSleepMilliseconds(50);
  }
}
