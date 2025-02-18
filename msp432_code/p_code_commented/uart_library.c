/*
 * File - UART.c
 * Brief -  Contains all the UART related functions
 * Author - Abhijeet Dutt Srivastava
 * University of Colorado Boulder
 * Embedded System Design
 */

#include "uart_library.h"

/*
************************************************************
Function Name: bt_send_string
************************************************************
Purpose: Functions is used to transmit the program over Bluetooth
************************************************************
Arguments: char * str  -  String to be transmitted
************************************************************
Returns: NONE
************************************************************
*/


void bt_send_string(char *str)
{
    while(*str!='\0')
    {
        while(!(EUSCI_A2->IFG & EUSCI_A_IFG_TXIFG));
        EUSCI_A2->TXBUF = *(str++);
    }
}

/*
************************************************************
Function Name: UART_Init
************************************************************
Purpose: Functions is used to initialise EUSCI_A2 UART
************************************************************
Arguments: NONE
************************************************************
Returns: NONE
************************************************************
*/


void UART_Init(void)
{
    P3->SEL0 |= BIT2 | BIT3;

    EUSCI_A2->CTLW0 |= EUSCI_A_CTLW0_SWRST; // Put eUSCI in reset
    EUSCI_A2->CTLW0 = EUSCI_A_CTLW0_SWRST | // Remain eUSCI in reset
        EUSCI_B_CTLW0_SSEL__SMCLK;
    EUSCI_A2->BRW = 19;                     // 12000000/16/115200
    EUSCI_A2->MCTLW = (8 << EUSCI_A_MCTLW_BRF_OFS) |
        EUSCI_A_MCTLW_OS16;

    //UART A2 INTERRUPT SETUP
    EUSCI_A2->CTLW0 &= ~EUSCI_A_CTLW0_SWRST; // Initialize eUSCI
    EUSCI_A2->IFG &= ~EUSCI_A_IFG_RXIFG;    // Clear eUSCI RX interrupt flag
    EUSCI_A2->IE |= EUSCI_A_IE_RXIE;        // Enable USCI_A0 RX interrupt

    __enable_irq();
    NVIC->ISER[0] = 1 << ((EUSCIA2_IRQn) & 31);
}
