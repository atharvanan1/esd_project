/*
 * spi_library.c
 *
 *  Created on: Dec 3, 2019
 *      Author: athar
 */

#include "spi_library.h"

void spi_init(void)
{
    // Set GPIO Pin functionality for SPI
    P1->SEL0 |= BIT5 | BIT6 | BIT7;

    // Put EUSCI Module in Reset
    EUSCI_B0->CTLW0 |= EUSCI_B_CTLW0_SWRST;

    // Set USCI as Master, Synchronous Communication, MSB First, and CKPH = 1 configuration
    EUSCI_B0->CTLW0 |= EUSCI_B_CTLW0_MST |
                       EUSCI_B_CTLW0_SYNC |
                       EUSCI_B_CTLW0_CKPH |
                       EUSCI_B_CTLW0_MSB;

    // Set CKPL = 0
    EUSCI_B0->CTLW0 &= ~EUSCI_B_CTLW0_CKPL;

    // Selecting SMCLK as clock for communication
    EUSCI_B0->CTLW0 |= EUSCI_B_CTLW0_SSEL__SMCLK;

    // Set bit clock speed
    EUSCI_B0->BRW = 0xB0;   // Keep FE when programming

    // Start EUSCI Module
    EUSCI_B0->CTLW0 &= ~EUSCI_B_CTLW0_SWRST;
}

void spi_send(uint8_t data)
{
    while(!(EUSCI_B0->IFG & EUSCI_B_IFG_TXIFG));
    EUSCI_B0->TXBUF = data;
}

uint8_t spi_receive(void)
{
    while(!((EUSCI_B0->IFG & EUSCI_B_IFG_RXIFG)));
    return EUSCI_B0->RXBUF;
}
