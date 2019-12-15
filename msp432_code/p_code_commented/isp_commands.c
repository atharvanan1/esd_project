/*
 * File - isp_commands.c
 * Brief -  Contains all the ISP functions
 * Author - Atharva Nandanwar
 * University of Colorado Boulder
 * Embedded System Design
 */

#include "isp_commands.h"

/*---------------Init and Helper Functions------------*/
void ISP_Init(void)
{
    spi_init();

    // Initialize reset pin
    P3->OUT |= BIT0;
    P3->DIR |= BIT0;

    // Initialize P1.1 as input for programming button
    P1->DIR &= ~(uint8_t) BIT1;
    P1->OUT |= BIT1;
    P1->REN |= BIT1;                         // Enable pull-up resistor (P1.1 output high)
    P1->SEL0 &= ~BIT0;
    P1->SEL1 &= ~BIT0;
    P1->IES |= BIT1;                         // Interrupt on high-to-low transition
    P1->IFG = 0;                            // Clear all P1 interrupt flags
    P1->IE |= BIT1;                          // Enable interrupt for P1.1

    // Enable Port 1 interrupt on the NVIC
    NVIC->ISER[1] = 1 << ((PORT1_IRQn) & 31);
}

void delay(uint32_t time)
{
    time *= 300;
    for(volatile uint32_t index = 0; index < time; index++);
}

/*
************************************************************
Function Name: Reset_High and Reset_Low
************************************************************
Purpose: To enter and stay in Serial programming mode,
the AVR microcontroller reset line has to be kept active (low)
************************************************************
Arguments: NONE
************************************************************
Returns: NONE
************************************************************
*/

void Reset_High(void)
{
    P3->OUT |= BIT0;
}

void Reset_Low(void)
{
    P3->OUT &= ~BIT0;
}

/*
************************************************************
Function Name: command_execute
************************************************************
Purpose: To wrap spi write instruction and read instruction 
to write and read strings to and from the target board
************************************************************
Arguments: uint8_t* tx_buff, uint8_t* rx_buff
-array to be written and capture response from target in an array
************************************************************
Returns: NONE
************************************************************
*/

void command_execute(uint8_t* tx_buff, uint8_t* rx_buff)
{
    for(uint8_t index = 0; index < 4; index++)
    {
        spi_send(*(tx_buff + index));
        *(rx_buff + index) = spi_receive();
    }
}

/*
************************************************************
Function Name: program
************************************************************
Purpose: Wrapper for programming flash memory
************************************************************
Arguments: NONE
************************************************************
Returns: NONE
************************************************************
*/

void program(void)
{
    for(uint8_t index = 0; index < 200; index++)
    {
        program_flash(commands[index]);
    }
}

/*
************************************************************
Function Name: verify
************************************************************
Purpose: Wrapper for verifying flash memory
************************************************************
Arguments: NONE
************************************************************
Returns: enum indicating if the program was written to correctly
************************************************************
*/

isp_status_t verify(void)
{
    for(uint8_t index = 0; index < 200; index++)
    {
        if(verify_flash(commands[index]) == ispVERIFYFAIL)
            return ispVERIFYFAIL;
    }
    return ispVERIFYPASS;
}
										/*---------------Execution Instructions---------------*/

										/*
************************************************************
Function Name: programming_enable
************************************************************
Purpose: When the reset pin is pulled active low, the only instruction
accepted by the SPI interface is programming enable.
i.e AC 53 xx yy
************************************************************
Arguments: NONE
************************************************************
Returns: enum indicating success or failure 
************************************************************
*/

isp_status_t programming_enable(void)
{
    Reset_Low();

    delay(20);

    // Programming Enable Command
    uint8_t Command[4] = {0xAC, 0x53, 0x00, 0x00};
    uint8_t RXBuff[4] = {0x00, 0x00, 0x00, 0x00};
    command_execute(Command, RXBuff);
	
/*If programming enable is successful, the target returns the second byte of the command
	i.e 53																				*/
	
    if(RXBuff[2] == 0x53)
    {
        return ispSUCCESS;
    }
    else
    {
        return ispFAIL;
    }
}

/*
************************************************************
Function Name: chip_erase
************************************************************
Purpose:Before new content can be written to the Flash Program Memory,
the memory has to be erased.
command format: AC 8x yy nn  
************************************************************
Arguments: NONE
************************************************************
Returns: NONE
************************************************************
*/

