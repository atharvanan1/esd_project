/* --COPYRIGHT--,BSD_EX
 * Copyright (c) 2014, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *******************************************************************************
 *
 *                       MSP432 CODE EXAMPLE DISCLAIMER
 *
 * MSP432 code examples are self-contained low-level programs that typically
 * demonstrate a single peripheral function or device feature in a highly
 * concise manner. For this the code may rely on the device's power-on default
 * register values and settings such as the clock configuration and care must
 * be taken when combining code from several examples to avoid potential side
 * effects. Also see http://www.ti.com/tool/mspdriverlib for an API functional
 * library & https://dev.ti.com/pinmux/ for a GUI approach to peripheral configuration.
 *
 * --/COPYRIGHT--*/
//******************************************************************************
//   MSP432P401 Demo - eUSCI_B0, SPI 3-Wire Master with clock polarity high
//
//   This example can be used with msp432p401x_euscib0_spi_10.c 3-wire Slave.
//
//   Description: SPI master talks to SPI slave using 3-wire mode. This example
//   increments data sent by the master starting at 0x01. Received data is
//   expected to be same as the previous transmission RXData = TXData - 1 if
//   this example is used with msp432p401x_euscib0_spi_10.c.
//
//   USCI RX ISR is used to handle communication with the CPU, normally in LPM0.
//
//             MSP432P401x Master                    MSP432P401x Slaves
//             -----------------                      -----------------
//         /|\|                 |                 /|\|                 |
//          | |                 |                  | |                 |
//          --|RST              |                  --|RST              |
//            |                 |                    |                 |
//            |                 | Data In (UCB0SIMO) |                 |
//            |             P1.6|------------------->|P1.6             |
//            |                 |                    |                 |
//            |                 | Data OUT (UCB0SOMI)|                 |
//            |             P1.7|<-------------------|P1.7             |
//            |                 |                    |                 |
//            |                 | S Clock (UCB0CLK)  |                 |
//            |             P1.5|------------------->|P1.5             |
//            |                 |                    |                 |
//
//   William Goh
//   Texas Instruments Inc.
//   June 2016 (updated) | June 2014 (created)
//   Built with CCSv6.1, IAR, Keil, GCC
//******************************************************************************
#include "ti/devices/msp432p4xx/inc/msp.h"
#include <stdint.h>

static uint8_t RXData[] = {0xFF, 0xFF, 0xFF, 0xFF};
static uint8_t TXData[] = {0xAC, 0x53, 0x00, 0x00, 0x00, 0x00};
//static uint8_t index = 0;
uint8_t Vendor_Code = 0;
uint8_t Part_Family = 0;
uint8_t Part_Number = 0;

// time = 6000, when delay is 20ms
void delay(uint32_t time)
{
    for(volatile uint32_t i = 0; i < time; i++);
}

