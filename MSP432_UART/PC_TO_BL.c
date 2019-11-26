//ESD LAB3
//Name: Abhijeet Dutt Srivastava
//PROGRAM: To Produce a serial loopback
//REFERENCE: MSP432 examples on UART

//******************************************************************************
#include "ti/devices/msp432p4xx/inc/msp.h"
#include "stdint.h"

uint8_t RX_data = 0;
uint8_t BT_RX_data = 0;
uint8_t FLAG = 0;
int main(void)
{
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;      // Stop watchdog timer

    CS->KEY = CS_KEY_VAL;                   // Unlock CS module for register access
    CS->CTL0 = 0;                           // Reset tuning parameters
    CS->CTL0 = CS_CTL0_DCORSEL_3;           // Set DCO to 12MHz (nominal, center of 8-16MHz range)
    CS->CTL1 = CS_CTL1_SELA_2 |             // Select ACLK = REFO
            CS_CTL1_SELS_3 |                // SMCLK = DCO
            CS_CTL1_SELM_3;                 // MCLK = DCO
    CS->KEY = 0;                            // Lock CS module from unintended accesses

/******Configure UART pins*****/
    // FOR PC Side Pins
    P1->SEL0 |= BIT2 | BIT3;                // set 2-UART pin as secondary function

    // For HC-05 side pins
    //P3->SEL1 &= ~(BIT2 | BIT3);                // Setup P3.2 and P3.2 as TX pins to bluetooth module
    P3->SEL0 |= BIT2 | BIT3;
    // From Datasheet page 141


    // DOUBT
    // P1-> SEL0 &= ~(BIT2 | BIT3);         // secondary module function selected
    // P1-> SEL1 |= (BIT2 | BIT3);            // secondary module function selected

    // Used for ISR checking
    P1->DIR |= BIT0;                        // RED LED PIN
    P1->OUT &= ~BIT0;

    P2->DIR |= BIT1;                        // GREEN LED PIN
    P2->OUT &= ~BIT1;

    // Configure UART A0
    EUSCI_A0->CTLW0 |= EUSCI_A_CTLW0_SWRST; // Put eUSCI in reset
    EUSCI_A0->CTLW0 = EUSCI_A_CTLW0_SWRST | // Remain eUSCI in reset
            EUSCI_B_CTLW0_SSEL__SMCLK;      // Configure eUSCI clock source for SMCLK
    EUSCI_A0->BRW = 78;                     // 12000000/16/115200
        EUSCI_A0->MCTLW = (2 << EUSCI_A_MCTLW_BRF_OFS) |
                EUSCI_A_MCTLW_OS16;


        EUSCI_A2->CTLW0 |= EUSCI_A_CTLW0_SWRST; // Put eUSCI in reset
    EUSCI_A2->CTLW0 = EUSCI_A_CTLW0_SWRST | // Remain eUSCI in reset
            EUSCI_B_CTLW0_SSEL__SMCLK;
    EUSCI_A2->BRW = 78;                     // 12000000/16/115200
    EUSCI_A2->MCTLW = (2 << EUSCI_A_MCTLW_BRF_OFS) |
            EUSCI_A_MCTLW_OS16;
        // Baud Rate calculation
        // 12000000/(16*9600) = 78.125
        // Fractional portion = 0.125
        // User's Guide Table 21-4: UCBRSx = 0x10
        // UCBRFx = int ( (78.125-78)*16) = 2




    /********************************************************
 * Initialise CTLW0
 ********************************************************/

//    EUSCI_A0->CTLW0 = EUSCI_A_CTLW0_SWRST &
//            ~(EUSCI_A_CTLW0_PEN) & // Disable Parity
//            ~(EUSCI_A_CTLW0_PAR) & // doesn't matter as parity is disabled
//            ~(EUSCI_A_CTLW0_MSB) & // LSB first
//            ~(EUSCI_A_CTLW0_SEVENBIT) & // 8 bit data
//            (EUSCI_A_CTLW0_MODE_0) & // UART mode
//            ~(EUSCI_A_CTLW0_SPB) | // One stop bit
//            ~(EUSCI_A_CTLW0_TXBRK) | // No break
//            ~(EUSCI_A_CTLW0_TXADDR); // Next frame is data

/********************************************************
     * Initialise CTLW0
********************************************************/

    // UART A0 INTERRUPT SETUP
    EUSCI_A0->CTLW0 &= ~EUSCI_A_CTLW0_SWRST; // Initialize eUSCI

    EUSCI_A0->IFG &= ~EUSCI_A_IFG_RXIFG;    // Clear eUSCI RX interrupt flag
    EUSCI_A0->IE |= EUSCI_A_IE_RXIE;        // Enable USCI_A0 RX interrupt
    EUSCI_A0->IE |= EUSCI_A_IE_TXIE;  // Enable USCI_A0 TX interrupt

    // UART A2 INTERRUPT SETUP
    EUSCI_A2->CTLW0 &= ~EUSCI_A_CTLW0_SWRST; // Initialize eUSCI
    //EUSCI_A2->IFG &= ~EUSCI_A_IFG_TXIFG;// clear TX Flag

    // Enable global interrupt
    __enable_irq();

    // Enable eUSCIA0 interrupt in NVIC module
    NVIC->ISER[0] = 1 << ((EUSCIA0_IRQn) & 31);
    // Enable eUSCIA2 interrupt in NVIC module
    //NVIC->ISER[0] = 1 << ((EUSCIA2_IRQn) & 31);

    // Enable sleep on exit from ISR
    //SCB->SCR |= SCB_SCR_SLEEPONEXIT_Msk;

    // Ensures SLEEPONEXIT occurs immediately
   // __DSB();

    // Enter LPM0
    //__sleep();

    while(1)
    {
        if(FLAG==1)
        {
        FLAG=0;
        while(!(EUSCI_A2->IFG & EUSCI_A_IFG_TXIFG));
        EUSCI_A2->TXBUF = RX_data;
        P1->OUT ^= BIT0;
        }

        __no_operation();                       // For debugger
    }
}

// UART interrupt service routine
void EUSCIA0_IRQHandler(void)
{
    if (EUSCI_A0->IFG & EUSCI_A_IFG_RXIFG) //& EUSCI_A_IFG_TXIFG
    {
        // Check if the TX buffer is empty first
        //P1->OUT |= BIT0;
            RX_data = EUSCI_A0->RXBUF;
            EUSCI_A0->IFG &=~ EUSCI_A_IFG_RXIFG;// Clear interrupt
            FLAG = 1;
            //while(!(EUSCI_A2->IFG & EUSCI_A_IFG_TXIFG));
            while(!(EUSCI_A0->IFG & EUSCI_A_IFG_TXIFG));
        //P2->OUT |= BIT1;
        // Echo the received character back
        //EUSCI_A0->TXBUF = 'A';           //EUSCI_A0->RXBUF;
            EUSCI_A0->TXBUF = RX_data;

            // GREEN LED TOGGLES
            P2->OUT ^= BIT1;
            //EUSCI_A2->TXBUF = RX_data;
    }
}


//void EUSCIA2_IRQHANDLER(void)
//{
//        // RED LED TOGGLES
//        //P1->OUT = P1->OUT ^ 1;
//        P1->OUT ^= BIT0;
//        while(!(EUSCI_A2->IFG & EUSCI_A_IFG_TXIFG));
//        EUSCI_A2->TXBUF = RX_data;
//}


