


#ifndef STATE_MACHINE_STATE_MACHINE_H_
#define STATE_MACHINE_STATE_MACHINE_H_
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "led_control.h"
#include "isp_commands.h"
#include "hex_parse.h"

typedef enum {
    eStart,
    eRX_Start,
    eRX_Finish,
    eProcessing_Done,
    eChecksum_Error,
    eCRC_Error,
    eButton_Pressed,
    eChip_Erased,
    eProgramming,
    eProgramming_Done,
    eVerify,
    eVerify_Successful,
    eVerify_Failed,
    eError,
} event_t;

typedef enum {
    sReceive_Data,
    sProcess_Data,
    sProgramming_Ready,
    sProgramming,
    sVerify,
    sChip_Erase,
    sError,
} state_t;

typedef enum {
    errNO_Error = 0x00,
    errSM_Error,
    errTarget_Error,
    errDisconnect,
} error_t;

typedef struct {
    state_t state;
    event_t event;
} state_machine_t;

typedef struct {
    uint8_t reception_ready;
    error_t error_flag;
    uint8_t button_pressed;
    uint8_t command_write;
} full_system_state_t;

void End_Program(void);
state_machine_t* State_Machine_Init(void);
void State_Machine_End(state_machine_t* sm);
void Set_Event(state_machine_t* sm, event_t event);
void Set_State(state_machine_t* sm, state_t state);
void Event_Handler(state_machine_t* sm, full_system_state_t* full_system);

#endif /* STATE_MACHINE_H_ */
