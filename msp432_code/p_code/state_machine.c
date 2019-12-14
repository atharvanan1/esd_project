/**
  * File Name       - state_machine.c
  * Description     - contains state machine subroutines
  * Author          - Atharva Nandanwar
  * Tools           - GNU C Compiler / ARM Compiler Toolchain
  * Leveraged Code  - https://github.com/EduMacedo99/FEUP-LCOM
  * URL             -
  */

#include "state_machine.h"

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

static inline void State_Init(state_machine_t* sm, full_system_state_t* full_system)
{
    Turn_Off(LED_Red);
    Turn_Off(LED_RGB_R);
    Turn_Off(LED_RGB_G);
    Turn_Off(LED_RGB_B);
    Turn_On(LED_RGB_G);

    for(uint16_t index = 0; index < 8400; index++)
        buffer[index] = 0;

    full_system->error_flag = errNO_Error;
    full_system->Query_Received = 0;
    full_system->Button_Pressed_Flag = 0;
    full_system->Checksum_Error = 0;
    full_system->CRC_Error = 0;
    full_system->RX_Complete = 0;
    full_system->RX_Flag = 0;
    full_system->Verification_Successful = 0;
    full_system->RX_Index = 0;
}

static inline void Receive_Data_State(state_machine_t* sm, full_system_state_t* full_system)
{
    if(sm->event == eStart)
    {
        State_Init(sm, full_system);

        __DSB();
        __sleep();

        full_system->Query_Received = 0;
        bt_send_string("S");

        // Check if the Reception has started,
        // and change the event to RX_Start
        if(full_system->RX_Flag == 1)
        {
            Set_Event(sm, eRX_Start);
            full_system->RX_Flag = 0;
        }
    }
    else if(sm->event == eRX_Start)
    {
        // Wait till Reception is finished
        while(full_system->RX_Complete == 0);

        // As it finishes, change the event
        // Reset the flag
        full_system->RX_Complete = 0;
        Set_Event(sm, eRX_Finish);
    }
    else if(sm->event == eRX_Finish)
    {
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
        full_system->Checksum_Error = 0;
        full_system->CRC_Error = 0;

        // Process Data here
        if(CRC_Check((uint8_t *)buffer, &full_system->RX_Index) == CRC_Error)
        {
            full_system->CRC_Error = 1;
        }

        hex_parse(buffer, full_system->Checksum_Error);

        // Change the even at the end of it
        if(full_system->Checksum_Error)
        {
            Turn_On(LED_Red);
            delay(5000);
            full_system->Checksum_Error = 0;
            Set_Event(sm, eChecksum_Error);
        }
        else if(full_system->CRC_Error)
        {
            Turn_On(LED_Red);
            delay(5000);
            full_system->CRC_Error = 0;
            Set_Event(sm, eCRC_Error);
        }
        else
        {
            Set_Event(sm, eProcessing_Done);
        }
    }
    else if(sm->event == eProcessing_Done)
    {
        Set_State(sm, sProgramming_Ready);
    }
    else if(sm->event == eChecksum_Error)
    {
        bt_send_string("f");
        Set_State(sm, sReceive_Data);
        Set_Event(sm, eStart);
    }
    else if(sm->event == eCRC_Error)
    {
        bt_send_string("F");
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
        Turn_Off(LED_RGB_G);
        Turn_On(LED_RGB_B);
        // Wait here till button is pressed
        while(full_system->Button_Pressed_Flag != 1);

        full_system->Button_Pressed_Flag = 0;
        // Change event when button is pressed
        Set_Event(sm, eButton_Pressed);
    }
    else if(sm->event == eButton_Pressed)
    {
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
        programming_enable();
        chip_erase();
        // Change event
        Set_Event(sm, eChip_Erased);
    }
    else if(sm->event == eVerify_Failed)
    {
        // Conduct a chip erase action
        programming_enable();
        chip_erase();
        // Change event
        Set_Event(sm, eChip_Erased);
    }
    else if(sm->event == eChip_Erased)
    {
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
        Set_Event(sm, eProgramming);
    }
    else if(sm->event == eProgramming)
    {
        // Do programming actions
        programming_enable();
        while(poll_busy() == ispBUSY);
        program();
        // Change the event to programming at the end
        Set_Event(sm, eProgramming_Done);
    }
    else if(sm->event == eProgramming_Done)
    {
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
        Set_Event(sm, eVerify);
    }
    else if(sm->event == eVerify)
    {
        // Verify Memory Contents
        isp_status_t Verification_Status = verify();

        if(Verification_Status == ispVERIFYPASS)
            full_system->Verification_Successful = 1;
        else
            full_system->Verification_Successful = 0;

        // Change event as per verification result
        if(full_system->Verification_Successful)
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
        // Send Programming Done message to Transmitter
        bt_send_string("C");
        Reset_High();

        Set_Event(sm, eStart);
        Set_State(sm, sReceive_Data);
    }
    else if(sm->event == eVerify_Failed)
    {
        Turn_Off(LED_RGB_B);
        Turn_On(LED_RGB_R);
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
    bt_send_string("E");
    Turn_On(LED_Red);
    while(1){
        Turn_Off(LED_Red);
        Turn_Off(LED_RGB_R);
        Turn_Off(LED_RGB_G);
        Turn_Off(LED_RGB_B);
        delay(1000);
        Turn_On(LED_Red);
        Turn_On(LED_RGB_R);
        Turn_On(LED_RGB_G);
        Turn_On(LED_RGB_B);
        delay(1000);
    }
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
