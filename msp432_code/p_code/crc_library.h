/*
 * checksum.h
 *
 *  Created on: 02-Dec-2019
 *      Author: Hp
 */

#ifndef CHECKSUM_H_
#define CHECKSUM_H_

#endif /* CHECKSUM_H_ */

#include "msp.h"

void CRC_Init(void);
void CRC_calculation(uint8_t input_data);
void CRC_Result_master(uint16_t *m_result);
void CRC_Result_slave(uint16_t *s_result, uint16_t m_result);
