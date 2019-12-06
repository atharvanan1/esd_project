/*
 * hex_parse.c
 *
 *  Created on: Dec 3, 2019
 *      Author: athar
 */


#include "hex_parser.h"

void hex_parse(hex_file_t file)
{
    char* lines[500];
    uint16_t number_of_lines = calculateNumberOfLines(file.file_pointer, file.number_of_characters);
}

command_t hex_line_parse(const char* line)
{
    command_t line_command;
    uint8_t calculatedChecksum = 0;
    uint8_t receivedChecksum = 0;

    uint8_t index = 0;
    uint8_t data_index = 0;
    uint8_t c = 0;
    while((c = *(line + index)) != '\0')
    {
        if(c >= '0' && c <= '9')
                c = c - '0';
        else if(c >= 'A' && c <= 'F')
                c = c - 'A' + 10;
        else if(c >= 'a' && c <= 'f')
                c = c - 'a' + 10;

        if(index >= 1 && index <= 2)
        {
            line_command.size = line_command.size * 16 + c;
        }
        else if(index >= 3 && index <= 6)
        {
            line_command.address = line_command.address * 16 + c;
        }
        else if(index >= 7 && index <= 8)
        {
            line_command.data_type = line_command.data_type * 16 + c;
        }
        else if(index >= 9 && index < 41)
        {
            line_command.data[data_index] = line_command.data[data_index] * 16 + c;
                if(index % 2 == 0)
                        data_index++;
        }
        if(index >= 41 && index <= 43)
        {
                receivedChecksum = receivedChecksum * 16 + c;
        }
        index++;
    }

    calculatedChecksum = line_command.size + ((line_command.address & 0xFF00) >> 8) \
            + (line_command.address & 0xFF) + line_command.data_type;

    for(index = 0; index < 16; index++)
    {
        calculatedChecksum += line_command.data[index];
    }

    if(receivedChecksum == calculatedChecksum)
    {
        line_command.checksum_validity = checksumValid;
    }
    else
    {
        line_command.checksum_validity = checksumInvalid;
    }
    return line_command;
}

uint16_t calculateNumberOfLines(const char* buffer, uint16_t length)
{
    uint16_t number_of_lines = 0;;
    for(uint16_t index = 0; index < length; index++)
    {
        if(*(buffer + index) == '\n')
        {
            number_of_lines++;
        }
    }
    return number_of_lines;
}


