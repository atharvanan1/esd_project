/*
 * spi_library.h
 *
 *  Created on: Dec 3, 2019
 *      Author: athar
 */

#ifndef SPI_LIBRARY_H_
#define SPI_LIBRARY_H_
#include <stdint.h>
#include "msp.h"

void spi_init(void);
void spi_send(uint8_t data);
uint8_t spi_receive(void);

#endif /* SPI_LIBRARY_H_ */
