/*
 * isp_commands.h
 *
 *  Created on: Dec 3, 2019
 *      Author: athar
 */

#ifndef ISP_COMMANDS_H_
#define ISP_COMMANDS_H_
#include "spi_library.h"
#include "msp.h"

typedef enum {
    ispSUCCESS,
    ispFAIL,
    ispBUSY,
    ispNOTBUSY,
    ispVERIFYPASS,
    ispVERIFYFAIL,
} isp_status_t;

typedef enum {
    VendorCode,
    PartFamily,
    PartNumber,
} sig_byte_t;

void isp_init(void);
void delay(uint32_t time);
void command_execute(uint8_t* txbuff, uint8_t* rxbuff);
isp_status_t programming_enable(void);
isp_status_t chip_erase(void);
isp_status_t poll_busy(void);
uint8_t read_signature_byte(sig_byte_t signature_byte);
uint8_t read_flash_high(uint16_t address);
uint8_t read_flash_low(uint16_t address);
uint8_t read_eeprom(uint16_t address);
uint8_t read_lock_bits(void);
uint8_t read_fuse_bits(void);
uint8_t read_fuse_high_bits(void);
uint8_t read_ex_fuse_bits(void);
uint8_t read_calibration_byte(void);
void load_ex_address_byte(uint8_t ex_addr);
void load_flash_high(uint16_t address, uint8_t data);
void load_flash_low(uint16_t address, uint8_t data);
void write_flash_page(uint16_t address);
void write_eeprom_memory(uint16_t address, uint8_t data);
void write_lock_bits(uint8_t data);
void write_fuse_bits(uint8_t data);
void write_fuse_high_bits(uint8_t data);
void write_ex_fuse_bits(uint8_t data);
isp_status_t verify_flash(uint16_t start_address, uint8_t* data_array);

#endif /* ISP_COMMANDS_H_ */
