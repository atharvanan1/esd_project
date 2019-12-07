/*****************************************************************************
*
* Copyright (C) 2013 - 2017 Texas Instruments Incorporated - http://www.ti.com/
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions
* are met:
*
* * Redistributions of source code must retain the above copyright
*   notice, this list of conditions and the following disclaimer.
*
* * Redistributions in binary form must reproduce the above copyright
*   notice, this list of conditions and the following disclaimer in the
*   documentation and/or other materials provided with the
*   distribution.
*
* * Neither the name of Texas Instruments Incorporated nor the names of
*   its contributors may be used to endorse or promote products derived
*   from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
* "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
* LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
* A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
* OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
* SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
* LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
* DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
* THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
* OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
******************************************************************************
*
* MSP432 empty main.c template
*
******************************************************************************/

#include "main.h"

//state_machine_t* StateMachine = NULL;
//system_state_t system = {/*Reset values*/};
char* buffer = ":100000000C9434000C943E000C943E000C943E0082\n \
        :100010000C943E000C943E000C943E000C943E0068\n \
        :100020000C943E000C943E000C943E000C943E0058\n \
        :100030000C943E000C943E000C943E000C943E0048\n \
        :100040000C943E000C943E000C943E000C943E0038\n \
        :100050000C943E000C943E000C943E000C943E0028\n \
        :100060000C943E000C943E0011241FBECFEFD8E04C\n \
        :10007000DEBFCDBF0E9440000C9456000C940000DF\n \
        :10008000219A299A2FE38DE093E0215080409040FF\n \
        :10009000E1F700C0000029982FE38DE093E02150A4\n \
        :1000A00080409040E1F700C00000EBCFF894FFCF14\n \
        :00000001FF\n";
command_t commands[200];

void Init(void)
{
    WDT_A->CTL = WDT_A_CTL_PW |             // Stop WDT
               WDT_A_CTL_HOLD;
    LED_Init();
    ISP_Init();
//    StateMachine = State_Machine_Init();
}

int main(void)
{
    hex_file_t hex_file = {
                           495,
                           buffer
    };
    hex_parse(&hex_file);
    Init();
    programming_enable();
    chip_erase();
    while(poll_busy() == ispBUSY);
    programming_enable();
    program();
    P3->OUT |= BIT0;
    while(1);
}
