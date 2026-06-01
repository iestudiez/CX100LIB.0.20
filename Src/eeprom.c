/**
 ******************************************************************************
 * @file        : eeprom.c
 * @author      : Estudiez, Ivan
 * @brief       : EEPROM Library (25LC640A 64 Kbit memory)
 * @date        : 11/08/2025
 * @version     : 1.1
 ******************************************************************************
 * Change control:
 * 1.0 - (08/08/2024) Release version
 * 1.1 - (11/08/2025) Minor changes
 *
 *
 ******************************************************************************
 */

#include "eeprom.h"
#include <stm32f4xx.h>
#include "spi.h"
#include "system_cx100.h"

// 25LC640A EEPROM Commands
// -----------------------------------------------------------------------------
#define EEPROM_CMD_READ						0x03
#define EEPROM_CMD_WRITE					0x02
#define EEPROM_CMD_WRDI						0x04
#define EEPROM_CMD_WREN						0x06
#define EEPROM_CMD_RDSR						0x05
#define EEPROM_CMD_WRSR						0x01

/**
 * -----------------------------------------------------------------------------
 * @brief			EEPROM Chip Select
 * -----------------------------------------------------------------------------
 * @param enable	Enable chip
 * -----------------------------------------------------------------------------
 */
void EEPROM_CS(bool enable)
{
	if (SPI1->SR & SPI_SR_BSY)
		return;

	if (enable)
		EEPROM_CS_PORT->ODR &= ~(1U << EEPROM_CS_PIN);
	else
		EEPROM_CS_PORT->ODR |= (1U << EEPROM_CS_PIN);

}

/**
 * -----------------------------------------------------------------------------
 * @brief		Read a portion of memory
 * -----------------------------------------------------------------------------
 * @param addr
 * @param data
 * @param size
 * -----------------------------------------------------------------------------
 */
void EEPROM_Read(uint16_t addr, uint8_t *data, uint8_t size)
{
	uint8_t read_cmd[3];

	// Check if the SPI bus is in use
	if (SPI1->SR & SPI_SR_BSY)
		return;

	read_cmd[0] = EEPROM_CMD_READ;
	read_cmd[1] = (uint8_t) addr >> 8;
	read_cmd[2] = (uint8_t) addr & 0x00FF;

	EEPROM_CS(true);

	SPI_Transmit(SPI1, read_cmd, 3);
	SPI_Receive(SPI1, data, size);

	EEPROM_CS(false);
}

/**
 * -----------------------------------------------------------------------------
 * @brief 		Write a portion of the memory
 * -----------------------------------------------------------------------------
 * @param addr
 * @param data
 * @param size
 * -----------------------------------------------------------------------------
 */
void EEPROM_Write(uint16_t addr, uint8_t *data, uint8_t size)
{
	uint8_t write_enable = EEPROM_CMD_WREN;
	uint8_t write_cmd[3];

	// Check if the SPI bus is in use
	if (SPI1->SR & SPI_SR_BSY)
		return;

	// 24LC640A Page size (32 bytes)
	if (size > 32)
		size = 32;

	write_cmd[0] = EEPROM_CMD_WRITE;
	write_cmd[1] = (uint8_t) addr >> 8;
	write_cmd[2] = (uint8_t) addr & 0x00FF;

	// Write Enable
	EEPROM_CS(true);
	SPI_Transmit(SPI1, &write_enable, 1);
	EEPROM_CS(false);

	// 10 micro-seconds delay
	SYSTEM_Delay(10);

	// Write Operation
	EEPROM_CS(true);
	SPI_Transmit(SPI1, write_cmd, 3);
	SPI_Transmit(SPI1, data, size);
	EEPROM_CS(false);
}

/**
 * -----------------------------------------------------------------------------
 * @brief
 * -----------------------------------------------------------------------------
 * @param pageNumber
 * @return
 * -----------------------------------------------------------------------------
 */
uint16_t EEPROM_GetPageAddress(uint16_t pageNumber)
{
	if (pageNumber > EEPROM_NUM_OF_PAGES)
		pageNumber = EEPROM_NUM_OF_PAGES;

	return pageNumber * EEPROM_PAGE_SIZE;
}

/**
 * -----------------------------------------------------------------------------
 * @brief 			Get a word from an array (little endian format)
 * -----------------------------------------------------------------------------
 * @param pArray	Pointer to the position of an element in the array
 * @return			Value of the selected array element
 * -----------------------------------------------------------------------------
 */
uint16_t EEPROM_GetWord(uint8_t *pArray)
{
	return (pArray[1] << 8U) + pArray[0];
}

/**
 * -----------------------------------------------------------------------------
 * @brief 			Store a word in an array (little endian format)
 * -----------------------------------------------------------------------------
 * @param pArray	Pointer to the save position in the array
 * @param value		Value to store
 * -----------------------------------------------------------------------------
 */
void EEPROM_StoreWord(uint8_t *pArray, uint16_t value)
{
	pArray[0] = (uint8_t) (value & 0x00FF);
	pArray[1] = (uint8_t) (value >> 8U);
}

/**
 * -----------------------------------------------------------------------------
 * @brief 			Packs up to 8 boolean variables into one byte
 * -----------------------------------------------------------------------------
 * @param strVar	Storage variable
 * @param var		Boolean variable
 * @param pos		Position
 * -----------------------------------------------------------------------------
 */
void EEPROM_PackByte(uint8_t *strVar, bool var, uint8_t pos)
{
	if (var)
		*strVar |= (1U << pos);
	else
		*strVar &= ~(1U << pos);
}

/**
 * -----------------------------------------------------------------------------
 * @brief 		Extracts a boolean variable from a byte
 * -----------------------------------------------------------------------------
 * @param var	Byte variable
 * @param pos	Position
 * @return		Boolean value
 * -----------------------------------------------------------------------------
 */
bool EEPROM_UnpackByte(uint8_t var, uint8_t pos)
{
	if (var & (1U << pos))
		return true;
	else
		return false;
}
