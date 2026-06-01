/**
 ******************************************************************************
 * @file        : glcd.h
 * @author      : Estudiez, Ivan
 * @brief       : Graphic library for monochrome LCDs
 * @date        : 30/04/2026
 * @version     : 1.0
 ******************************************************************************
 *
 *
 *
 ******************************************************************************
 */

#include "stdint.h"
#include "stdbool.h"

#ifndef INC_GLCD_H_
#define INC_GLCD_H_

#define GLCD_FONT_HEIGHT					0		// Height in pixels
#define GLCD_FONT_CH_SIZE					1		// Char size in bytes
#define GLCD_FONT_FIRST_CH					2		// First character of the font
#define GLCD_FONT_NUMBER_OF_CH				3		// Number of characters in the font
#define GLCD_FONT_START						4		// Position of the first character of the font

// Definitions
// -----------------------------------------------------------------------------
typedef struct TextBox_t
{
	uint8_t x1;
	uint8_t y1;
	uint8_t x2;
	uint8_t y2;
} TextBox_t;

typedef enum BoxType_e
{
	GLCD_BOXTYPE_WHITE = 0,
	GLCD_BOXTYPE_BLACK = 1,
	GLCD_BOXTYPE_REVERSE = 2
} BoxType_e;

// Public functions
// -----------------------------------------------------------------------------

TextBox_t GLCD_DrawText(const char *string, uint8_t x, uint8_t y, const unsigned char *font, uint8_t spacing, bool color);
uint8_t GLCD_TextWidth(char *string, const unsigned char *font, uint8_t spacing);
void GLCD_ScreenBitmap(const unsigned char bitmap[]);
void GLCD_PutBitmap(uint8_t x, uint8_t y, const unsigned char bitmap[], bool mode);
void GLCD_PutTile(uint8_t column, uint8_t page, const uint8_t bitmap[]);
void GLCD_DrawBox(uint8_t x, uint8_t y, uint8_t width, uint8_t height, BoxType_e opColor);
void GLCD_ProgressBar(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t value);
void GLCD_DrawLine(int x1, int y1, int x2, int y2, bool color);

#endif /* INC_GLCD_H_ */
