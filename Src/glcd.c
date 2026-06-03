/**
 ******************************************************************************
 * @file        : glcd.c
 * @author      : Estudiez, Ivan
 * @brief       : Graphic library for monochrome LCD (128x64)
 * @date        : 30/04/2026
 * @version     : 1.0
 ******************************************************************************
 *
 *
 *
 ******************************************************************************
 */
#include <stm32f4xx.h>
#include "glcd.h"
#include "stdint.h"
#include "stdbool.h"
#include "lcd.h"
#include "dma.h"

// Type definitions
// -----------------------------------------------------------------------------
typedef struct CharSettings_t
{
	unsigned char ch;
	uint8_t x;
	uint8_t y;
	uint8_t height;
	uint8_t offset;
	uint8_t charPages;
	uint8_t totalPages;
	bool color;
} CharSettings_t;

// Private prototypes
// -----------------------------------------------------------------------------
TextBox_t
GLCD_DrawChar(CharSettings_t c, const unsigned char *font);

/**
 * ----------------------------------------------------------------------------
 * @brief				Calculate the length of the text
 * ----------------------------------------------------------------------------
 * @param string		Text
 * @param font			Text font to use
 * @param spacing		Space between characters (in pixels)
 * @return				Text length in pixels
 * ----------------------------------------------------------------------------
 */
uint8_t GLCD_TextWidth(char *string, const unsigned char *font, uint8_t spacing)
{
	unsigned char ch;
	uint8_t width = 0;
	uint16_t posReader;

	while (*string != 0)
	{
		// Read a character of the string
		ch = *string++;

		// Position of the passed character in the font array.
		// This position contains the letter width.
		posReader = (ch - font[GLCD_FONT_FIRST_CH]) * font[GLCD_FONT_CH_SIZE] + GLCD_FONT_START;

		width += font[posReader] + 1 + spacing;
	}
	return width;
}

/**
 * ----------------------------------------------------------------------------
 * @brief				Draw a text
 *  ---------------------------------------------------------------------------
 * @param string		Text
 * @param x				x-coordinate of the starting point of the text
 * @param y				y-coordinate of the starting point of the text
 * @param font			Text font to use
 * @param spacing		Space between characters
 * @param color
 * @return				Text box dimensions
 *  -----------------------------------------------------------------------------
 */
TextBox_t GLCD_DrawText(const char *string, uint8_t x, uint8_t y, const unsigned char *font, uint8_t spacing,
		bool color)
{
	TextBox_t ret;
	TextBox_t tmp;
	CharSettings_t c;

	ret.x1 = x;
	ret.y1 = y;
	c.x = x;
	c.y = y;

	// Check boundaries
	if (x >= LCD_SCREEN_WIDTH)
		return ret;

	if (y >= LCD_SCREEN_HEIGHT)
		return ret;

	// Check maximum height
	c.height = font[GLCD_FONT_HEIGHT];
	if (c.height > 24)
		return ret;

	// Set the vertical offset of the char on the page.
	c.offset = y % 8;

	// Set the number of pages used by a char
	if (font[GLCD_FONT_HEIGHT] % 8)
		c.charPages = (font[GLCD_FONT_HEIGHT] / 8) + 1;
	else
		c.charPages = (font[GLCD_FONT_HEIGHT] / 8);

	// Set the number of pages needed to print a char
	if ((font[GLCD_FONT_HEIGHT] + c.offset) % 8)
		c.totalPages = ((font[GLCD_FONT_HEIGHT] + c.offset) / 8) + 1;
	else
		c.totalPages = ((font[GLCD_FONT_HEIGHT] + c.offset) / 8);

	// Spacing
	spacing += 1;

	// Color selection
	if (color)
		c.color = true;
	else
		c.color = false;

	while (*string != 0)
	{
		c.ch = *string++;
		tmp = GLCD_DrawChar(c, font);

		// Leave a single space between characters
		c.x = tmp.x2 + spacing;
	}

	ret.x2 = tmp.x2;
	ret.y2 = tmp.y2;

	return ret;
}

/**
 * -----------------------------------------------------------------------------
 * @brief			Draws a char in the screen
 * -----------------------------------------------------------------------------
 * @param c			Character settings variable
 * @param font		Text font to use
 * @return			Text box dimensions
 * -----------------------------------------------------------------------------
 */