int main(void)
{
    volatile uint32_t i;
//    uint8_t previousData;

    WDT_A->CTL = WDT_A_CTL_PW |             // Stop watchdog timer
            WDT_A_CTL_HOLD;

    P1->OUT &= ~BIT0;
    P1->DIR |= BIT0;                        // Set P1.0 LED

//    P1->OUT &= ~BIT5;
//    P1->DIR |= BIT5;

    P3->OUT |= BIT0;
    P3->DIR |= BIT0;

//    delay(2000);

    P1->SEL0 |= BIT5 | BIT6 | BIT7;         // Set P1.5, P1.6, and P1.7 as
                                            // SPI pins functionality

    EUSCI_B0->CTLW0 |= EUSCI_B_CTLW0_SWRST; // Put eUSCI state machine in reset
    EUSCI_B0->CTLW0 = EUSCI_B_CTLW0_SWRST | // Remain eUSCI state machine in reset
            EUSCI_B_CTLW0_MST |             // Set as SPI master
            EUSCI_B_CTLW0_SYNC |            // Set as synchronous mode
            EUSCI_B_CTLW0_CKPH |
            EUSCI_B_CTLW0_MSB;              // MSB first
    EUSCI_B0->CTLW0 &= ~EUSCI_B_CTLW0_CKPL;
//    EUSCI_B0->IE |= EUSCI_B_IE_RXIE;
    EUSCI_B0->CTLW0 |= EUSCI_B_CTLW0_SSEL__SMCLK; // ACLK
    EUSCI_B0->BRW = 0x30;                   // /2,fBitClock = fBRCLK/(UCBRx+1).
    EUSCI_B0->CTLW0 &= ~EUSCI_B_CTLW0_SWRST;// Initialize USCI state machine

    // Enable global interrupt
//    __enable_irq();

    // Enable eUSCI_B0 interrupt in NVIC module
//    NVIC->ISER[0] = 1 << ((EUSCIB0_IRQn) & 31);

//    for(uint8_t k = 0; k < 10; k++)
//    {
    P3->OUT &= ~BIT0;
    delay(20);
    P3->OUT |= BIT0;
    delay(20);

    P3->OUT &= ~BIT0;

    delay(8000);
    delay(8000);
    delay(8000);
    delay(8000);

    for(volatile uint8_t jack = 0; jack < 4; jack++)
    {
        while(!(EUSCI_B0->IFG & EUSCI_B_IFG_TXIFG));
        EUSCI_B0->TXBUF = TXData[jack];           // Transmit characters
        while(!((EUSCI_B0->IFG & EUSCI_B_IFG_RXIFG)));
        RXData[jack] = EUSCI_B0->RXBUF;
    }

    // Vendor Code Read

    TXData[0] = 0x30;
    TXData[1] = 0x00;
    TXData[2] = 0x00;
    TXData[3] = 0x00;

    if(RXData[2] == 0x53)
    {
        P1->OUT |= BIT0;
        for(volatile uint8_t jack = 0; jack < 4; jack++)
        {
            while(!(EUSCI_B0->IFG & EUSCI_B_IFG_TXIFG));
            EUSCI_B0->TXBUF = TXData[jack];           // Transmit characters
            while(!((EUSCI_B0->IFG & EUSCI_B_IFG_RXIFG)));
            RXData[jack] = EUSCI_B0->RXBUF;
        }
    }

    Vendor_Code = RXData[3];

    // Part Family Code Read

    TXData[0] = 0x30;
    TXData[1] = 0x00;
    TXData[2] = 0x01;
    TXData[3] = 0x00;

    if(RXData[3] == 0x1E)
    {
        P1->OUT |= BIT0;
        for(volatile uint8_t jack = 0; jack < 4; jack++)
        {
            while(!(EUSCI_B0->IFG & EUSCI_B_IFG_TXIFG));
            EUSCI_B0->TXBUF = TXData[jack];           // Transmit characters
            while(!((EUSCI_B0->IFG & EUSCI_B_IFG_RXIFG)));
            RXData[jack] = EUSCI_B0->RXBUF;
        }
    }

    Part_Family = RXData[3];

    // Part Number Read
    TXData[0] = 0x30;
    TXData[1] = 0x00;
    TXData[2] = 0x02;
    TXData[3] = 0x00;

    if(RXData[3] == 0x95)
    {
        P1->OUT |= BIT0;
        for(volatile uint8_t jack = 0; jack < 4; jack++)
        {
            while(!(EUSCI_B0->IFG & EUSCI_B_IFG_TXIFG));
            EUSCI_B0->TXBUF = TXData[jack];           // Transmit characters
            while(!((EUSCI_B0->IFG & EUSCI_B_IFG_RXIFG)));
            RXData[jack] = EUSCI_B0->RXBUF;
        }
    }

    Part_Number = RXData[3];

    // High Byte Read

    TXData[0] = 0x28;
    TXData[1] = 0x01;
    TXData[2] = 0x02;
    TXData[3] = 0x00;

    for(volatile uint8_t jack = 0; jack < 4; jack++)
    {
        while(!(EUSCI_B0->IFG & EUSCI_B_IFG_TXIFG));
        EUSCI_B0->TXBUF = TXData[jack];           // Transmit characters
        while(!((EUSCI_B0->IFG & EUSCI_B_IFG_RXIFG)));
        RXData[jack] = EUSCI_B0->RXBUF;
    }

    uint8_t data_high = RXData[3];

    // Low Byte Read

    TXData[0] = 0x20;
    TXData[1] = 0x01;
    TXData[2] = 0x02;
    TXData[3] = 0x00;

    for(volatile uint8_t jack = 0; jack < 4; jack++)
    {
        while(!(EUSCI_B0->IFG & EUSCI_B_IFG_TXIFG));
        EUSCI_B0->TXBUF = TXData[jack];           // Transmit characters
        while(!((EUSCI_B0->IFG & EUSCI_B_IFG_RXIFG)));
        RXData[jack] = EUSCI_B0->RXBUF;
    }

    uint8_t data_low = RXData[3];

    // Load Data at High Byte

    TXData[0] = 0x48;
    TXData[1] = 0x01;
    TXData[2] = 0x02;
    TXData[3] = 0x55;

    for(volatile uint8_t jack = 0; jack < 4; jack++)
    {
        while(!(EUSCI_B0->IFG & EUSCI_B_IFG_TXIFG));
        EUSCI_B0->TXBUF = TXData[jack];           // Transmit characters
        while(!((EUSCI_B0->IFG & EUSCI_B_IFG_RXIFG)));
        RXData[jack] = EUSCI_B0->RXBUF;
    }

    // Load Data at Low Byte

    TXData[0] = 0x40;
    TXData[1] = 0x01;
    TXData[2] = 0x02;
    TXData[3] = 0x55;

    for(volatile uint8_t jack = 0; jack < 4; jack++)
    {
        while(!(EUSCI_B0->IFG & EUSCI_B_IFG_TXIFG));
        EUSCI_B0->TXBUF = TXData[jack];           // Transmit characters
        while(!((EUSCI_B0->IFG & EUSCI_B_IFG_RXIFG)));
        RXData[jack] = EUSCI_B0->RXBUF;
    }

    //-------------------------------------------------------

    // Write Page Buffer in Program Memory

    TXData[0] = 0x4C;
    TXData[1] = 0x01;
    TXData[2] = 0x02;
    TXData[3] = 0x00;

    for(volatile uint8_t jack = 0; jack < 4; jack++)
    {
        while(!(EUSCI_B0->IFG & EUSCI_B_IFG_TXIFG));
        EUSCI_B0->TXBUF = TXData[jack];           // Transmit characters
        while(!((EUSCI_B0->IFG & EUSCI_B_IFG_RXIFG)));
        RXData[jack] = EUSCI_B0->RXBUF;
    }

   delay(2000);

    // High Byte Read

    TXData[0] = 0x28;
    TXData[1] = 0x01;
    TXData[2] = 0x02;
    TXData[3] = 0x00;

    for(volatile uint8_t jack = 0; jack < 4; jack++)
    {
        while(!(EUSCI_B0->IFG & EUSCI_B_IFG_TXIFG));
        EUSCI_B0->TXBUF = TXData[jack];           // Transmit characters
        while(!((EUSCI_B0->IFG & EUSCI_B_IFG_RXIFG)));
        RXData[jack] = EUSCI_B0->RXBUF;
    }

    data_high = RXData[3];

    // Low Byte Read

    TXData[0] = 0x20;
    TXData[1] = 0x01;
    TXData[2] = 0x02;
    TXData[3] = 0x00;

    for(volatile uint8_t jack = 0; jack < 4; jack++)
    {
        while(!(EUSCI_B0->IFG & EUSCI_B_IFG_TXIFG));
        EUSCI_B0->TXBUF = TXData[jack];           // Transmit characters
        while(!((EUSCI_B0->IFG & EUSCI_B_IFG_RXIFG)));
        RXData[jack] = EUSCI_B0->RXBUF;
    }

    data_low = RXData[3];

//    delay(2000);
    P3->OUT |= BIT0;
    P1->OUT &= ~BIT0;
//    delay(2000);
//    }
    while(1);

//    while(1)
//    {
//        __sleep();
//        __no_operation();                   // For debug,Remain in LPM0
//
//
//        // Check the received data
//        if(index == 0)
//        {
//            previousData = 0;
//        }
//        else
//        {
//            previousData = TXData[index - 1];
//        }
//        if (RXData != (previousData))
//        {
//            // If the Received data is not equal to TXData-1, then
//            // Set P1.0 LED
//            P1->OUT |= BIT0;
//        }
//        else
//        {
//            P1->OUT &= ~BIT0;
//        }
//
////        for (i = 2000; i > 0; i--);         // Delay before next transmission
////        delay(100);
//        index++;                           // Increment transmit data
//        if(index == 4)
//        {
//            __disable_irq();
//            P3->OUT |= BIT0;
//            while(1);
//        }
//    }
}

// SPI interrupt service routine
//void EUSCIB0_IRQHandler(void)
//{
//    if (EUSCI_B0->IFG & EUSCI_B_IFG_RXIFG)
//    {
//        RXData = EUSCI_B0->RXBUF;
//        if(RXData == 0x53)
//        {
//            P1->OUT |= BIT0;
//        }
//
//        // Clear the receive interrupt flag
//        EUSCI_B0->IFG &= ~EUSCI_B_IFG_RXIFG;
//    }
//}
