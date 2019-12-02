/*
 * hex_parser.h
 *
 *  Created on: Dec 2, 2019
 *      Author: athar
 */

#ifndef HEX_PARSER_H_
#define HEX_PARSER_H_
#include <stdint.h>

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
#endif /* HEX_PARSER_H_ */