TextBox_t GLCD_DrawChar(CharSettings_t c, const unsigned char *font)
{
	uint8_t i;
	uint8_t j;
	uint8_t charWidth;
	uint8_t posPage;
	uint8_t drawMode;

	uint16_t posReader;
	uint16_t startingLine;

	uint32_t buffer;
	TextBox_t ret;

	ret.x1 = c.x;
	ret.y1 = c.y;
	ret.x2 = c.x;
	ret.y2 = c.y;

	// Check boundaries
	if (c.x >= LCD_SCREEN_WIDTH)
		return ret;

	if (c.y >= LCD_SCREEN_HEIGHT)
		return ret;

	// Set character position page
	posPage = (c.y / 8);

	// Check if the character is included in the font file
	if ((c.ch - font[GLCD_FONT_FIRST_CH]) > font[GLCD_FONT_NUMBER_OF_CH])
		return ret;

	// Position of the passed character in the font array.
	posReader = (c.ch - font[GLCD_FONT_FIRST_CH]) * font[GLCD_FONT_CH_SIZE] + GLCD_FONT_START;

	// Read first byte from this position, this gives letter width
	charWidth = font[posReader];

	// Point to the start of char
	posReader++;

	// Draw the character from left (first column) to right (last column).
	for (i = 0; i < charWidth; i++)
	{
		// Clear buffer
		buffer = 0;

		// Point to the beginning of the character column.
		startingLine = posReader + (i * c.charPages);

		// Load a column of the character into the buffer.
		for (j = 0; j < c.charPages; j++)
			buffer |= (uint32_t) (font[startingLine + j] << (j * 8));

		// Apply vertical offset
		buffer = (buffer << c.offset);

		// Color selection
		if (c.color)
			drawMode = 0;
		else
			drawMode = 3;

		// Draw a column of the character (top to bottom)
		for (j = 0; j < c.totalPages; j++)
			LCD_Byte((uint8_t) (buffer >> (j * 8)), c.x + i, posPage + j, drawMode);
	}

	ret.y1 += c.offset;
	ret.y2 += ret.y1 + c.height;
	ret.x2 += charWidth;

	return ret;
}

/**
 * -----------------------------------------------------------------------------
 * @brief 			Displays a bitmap the full size of the screen.
 * 					Using DMA for the transfer.
 * -----------------------------------------------------------------------------
 * @param bitmap 	Bitmap to drawn (128x64 bits - 1024 bytes)
 * -----------------------------------------------------------------------------
 */
void GLCD_ScreenBitmap(const unsigned char bitmap[])
{
	// Mark all pages as dirty
	LCD_DirtyPages = 0xFF;

	// Size check
	if (sizeof(&bitmap) > LCD_SCREEN_RAM)
		return;

	// Clear transfer complete flag
	DMA2->LIFCR |= DMA_LISR_TCIF0;

	// Set memory transfer size
	DMA2_Stream0->NDTR = (LCD_SCREEN_WIDTH * LCD_SCREEN_HEIGHT) / 8;

	// Set source
	DMA2_Stream0->PAR = (uint32_t) bitmap;

	// Set destination
	DMA2_Stream0->M0AR = (uint32_t) LCD_Buffer;

	// Start transfer
	DMA_Stream_Enable(DMA2_Stream0);
}

/**
 * -----------------------------------------------------------------------------
 * @brief			Displays a bitmap on a portion of the screen.
 * 					The LCD screen dimensions must be 128x64.
 * -----------------------------------------------------------------------------
 * @param x			Bitmap horizontal position (index 0)
 * @param y			Bitmap vertical position (index 0)
 * @param bitmap	Bitmap to drawn
 * @param mode		0 = OR mode, 1 = overwrite mode
 * -----------------------------------------------------------------------------
 */
void GLCD_PutBitmap(uint8_t x, uint8_t y, const unsigned char bitmap[], bool mode)
{
	uint8_t width = bitmap[0];
	uint8_t height = bitmap[1];
	uint8_t startPage;
	uint8_t vertOffset;
	uint8_t bitmapPages;

	uint8_t ix;
	uint8_t ipage;

	uint8_t const ARRAY_OFFSET = 2;
	uint64_t buffer;

	// Horizontal limit checking
	if (x > LCD_SCREEN_WIDTH)
		return;

	// Vertical limit checking
	if (y > LCD_SCREEN_HEIGHT)
		return;

	// Bitmap start page
	startPage = y / 8;

	// Bitmap number of pages
	if (height % 8)
		bitmapPages = (height / 8) + 1;
	else
		bitmapPages = (height / 8);

	// Bitmap vertical offset
	vertOffset = y % 8;

	// Clear the bitmap print area
	if (mode)
		GLCD_DrawBox(x, y, x + width - 1, y + height, 0);

	// Draw bitmap from left to right
	for (ix = 0; ix < width; ix++)
	{
		// Clear buffer
		buffer = 0;

		// Load a column of the bitmap into the buffer
		for (ipage = 0; ipage < bitmapPages; ipage++)
			buffer |= (uint64_t) (bitmap[(ipage * width) + ix + ARRAY_OFFSET]) << (ipage * 8);

		// Apply vertical offset
		buffer = buffer << ((startPage * 8) + vertOffset);

		// Draw a column of the bitmap (top to bottom)
		for (ipage = 0; ipage <= (bitmapPages + startPage); ipage++)
			LCD_Byte((uint8_t) (buffer >> (ipage * 8)), x + ix, ipage,
			LCD_WRITE_MODE_OR);
	}
}

