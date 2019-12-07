/*
 * isp_commands.c
 *
 *  Created on: Dec 3, 2019
 *      Author: athar
 */

#include "isp_commands.h"

/*---------------Init and Helper Functions------------*/
void ISP_Init(void)
{
    spi_init();

    // Initialize reset pin
    P3->OUT |= BIT0;
    P3->DIR |= BIT0;
}

void delay(uint32_t time)
{
    time *= 300;
    // TODO: devise a method to add delay with microseconds as input
    for(volatile uint32_t index = 0; index < time; index++);
}

void command_execute(uint8_t* tx_buff, uint8_t* rx_buff)
{
    for(uint8_t index = 0; index < 4; index++)
    {
        spi_send(*(tx_buff + index));
        *(rx_buff + index) = spi_receive();
    }
}

void program(void)
{
    for(uint8_t index = 0; index < 200; index++)
    {
        program_flash(commands[index]);
    }
}
/*---------------Execution Instructions---------------*/

isp_status_t programming_enable(void)
{
    P3->OUT &= ~BIT0;

    delay(20);

    // Programming Enable Command
    uint8_t Command[4] = {0xAC, 0x53, 0x00, 0x00};
    uint8_t RXBuff[4] = {0x00, 0x00, 0x00, 0x00};
    command_execute(Command, RXBuff);

    if(RXBuff[2] == 0x53)
    {
        return ispSUCCESS;
    }
    else
    {
        return ispFAIL;
    }
}

isp_status_t chip_erase(void)
{
    // Poll busy flag
    while(poll_busy() == ispBUSY);

    // Erase Command
    uint8_t Command[4] = {0xAC, 0x80, 0x00, 0x00};
    uint8_t RXBuff[4] = {0x00, 0x00, 0x00, 0x00};
    command_execute(Command, RXBuff);

    // TODO: Add delay of 9 ms or maybe poll? Test the code out
    delay(20);

    P3->OUT |= BIT0;
}

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

        while(poll_busy() == ispBUSY);

    //    if(verify_flash(command.address, command.data) == ispVERIFYFAIL);
    //    {
    //        while(1);
    //    }
    }
}

/*-----------------Read Instructions-------------------*/
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

uint8_t read_flash_high(uint16_t address)
{
    uint8_t Command[4] = {0x28, address & 0xFF00, address & 0x00FF, 0x00};
    uint8_t RXBuff[4] = {0x00, 0x00, 0x00, 0x00};
    command_execute(Command, RXBuff);

    return RXBuff[3];
}

uint8_t read_flash_low(uint16_t address)
{
    uint8_t Command[4] = {0x20, address & 0xFF00, address & 0x00FF, 0x00};
    uint8_t RXBuff[4] = {0x00, 0x00, 0x00, 0x00};
    command_execute(Command, RXBuff);

    return RXBuff[3];
}

uint8_t read_eeprom(uint16_t address)
{
    uint8_t Command[4] = {0xA0, address & 0x0300, address & 0x00FF, 0x00};
    uint8_t RXBuff[4] = {0x00, 0x00, 0x00, 0x00};
    command_execute(Command, RXBuff);

    return RXBuff[3];
}

uint8_t read_lock_bits(void)
{
    uint8_t Command[4] = {0x58, 0x00, 0x00, 0x00};
    uint8_t RXBuff[4] = {0x00, 0x00, 0x00, 0x00};
    command_execute(Command, RXBuff);

    return RXBuff[3];
}

uint8_t read_fuse_bits(void)
{
    uint8_t Command[4] = {0x50, 0x00, 0x00, 0x00};
    uint8_t RXBuff[4] = {0x00, 0x00, 0x00, 0x00};
    command_execute(Command, RXBuff);

    return RXBuff[3];
}

uint8_t read_fuse_high_bits(void)
{
    uint8_t Command[4] = {0x58, 0x08, 0x00, 0x00};
    uint8_t RXBuff[4] = {0x00, 0x00, 0x00, 0x00};
    command_execute(Command, RXBuff);

    return RXBuff[3];
}

uint8_t read_ex_fuse_bits(void)
{
    uint8_t Command[4] = {0x50, 0x08, 0x00, 0x00};
    uint8_t RXBuff[4] = {0x00, 0x00, 0x00, 0x00};
    command_execute(Command, RXBuff);

    return RXBuff[3];
}

uint8_t read_calibration_byte(void)
{
    uint8_t Command[4] = {0x38, 0x00, 0x00, 0x00};
    uint8_t RXBuff[4] = {0x00, 0x00, 0x00, 0x00};
    command_execute(Command, RXBuff);

    return RXBuff[3];
}

/*-------------------Write Instructions------------------*/
void load_ex_address_byte(uint8_t ex_addr)
{
    uint8_t Command[4] = {0x4D, 0x00, ex_addr, 0x00};
    uint8_t RXBuff[4] = {0x00, 0x00, 0x00, 0x00};
    command_execute(Command, RXBuff);
}

void load_flash_high(uint16_t address, uint8_t data)
{
    uint8_t Command[4] = {0x48, 0x00, address & 0x00FF, data};
    uint8_t RXBuff[4] = {0x00, 0x00, 0x00, 0x00};
    command_execute(Command, RXBuff);
}

void load_flash_low(uint16_t address, uint8_t data)
{
    uint8_t Command[4] = {0x40, 0x00, address & 0x00FF, data};
    uint8_t RXBuff[4] = {0x00, 0x00, 0x00, 0x00};
    command_execute(Command, RXBuff);
}

void write_flash_page(uint16_t address)
{
    uint8_t Command[4] = {0x4C, address & 0xFF00, address & 0x00FF, 0x00};
    uint8_t RXBuff[4] = {0x00, 0x00, 0x00, 0x00};
    command_execute(Command, RXBuff);
}

void write_eeprom_memory(uint16_t address, uint8_t data)
{
    uint8_t Command[4] = {0xC0, address & 0x0300, address & 0x00FF, data};
    uint8_t RXBuff[4] = {0x00, 0x00, 0x00, 0x00};
    command_execute(Command, RXBuff);
}

void write_lock_bits(uint8_t data)
{
    uint8_t Command[4] = {0xAC, 0xE0, 0x00, data};
    uint8_t RXBuff[4] = {0x00, 0x00, 0x00, 0x00};
    command_execute(Command, RXBuff);
}

void write_fuse_bits(uint8_t data)
{
    uint8_t Command[4] = {0xAC, 0xA0, 0x00, data};
    uint8_t RXBuff[4] = {0x00, 0x00, 0x00, 0x00};
    command_execute(Command, RXBuff);
}

void write_fuse_high_bits(uint8_t data)
{
    uint8_t Command[4] = {0xAC, 0xA8, 0x00, data};
    uint8_t RXBuff[4] = {0x00, 0x00, 0x00, 0x00};
    command_execute(Command, RXBuff);
}

void write_ex_fuse_bits(uint8_t data)
{
    uint8_t Command[4] = {0xAC, 0xA4, 0x00, data};
    uint8_t RXBuff[4] = {0x00, 0x00, 0x00, 0x00};
    command_execute(Command, RXBuff);
}

/*--------------Verify Instructions--------------*/

isp_status_t verify_flash(uint16_t start_address, uint8_t* data_array)
{
    uint8_t tmp_array[16];
    for(uint8_t array_index = 0, address_index = 0; array_index < 16; array_index++)
    {
        if(array_index & 0x01 != 0)
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
        if(tmp_array[index] != data_array[index])
        {
            return ispVERIFYFAIL;
        }
    }
    return ispVERIFYPASS;
}



