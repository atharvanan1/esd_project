/*
 * Checksum.c
 *
 *  Created on: 02-Dec-2019
 *      Author: Hp
 */

#include "crc_library.h"


/*********************************************************
 First, use the CRC16 hardware module to calculate the CRC
 Seed value is provided to the CRC Initialisation register
**********************************************************/


void CRC_Init(void)
{
    uint16_t CRC16_Init = 0xFFFF;
    CRC32->INIRES16 = CRC16_Init;           // Init CRC16 HW module

}

/*********************************************************

Input data to the CRC data input register

**********************************************************/

void CRC_calculation(uint8_t input_data)
{
    CRC32->DIRB16 = input_data;
}

/*********************************************************

Collect result of CRC (MASTER SIDE)

**********************************************************/

uint16_t CRC_Result(void)
{
    return CRC32->INIRES16;
}

crc_status_t CRC_Check(uint8_t* buffer, volatile uint16_t* RX_Index)
{
    uint8_t CRC;
    crc_status_t CRC_Status;

    CRC_Init();

    for(uint16_t CRC_Index = 0; CRC_Index < (*RX_Index-1); CRC_Index++ )
    {
       CRC_calculation(buffer[CRC_Index]);
    }

    CRC = (uint8_t) CRC_Result();

    *RX_Index = *RX_Index - 1;
    if(buffer[*RX_Index] != CRC)
    {
      CRC_Status = CRC_Error;
    }
    else
    {
      CRC_Status = CRC_Correct;
    }

    buffer[*RX_Index] = '\n';
    *RX_Index = *RX_Index + 1;
    buffer[*RX_Index] = '\0';

    return CRC_Status;
}
