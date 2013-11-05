/*
 * board_hw.c
 *
 *  Created on: 28.10.2013
 *      Author: Peter Schuster
 */

#include "hal.h"
#include "halconf.h"

#ifdef __cplusplus
extern "C" {
#endif

I2CConfig i2c_1_conf;
I2CConfig i2c_2_conf;

SPIConfig spi_1_conf;

SerialConfig sd_2_conf;

void init_board_hal(void) {

	//
	// I2C #1 - BI8 boards

	i2c_1_conf.op_mode = OPMODE_I2C;
	i2c_1_conf.duty_cycle = STD_DUTY_CYCLE;
	i2c_1_conf.clock_speed = 100000;

	i2cStart(&I2CD1, &i2c_1_conf);

	palSetPadMode(GPIOB, GPIOB_I2C1_SCL, PAL_MODE_ALTERNATE(4) | PAL_STM32_OTYPE_OPENDRAIN);   /* SCL */
	palSetPadMode(GPIOB, GPIOB_I2C1_SDA, PAL_MODE_ALTERNATE(4) | PAL_STM32_OTYPE_OPENDRAIN);   /* SDA */

	//
	// I2C #2 - Display-Buttons, EEPROM

	i2c_2_conf.op_mode = OPMODE_I2C;
	i2c_2_conf.duty_cycle = STD_DUTY_CYCLE;
	i2c_2_conf.clock_speed = 100000;

	i2cStart(&I2CD2, &i2c_2_conf);

	palSetPadMode(GPIOF, GPIOF_PIN1, PAL_MODE_ALTERNATE(4) | PAL_STM32_OTYPE_OPENDRAIN);   /* SCL */
	palSetPadMode(GPIOF, GPIOF_PIN0, PAL_MODE_ALTERNATE(4) | PAL_STM32_OTYPE_OPENDRAIN);   /* SDA */

	//
	// SPI #1 - Display
	spi_1_conf.end_cb = NULL; 		// Operation complete callback
	spi_1_conf.ssport = GPIOA;		// Chip select line port
	spi_1_conf.sspad = GPIOA_PIN4;	// Chip select line pad number

	/*
	 * SPI1 is on APB2 --> PCLK2 (= 59 MHz)
	 *   ==> clk = PCLK2 / (2 << BR[2:0]
	 *
	 * BR2 -> 001 = 1 --> 59 / (2 << 1) == 59 / 4 -> ~14.75 MHz
	 */
	spi_1_conf.cr1 = SPI_CR1_BR_0;

	palSetPadMode(GPIOA, GPIOA_PIN5, PAL_MODE_ALTERNATE(5) | PAL_STM32_OSPEED_HIGHEST);       /* SCK  		*/
	palSetPadMode(GPIOB, GPIOB_PIN5, PAL_MODE_ALTERNATE(5) | PAL_STM32_OSPEED_HIGHEST);       /* MOSI 		*/
	palSetPadMode(GPIOA, GPIOA_PIN4, PAL_MODE_OUTPUT_PUSHPULL | PAL_STM32_OSPEED_HIGHEST);    /* NSS  		*/
	palSetPadMode(GPIOD, GPIOD_PIN0, PAL_MODE_OUTPUT_PUSHPULL | PAL_STM32_OSPEED_HIGHEST);    /* DISP_MODE  */

	palSetPad(GPIOA, GPIOA_PIN4);	// set NSS 		 <= 1
	palClearPad(GPIOD, GPIOD_PIN0);	// set DISP_MODE <= 0

	spiStart(&SPID1, &spi_1_conf);

	//
	// USART2 - VISCA

	palSetPadMode(GPIOD, GPIOD_PIN5, PAL_MODE_ALTERNATE(7));   /* USART2_TX */
	palSetPadMode(GPIOD, GPIOD_PIN6, PAL_MODE_ALTERNATE(7));   /* USART2_RX */

	sd_2_conf.speed = 9600;
	sd_2_conf.cr1   = 0;
	sd_2_conf.cr2   = USART_CR2_STOP1_BITS | USART_CR2_LINEN;
	sd_2_conf.cr3   = 0;

	sdStart(&SD2, &sd_2_conf);
}

#ifdef __cplusplus
}
#endif
