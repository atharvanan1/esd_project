/**
  * File Name       - state_machine.c
  * Description     - contains state machine subroutines
  * Author          - Atharva Nandanwar
  * Tools           - GNU C Compiler / ARM Compiler Toolchain
  * Leveraged Code  - https://github.com/EduMacedo99/FEUP-LCOM
  * URL             -
  */

#include "state_machine.h"

uint8_t Button_Pressed_Flag = 0;
uint8_t checksum_error = 0;
uint8_t crc_error = 0;
uint8_t RX_Complete = 0;
uint8_t RX_Flag = 0;
uint8_t Verification_Successful = 0;

void put_string(char *str)
{
    while(*str!='\0')
    {
        while(!(EUSCI_A0->IFG & EUSCI_A_IFG_TXIFG));
        EUSCI_A0->TXBUF = *(str++);
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

    sm->state = sReceive_Data;
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

/*-------------------------------*/
/* State Machine State Functions */
/*-------------------------------*/
static inline void Receive_Data_State(state_machine_t* sm, full_system_state_t* full_system)
{
    if(sm->event == eStart)
    {
//        __DSB();
//        __sleep();
        put_string("State: Receive Data; Event: Start\n\r");
        while(RX_Flag == 0);
        // Check if the Reception has started,
        // and change the event to RX_Start
        if(RX_Flag == 1)
        {
            Set_Event(sm, eRX_Start);
            RX_Flag = 0;
        }
    }
    else if(sm->event == eRX_Start)
    {
        put_string("State: Receive Data; Event: RX_Start\n\r");
        // Wait till Reception is finished
        while(RX_Complete == 0);
        // As it finishes, change the event
        RX_Complete = 0;
        Set_Event(sm, eRX_Finish);
    }
    else if(sm->event == eRX_Finish)
    {
        put_string("State: Receive Data; Event: RX_Finish\n\r");
        Set_State(sm, sProcess_Data);
    }
    else
    {
        Set_Event(sm, eError);
        Set_State(sm, sError);
    }
}

static inline void Process_Data_State(state_machine_t* sm, full_system_state_t* full_system)
{
    if(sm->event == eRX_Finish)
    {
        // Process Data here
        put_string("State: Process Data; Event: RX_Finish\n\r");
        checksum_error = 0;
        crc_error = 0;
        // Change the even at the end of it
        if(checksum_error)
        {
            checksum_error = 0;
            Set_Event(sm, eChecksum_Error);
        }
        else if(crc_error)
        {
            crc_error = 0;
            Set_Event(sm, eCRC_Error);
        }
        else
        {
            Set_Event(sm, eProcessing_Done);
        }
    }
    else if(sm->event == eProcessing_Done)
    {
        put_string("State: Process Data; Event: Processing Done\n\r");
        Set_State(sm, sProgramming_Ready);
    }
    else if(sm->event == eChecksum_Error)
    {
        put_string("State: Process Data; Event: Checksum Error\n\r");
        // Send message to Transmitter
        Set_State(sm, sReceive_Data);
        Set_Event(sm, eStart);
    }
    else if(sm->event == eCRC_Error)
    {
        put_string("State: Process Data; Event: CRC Error\n\r");
        // Send message to Transmitter
        Set_State(sm, sReceive_Data);
        Set_Event(sm, eStart);
    }
    else
    {
        Set_Event(sm, eError);
        Set_State(sm, sError);
    }
}

static inline void Programming_Ready_State(state_machine_t* sm, full_system_state_t* full_system)
{
    if(sm->event == eProcessing_Done)
    {
        put_string("State: Programming Ready; Event: Processing Done\n\r");
        // Wait here till button is pressed
        while(Button_Pressed_Flag != 1);

        Button_Pressed_Flag = 0;
        // Change event when button is pressed
        Set_Event(sm, eButton_Pressed);
    }
    else if(sm->event == eButton_Pressed)
    {
        put_string("State: Programming Ready; Event: Button Pressed\n\r");
        Set_State(sm, sChip_Erase);
    }
    else
    {
        Set_Event(sm, eError);
        Set_State(sm, sError);
    }
}

static inline void Chip_Erase_State(state_machine_t* sm, full_system_state_t* full_system)
{
    if(sm->event == eButton_Pressed)
    {
        // Conduct a chip erase action
        put_string("State: Chip Erase; Event: Button Pressed\n\r");
        // Change event
        Set_Event(sm, eChip_Erased);
    }
    else if(sm->event == eVerify_Failed)
    {
        // Conduct a chip erase action
        put_string("State: Chip Erase; Event: Verify Fail\n\r");
        // Change event
        Set_Event(sm, eChip_Erased);
    }
    else if(sm->event == eChip_Erased)
    {
        put_string("State: Chip Erase; Event: Chip Erased\n\r");
        Set_State(sm, sProgramming);
    }
    else
    {
        Set_Event(sm, eError);
        Set_State(sm, sError);
    }
}

static inline void Programming_State(state_machine_t* sm, full_system_state_t* full_system)
{
    if(sm->event == eChip_Erased)
    {
        put_string("State: Programming; Event: Chip Erased\n\r");
        Set_Event(sm, eProgramming);
    }
    else if(sm->event == eProgramming)
    {
        // Do programming actions
        put_string("State: Programming; Event: Programming\n\r");
        // Change the event to programming at the end
        Set_Event(sm, eProgramming_Done);
    }
    else if(sm->event == eProgramming_Done)
    {
        put_string("State: Programming; Event: Programming Done\n\r");
        Set_State(sm, sVerify);
    }
    else
    {
        Set_Event(sm, eError);
        Set_State(sm, sError);
    }
}

static inline void Verify_State(state_machine_t* sm, full_system_state_t* full_system)
{
    if(sm->event == eProgramming_Done)
    {
        put_string("State: Verify; Event: Programming Done\n\r");
        Set_Event(sm, eVerify);
    }
    else if(sm->event == eVerify)
    {
        // Verify Memory Contents
        put_string("State: Verify; Event: Verify\n\r");

        Verification_Successful = 1;

        // Change event as per verification result
        if(Verification_Successful)
        {
            Set_Event(sm, eVerify_Successful);
        }
        else
        {
            Set_Event(sm, eVerify_Failed);
        }
    }
    else if(sm->event == eVerify_Successful)
    {
        put_string("State: Verify; Event: Verify Successful\n\r");
        // Send Programming Done message to Transmitter

        Set_Event(sm, eStart);
        Set_State(sm, sReceive_Data);
    }
    else if(sm->event == eVerify_Failed)
    {
        put_string("State: Verify; Event: Verify Failed\n\r");
        // Do chip erase based  restart 3 times and then go to error
        Set_State(sm, sChip_Erase);
    }
    else
    {
        Set_Event(sm, eError);
        Set_State(sm, sError);
    }
}

static inline void Error_State(state_machine_t* sm, full_system_state_t* full_system)
{
    put_string("State: Error\n\r");
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
    // State - Receive Data
    case sReceive_Data:
        Receive_Data_State(sm, full_system);
        break;

    // State - Process Data
    case sProcess_Data:
        Process_Data_State(sm, full_system);
        break;

    // State - Programming Ready
    case sProgramming_Ready:
        Programming_Ready_State(sm, full_system);
        break;

    // State - Chip Erase
    case sChip_Erase:
        Chip_Erase_State(sm, full_system);
        break;

    // State - Programming
    case sProgramming:
        Programming_State(sm, full_system);
        break;

    // State - Verify
    case sVerify:
        Verify_State(sm, full_system);
        break;

    // State - Error
    case sError:
        Error_State(sm, full_system);
        break;
    default:
        Error_State(sm, full_system);
        break;
    }
}
