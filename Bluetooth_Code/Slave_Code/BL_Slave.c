//******************************************************************************

// SLAVE SIDE CODE
// TAKES INPUT VIA A2
// SENDS OUTPUT VIA A0

//******************************************************************************
#include "ti/devices/msp432p4xx/inc/msp.h"
#include "stdint.h"
#include "UART.h"
#include "AT_Commands.h"
#include "stdlib.h"
#include "CRC.h"

/***********************************************************************
Programming Flag Set when Code is received and CRC comparison is correct
************************************************************************/
uint8_t Program_Flag = 0;
/***********************************************************************
RX Index keeps count of number of bytes received
************************************************************************/
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
uint8_t CRC_Flag = 0;

volatile uint8_t *buffer = NULL;
volatile uint8_t counter = 0;

/*******************************************************
 Start Bit to be set when slave is ready to receive code
********************************************************/

uint8_t Start = 0;

int main(void)
{
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;      // Stop watchdog timer

    CS->KEY = CS_KEY_VAL;                   // Unlock CS module for register access
    CS->CTL0 = 0;                           // Reset tuning parameters
    CS->CTL0 = CS_CTL0_DCORSEL_3;           // Set DCO to 12MHz (nominal, center of 8-16MHz range)
    CS->CTL1 = CS_CTL1_SELA_2 |             // Select ACLK = REFO
            CS_CTL1_SELS_3 |                // SMCLK = DCO
            CS_CTL1_SELM_3;                 // MCLK = DCO
    CS->KEY = 0;                            // Lock CS module from unintended accesses

/******Configure UART pins*****/
    // FOR PC Side Pins
    P1->SEL0 |= BIT2 | BIT3;                // set 2-UART pin as secondary function

    // For HC-05 side pins
    //P3->SEL1 &= ~(BIT2 | BIT3);                // Setup P3.2 and P3.2 as TX pins to bluetooth module
    P3->SEL0 |= BIT2 | BIT3;
    // PIN 3.2 RXD
    // PIN 3.3 TXD

    // From Datasheet page 141


    // DOUBT
    // P1-> SEL0 &= ~(BIT2 | BIT3);         // secondary module function selected
    // P1-> SEL1 |= (BIT2 | BIT3);            // secondary module function selected

    // Used for ISR checking
    P1->DIR |= BIT0;                        // RED LED PIN
    P1->OUT &= ~BIT0;

    P2->DIR |= BIT1;                        // GREEN LED PIN
    P2->OUT &= ~BIT1;

    P2->DIR |= BIT2;                        // BLUE LED PIN
    P2->OUT &= ~BIT2;

    // Configure UART A0
        EUSCI_A0->CTLW0 |= EUSCI_A_CTLW0_SWRST; // Put eUSCI in reset
        EUSCI_A0->CTLW0 = EUSCI_A_CTLW0_SWRST | // Remain eUSCI in reset
            EUSCI_B_CTLW0_SSEL__SMCLK;      // Configure eUSCI clock source for SMCLK
        EUSCI_A0->BRW = 19;                     // 12000000/16/115200
        EUSCI_A0->MCTLW = (8 << EUSCI_A_MCTLW_BRF_OFS) |
                EUSCI_A_MCTLW_OS16;
        // configured as 38400 baud rate

        EUSCI_A2->CTLW0 |= EUSCI_A_CTLW0_SWRST; // Put eUSCI in reset
        EUSCI_A2->CTLW0 = EUSCI_A_CTLW0_SWRST | // Remain eUSCI in reset
            EUSCI_B_CTLW0_SSEL__SMCLK;
        EUSCI_A2->BRW = 19;                     // 12000000/16/115200
        EUSCI_A2->MCTLW = (8 << EUSCI_A_MCTLW_BRF_OFS) |
            EUSCI_A_MCTLW_OS16;

    // UART A0 INTERRUPT SETUP
    EUSCI_A0->CTLW0 &= ~EUSCI_A_CTLW0_SWRST; // Initialize eUSCI
    EUSCI_A0->IFG &= ~EUSCI_A_IFG_RXIFG;    // Clear eUSCI RX interrupt flag
    EUSCI_A0->IE |= EUSCI_A_IE_RXIE;        // Enable USCI_A0 RX interrupt

    //UART A2 INTERRUPT SETUP
    EUSCI_A2->CTLW0 &= ~EUSCI_A_CTLW0_SWRST; // Initialize eUSCI
    EUSCI_A2->IFG &= ~EUSCI_A_IFG_RXIFG;    // Clear eUSCI RX interrupt flag
    EUSCI_A2->IE |= EUSCI_A_IE_RXIE;        // Enable USCI_A0 RX interrupt

    //EUSCI_A2->IFG &= ~EUSCI_A_IFG_TXIFG;// clear TX Flag
    // Enable global interrupt
    __enable_irq();

    // Enable eUSCIA0 interrupt in NVIC module

    NVIC->ISER[0] = 1 << ((EUSCIA0_IRQn) & 31);
    NVIC->ISER[0] = 1 << ((EUSCIA2_IRQn) & 31);

    buffer = (uint8_t*)malloc(256*sizeof(uint8_t));
//    UART2_stringTx(IQ_binding);


    // If slave is ready to receive program
    // Set when the programmer is ready to receive code
    if(1==Start)
    {
        // UART IRQ to be disabled before Start bit
        // Enable UART IRQ
        Start = 0;
        while(!(EUSCI_A2 ->IFG & EUSCI_A_IFG_TXIFG));
        EUSCI_A2->TXBUF = 'S';
    }


    while(1)
    {
           if(1==CRC_Flag)
           {
               // Should be in a critical section
               CRC_Flag = 0;
               CRC_Result = 0;
               counter = 0;

               CRC_Init();

               for(CRC_Index = 0; CRC_Index < RX_Index; CRC_Index++ )
                   {
                       CRC_calculation(buffer[CRC_Index]);
                   }
               CRC_Result_master(&CRC_Result);

               if(buffer[RX_Index != (uint8_t)CRC_Result])
                   {
                       P1->OUT |= BIT0;
                       // Send Transmission FAIL
                       // Keep SPI coding disabled
                   }
               else
                   {
                       P2->OUT |= BIT2;
                       // Send Transmission Success
                       // Enable SPI coding
                   }

// Just to see the received data on putty
               for(counter = 0; counter < RX_Index; counter++)
               {
                   while(!(EUSCI_A0->IFG & EUSCI_A_IFG_TXIFG));
                   EUSCI_A0->TXBUF = buffer[counter];
               }
// Just to see received data on putty

               // Make RX_Index 0 again for new data
               RX_Index = 0;
               // Should be in a critical section
           }
    }

}

void EUSCIA2_IRQHandler(void)
{
            if (EUSCI_A2->IFG & EUSCI_A_IFG_RXIFG)
            {
            P2->OUT ^= BIT1;
//           while(!(EUSCI_A0->IFG & EUSCI_A_IFG_TXIFG));
//           EUSCI_A0->TXBUF = EUSCI_A2->RXBUF;
            if(EUSCI_A2->RXBUF == '@')
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

// NOT NEEDED
void EUSCIA0_IRQHandler(void)
{
            if (EUSCI_A0->IFG & EUSCI_A_IFG_RXIFG)
            {
                P2->OUT ^= BIT1;
            }
}
// NOT NEEDED
