/*
 * board_hw.c
 *
 *  Created on: 28.10.2013
 *      Author: Peter Schuster
 */

#include "board_hw.h"

I2CConfig i2c_1_conf;
I2CConfig i2c_2_conf;

SPIConfig spi_1_conf;

#define ADC3_CH_NUM     4
#define ADC3_SMP_DEPTH  4

static adcsample_t adc3_samples[ADC3_CH_NUM * ADC3_SMP_DEPTH];

/*
 * ADC streaming callback.
 */
size_t adc3_samples_count = 0;
static void adc_cb(ADCDriver *adcp, adcsample_t *buffer, size_t n) {

  (void)adcp;
  if (adc3_samples == buffer) {
      adc3_samples_count = n;
  } else {
      adc3_samples_count += n;
  }
}

static void adc_error_cb(ADCDriver *adcp, adcerror_t err) {

  (void)adcp;
  (void)err;
}

/*
 * ADC conversion group.
 * Mode:        Continuous, 16 samples of 8 channels, SW triggered.
 * Channels:    IN11, IN12, IN11, IN12, IN11, IN12, Sensor, VRef.
 */
static const ADCConversionGroup adcgrp_conf3 = {
  TRUE,                         // Enables the circular buffer mode for the group.
  ADC3_CH_NUM,                  // Number of the analog channels belonging to the conversion group.
  adc_cb,                       // Callback function associated to the group or @p NULL.
  adc_error_cb,                 // Error callback or @p NULL.
  0,                            // CR1
  ADC_CR2_SWSTART,              // CR2
  0,                            // SMPR1 - In this field must be specified the sample times for channels 10...18.
  ADC_SMPR2_SMP_AN4(ADC_SAMPLE_56)
   | ADC_SMPR2_SMP_AN5(ADC_SAMPLE_56)
   | ADC_SMPR2_SMP_AN6(ADC_SAMPLE_56)
    | ADC_SMPR2_SMP_AN7(ADC_SAMPLE_56),  // SMPR2 - In this field must be specified the sample times for channels 0...9.
  ADC_SQR1_NUM_CH(ADC3_CH_NUM), // Conversion group sequence 13...16 + sequence length.
  0,                            // Conversion group sequence 7...12.
  ADC_SQR3_SQ1_N(ADC_CHANNEL_IN4)
   | ADC_SQR3_SQ2_N(ADC_CHANNEL_IN5)
   | ADC_SQR3_SQ3_N(ADC_CHANNEL_IN6)
    | ADC_SQR3_SQ4_N(ADC_CHANNEL_IN7)   // Conversion group sequence 1...6.
};

#if VISCA_UART
/*
 * This callback is invoked when a transmission buffer has been completely
 * read by the driver.
 */
static void uart2_tx_end(UARTDriver *uartp) {
    (void)uartp;
    // TODO?
}

/*
 * This callback is invoked when a transmission has physically completed.
 */
static void uart2_tx_end_physical(UARTDriver *uartp) {
    (void)uartp;
    // TODO?
}

/*
 * This callback is invoked when a receive buffer has been completely written.
 */
static void uart2_rx_buffer_filled(UARTDriver *uartp) {
    (void)uartp;
    // TODO?
}

/*
 * This callback is invoked when a character is received but the application
 * was not ready to receive it, the character is passed as parameter.
 */
static void uart2_char_rx(UARTDriver *uartp, uint16_t c) {
    (void)uartp;
    (void)c;
    // TODO?
}

/*
 * This callback is invoked on a receive error, the errors mask is passed
 * as parameter.
 */
void uart2_rx_error(UARTDriver *uartp, uartflags_t e) {
    (void)uartp;
    (void)e;
    // TODO?
}

/*
 * UART 2 driver configuration structure.
 */
static UARTConfig uart_conf2 = {
  uart2_tx_end,
  uart2_tx_end_physical,
  uart2_rx_buffer_filled,
  uart2_char_rx,
  uart2_rx_error,
  9600,            // speed
  0,                // CR1
  USART_CR2_STOP1_BITS | USART_CR2_LINEN,  // CR2
  0                 // CR3
};

#else // -> !VISCA_UART
   SerialConfig sd_2_conf;
#endif // end VISCA_UART

void init_board_hal(void) {

	//
	// I2C #1 - BI8 boards

	i2c_1_conf.op_mode = OPMODE_I2C;
	i2c_1_conf.duty_cycle = FAST_DUTY_CYCLE_2;
	i2c_1_conf.clock_speed = 400000;

	i2cStart(&I2CD1, &i2c_1_conf);

	palSetPadMode(GPIOB, GPIOB_I2C1_SCL, PAL_MODE_ALTERNATE(4) | PAL_STM32_OTYPE_OPENDRAIN);   /* SCL */
	palSetPadMode(GPIOB, GPIOB_I2C1_SDA, PAL_MODE_ALTERNATE(4) | PAL_STM32_OTYPE_OPENDRAIN);   /* SDA */

	//
	// I2C #2 - Display-Buttons, EEPROM

	i2c_2_conf.op_mode = OPMODE_I2C;
	i2c_2_conf.duty_cycle = FAST_DUTY_CYCLE_2;
	i2c_2_conf.clock_speed = 400000;

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

#if	VISCA_UART
	uartStart(&UARTD2, &uart_conf2);
#else
	sd_2_conf.speed = 9600;
	sd_2_conf.cr1   = 0;
	sd_2_conf.cr2   = USART_CR2_STOP1_BITS | USART_CR2_LINEN;
	sd_2_conf.cr3   = 0;

	sdStart(&SD2, &sd_2_conf);
#endif

	//
	// ADC 3

	palSetPadMode(GPIOF, GPIOF_PIN6, PAL_MODE_INPUT_ANALOG);    // PF6: Slider
	palSetPadMode(GPIOF, GPIOF_PIN7, PAL_MODE_INPUT_ANALOG);    // PF7: Joy-X
	palSetPadMode(GPIOF, GPIOF_PIN8, PAL_MODE_INPUT_ANALOG);    // PF8: Joy-Y
	palSetPadMode(GPIOF, GPIOF_PIN9, PAL_MODE_INPUT_ANALOG);    // PF9: Joy-Z

	adcStart(&ADCD3, NULL);

	// Start continous adc conversions
	adcStartConversion(&ADCD3, &adcgrp_conf3, adc3_samples, ADC3_SMP_DEPTH);
}
