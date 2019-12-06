/*
 * led_control.c
 *
 *  Created on: Dec 5, 2019
 *      Author: athar
 */

#include "led_control.h"

void LED_Init(void)
{
    // Initialize all the LEDs with output low
    P1->OUT &= ~BIT0;
    P2->OUT &= ~(BIT0 | BIT1 | BIT2);

    // Set them as output
    P1->DIR |= BIT0;
    P2->DIR |= BIT0 | BIT1 | BIT2;
}

void Turn_On(led_t LED)
{
    if(LED == LED_Red)
        P1->OUT |= BIT0;
    else if(LED == LED_RGB_R)
        P2->OUT |= BIT0;
    else if(LED == LED_RGB_G)
        P2->OUT |= BIT1;
    else if(LED == LED_RGB_B)
        P2->OUT |= BIT2;
}

void Turn_Off(led_t LED)
{
    if(LED == LED_Red)
        P1->OUT &= ~BIT0;
    else if(LED == LED_RGB_R)
        P2->OUT &= ~BIT0;
    else if(LED == LED_RGB_G)
        P2->OUT &= ~BIT1;
    else if(LED == LED_RGB_B)
        P2->OUT &= ~BIT2;
}
