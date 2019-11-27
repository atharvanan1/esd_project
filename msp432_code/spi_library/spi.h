/*
 * spi.h
 *
 *  Created on: Nov 25, 2019
 *      Author: athar
 */

#ifndef SPI_H_
#define SPI_H_

#include "msp.h"

typedef enum {
    ACLK,
    SMCLK,
} clock_select_t;

typedef enum {
    Low,
    High,
} clock_pol_t;

typedef enum {
    Change_at_Edge,
    Capture_at_Edge,
} clock_ph_t;

typedef enum {
    Length_8Bit,
    Length_7Bit,
}data_length_t;

typedef enum {
    LSB_First,
    MSB_First,
} data_order_t;

typedef struct {
    data_length_t data_length,
    data_order_t data_order,
} data_format_t;

typedef enum {
    Slave,
    Master,
} mode_t;

typedef struct {
    clock_select_t clock;
    clock_pol_t clock_polarity;
    clock_ph_t clock_phase;
    data_format_t data_format;
    mode_t mode;
} spi_config_t;

#endif /* SPI_H_ */