void chip_erase(void)
{
    // Poll busy flag
    while(poll_busy() == ispBUSY);

    // Erase Command
    uint8_t Command[4] = {0xAC, 0x80, 0x00, 0x00};
    uint8_t RXBuff[4] = {0x00, 0x00, 0x00, 0x00};
    command_execute(Command, RXBuff);

    delay(20);
	// To end the chip erase cycle the reset line has to be release (Reset is active low)
    Reset_High();
}

/*
************************************************************
Function Name: poll_busy
************************************************************
Purpose: To check if the target has finished its previous operation 
command Fx yy zz nn  
************************************************************
Arguments: NONE
************************************************************
Returns: enum indicating if its busy or not
************************************************************
*/

isp_status_t poll_busy(void)
{
    uint8_t Command[4] = {0xF0, 0x00, 0x00, 0x00};
    uint8_t RXBuff[4] = {0x00, 0x00, 0x00, 0x00};
    command_execute(Command, RXBuff);

    if(RXBuff[3] & 0x01)
        return ispBUSY;
    else
        return ispNOTBUSY;
}

/*
************************************************************
Function Name: program_flash
************************************************************
Purpose: To write to the flash memory of the target  
************************************************************
Arguments: structure containing parameters from hex parsing
************************************************************
Returns: NONE
************************************************************
*/

void program_flash(command_t command)
{
    uint16_t address_index = command.address;
    if(command.data_type != 0x00)
        ;
    else if(command.size == 0)
        ;
    else
    {
        while(poll_busy() == ispBUSY);

        for(uint8_t index = 0; index < command.size; index++)
        {
            if(index % 2 == 0)
            {
                load_flash_low(address_index, command.data[index]);
            }
            else
            {
                load_flash_high(address_index, command.data[index]);
                address_index++;
            }
        }
        write_flash_page(command.address);
    }
}

/*
************************************************************
Function Name: verify_flash
************************************************************
Purpose: To read and verify that the flash of the target
has been written to correctly
************************************************************
Arguments: structure
************************************************************
Returns: enum indicating if the flash was written correctly or not
************************************************************
*/

isp_status_t verify_flash(command_t command)
{
    uint8_t tmp_array[16];
    uint16_t start_address = command.address;

    if(command.data_type != 0x00)
        ;
    else if(command.size == 0)
        ;
    else
    {
        for(uint8_t array_index = 0, address_index = 0; array_index < 16; array_index++)
        {
            while(poll_busy() == ispBUSY);
            if(array_index % 2 == 0)
            {
                tmp_array[array_index] = read_flash_low(start_address + address_index);
            }
            else
            {
                tmp_array[array_index] = read_flash_high(start_address + address_index);
                address_index++;
            }
        }

        for(uint8_t index = 0; index < 16; index++)
        {
            if(tmp_array[index] != command.data[index])
            {
                return ispVERIFYFAIL;
            }
        }
    }
    return ispVERIFYPASS;
}


/*-----------------Read Instructions-------------------*/

/*
************************************************************
Function Name: read_signature_byte
************************************************************
Purpose: Function to acquire device code which helps identify chip vendor,
part family and part number.
Read signature byte command: 30,00,00,00
Third byte = 0x00 for vendor code
= 0x01 for Partfamily
= 0x02 for Partnumber
************************************************************
Arguments: enum indicating which parameter we wish to query for
************************************************************
Returns: uint8_t response from the target
************************************************************
*/

uint8_t read_signature_byte(sig_byte_t signature_byte)
{
    uint8_t Command[4] = {0x30, 0x00, 0x00, 0x00};
    if(signature_byte == VendorCode)
        Command[2] = 0x00;
    else if(signature_byte == PartFamily)
        Command[2] = 0x01;
    else if(signature_byte == PartNumber)
        Command[2] = 0x02;
    uint8_t RXBuff[4] = {0x00, 0x00, 0x00, 0x00};
    command_execute(Command, RXBuff);

    return RXBuff[3];
}

/*
************************************************************
Function Name: read_flash_high
************************************************************
Purpose: To read the high byte from the memory.
command: 28, address MSB, address LSB
************************************************************
Arguments: address that we wish to read from
************************************************************
Returns: uint8_t response from the target
************************************************************
*/

uint8_t read_flash_high(uint16_t address)
{
    uint8_t Command[4] = {0x28, address & 0xFF00, address & 0x00FF, 0x00};
    uint8_t RXBuff[4] = {0x00, 0x00, 0x00, 0x00};
    command_execute(Command, RXBuff);

    return RXBuff[3];
}

