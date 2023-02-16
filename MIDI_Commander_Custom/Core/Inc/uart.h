/*
 * uart.h
 *
 *  Created on: 16 Feb 2023
 *      Author: ben
 */

#ifndef INC_UART_H_
#define INC_UART_H_

#include <stdint.h>

void uart_init(void);
void uart_putc(uint8_t c);

#endif /* INC_UART_H_ */
