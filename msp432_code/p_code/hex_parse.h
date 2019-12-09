/*
 * hex_parse.h
 *
 *  Created on: Dec 3, 2019
 *      Author: athar
 */

#ifndef HEX_PARSE_H_
#define HEX_PARSE_H_
#include <stdint.h>
#include <stdlib.h>
#include "common.h"

#define START_CHARACTER     1
#define ADDRESS_WIDTH       4
#define DATA_TYPE_WIDTH     2
#define SIZE_WIDTH          2

typedef enum {
    pChecksum,
    fChecksum,
    fInvalidInput,
} hp_status_t;

typedef enum {
    checksumValid,
    checksumInvalid,
} checksum_validity_t;

typedef struct {
    uint8_t size;
    uint16_t address;
    uint8_t data_type;
    uint8_t data[16];
    checksum_validity_t checksum_validity;
} command_t;

typedef struct {
    uint16_t number_of_characters;
    char* file_pointer;
} hex_file_t;

// Function Prototypes
uint16_t calculateNumberOfLines(const char* buffer, uint16_t length);
void hex_parse(int8_t* buffer, uint8_t Checksum_Error);
command_t hex_line_parse(const int8_t* line);

extern command_t commands[200];

#endif /* HEX_PARSE_H_ */
