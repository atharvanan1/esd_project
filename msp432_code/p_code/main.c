/*****************************************************************************
*
* Copyright (C) 2013 - 2017 Texas Instruments Incorporated - http://www.ti.com/
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions
* are met:
*
* * Redistributions of source code must retain the above copyright
*   notice, this list of conditions and the following disclaimer.
*
* * Redistributions in binary form must reproduce the above copyright
*   notice, this list of conditions and the following disclaimer in the
*   documentation and/or other materials provided with the
*   distribution.
*
* * Neither the name of Texas Instruments Incorporated nor the names of
*   its contributors may be used to endorse or promote products derived
*   from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
* "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
* LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
* A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
* OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
* SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
* LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
* DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
* THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
* OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
******************************************************************************
*
* MSP432 empty main.c template
*
******************************************************************************/

#include "main.h"

//state_machine_t* StateMachine = NULL;
//system_state_t system = {/*Reset values*/};


/***********************************************************************
Programming Flag Set when Code is received and CRC comparison is correct
************************************************************************/
uint8_t Program_Flag = 0;
// RX Index Info
volatile uint16_t RX_Index = 0;
/***********************************************************************
CRC_Index is used while performing CRC on input data
************************************************************************/
volatile uint16_t CRC_Index = 0;
/***********************************************************************
CRC_Result stores the result of the CRC
************************************************************************/
uint16_t CRC_Result = 0;
/***********************************************************************
CRC_Flag is used to start CRC when data is received
************************************************************************/
volatile uint8_t CRC_Flag = 0;
/*******************************************************
 Start Bit to be set when slave is ready to receive code
********************************************************/
uint8_t Start = 0;

int8_t buffer[] = ":100000000C9434000C943E000C943E000C943E0082\r\n\
:100010000C943E000C943E000C943E000C943E0068\r\n\
:100020000C943E000C943E000C943E000C943E0058\r\n\
:100030000C943E000C943E000C943E000C943E0048\r\n\
:100040000C943E000C943E000C943E000C943E0038\r\n\
:100050000C943E000C943E000C943E000C943E0028\r\n\
:100060000C943E000C943E0011241FBECFEFD8E04C\r\n\
:10007000DEBFCDBF0E9440000C9456000C940000DF\r\n\
:10008000219A299A2FE38DE093E0215080409040FF\r\n\
:10009000E1F700C0000029982FE38DE093E02150A4\r\n\
:1000A00080409040E1F700C00000EBCFF894FFCF14\r\n\
:00000001FF\r\n\0";
command_t commands[200];

void Init(void)
{
    WDT_A->CTL = WDT_A_CTL_PW |             // Stop WDT
               WDT_A_CTL_HOLD;

    CS->KEY = CS_KEY_VAL;                   // Unlock CS module for register access
    CS->CTL0 = 0;                           // Reset tuning parameters
    CS->CTL0 = CS_CTL0_DCORSEL_3;           // Set DCO to 12MHz (nominal, center of 8-16MHz range)
    CS->CTL1 = CS_CTL1_SELA_2 |             // Select ACLK = REFO
            CS_CTL1_SELS_3 |                // SMCLK = DCO
            CS_CTL1_SELM_3;                 // MCLK = DCO
    CS->KEY = 0;                            // Lock CS module from unintended accesses

    LED_Init();
    UART_Init();
    ISP_Init();
//    StateMachine = State_Machine_Init();
}

int main(void)
{
    Init();
    // If slave is ready to receive program
    // Set when the programmer is ready to receive code
    Start = 1;
    if(1 == Start)
    {
        // UART IRQ to be disabled before Start bit
        // Enable UART IRQ
        NVIC->ISER[0] = 1 << ((EUSCIA2_IRQn) & 31);
        Start = 0;
        while(!(EUSCI_A2 ->IFG & EUSCI_A_IFG_TXIFG));
        EUSCI_A2->TXBUF = 'S';
    }

    // Should be in a critical section
    CRC_Flag = 0;
    CRC_Result = 0;

    while(CRC_Flag == 0);

   CRC_Init();

   for(CRC_Index = 0; CRC_Index < RX_Index; CRC_Index++ )
   {
       CRC_calculation(buffer[CRC_Index]);
   }
   CRC_Result_master(&CRC_Result);

   RX_Index = RX_Index - 1;
   if(buffer[RX_Index] != (uint8_t)CRC_Result)
   {
       Turn_On(LED_RGB_R);
       // Send Transmission FAIL
       // Keep SPI coding disabled
   }
   else
   {
       Turn_On(LED_RGB_B);
       // Send Transmission Success
       // Enable SPI coding
   }

   buffer[RX_Index] = '\n';
   RX_Index = RX_Index + 1;
   buffer[RX_Index] = '\0';

   hex_file_t hex_file = {
                          RX_Index,
                          (char*) buffer
   };

   // Make RX_Index 0 again for new data
   RX_Index = 0;
   // Should be in a critical section
   EUSCI_A2->CTLW0 |= EUSCI_A_CTLW0_SWRST;

   hex_parse(&hex_file);
   programming_enable();
   chip_erase();
   while(poll_busy() == ispBUSY);
   programming_enable();
//   delay(3000);
   program();
   Turn_Off(LED_RGB_R);
   Turn_On(LED_RGB_B);
   P3->OUT |= BIT0;
   while(1);
}
