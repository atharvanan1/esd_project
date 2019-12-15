/*
 * File - spi_library.h
 * Brief -  Contains all the declarations for the SPI related functions
 * Author - Atharva Nandanwar 
 * University of Colorado Boulder
 * Embedded System Design
 */

#ifndef SPI_LIBRARY_H_
#define SPI_LIBRARY_H_
#include <stdint.h>
#include "common.h"

void spi_init(void);
void spi_send(uint8_t data);
uint8_t spi_receive(void);

#endif /* SPI_LIBRARY_H_ */
