/*
 * UART.c
 *
 *  Created on: 26-Nov-2019
 *      Author: Hp
 */

#include "UART.h"

void UART2_stringTx(char *str)
{
    while(*str!='\0')
    {
        while(!(EUSCI_A2->IFG & EUSCI_A_IFG_TXIFG));
        EUSCI_A2->TXBUF = *str;
        str++;
    }
}

void UART1_stringTx(char *str)
{
    while(*str!='\0')
    {
        while(!(EUSCI_A0->IFG & EUSCI_A_IFG_TXIFG));
        EUSCI_A0->TXBUF = *(str++);

        //str++;
    }
}

void UARTBL_stringRx(char *str)
{
    while(*str!='\0')
    {
    P1->OUT ^= BIT0;
    *str = EUSCI_A2->RXBUF;
    EUSCI_A2->IFG &=~ EUSCI_A_IFG_RXIFG;// Clear interrupt
    str++;
    }
}

//void UART_configure_pins(void)
//{
//}




