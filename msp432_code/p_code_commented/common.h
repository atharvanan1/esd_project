/*
 * File - common.h
 * Brief -  Contains all the system wide variables and libraries
 * Author - Atharva Nandanwar
 * University of Colorado Boulder
 * Embedded System Design
 */

#ifndef COMMON_H_
#define COMMON_H_

#include "msp.h"
#include "led_control.h"


//Enum for indicating various errors

typedef enum {
    errNO_Error = 0x00,
    errSM_Error,
    errTarget_Error,
    errDisconnect,
} error_t;


// Struct for all the flags and errors associated with the state machine

typedef struct {
    error_t error_flag;
    uint8_t Query_Received;
    uint8_t Button_Pressed_Flag;
    uint8_t Checksum_Error;
    uint8_t CRC_Error;
    uint8_t RX_Complete;
    uint8_t RX_Flag;
    uint8_t Verification_Successful;
    volatile uint16_t RX_Index;
} full_system_state_t;

extern full_system_state_t* system_state;
extern int8_t buffer[8400];

#endif /* COMMON_H_ */
