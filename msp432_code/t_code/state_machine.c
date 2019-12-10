/*
 * state_machine.c
 *
 *  Created on: 07-Dec-2019
 *      Author: Hp
 */

#include "state_machine.h"



void UART_printf(char *str)
{
    while(*str!='\0')
    {
        while(!(EUSCI_A0->IFG & EUSCI_A_IFG_TXIFG));
        EUSCI_A0->TXBUF = *(str++);

        //str++;
    }
}

/*
 * Function - End_Program
 * Brief - ends the program by going into infinite loop
 */
void End_Program(void)
{
    while(1);
}

/*
 * Function - State_Machine_Init
 * Brief - Initializes state machine
 * Arguments -
 * state_machine_type -> State-driven or Table-driven state machine
 * Return -
 * returns a pointer to state machine struct
 */
state_machine_t* State_Machine_Init()
{
    state_machine_t* sm = (state_machine_t *) malloc(sizeof(state_machine_t));

    if(sm == NULL)
    {
        return NULL;
    }

    sm->state = sPC_Reception;
    sm->event = eStart;

    return sm;
}

/*
 * Function - State_Machine_End
 * Brief - Ends state machine
 * Arguments -
 * pointer to state_machine struct
 */
void State_Machine_End(state_machine_t* sm)
{
    if(sm == NULL)
        return;

    free(sm);
    sm = NULL;
}

/*
 * Function - Set_Event
 * Brief - Sets events
 * Arguments -
 * state_machine struct -> which state_machine
 * event -> which event to set the state_machine to
 */
void Set_Event(state_machine_t* sm, event_t event)
{
    sm->event = event;
}

/*
 * Function - Set_State
 * Brief - Sets state
 * Arguments -
 * state_machine struct -> which state_machine
 * state -> which state to set the state_machine to
 */
void Set_State(state_machine_t* sm, state_t state)
{
    sm->state = state;
}





/*
 *
 * State machine state functions
 *
 */

static inline void Receive_Data_State(state_machine_t* sm, full_system_state_t* full_system)
{
    if(sm->event==eStart)
        {

        UART_printf("Paste the program data from the hex file\n\r");

            __DSB();
               __sleep();
            // If User sends and data, machine is in this state
            while(full_system->PC_RX_Flag == 0);
            Set_Event(sm, eRX_Start);
            full_system->PC_RX_Flag = 0;

        }

    else if(sm->event==eRX_Start)
        {
        UART_printf("Input '@' when done\n\r");
        // When the entire program is received RX_Complete is set
        while(full_system->RX_Complete == 0);
        Set_Event(sm, eRX_Finish);
        full_system->RX_Complete = 0;
        }
    else if(sm->event==eRX_Finish)
        {
        UART_printf("c");
            Set_State(sm, sProcess_Data);
        }
    else
        {
        UART_printf("Receive data State : Event else - error\n\r");
            Set_Event(sm, eError);
            Set_State(sm, sError);
        }

}

static inline void  Process_Data_State(state_machine_t* sm, full_system_state_t* full_system)
{
    if(sm->event == eRX_Finish)
        {
        UART_printf("d");
        Set_Event(sm, eProcessing_Start);


        }
    else if(sm->event == eProcessing_Start)
        {
        //enable CRC-Flag and start processing
        SD_CRC_Processing(buffer, ISR_index);
        UART_printf("Process data State : Event eProcessing_Start\n\r");
        Set_Event(sm, eProcessing_Finish);
        }
    else if(sm->event == eProcessing_Finish)
        {
        UART_printf("Process data State : Event eProcesing_Finish\n\r");
        Set_State(sm, sQuery_Programmer);
        }
    else
        {
        UART_printf("Process data State : Else - Error \n\r");
            Set_Event(sm, eError);
            Set_State(sm, sError);
        }

}

static inline void  Query_Programmer_State(state_machine_t* sm, full_system_state_t* full_system)
{
    if(sm->event == eProcessing_Finish)
        {
        // Send query to slave and check response
        // then chnage even to ready
        while(!(EUSCI_A2->IFG & EUSCI_A_IFG_TXIFG));
        EUSCI_A2->TXBUF = 'R';

        UART_printf("Query Programmer State : eProcessing_Finish\n\r");
        while(full_system->RX_Response == 0);
        full_system->RX_Response = 0;
        Set_Event(sm, eProgrammer_Ready);
        }
    else if(sm->event == eProgrammer_Ready)
        {
        //query programmer and wait for response
        UART_printf("Query Programmer State : eProgrammer_Ready\n\r");

        Set_State(sm, sTransmission);


        }
    else
        {
        UART_printf("Query Programmer State : ELSE - Error\n\r");
            Set_Event(sm, eError);
            Set_State(sm, sError);
        }




}

