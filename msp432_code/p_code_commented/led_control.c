/*
 * File - led_control.c
 * Brief -  Contains all the led related functions
 * Author - Atharva Nandanwar
 * University of Colorado Boulder
 * Embedded System Design
 */

#include "led_control.h"

/*
LED INITIALISATION CODE 

*/

void LED_Init(void)
{
    // Initialize all the LEDs with output low
    P1->OUT &= ~BIT0;
    P2->OUT &= ~(BIT0 | BIT1 | BIT2);

    // Set them as output
    P1->DIR |= BIT0;
    P2->DIR |= BIT0 | BIT1 | BIT2;
}

/*
************************************************************
Function Name: Turn_On
************************************************************
Purpose: Functions to control which led is turned on 
************************************************************
Arguments: enum to choose which led is turned on
************************************************************
Returns: NONE
************************************************************
*/

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

/*
************************************************************
Function Name: Turn_off
************************************************************
Purpose: Functions to control which led is turned off 
************************************************************
Arguments: enum to choose which led is turned off
************************************************************
Returns: NONE
************************************************************
*/

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
