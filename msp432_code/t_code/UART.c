/*
 * UART.c
 *
 *  Created on: 26-Nov-2019
 *      Author: Hp
 */

#include "UART.h"

#define EOF (-1)

void UART2_stringTx(char *str)
{
    while(*str!='\0')
    {
        while(!(EUSCI_A2->IFG & EUSCI_A_IFG_TXIFG));
        EUSCI_A2->TXBUF = *(str++);

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

void CRC_Processing(volatile uint8_t *crc_flag,volatile  uint8_t *start_flag,volatile uint8_t *tx_flag, volatile uint8_t *buffer, uint16_t total_count)
{
    uint16_t index = 0;

    if(1 == *crc_flag)
    {

        uint16_t CRC_result = 0;
        *crc_flag = 0;
        CRC_Init();

        for(index = 0; index < total_count; index++)
            {
                    CRC_calculation(buffer[index]);
            }

        CRC_Result_master(&CRC_result);
        buffer[total_count] = (uint8_t)CRC_result;

//              while(!(EUSCI_A0->IFG & EUSCI_A_IFG_TXIFG));
//              EUSCI_A0->TXBUF = buffer[ISR_index] ;
//
//              while(!(EUSCI_A0->IFG & EUSCI_A_IFG_TXIFG));
//              EUSCI_A0->TXBUF = (uint8_t)CRC_result ;

        // Should be in a critical section
        if(1==*start_flag)
        {
            *start_flag = 0;
            *tx_flag = 1;
        }
    }
}

void TX_Transmission(volatile uint8_t *txn_flag,volatile uint16_t *total_count, volatile uint8_t *buffer )
{
    uint16_t index = 0;
    if(1==*txn_flag)
              {
                  *txn_flag = 0;
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
}

void SD_CRC_Processing(volatile uint8_t *buffer, uint16_t total_count)
{
    uint16_t index = 0;

        uint16_t CRC_result = 0;

        CRC_Init();

        for(index = 0; index < total_count; index++)
            {
                    CRC_calculation(buffer[index]);
            }

        CRC_Result_master(&CRC_result);
        buffer[total_count] = (uint8_t)CRC_result;

//              while(!(EUSCI_A0->IFG & EUSCI_A_IFG_TXIFG));
//              EUSCI_A0->TXBUF = buffer[ISR_index] ;
//
//              while(!(EUSCI_A0->IFG & EUSCI_A_IFG_TXIFG));
//              EUSCI_A0->TXBUF = (uint8_t)CRC_result ;

        // Should be in a critical section

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


