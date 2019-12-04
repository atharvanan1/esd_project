/******************************************************************************

 MASTER SIDE CODE
 TAKES INPUT VIA A0
 SENDS OUTPUT VIA A2

******************************************************************************/

#include <CRC.h>
#include "ti/devices/msp432p4xx/inc/msp.h"
#include "stdint.h"
#include "UART.h"
#include "AT_Commands.h"
#include "stdlib.h"

volatile uint8_t CRC_Flag = 0;
volatile uint8_t FLAG = 0;
volatile uint8_t *buffer = NULL;
volatile uint16_t ISR_index = 0;
volatile uint16_t CRC_index = 0;
uint16_t CRC_result = 0;


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

    // Used for ISR checking
    P1->DIR |= BIT0;                        // RED LED PIN
    P1->OUT &= ~BIT0;

    P2->DIR |= BIT1;                        // GREEN LED PIN
    P2->OUT &= ~BIT1;

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

    buffer = (uint8_t*)malloc(300*sizeof(uint8_t));

    UART2_stringTx(set_master);


    while(1)
    {
          if(1 == CRC_Flag)
          {

              // Should be in a critical section
              CRC_result = 0;
              CRC_Flag = 0;

              CRC_Init();

              for(CRC_index = 0; CRC_index <= ISR_index; CRC_index++)
                  {
                          CRC_calculation(buffer[CRC_index]);
                  }

              CRC_Result_master(&CRC_result);
              buffer[(ISR_index+1)] = (uint8_t)CRC_result;

              // Should be in a critical section

          FLAG = 1;
          }

          if(1==FLAG)
          {
               FLAG = 0;
               //UART2_stringTx(buffer);
               for(CRC_index = 0; CRC_index <= (ISR_index+1) ; CRC_index++)
                   {
                       while(!(EUSCI_A2->IFG & EUSCI_A_IFG_TXIFG));
                       EUSCI_A2->TXBUF = buffer[CRC_index];
                   }
               while(!(EUSCI_A2->IFG & EUSCI_A_IFG_TXIFG));
               EUSCI_A2->TXBUF = '@';

               ISR_index = 0;
          }
    }
}

void EUSCIA2_IRQHandler(void)
{
            if (EUSCI_A2->IFG & EUSCI_A_IFG_RXIFG)
            {
            P2->OUT ^= BIT1;
            while(!(EUSCI_A0->IFG & EUSCI_A_IFG_TXIFG));
            EUSCI_A0->TXBUF = EUSCI_A2->RXBUF;
            }
}

void EUSCIA0_IRQHandler(void)
{
//        uint8_t index = 0;
            if (EUSCI_A0->IFG & EUSCI_A_IFG_RXIFG)
            {
                P2->OUT ^= BIT1;

                if(EUSCI_A0->RXBUF == '@')
                    {
                        CRC_Flag = 1;
                    }
                else
                    {
                        buffer[ISR_index]=EUSCI_A0->RXBUF;
                        ISR_index +=1;
                    }
            }
}

