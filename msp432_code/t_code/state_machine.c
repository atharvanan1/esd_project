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

    sm->state = sReception_Wait;
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

static inline void Reception_Wait_State(state_machine_t* sm, system_state_t* system)
{
    if(system->error_flag != errNO_Error)
    {
        Set_Event(sm, eError);
    }

    if(sm->event == eStart)
    {
        // TODO: Add sleep state, where it will wait for UART RX Interrupt
        // Once that is done, eStart will do nothing but just wait for interrupt.
        // As soon as the interrupt comes, we will raise the flag, and start
        // collecting the data in... I think just shift to other state?
        // I would say wait here... go to sleep here.... :P

        // Code for waiting here
        // TODO: Figure out how to do it.
        ;
    }
    else if(sm->event == eReception_Start)
    {
        // No idea how I would integrate the reception code here.. but oh well
        ;
    }
    else if(sm->event == eEnd_Of_Reception)
    {
        // Do bunch of tasks like CRC unslapping, and organizing program data into
        // the commands by parsing the hex file that we just received.

        // By the end of it
        Set_State(sm, sProgram_Ready);
    }
    else if(sm->event == eProgram_Button_Pressed)
    {
        // Do nothing if this event occurs
        system->button_pressed = 0;
    }
    else if(sm->event == eError)
    {
        // Change to error state
        Set_State(sm, sError);
    }
}

static inline void Programming_Ready_State(state_machine_t* sm, system_state_t* system)
{
    if(system->error_flag != errNO_Error)
    {
        Set_Event(sm, eError);
    }

    if(sm->event == eStart)
    {
        // Do nothing
        ;
    }
    else if(sm->event == eReception_Start)
    {
        // Do nothing
        ;
    }
    else if(sm->event == eEnd_Of_Reception)
    {
        // Infinitely wait for someone to press the button,
        // Light up LED that I am in this state
        Turn_On(LED_RGB_G);
        while(!(system->button_pressed));
        system->button_pressed = 0;

        // By the end of it
        Set_Event(sm, eProgram_Button_Pressed);
    }
    else if(sm->event == eProgram_Button_Pressed)
    {
        // Change to programming state
        Set_State(sm, sProgram);
    }
    else if(sm->event == eError)
    {
        // Change to error state
        Set_State(sm, sError);
    }
}

static inline void Programming_State(state_machine_t* sm, system_state_t* system)
{
    if(system->error_flag != errNO_Error)
    {
        Set_Event(sm, eError);
    }

    if(sm->event == eStart)
    {
        // Do nothing
        ;
    }
    else if(sm->event == eReception_Start)
    {
        // Do nothing
        ;
    }
    else if(sm->event == eEnd_Of_Reception)
    {
        // Do nothing
        ;
    }
    else if(sm->event == eProgram_Button_Pressed)
    {
        // Do some programming from here...

        // By the end of it, change event to eStart and state to sReception_Wait
        Turn_Off(LED_RGB_G);
        Set_Event(sm, eStart);
        Set_State(sm, sReception_Wait);
    }
    else if(sm->event == eError)
    {
        // Change to error state
        Set_State(sm, sError);
    }
}

static inline void Error_State(state_machine_t* sm, system_state_t* system)
{
    if(sm->event == eError)
    {
        while(1)
        {
            // Do some LED blink
        }
    }
}

/*
 * Function - Event_Handler
 * Brief - handles the events related to state machines
 * Arguments -
 * state_machine struct -> which state_machine
 * system_state struct -> a pointer to system state global struct
 */
void Event_Handler(state_machine_t* sm, system_state_t* system)
{
    // If state machine doesn't exist, exit
    if (sm == NULL)
    {
        system->error_flag = errSM_Error;
        exit(1);
    }

    // Check for state
    switch(sm->state)
    {
    // State - Reception Wait
    case sReception_Wait:
        Reception_Wait_State(sm, system);
        break;
    // State - Ready for Programming
    case sProgram_Ready:
        Programming_Ready_State(sm, system);
        break;
    // State - Programming
    case sProgram:
        Programming_State(sm, system);
        break;
    // State - Error
    case sError:
        Error_State(sm, system);
        break;
    }
}
