#ifndef STATE_MACHINE_STATE_MACHINE_H_
#define STATE_MACHINE_STATE_MACHINE_H_
#include <stdint.h>
#include <stdlib.h>
#include "led_control.h"
#include "isp_commands.h"
#include "hex_parse.h"

typedef enum {
    eStart,
    ePC_RX_Start,
    ePC_RX_Finish,
    eProcessing_Finish,
    eProgrammer_Ready,
    eTransmit_Complete,
    eCRC_Error,
    eAll_OK,
    eChecksum_Error,
} event_t;

typedef enum {
    sPC_Reception_Wait,
    sPC_Reception,
    sProcess_Data,
    sQuery_Programmer,
    sTransmission,
    sCheck_For_Validity,
    sError,
} state_t;

typedef enum {
    errNO_Error = 0x00,
    errSM_Error = 0x01,
    errDisconnect = 0x02,
} error_t;

typedef struct {
    state_t state;
    event_t event;
} state_machine_t;

typedef struct {
    uint8_t reception_ready;
    error_t error_flag;
    uint8_t button_pressed;
} system_state_t;

void End_Program(void);
state_machine_t* State_Machine_Init(void);
void State_Machine_End(state_machine_t* sm);
void Set_Event(state_machine_t* sm, event_t event);
void Set_State(state_machine_t* sm, state_t state);
void Event_Handler(state_machine_t* sm, system_state_t* system);

#endif /* STATE_MACHINE_H_ */
