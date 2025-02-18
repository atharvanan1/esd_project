/*
 * File - hex_parse.h
 * Brief -  Contains all the hex parsing functions. 
 * Author - Atharva Nandanwar
 * University of Colorado Boulder
 * Embedded System Design
 */


#include "hex_parse.h"

/*
************************************************************
Function Name: hex_parse
************************************************************
Purpose: This function executes complete hex parsing, using
line parsing function
************************************************************
Arguments: int8_t* buffer, uint8_t Checksum_Error
************************************************************
Returns: NONE
************************************************************
*/

void hex_parse(int8_t* buffer, uint8_t Checksum_Error)
{
    int8_t* line[50];
    uint16_t index = 0;
    uint16_t line_count = 0;
    int8_t ch = 0;

    // Line Zero will always start from first address
    line[0] = buffer;

    index++;
    // Till the end of file - change the EOF value
    while((ch = *(buffer + index)) != '\0')
    {
        // Registering new lines as lines, and added them into line
        // buffer.
        // Note: line_count is incremented first to maintain appropriate
        // pointer to the index. 0 is already filled, 1, 2, and... so on
        // index + 1 ---> because we are going through current line and will
        // store the next line, and not current line.
        if(ch == ':')
        {
            line_count++;
            line[line_count] = buffer + index;
        }
        index++;
    }
    // Line Count only provides index, incrementing again to denote
    // actual line count.
    line_count++;

    for(uint8_t index = 0; index < line_count; index++)
    {
        commands[index] = hex_line_parse(line[index]);
        if(commands[index].checksum_validity == checksumValid)
        {
            Checksum_Error = 1;
            break;
        }
    }
}

/*
************************************************************
Function Name: hex_line_parse
************************************************************
Purpose: Function parses individual line; actual parsing is 
done here
************************************************************
Arguments: const int8_t* line
************************************************************
Returns: struct command_t for various parameters 
************************************************************
*/

command_t hex_line_parse(const int8_t* line)
{
    command_t line_command;
    line_command.address = 0;
    line_command.size= 0;
    line_command.data_type = 0;
    line_command.checksum_validity = checksumInvalid;
    for(uint8_t index = 0; index < 16; index++)
    {
        line_command.data[index] = 0;
    }

    uint8_t calculatedChecksum = 0;
    uint8_t receivedChecksum = 0;

    // TODO: Initialize all the members of struct command_t
    uint8_t index = 0;
    uint8_t data_index = 0;
    uint8_t c = 0;
    while((c = *(line + index)) != 0x0a)
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
                        data_index++; //well done
        }
        if(index >= 41 && index <= 43)
        {
                receivedChecksum = receivedChecksum * 16 + c;
        }
        index++;
    }

    calculatedChecksum = line_command.size + ((line_command.address & 0xFF00) >> 8) \
            + (line_command.address & 0xFF) + line_command.data_type;

    for(index = 0; index < line_command.size; index++)
    {
        calculatedChecksum += line_command.data[index];
    }

    calculatedChecksum = ~calculatedChecksum + 1;

    if(receivedChecksum == calculatedChecksum)
    {
        line_command.checksum_validity = checksumValid;
    }
    else
    {
        line_command.checksum_validity = checksumInvalid;
    }

    // Changing the address for ATmega328p specific memory
    line_command.address = line_command.address >> 1;

    // This pointer shall be freed by function that has finished usage
    return line_command;
}