/*
************************************************************
Function Name: read_flash_low
************************************************************
Purpose: To read the low byte from the memory.
command: 20, address MSB, address LSB
************************************************************
Arguments: address that we wish to read from
************************************************************
Returns: uint8_t response from the target
************************************************************
*/

uint8_t read_flash_low(uint16_t address)
{
    uint8_t Command[4] = {0x20, address & 0xFF00, address & 0x00FF, 0x00};
    uint8_t RXBuff[4] = {0x00, 0x00, 0x00, 0x00};
    command_execute(Command, RXBuff);

    return RXBuff[3];
}

/*
************************************************************
Function Name: read_eeprom
************************************************************
Purpose: To read EEPROM memory
************************************************************
Arguments: address that we wish to read from
************************************************************
Returns: uint8_t response from the target
************************************************************
*/

uint8_t read_eeprom(uint16_t address)
{
    uint8_t Command[4] = {0xA0, address & 0x0300, address & 0x00FF, 0x00};
    uint8_t RXBuff[4] = {0x00, 0x00, 0x00, 0x00};
    command_execute(Command, RXBuff);

    return RXBuff[3];
}

/*
************************************************************
Function Name: read_lock_bits
************************************************************
Purpose: To read the lock bits of the target
command: 58,00,00
************************************************************
Arguments: NONE
************************************************************
Returns: uint8_t response from the target
************************************************************
*/

uint8_t read_lock_bits(void)
{
    uint8_t Command[4] = {0x58, 0x00, 0x00, 0x00};
    uint8_t RXBuff[4] = {0x00, 0x00, 0x00, 0x00};
    command_execute(Command, RXBuff);

    return RXBuff[3];
}

/*
************************************************************
Function Name: read_fuse_bits
************************************************************
Purpose: To read the fuse bits of the target
command: 50,00,00
************************************************************
Arguments: NONE
************************************************************
Returns: uint8_t response from the target
************************************************************
*/

uint8_t read_fuse_bits(void)
{
    uint8_t Command[4] = {0x50, 0x00, 0x00, 0x00};
    uint8_t RXBuff[4] = {0x00, 0x00, 0x00, 0x00};
    command_execute(Command, RXBuff);

    return RXBuff[3];
}

/*
************************************************************
Function Name: read_fuse_high_bits
************************************************************
Purpose: To read the fuse bits of the target
command: 58,08,00
************************************************************
Arguments: NONE
************************************************************
Returns: uint8_t response from the target
************************************************************
*/

uint8_t read_fuse_high_bits(void)
{
    uint8_t Command[4] = {0x58, 0x08, 0x00, 0x00};
    uint8_t RXBuff[4] = {0x00, 0x00, 0x00, 0x00};
    command_execute(Command, RXBuff);

    return RXBuff[3];
}

/*
************************************************************
Function Name: read_ex_fuse_bits
************************************************************
Purpose: To read the extended fuse bits of the target
command: 50,08,00
************************************************************
Arguments: NONE
************************************************************
Returns: uint8_t response from the target
************************************************************
*/

uint8_t read_ex_fuse_bits(void)
{
    uint8_t Command[4] = {0x50, 0x08, 0x00, 0x00};
    uint8_t RXBuff[4] = {0x00, 0x00, 0x00, 0x00};
    command_execute(Command, RXBuff);

    return RXBuff[3];
}

/*
************************************************************
Function Name: read_caliberation_byte
************************************************************
Purpose: To read the caliberation byte of the target
command: 38,00,00
************************************************************
Arguments: NONE
************************************************************
Returns: uint8_t response from the target
************************************************************
*/

uint8_t read_calibration_byte(void)
{
    uint8_t Command[4] = {0x38, 0x00, 0x00, 0x00};
    uint8_t RXBuff[4] = {0x00, 0x00, 0x00, 0x00};
    command_execute(Command, RXBuff);

    return RXBuff[3];
}

/*-------------------Write Instructions------------------*/

/*
************************************************************
Function Name: load_ex_address_byte
************************************************************
Purpose: To load extended address byte
command: 4D,00,extended address
************************************************************
Arguments: uint8_t ex_addr - extended address
************************************************************
Returns: NONE
************************************************************
*/

void load_ex_address_byte(uint8_t ex_addr)
{
    uint8_t Command[4] = {0x4D, 0x00, ex_addr, 0x00};
    uint8_t RXBuff[4] = {0x00, 0x00, 0x00, 0x00};
    command_execute(Command, RXBuff);
}