/**
 * -----------------------------------------------------------------------------
 * @brief			Displays a bitmap on a portion of the screen.
 * 					Faster than LCD_PutBitmap() method.
 * -----------------------------------------------------------------------------
 * @param column	Bitmap start column (index 0)
 * @param page		Bitmap start page (index 0)
 * @param bitmap	Bitmap to drawn
 * -----------------------------------------------------------------------------
 */
void GLCD_PutTile(uint8_t column, uint8_t page, const uint8_t bitmap[])
{
	uint8_t width = bitmap[0];
	uint8_t height;
	uint8_t posPage;
	uint16_t bitmapPos;
	uint8_t const arrayOffset = 2;

	// Column limit checking
	if ((column + width) > LCD_SCREEN_WIDTH)
		return;

	// Determine the number of pages in the bitmap.
	if (bitmap[1] % 8)
		height = (bitmap[1] / 8) + 1;
	else
		height = (bitmap[1] / 8);

	// Page limit checking
	if ((page + height) > LCD_SCREEN_PAGES)
		return;

	// Draw from top to bottom
	for (uint8_t ipage = 0; ipage < height; ipage++)
	{
		posPage = ipage + page;

		// Mark dirty pages
		LCD_DirtyPages |= 1 << posPage;

		// Draw from left to right
		for (uint8_t icol = 0; icol < width; icol++)
		{
			bitmapPos = (ipage * width) + icol + arrayOffset;
			LCD_Byte(bitmap[bitmapPos], column + icol, posPage, 1);
		}
	}
}

/**
 * -----------------------------------------------------------------------------
 * @brief				Draw a box
 * -----------------------------------------------------------------------------
 * @param x1 			Box x1 coordinate (index = 0)
 * @param y1			Box y1 coordinate (index = 0)
 * @param x2			Box x2 coordinate (index = 0)
 * @param y2			Box y2 coordinate (index = 0)
 * @param boxType		0 = white, 1 = black, 2 = reverse (XOR operation)
 * -----------------------------------------------------------------------------
 */
void GLCD_DrawBox(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, BoxType_e boxType)
{
	uint8_t width;
	uint8_t height;
	uint8_t boxPages;
	uint8_t boxPixels;
	uint8_t totalPages;
	uint8_t ix;
	uint8_t ipage;
	uint64_t buffer;

	// Check the horizontal limits
	if (x1 > LCD_SCREEN_WIDTH)
		return;

	// check the vertical limits
	if (y1 > LCD_SCREEN_HEIGHT)
		return;

	// Check horizontal coordinates
	if (x2 > x1)
		width = (x2 - x1) + 1;
	else
		return;

	// Check vertical coordinates
	if (y2 > y1)
		height = (y2 - y1) + 1;
	else
		return;

	// Box whole number of pages
	boxPages = (height / 8);

	// Box number of pixels
	boxPixels = height % 8;

	// Total box height = boxPages + boxPixels

	// Total amount of pages
	if (y2 % 8)
		totalPages = y2 / 8;
	else
		totalPages = (y2 / 8) + 1;

	// Draw box from left to right
	for (ix = 0; ix < width; ix++)
	{
		// Clear buffer
		buffer = 0;

		// Draw a column of the box
		for (ipage = 0; ipage < boxPages; ipage++)
			buffer |= (uint64_t) 0xFF << (ipage * 8);
		if (boxPixels)
		{
			buffer = buffer << boxPixels;

			// Technique to generate a binary number composed of ones
			// (1 << positions) - 1
			buffer |= (0x01 << boxPixels) - 1;
		}

		// Apply vertical offset
		buffer = (buffer << y1);

		// Apply color
		if (boxType == GLCD_BOXTYPE_WHITE)
			buffer ^= 0xFFFFFFFFFFFFFFFF;

		// Draw a column of the bitmap (top to bottom)
		for (ipage = 0; ipage <= totalPages; ipage++)
			switch (boxType)
			{
			case GLCD_BOXTYPE_WHITE:
				LCD_Byte((uint8_t) (buffer >> (ipage * 8)), x1 + ix, ipage,
				LCD_WRITE_MODE_AND);
				break;
			case GLCD_BOXTYPE_BLACK:
				LCD_Byte((uint8_t) (buffer >> (ipage * 8)), x1 + ix, ipage,
				LCD_WRITE_MODE_OR);
				break;
			case GLCD_BOXTYPE_REVERSE:
				LCD_Byte((uint8_t) (buffer >> (ipage * 8)), x1 + ix, ipage,
				LCD_WRITE_MODE_XOR);
				break;
			}
	}
}

