/*
 * File - common.h
 * Brief -  Contains all the system wide variables and libraries
 * Author - Abhijeet Dutt Srivastava
 * University of Colorado Boulder
 * Embedded System Design
 */

#ifndef COMMON_H_
#define COMMON_H_

#include "ti/devices/msp432p4xx/inc/msp.h"


extern volatile uint8_t buffer[8400]; //before testing it was int8_t
extern volatile uint16_t ISR_index;


typedef enum {
    errNO_Error = 0x00,
    errSM_Error = 0x01,
    errDisconnect = 0x02,
} error_t;


typedef struct {
    uint8_t reception_ready;
    error_t error_flag;
    uint8_t button_pressed;
    uint8_t PC_RX_Flag;
    uint8_t Checksum_Failure;
    uint8_t CRC_Failure;
    uint8_t RX_Complete;
    uint8_t response;
    uint8_t RX_Response;
    uint8_t TX_Flag;
    uint8_t All_Ok;

} full_system_state_t;

extern full_system_state_t* system_status;

#endif /* COMMON_H_ */

