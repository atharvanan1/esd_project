/*
 * File - CRC.h
 * Brief -  Contains all the declarations for CRC related functions
 * Author - Abhijeet Dutt Srivastava
 * University of Colorado Boulder
 * Embedded System Design
 */

#ifndef CHECKSUM_H_
#define CHECKSUM_H_

#include "ti/devices/msp432p4xx/inc/msp.h"

void CRC_Init(void);
void CRC_calculation(uint8_t input_data);
void CRC_Result_master(uint16_t *m_result);
void CRC_Result_slave(uint16_t *s_result, uint16_t m_result);
void SD_CRC_Processing(volatile uint8_t *buffer, uint16_t total_count);



#endif /* CHECKSUM_H_ */


