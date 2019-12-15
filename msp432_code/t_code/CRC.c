/*
 * File - CRC.c
 * Brief -  Contains all the CRC related functions
 * Author - Abhijeet Dutt Srivastava
 * University of Colorado Boulder
 * Embedded System Design
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

/*
************************************************************
Function Name: SD_CRC_Processing
************************************************************
Purpose: Function is used to perform CRC on the data received from the
         user
************************************************************
Arguments: volatile uint8_t *buffer, uint16_t total_count (count of the data)
************************************************************
Returns: NONE
************************************************************
*/

void SD_CRC_Processing(volatile uint8_t *buffer, uint16_t total_count)
{
    uint16_t index = 0;

        uint16_t CRC_result = 0;

        CRC_Init();

        for(index = 0; index < total_count; index++)
            {
                    CRC_calculation(buffer[index]);
            }

        CRC_Result_master(&CRC_result);
        buffer[total_count] = (uint8_t)CRC_result;
}