/*
************************************************************
Function Name: load_flash_high
************************************************************
Purpose: To load program memory page, high byte
command: 48,00,address LSB
************************************************************
Arguments: uint8_t address and uint8_t data
************************************************************
Returns: NONE
************************************************************
*/

void load_flash_high(uint16_t address, uint8_t data)
{
    uint8_t Command[4] = {0x48, 0x00, address & 0x00FF, data};
    uint8_t RXBuff[4] = {0x00, 0x00, 0x00, 0x00};
    command_execute(Command, RXBuff);
}

/*
************************************************************
Function Name: load_flash_low
************************************************************
Purpose: To load the program memory page, low byte
command: 40,00,address LSB
************************************************************
Arguments: uint8_t address and uint8_t data
************************************************************
Returns: NONE
************************************************************
*/

void load_flash_low(uint16_t address, uint8_t data)
{
    uint8_t Command[4] = {0x40, 0x00, address & 0x00FF, data};
    uint8_t RXBuff[4] = {0x00, 0x00, 0x00, 0x00};
    command_execute(Command, RXBuff);
}

/*
************************************************************
Function Name: write_flash_page
************************************************************
Purpose: To write to the program memory page
command: 4C, address MSB ,address LSB
************************************************************
Arguments: uint8_t address
************************************************************
Returns: NONE
************************************************************
*/

void write_flash_page(uint16_t address)
{
    uint8_t Command[4] = {0x4C, address & 0xFF00, address & 0x00FF, 0x00};
    uint8_t RXBuff[4] = {0x00, 0x00, 0x00, 0x00};
    command_execute(Command, RXBuff);
}

/*
************************************************************
Function Name: write_eeprom_memory
************************************************************
Purpose: To write to the EEPROM memory
command: C0, 0000 00aa, aaaa aaaa
************************************************************
Arguments: uint8_t address uint8_t data
************************************************************
Returns: NONE
************************************************************
*/

void write_eeprom_memory(uint16_t address, uint8_t data)
{
    uint8_t Command[4] = {0xC0, address & 0x0300, address & 0x00FF, data};
    uint8_t RXBuff[4] = {0x00, 0x00, 0x00, 0x00};
    command_execute(Command, RXBuff);
}

/*
************************************************************
Function Name: write_lock_bits
************************************************************
Purpose: To write to the lock bits
command: AC, E0, 00, data byte in
************************************************************
Arguments: uint8_t data
************************************************************
Returns: NONE
************************************************************
*/

void write_lock_bits(uint8_t data)
{
    uint8_t Command[4] = {0xAC, 0xE0, 0x00, data};
    uint8_t RXBuff[4] = {0x00, 0x00, 0x00, 0x00};
    command_execute(Command, RXBuff);
}

/*
************************************************************
Function Name: write_fuse_bits
************************************************************
Purpose: To write to the fuse bits
command: AC, A0, 00, data byte in
************************************************************
Arguments: uint8_t data
************************************************************
Returns: NONE
************************************************************
*/

void write_fuse_bits(uint8_t data)
{
    uint8_t Command[4] = {0xAC, 0xA0, 0x00, data};
    uint8_t RXBuff[4] = {0x00, 0x00, 0x00, 0x00};
    command_execute(Command, RXBuff);
}

/*
************************************************************
Function Name: write_fuse_high_bits
************************************************************
Purpose: To write to the high bytes of the fuse
command: AC, A8, 00, data byte in
************************************************************
Arguments: uint8_t data
************************************************************
Returns: NONE
************************************************************
*/

void write_fuse_high_bits(uint8_t data)
{
    uint8_t Command[4] = {0xAC, 0xA8, 0x00, data};
    uint8_t RXBuff[4] = {0x00, 0x00, 0x00, 0x00};
    command_execute(Command, RXBuff);
}

/*
************************************************************
Function Name: write_ex_fuse_bits
************************************************************
Purpose: To write to the extended fuse bits
command: AC, A4, 00, data byte in
************************************************************
Arguments: uint8_t data
************************************************************
Returns: NONE
************************************************************
*/

void write_ex_fuse_bits(uint8_t data)
{
    uint8_t Command[4] = {0xAC, 0xA4, 0x00, data};
    uint8_t RXBuff[4] = {0x00, 0x00, 0x00, 0x00};
    command_execute(Command, RXBuff);
}



