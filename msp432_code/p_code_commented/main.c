/*
 * File - main.c
 * Brief -  Main Program 
 * Author - Atharva Nandanwar 
 * University of Colorado Boulder
 * Embedded System Design
 */

#include "main.h"

state_machine_t* State_Machine = NULL;
full_system_state_t system_state_data = {/*Reset Values*/};
full_system_state_t* system_state = &system_state_data;
#define max_commands 200

int8_t buffer[8400];
command_t commands[max_commands];

void Init(void)
{
    WDT_A->CTL = WDT_A_CTL_PW |             // Stop WDT
               WDT_A_CTL_HOLD;

    CS->KEY = CS_KEY_VAL;                   // Unlock CS module for register access
    CS->CTL0 = 0;                           // Reset tuning parameters
    CS->CTL0 = CS_CTL0_DCORSEL_3;           // Set DCO to 12MHz (nominal, center of 8-16MHz range)
    CS->CTL1 = CS_CTL1_SELA_2 |             // Select ACLK = REFO
            CS_CTL1_SELS_3 |                // SMCLK = DCO
            CS_CTL1_SELM_3;                 // MCLK = DCO
    CS->KEY = 0;                            // Lock CS module from unintended accesses

    LED_Init();
    UART_Init();
    ISP_Init();
    State_Machine = State_Machine_Init();
}

int main(void)
{
    Init();

    while(1)
    {
        Event_Handler(State_Machine, system_state);
    }
}

void EUSCIA2_IRQHandler(void)
{
    if (EUSCI_A2->IFG & EUSCI_A_IFG_RXIFG)
    {
//        Turn_On(LED_Red);
        int8_t ch = EUSCI_A2->RXBUF;
        if(State_Machine->state == sReceive_Data)
        {
            system_state->RX_Flag = 1;
            if(ch == 'R')
            {
                system_state->Query_Received = 1;
            }
            else if(ch == EOF)
            {
                system_state->RX_Complete = 1;
            }
            else
            {
                buffer[system_state->RX_Index] = ch;
                system_state->RX_Index +=1;
            }
        }
    }
}

void PORT1_IRQHandler(void)
{
    volatile uint32_t i;

    // Toggling the output on the LED
    if(P1->IFG & BIT1)
    {
        // Delay for switch debounce
        for(i = 0; i < 10000; i++)

        P1->IFG &= ~BIT1;

        system_state->Button_Pressed_Flag = 1;
    }
}

