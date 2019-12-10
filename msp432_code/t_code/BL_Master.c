/******************************************************************************

 MASTER SIDE CODE
 TAKES INPUT VIA A0
 SENDS OUTPUT VIA A2

******************************************************************************/

#include <CRC.h>
#include "ti/devices/msp432p4xx/inc/msp.h"
#include "stdint.h"
//#include "UART.h"
#include "AT_Commands.h"
#include "stdlib.h"
#include "common.h"
#include "state_machine.h"

#define RESET (0)
full_system_state_t system_state_data = {RESET,errNO_Error,RESET,RESET,RESET,RESET,RESET
                                         ,RESET,RESET,RESET,RESET};
full_system_state_t* system_state = &system_state_data;

state_machine_t *state_machine = NULL;

volatile uint8_t CRC_Flag = 0;
volatile uint8_t TXN_Flag = 0;
volatile uint8_t Start_Flag = 0;
volatile uint8_t buffer[8400];
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
    // PIN 3.2 RXD
    // PIN 3.3 TXD

    UART0_Init();
    UART2_Init();
    State_Machine_Init();

    // RED LED IS USED FOR ERROR
    P1->DIR |= BIT0;                        // RED LED PIN
    P1->OUT &= ~BIT0;

    // GREEN LED WHEN TX IS COMPLETE
    P2->DIR |= BIT1;                        // GREEN LED PIN
    P2->OUT &= ~BIT1;

    // BLUE LED WHEN ALL IS OKAY
    P2->DIR |= BIT2;                        // GREEN LED PIN
    P2->OUT &= ~BIT2;


    // Enable global interrupt
    __enable_irq();

    // Enable eUSCIA0 interrupt in NVIC module

    NVIC->ISER[0] = 1 << ((EUSCIA0_IRQn) & 31);
    NVIC->ISER[0] = 1 << ((EUSCIA2_IRQn) & 31);

    //UART2_stringTx(set_master);

    state_machine_t* state_machine = NULL;
    state_machine = State_Machine_Init();

    while(1)
    {
//        CRC_Processing(&CRC_Flag, &Start_Flag, &TXN_Flag, buffer, ISR_index);
//        TX_Transmission(&TXN_Flag, &ISR_index,buffer);
          Event_Handler(state_machine, system_state);
    }
}


void EUSCIA2_IRQHandler(void)
{
            if (EUSCI_A2->IFG & EUSCI_A_IFG_RXIFG)
            {
            P2->OUT ^= BIT1;
            if(EUSCI_A2->RXBUF == 'S')
                {
                    system_state->RX_Response = 1;
                }
            else if(EUSCI_A2->RXBUF == 'F')
                 {
                    //CRC error
                    system_state->CRC_Failure = 1;
                 }
            else if(EUSCI_A2->RXBUF == 'f')
                 {
                    //Checksum Error
                    system_state->Checksum_Failure = 1;
                 }
            else if(EUSCI_A2->RXBUF == 'C')
                 {
                    //Everything is fine
                    P2->OUT |= BIT2;
                    system_state->All_Ok = 1;
                 }
            else if(EUSCI_A2->RXBUF == 'E')
            {
                system_state->error_flag = errSM_Error;
                // What to do here?
            }
            //REMOVE
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

                system_state->PC_RX_Flag = 1;

                if(EUSCI_A0->RXBUF == '@')
                    {
                        //CRC_Flag = 1;
                    system_state->RX_Complete = 1;

                    }
                else
                    {
                        buffer[ISR_index] = EUSCI_A0->RXBUF;
                        ISR_index +=1;
                    }
            }
}

