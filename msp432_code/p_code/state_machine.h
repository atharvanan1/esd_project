

#ifndef STATE_MACHINE_STATE_MACHINE_H_
#define STATE_MACHINE_STATE_MACHINE_H_
#include <stdint.h>
#include <stdlib.h>
#include "led_control.h"
#include "isp_commands.h"
#include "hex_parse.h"

typedef enum {
    eStart,
    eQuery_Reception,
    eData_Reception,
    eEnd_Of_Data_Reception,
    eProcessing_Finish,
    eProgram_Button_Pressed,
    eProgrammingDone,
    eVerifyFail,
    eVerifySuccess,
    eCRC_Error,
    eChecksum_Error,
    eError,
} event_t;

typedef enum {
    sReception_Wait,
    sReceive,
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
} system_state_t;

void End_Program(void);
state_machine_t* State_Machine_Init(void);
void State_Machine_End(state_machine_t* sm);
void Set_Event(state_machine_t* sm, event_t event);
void Set_State(state_machine_t* sm, state_t state);
void Event_Handler(state_machine_t* sm, system_state_t* system);

#endif /* STATE_MACHINE_H_ */
