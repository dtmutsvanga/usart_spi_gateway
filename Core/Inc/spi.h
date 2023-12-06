/**
 * @file   spi.h
 * @author thakoldun
 * @date 2023-12-06
 * @brief SPI interface
*/

#pragma once 
#include <stdint.h>
void spi_init(void);
void spi_transmit(uint8_t *data, uint16_t len);
uint8_t spi_receive(uint8_t *pbuff, uint16_t len);