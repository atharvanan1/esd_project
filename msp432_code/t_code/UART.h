/*
 * UART.h
 *
 *  Created on: 26-Nov-2019
 *      Author: Hp
 */

#ifndef UART_H_
#define UART_H_





#endif /* UART_H_ */

#include "ti/devices/msp432p4xx/inc/msp.h"
#include "CRC.h"
void UART2_stringTx(char *str);
void UART1_stringTx(char *str);
void UART2_Init(void);
void UART0_Init(void);
void CRC_Processing(volatile uint8_t *crc_flag,volatile  uint8_t *start_flag,volatile uint8_t *tx_flag,volatile uint8_t *buffer, uint16_t total_count);
void TX_Transmission(volatile uint8_t *txn_flag,volatile uint16_t *total_count, volatile uint8_t *buffer );
void SD_CRC_Processing(volatile uint8_t *buffer, uint16_t total_count);
void SD_Transmission(volatile uint16_t *total_count, volatile uint8_t *buffer);
void query_slave(void);
