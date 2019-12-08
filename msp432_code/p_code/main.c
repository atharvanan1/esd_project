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
full_system_state_t full_system = {0, errNO_Error, 0, 0};


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
//    UART_Init();
//    ISP_Init();
//    StateMachine = State_Machine_Init();
}

int main(void)
{
    Init();

    // Configure UART pins
    P1->SEL0 |= BIT2 | BIT3;                // set 2-UART pin as secondary function

    // Configure UART
    EUSCI_A0->CTLW0 |= EUSCI_A_CTLW0_SWRST; // Put eUSCI in reset
    EUSCI_A0->CTLW0 = EUSCI_A_CTLW0_SWRST | // Remain eUSCI in reset
            EUSCI_B_CTLW0_SSEL__SMCLK;      // Configure eUSCI clock source for SMCLK
    // Baud Rate calculation
    // 12000000/(16*9600) = 78.125
    // Fractional portion = 0.125
    // User's Guide Table 21-4: UCBRSx = 0x10
    // UCBRFx = int ( (78.125-78)*16) = 2
    EUSCI_A0->BRW = 78;                     // 12000000/16/9600
    EUSCI_A0->MCTLW = (2 << EUSCI_A_MCTLW_BRF_OFS) |
            EUSCI_A_MCTLW_OS16;

    EUSCI_A0->CTLW0 &= ~EUSCI_A_CTLW0_SWRST; // Initialize eUSCI
    EUSCI_A0->IFG &= ~EUSCI_A_IFG_RXIFG;    // Clear eUSCI RX interrupt flag

    state_machine_t* state_machine = NULL;
    state_machine = State_Machine_Init();
    while(1)
    {
        Event_Handler(state_machine, &full_system);
    }
}
//
//int putchar(int _x)
//{
//    while(!(EUSCI_A0->IFG & EUSCI_A_IFG_TXIFG));
//    EUSCI_A0->TXBUF = _x;
//    return _x;
//}
//
//int getchar(void)
//{
//    while(!(EUSCI_A0->IFG & EUSCI_A_IFG_RXIFG));
//    return EUSCI_A0->RXBUF;
//}
