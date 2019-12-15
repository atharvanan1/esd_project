/*
 * UART.h
 *
 *  Created on: 26-Nov-2019
 *      Author: Abhijeet
 */

#ifndef UART_H_
#define UART_H_

#include "common.h"

#define EOF (-1)

extern volatile uint16_t RX_Index;
extern int8_t buffer[8400];
extern volatile uint8_t CRC_Flag;

void bt_send_string(char *str);
void UART_Init(void);

#endif /* UART_H_ */
