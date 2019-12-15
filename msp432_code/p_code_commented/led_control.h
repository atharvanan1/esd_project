/*
 * File - led_control.h
 * Brief -  Contains all the declarations for the led related functions
 * Author - Atharva Nandanwar
 * University of Colorado Boulder
 * Embedded System Design
 */

#ifndef LED_CONTROL_H_
#define LED_CONTROL_H_
#include "msp.h"

typedef enum {
    LED_Red,
    LED_RGB_R,
    LED_RGB_G,
    LED_RGB_B,
} led_t;

void LED_Init(void);
void Turn_On(led_t LED);
void Turn_Off(led_t LED);


#endif /* LED_CONTROL_H_ */
