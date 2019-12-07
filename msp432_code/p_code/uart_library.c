/*
 * UART.c
 *
 *  Created on: 26-Nov-2019
 *      Author: Hp
 */

#include <uart_library.h>

void bt_send_string(char *str)
{
    while(*str!='\0')
    {
        while(!(EUSCI_A2->IFG & EUSCI_A_IFG_TXIFG));
        EUSCI_A2->TXBUF = *(str++);

    }
}

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
}

void EUSCIA2_IRQHandler(void)
{
    if (EUSCI_A2->IFG & EUSCI_A_IFG_RXIFG)
    {
        P2->OUT ^= BIT1;
        if(EUSCI_A2->RXBUF == (uint8_t) EOF)
        {
            CRC_Flag = 1;
        }
        else
        {
            buffer[RX_Index]=EUSCI_A2->RXBUF;
            RX_Index +=1;
        }
    }
}