/**
 * -----------------------------------------------------------------------------
 * @brief 			Draw a progress bar on the screen
 * -----------------------------------------------------------------------------
 * @param x1		X1 Coordinate
 * @param y1		Y1 Coordinate
 * @param x2		X2 Coordinate
 * @param y2		Y2 Coordinate
 * @param value		Progress bar value (0 - 100)
 * -----------------------------------------------------------------------------
 */
void GLCD_ProgressBar(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t value)
{
	uint16_t barValue;
	uint8_t len;

	if (x1 >= x2)
		return;

	if (value > 100)
		value = 100;

	if (x2 >= LCD_SCREEN_WIDTH)
		x2 = LCD_SCREEN_WIDTH;

	len = (x2 - x1) + 1;

	// Calculating the progress bar percentage
	barValue = (uint8_t) ((uint32_t) ((value * len) / 100));

	GLCD_DrawBox(x1, y1, barValue + x1, y2, 2);
}

/**
 * -----------------------------------------------------------------------------
 * @brief				Draw a line
 * -----------------------------------------------------------------------------
 * @param x1			Start point (x-axis)
 * @param y1			Start point (y-axis)
 * @param x2			End point (x-axis)
 * @param y2			End point (y-axis)
 * @param color			0 = white, 1 = black.
 * -----------------------------------------------------------------------------
 */
void GLCD_DrawLine(int x1, int y1, int x2, int y2, bool color)
{
	int xinc1, yinc1, den, num, numadd, numpixels, curpixel, xinc2, yinc2;

	if ((x2 <= x1) || (y2 <= y1))
		return;

	int deltax = x2 - x1;    	// The difference between the x's
	int deltay = y2 - y1;    	// The difference between the y's
	int x = x1;                 // Start x off at the first pixel
	int y = y1;                 // Start y off at the first pixel

	if (x2 >= x1)
	{
		// The x-values are increasing
		xinc1 = 1;
		xinc2 = 1;
	}
	else
	{
		// The x-values are decreasing
		xinc1 = -1;
		xinc2 = -1;
	}

	if (y2 >= y1)
	{
		// The y-values are increasing
		yinc1 = 1;
		yinc2 = 1;
	}
	else
	{
		// The y-values are decreasing
		yinc1 = -1;
		yinc2 = -1;
	}

	if (deltax >= deltay)     // There is at least one x-value for every y-value
	{
		xinc1 = 0;           // Don't change the x when numerator >= denominator
		yinc2 = 0;              	// Don't change the y for every iteration
		den = deltax;
		num = deltax / 2;
		numadd = deltay;
		numpixels = deltax;     	// There are more x-values than y-values
	}
	else                    // There is at least one y-value for every x-value
	{
		xinc2 = 0;              	// Don't change the x for every iteration
		yinc1 = 0;           // Don't change the y when numerator >= denominator
		den = deltay;
		num = deltay / 2;
		numadd = deltax;
		numpixels = deltay;     	// There are more y-values than x-values
	}

	for (curpixel = 0; curpixel <= numpixels; curpixel++)
	{
		LCD_Pixel(x, y, color);    	// Draw the current pixel
		num += numadd;      // Increase the numerator by the top of the fraction
		if (num >= den)         	// Check if numerator >= denominator
		{
			num -= den;           	// Calculate the new numerator value
			x += xinc1;           	// Change the x as appropriate
			y += yinc1;           	// Change the y as appropriate
		}
		x += xinc2;             	// Change the x as appropriate
		y += yinc2;             	// Change the y as appropriate
	}
}
