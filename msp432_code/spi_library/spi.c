/*
 * spi.c
 *
 *  Created on: Nov 25, 2019
 *      Author: athar
 */

#include "spi.h"

/**
 * EUSCI_A0_SPI
 * EUSCI_A1_SPI
 * EUSCI_A2_SPI
 * EUSCI_A3_SPI
 * EUSCI_B0_SPI
 * EUSCI_B1_SPI
 * EUSCI_B2_SPI
 * EUSCI_B3_SPI
 */

void SPI_Init(EUSCI_B_SPI_Type* SPI_Base, spi_config_t &spi_config)
{
    SPI_Base->CTLW0 |= UCSWRST;
    SPI_Base->CTLW0 |= (spi_config->mode) << UCMST_OFS | \
                       (spi_config->data_length) << UC7BIT_OFS \
                       (spi_config->clock_polarity) << UCCKPL_OFS \
                       (spi_config->clock_phase) << UCCKPH_OFS;
}

void SPI_Send(uint8_t data)
{

}

uint8_t SPI_Receive(void)
{

}
