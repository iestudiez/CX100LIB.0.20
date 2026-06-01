/**
 ******************************************************************************
 * @file        : spi.h
 * @author      : Estudiez, Ivan
 * @brief       : STM32F407 SPI Library
 * @date        : 14/02/2024
 * @version     : 1.0
 ******************************************************************************
 *
 *
 *
 *
 *
 ******************************************************************************
 */

#ifndef SPI_H_
#define SPI_H_

#include "stm32f4xx.h"
#include <stdint.h>
#include <stdbool.h>
#include "gpio.h"

#define SPI_READ_OPERATION 0x80

typedef enum
{
	CLK_DIV_2 = 0,
	CLK_DIV_4 = 1,
	CLK_DIV_8 = 2,
	CLK_DIV_16 = 3,
	CLK_DIV_32 = 4,
	CLK_DIV_64 = 5,
	CLK_DIV_128 = 6,
	CLK_DIV_256 = 7,
} Baudrate_Enum;

typedef struct
{
	SPI_TypeDef *module;
	Baudrate_Enum br;
	bool cpol;
	bool cpha;
	bool rxonly;
	bool lsbfrst;
	bool mstr;
	bool dff;
	bool ssm;
	bool ssi;
	bool txdma;
	bool rxdma;
} SPI_Config_t;

void SPI_Init(SPI_Config_t spi);
void SPI_Config_Reset(SPI_Config_t *spiConf);
void SPI_Enable(SPI_TypeDef *spi);
void SPI_Disable(SPI_TypeDef *spi);
void SPI_Transmit(SPI_TypeDef *spi, uint8_t *data, uint32_t size);
void SPI_Receive(SPI_TypeDef *spi, uint8_t *data, uint32_t size);
void SPI_Select(GPIO_TypeDef *gpio, uint8_t pin, bool enable);

// ============================================================================
// Public variables
// ============================================================================

#endif /* SPI_H_ */
