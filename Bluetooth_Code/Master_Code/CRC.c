/*
 * Checksum.c
 *
 *  Created on: 02-Dec-2019
 *      Author: Hp
 */

#include <CRC.h>



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

void CRC_Result_master(uint16_t *m_result)
{
    *m_result = CRC32->INIRES16;
}

/*********************************************************

Collect and compare result of CRC with CRC obtained from master

**********************************************************/

void CRC_Result_slave(uint16_t *s_result, uint16_t m_result)
{
    *s_result = CRC32->INIRES16;
// Compare CRC results
    if(*s_result == m_result)
        {
        P1->OUT ^= BIT0;
// SEND OK BACK TO MASTER AND CARRY ON



        }
    else
        {
        P2->OUT ^=BIT1;
// SEND TRANSMISSION ERROR TO MASTER AND ASK IT to SEND CODE AGAIN



        }
}
