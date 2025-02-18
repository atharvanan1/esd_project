/*
 * File - crc_library.h
 * Brief -  Contains all the declarations for CRC functions. 
 * Author - Abhijeet Dutt Srivastava 
 * University of Colorado Boulder
 * Embedded System Design
 */

#ifndef CHECKSUM_H_
#define CHECKSUM_H_

#include "common.h"

typedef enum {
    CRC_Correct,
    CRC_Error,
} crc_status_t;

void CRC_Init(void);
void CRC_calculation(uint8_t input_data);
uint16_t CRC_Result(void);
crc_status_t CRC_Check(uint8_t* buffer, volatile uint16_t* RX_Index);

#endif /* CHECKSUM_H_ */
