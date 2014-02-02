/*
 * board_hw.h
 *
 *  Created on: 28.10.2013
 *      Author: Peter Schuster
 */

#ifndef _BOARD_HW_H_
#define _BOARD_HW_H_

#include "hal.h"
#include "halconf.h"

#define ADC3_CH_NUM     4
#define ADC3_SMP_DEPTH  4

// VISCA to use UART (-> TRUE) or SERIAL (-> FALSE) driver?
#define VISCA_UART STM32_UART_USE_USART2

#ifdef __cplusplus
extern "C" {
#endif

void init_board_hal(void);

void enable_adc(adcsample_t* buffer);

#ifdef __cplusplus
}
#endif

#endif // _BOARD_HW_H_
