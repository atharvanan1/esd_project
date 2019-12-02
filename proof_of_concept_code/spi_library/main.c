#include "msp.h"
//#include "spi.h"

/**
 * main.c
 */
void main(void)
{
	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;		// stop watchdog timer

	P1->OUT &= ~BIT0;
	P1->DIR |= BIT0;                        // Set P1.0 LED

	P3->OUT |= BIT0;                        // Set P3.0 High
	P3->DIR |= BIT0;

	P1->OUT |= BIT5;                        // Set SCK High
	P1->DIR |= BIT5;

	P3->OUT &= ~BIT0;                       // Set Reset pin Low
	P1->OUT &= ~BIT5;                       // Set SCK Low


    P1->SEL0 |= BIT5 | BIT6 | BIT7;         // Set P1.5, P1.6, and P1.7 as
                                            // SPI pins functionality

    EUSCI_B0->CTLW0 |= EUSCI_B_CTLW0_SWRST; // Put eUSCI state machine in reset
    EUSCI_B0->CTLW0 = EUSCI_B_CTLW0_SWRST | // Remain eUSCI state machine in reset
            EUSCI_B_CTLW0_MST |             // Set as SPI master
            EUSCI_B_CTLW0_SYNC |            // Set as synchronous mode
            EUSCI_B_CTLW0_CKPL |            // Set clock polarity high
            EUSCI_B_CTLW0_MSB;              // MSB first

    EUSCI_B0->CTLW0 |= EUSCI_B_CTLW0_SSEL__ACLK; // ACLK
    EUSCI_B0->BRW = 0x01;                   // /2,fBitClock = fBRCLK/(UCBRx+1).
    EUSCI_B0->CTLW0 &= ~EUSCI_B_CTLW0_SWRST;// Initialize USCI state machine

    uint8_t TXData[] = {0xAC, 0x53, 0x00, 0x00};
    uint8_t i = 0;
}
