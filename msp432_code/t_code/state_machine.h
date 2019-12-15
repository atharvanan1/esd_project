/*
 * File - state_machine.h
 * Brief -  Contains all the declarations for the state machine related functions
 * Author - Abhijeet Dutt Srivastava
 * University of Colorado Boulder
 * Embedded System Design
 */

#ifndef STATE_MACHINE_STATE_MACHINE_H_
#define STATE_MACHINE_STATE_MACHINE_H_
//#include <stdint.h>
//#include <stdlib.h>
//#include "led_control.h"
//#include "isp_commands.h"
//#include "hex_parse.h"

#include "stdlib.h"
#include "common.h"
#include "UART.h"

typedef enum {
    eStart,
    eRX_Start,
    eRX_Finish,
    eProcessing_Start,
    eProcessing_Finish,
    eProgrammer_Ready,
    eTransmit,
    eTransmit_Complete,
    eCRC_Error,
    ePROGRAMMING_DONE,
    eChecksum_Error,
    eError
} event_t;

typedef enum {
    sPC_Reception,
    sProcess_Data,
    sQuery_Programmer,
    sTransmission,
    sCheck_For_Validity,
    sError,

} state_t;


typedef struct {
    state_t state;
    event_t event;
} state_machine_t;
extern state_machine_t *state_machine;

//typedef struct {
//    uint8_t reception_ready;
//    error_t error_flag;
//    uint8_t button_pressed;
//    uint8_t PC_RX_Flag;
//    uint8_t Checksum_Failure;
//    uint8_t CRC_Failure;
//    uint8_t RX_Complete;
//    uint8_t response;
//    uint8_t CRC_Complete;
//    uint8_t RX_Response;
//    uint8_t TX_Flag;
//
//} full_system_state_t;

void End_Program(void);
state_machine_t* State_Machine_Init(void);
void State_Machine_End(state_machine_t* sm);
void Set_Event(state_machine_t* sm, event_t event);
void Set_State(state_machine_t* sm, state_t state);
void Event_Handler(state_machine_t* sm, full_system_state_t* full_system);

#endif /* STATE_MACHINE_H_ */
