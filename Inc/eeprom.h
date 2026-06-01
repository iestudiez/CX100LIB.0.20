/**
 ******************************************************************************
 * @file        : eeprom.h
 * @author      : Estudiez, Ivan
 * @brief       : EEPROM Library (25LC640A 64 Kbit memory)
 * @date        : 08/08/2024
 * @version     : 1.0
 ******************************************************************************
 *
 * WARNING!
 * 25LC640A Has 256 pages of 32 bytes each
 *
 * Page write operations are limited to writing bytes within a single physical
 * page, regardless of the number of bytes actually being written.
 *
 * Physical page boundaries start at addresses that are integer multiples of
 * the page buffer size (or ‘page size’) and, end at addresses that are integer
 * multiples of page size – 1.
 *
 * If a Page Write command attempts to write across a physical page boundary,
 * the result is that the data wraps around to the beginning of the current
 * page (overwriting data previously stored there), instead of being written
 * to the next page as might be expected.
 *
 * It is therefore necessary for the application software to prevent page write
 * operations that would attempt to cross a page boundary.
 *
 * ----------------------------------------------------------------------------
 * Change control:
 * 1.0 - (08/08/2024) Release version
 * 1.1 - (11/08/2025) Minor changes
 *
 *
 ******************************************************************************
 */

#ifndef EEPROM_H_
#define EEPROM_H_

#include <stdbool.h>
#include <stdint.h>

#define EEPROM_MEMORY_SIZE 			8912
#define EEPROM_PAGE_SIZE 			32
#define EEPROM_NUM_OF_PAGES 		256
#define EEPROM_CS_PORT				GPIOB
#define EEPROM_CS_PIN				0U

// Public methods
// ============================================================================
void EEPROM_CS(bool enable);
void EEPROM_Read(uint16_t addr, uint8_t *data, uint8_t size);
void EEPROM_Write(uint16_t addr, uint8_t *data, uint8_t size);
void EEPROM_StoreWord(uint8_t *pArray, uint16_t value);
void EEPROM_PackByte(uint8_t *strVar, bool var, uint8_t pos);
uint16_t EEPROM_GetPageAddress(uint16_t pageNumber);
uint16_t EEPROM_GetWord(uint8_t *pArray);
bool EEPROM_UnpackByte(uint8_t var, uint8_t pos);

#endif /* EEPROM_H_ */