static inline void  Transmission(state_machine_t* sm, full_system_state_t* full_system)
{

    if(sm->event == eProgrammer_Ready)
        {
        UART_printf("Transmission State : eProgrammer_Ready\n\r");
        Set_Event(sm, eTransmit);

        }
    else if(sm->event == eTransmit)
    {
        // Transmit actions here
        // Change event after transmit is complete

        UART_printf("Transmission State : eTransmit\n\r");
        SD_Transmission(&ISR_index, buffer);
        // polling transmission here
        Set_Event(sm, eTransmit_Complete);
    }
    else if(sm->event == eTransmit_Complete)
        {
        UART_printf("Transmission State : eTransmit_Complete\n\r");
        Set_State(sm, sCheck_For_Validity);

        }
    else
        {
        UART_printf("Transmission State : ELSE - Error\n\r");
        Set_Event(sm, eError);
        Set_State(sm, sError);
        }

}

static inline void  Wait_For_Validity_State(state_machine_t* sm, full_system_state_t* full_system)
{
    if(sm->event == eTransmit_Complete)
        {
//        UART_printf("Wait_For_Validity State : eTransmit_Complete\n\r");

        // Poll RX here
        // Add conditions for your Errors

        //Wait for response from programmer
            if(full_system->CRC_Failure == 1)
                {
                    full_system->CRC_Failure = 0;
                    Set_Event(sm, eCRC_Error);
                }
            else if(full_system->Checksum_Failure ==1)
                {
                    full_system->Checksum_Failure = 0;
                    Set_Event(sm, eChecksum_Error);
                }
            else if(full_system->All_Ok == 1)
                {

                    full_system->All_Ok = 0;
                    Set_Event(sm, ePROGRAMMING_DONE);
                }
            if(full_system->error_flag == errSM_Error)
                {

                    full_system->error_flag = errNO_Error;
                    Set_State(sm, sError);
                }
        }

    else if(sm->event == eCRC_Error)
        {
//        UART_printf("Wait_For_Validity State : eCRC_Error\n\r");

        Set_State(sm, sError);

        }
    else if(sm->event == eChecksum_Error)
        {
//        UART_printf("Wait_For_Validity State : eChecksum_Error\n\r");
        Set_Event(sm, eStart);
        Set_State(sm, sPC_Reception);
        }
    else if(sm->event == ePROGRAMMING_DONE)
        {
        UART_printf("Wait_For_Validity State : eProgramming_Done\n\r");
        Set_Event(sm, eStart);
        Set_State(sm, sPC_Reception);
        }
    else
        {
//        UART_printf("Wait_For_Validity State : ELSE - Error\n\r");
        Set_Event(sm, eError);
        Set_State(sm, sError);
        }
}

static inline void Error_State(state_machine_t* sm, full_system_state_t* full_system)
{
    P1->OUT |= BIT0;
    while(1);
}

/*
 * Function - Event_Handler
 * Brief - handles the events related to state machines
 * Arguments -
 * state_machine struct -> which state_machine
 * system_state struct -> a pointer to full_system state global struct
 */
void Event_Handler(state_machine_t* sm, full_system_state_t* full_system)
{
    // If state machine doesn't exist, exit
    if (sm == NULL)
    {
        full_system->error_flag = errSM_Error;
        exit(1);
    }

    // Check for state
    switch(sm->state)
    {
    case sPC_Reception:
        Receive_Data_State(sm,full_system);
        break;
    case sProcess_Data:
        Process_Data_State(sm, full_system);
        break;

    case sQuery_Programmer:
        Query_Programmer_State(sm, full_system);
        break;

    case sTransmission:
        Transmission(sm, full_system);
        break;


    case sCheck_For_Validity:
        Wait_For_Validity_State(sm, full_system);
        break;


    case sError:
        Error_State(sm, full_system);
        break;
    }
}

