/*
 * File - UART.c
 * Brief -  Contains all the UART related functions
 * Author - Abhijeet Dutt Srivastava
 * University of Colorado Boulder
 * Embedded System Design
 */

#include "UART.h"

#define EOF (-1)

/*
************************************************************
Function Name: UART2_stringTx
************************************************************
Purpose: Functions is used to transmit the program over Bluetooth
************************************************************
Arguments: char * str  -  String to be transmitted
************************************************************
Returns: NONE
************************************************************
*/

void UART2_stringTx(char *str)
{
    while(*str!='\0')
    {
        while(!(EUSCI_A2->IFG & EUSCI_A_IFG_TXIFG));
        EUSCI_A2->TXBUF = *(str++);

    }
}

/*
************************************************************
Function Name: UART1_stringTx
************************************************************
Purpose: Functions is used to transmit messages over EUSCI_A0
         to the PC
************************************************************
Arguments: char * str  -  String to be transmitted
************************************************************
Returns: NONE
************************************************************
*/

void UART1_stringTx(char *str)
{
    while(*str!='\0')
    {
        while(!(EUSCI_A0->IFG & EUSCI_A_IFG_TXIFG));
        EUSCI_A0->TXBUF = *(str++);

        //str++;
    }
}

/*
************************************************************
Function Name: UART2_Init
************************************************************
Purpose: Functions is used to initialise EUSCI_A2 UART
************************************************************
Arguments: NONE
************************************************************
Returns: NONE
************************************************************
*/

void UART2_Init(void)
{
    P3->SEL0 |= BIT2 | BIT3;

      EUSCI_A2->CTLW0 |= EUSCI_A_CTLW0_SWRST; // Put eUSCI in reset
      EUSCI_A2->CTLW0 = EUSCI_A_CTLW0_SWRST | // Remain eUSCI in reset
                        EUSCI_B_CTLW0_SSEL__SMCLK;
      EUSCI_A2->BRW = 19;                     // 12000000/16/115200
      EUSCI_A2->MCTLW = (8 << EUSCI_A_MCTLW_BRF_OFS) |
               EUSCI_A_MCTLW_OS16;
      EUSCI_A2->CTLW0 &= ~EUSCI_A_CTLW0_SWRST; // Initialize eUSCI
      EUSCI_A2->IFG &= ~EUSCI_A_IFG_RXIFG;    // Clear eUSCI RX interrupt flag
      EUSCI_A2->IE |= EUSCI_A_IE_RXIE;        // Enable USCI_A0 RX interrupt

}


/*
************************************************************
Function Name: UART0_Init
************************************************************
Purpose: Functions is used to initialise EUSCI_A0 UART
************************************************************
Arguments: NONE
************************************************************
Returns: NONE
************************************************************
*/

void UART0_Init(void)
{
    P1->SEL0 |= BIT2 | BIT3;                // set 2-UART pin as secondary function

    EUSCI_A0->CTLW0 |= EUSCI_A_CTLW0_SWRST; // Put eUSCI in reset
    EUSCI_A0->CTLW0 = EUSCI_A_CTLW0_SWRST | // Remain eUSCI in reset
                      EUSCI_B_CTLW0_SSEL__SMCLK;      // Configure eUSCI clock source for SMCLK
    EUSCI_A0->BRW = 19;                     // 12000000/16/115200
    EUSCI_A0->MCTLW = (8 << EUSCI_A_MCTLW_BRF_OFS) |
                      EUSCI_A_MCTLW_OS16;
            // configured as 38400 baud rate
    // UART A0 INTERRUPT SETUP
       EUSCI_A0->CTLW0 &= ~EUSCI_A_CTLW0_SWRST; // Initialize eUSCI
       EUSCI_A0->IFG &= ~EUSCI_A_IFG_RXIFG;    // Clear eUSCI RX interrupt flag
       EUSCI_A0->IE |= EUSCI_A_IE_RXIE;        // Enable USCI_A0 RX interrupt

}


void SD_Transmission(volatile uint16_t *total_count, volatile uint8_t *buffer)
{
    uint16_t index = 0;

                   //UART2_stringTx(buffer);
                   for(index = 0; index <= (*total_count) ; index++)
                       {
                           while(!(EUSCI_A2->IFG & EUSCI_A_IFG_TXIFG));
                           EUSCI_A2->TXBUF = buffer[index];
                       }
                   while(!(EUSCI_A2->IFG & EUSCI_A_IFG_TXIFG));
                   EUSCI_A2->TXBUF = EOF;
                   // CHANGE TO EOF AFTER TESTING
                   *total_count = 0;

}


