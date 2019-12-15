/*
 * led_control.h
 *
 *  Created on: Dec 5, 2019
 *      Author: athar
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
