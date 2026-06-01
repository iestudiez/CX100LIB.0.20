/**
 ******************************************************************************
 * @file        : lcd.c
 * @author      : Estudiez, Ivan
 * @brief       : ST7565 Display Library
 * @date        : 12/06/2024
 * @version     : 1.0
 ******************************************************************************
 *
 *
 *
 *
 *
 ******************************************************************************
 */

#include <lcd.h>
#include <system_cx100.h>
#include "spi.h"
#include "dma.h"

// Public variables
// ----------------------------------------------------------------------------
uint8_t LCD_Buffer[LCD_SCREEN_WIDTH * LCD_SCREEN_HEIGHT / LCD_SCREEN_PAGES];
uint8_t LCD_DirtyPages = 0;

// Private variables
// ----------------------------------------------------------------------------
uint8_t gCurrentPage = 0;

// Function Prototypes
// ----------------------------------------------------------------------------
void LCD_SendPage(uint8_t page);

/**
 * -----------------------------------------------------------------------------
 * @brief 		LCD Chip Select
 * -----------------------------------------------------------------------------
 * @param s
 *  ----------------------------------------------------------------------------
 */
void LCD_CS(bool s)
{
	if (SPI1->SR & SPI_SR_BSY)
		return;

	if (s)
		LCD_ST7565_CS_PORT->ODR &= ~(1U << LCD_ST7565_CS_PIN);
	else
		LCD_ST7565_CS_PORT->ODR |= (1U << LCD_ST7565_CS_PIN);
}

/**
 * -----------------------------------------------------------------------------
 * @brief 		LCD Register Select
 * -----------------------------------------------------------------------------
 * @param s
 * -----------------------------------------------------------------------------
 */
void LCD_RS(bool s)
{
	if (s)
		LCD_ST7565_RS_PORT->ODR |= (1U << LCD_ST7565_RS_PIN);
	else
		LCD_ST7565_RS_PORT->ODR &= ~(1U << LCD_ST7565_RS_PIN);
}

// -----------------------------------------------------------------------------
// LCD_Data()
// ----------
// Send Data
// -----------------------------------------------------------------------------

/**
 * -----------------------------------------------------------------------------
 * @brief 		Send data to the LCD
 * -----------------------------------------------------------------------------
 * @param data	Data byte to be send
 * -----------------------------------------------------------------------------
 */
void LCD_Data(uint8_t data)
{
	LCD_CS(true);
	LCD_RS(true);
	SPI_Transmit(SPI1, &data, 1);
	LCD_CS(false);
}

/**
 * -----------------------------------------------------------------------------
 * @brief 				Send a command to LCD
 * -----------------------------------------------------------------------------
 * @param command		Command byte
 * -----------------------------------------------------------------------------
 */
void LCD_Cmd(uint8_t command)
{
	LCD_CS(true);
	LCD_RS(false);
	SPI_Transmit(SPI1, &command, 1);
	LCD_CS(false);
}

/**
 * -----------------------------------------------------------------------------
 * @brief 			Hardware Display Reset
 * -----------------------------------------------------------------------------
 * @param rst
 * -----------------------------------------------------------------------------
 */
void LCD_Reset(bool rst)
{
	if (rst)
		GPIO_Write(LCD_ST7565_RST_PORT, LCD_ST7565_RST_PIN, false);
	else
		GPIO_Write(LCD_ST7565_RST_PORT, LCD_ST7565_RST_PIN, true);
}

/**
 * -----------------------------------------------------------------------------
 * @brief 			Clear Display
 * -----------------------------------------------------------------------------
 */
void LCD_Clear(void)
{
	uint8_t j;
	uint8_t page = 0;

	// Display OFF
	LCD_Cmd(LCD_CMD__DISPLAY_OFF);

	// Display start address + 0x40
	LCD_Cmd(LCD_CMD__DISPLAY_START);

	// 64 pixel display (8 pixels per page = 8 pages)
	for (page = 0; page < 8; page++)
	{
		LCD_SetPage(page);
		LCD_SetColumn(0);
		// 128 Columns wide
		for (j = 0; j < 128; j++)
		{
			LCD_Data(0x00);
		}
	}
	LCD_Cmd(LCD_CMD__DISPLAY_ON);
}

/**
 * -----------------------------------------------------------------------------
 * @brief 			Set Page
 * -----------------------------------------------------------------------------
 * @param cAdr
 * -----------------------------------------------------------------------------
 */
void LCD_SetPage(uint8_t pAdr)
{
	if (pAdr > 7)
		return;
	LCD_Cmd(0xB0 | pAdr);
}

/**
 * -----------------------------------------------------------------------------
 * @brief 			Set Column
 * -----------------------------------------------------------------------------
 * @param cAdr
 * -----------------------------------------------------------------------------
 */
void LCD_SetColumn(uint8_t cAdr)
{
	if (cAdr > 131)
		return;

	LCD_Cmd(LCD_CMD__SET_COL_UPPER | (cAdr >> 4U));
	LCD_Cmd(LCD_CMD__SET_COL_LOWER | (cAdr & 0x0F));
}

/**
 * -----------------------------------------------------------------------------
 * @brief 	Display Initialization Routine
 * -----------------------------------------------------------------------------
 */
void LCD_Init(void)
{
	LCD_Reset(true);
	SYSTEM_Delay(4000);
	LCD_Reset(false);

	// Segment Driver Direction Select
	LCD_Cmd(LCD_CMD__ADC_NORMAL);

	LCD_Cmd(LCD_CMD__DISPLAY_OFF);

	// COM direction scan
	LCD_Cmd(0xC8);

	// LCD bias set
	LCD_Cmd(0xA2);

	// Power Control set
	LCD_Cmd(0x2F);

	// Resistor Ratio Set
	LCD_Cmd(LCD_CMD__RES_RATIO | 0x06);

	// Set Contrast (2 byte command - Value: 0 to 64)
	LCD_Cmd(LCD_CMD__SET_CONTRAST);
	LCD_Cmd(0x0F);

	// Display ON
	LCD_Cmd(LCD_CMD__DISPLAY_ON);
	LCD_Cmd(LCD_CMD__DISPLAY_START);
}

/**
 * -----------------------------------------------------------------------------
 * @brief 				Set the screen contrast
 * -----------------------------------------------------------------------------
 * @param resRatio
 * @param contrast
 * -----------------------------------------------------------------------------
 */
void LCD_Contrast(uint8_t resRatio, uint8_t contrast)
{
	if (resRatio > 7 || contrast > 63)
		return;

	LCD_Cmd(LCD_CMD__RES_RATIO | resRatio);
	LCD_Cmd(LCD_CMD__SET_CONTRAST);
	LCD_Cmd(contrast);
}

/**
 * -----------------------------------------------------------------------------
 * @brief 			Write a pixel to the buffer
 * -----------------------------------------------------------------------------
 * @param x			Horizontal position
 * @param y			Vertical position
 * @param color		Color of the pixel 0 = white, 1 = black
 * -----------------------------------------------------------------------------
 */
void LCD_Pixel(uint8_t x, uint8_t y, bool color)
{
	if (x >= LCD_SCREEN_WIDTH || y >= LCD_SCREEN_HEIGHT)
		return;

	uint16_t array_pos = x + ((y / LCD_SCREEN_PAGES) * LCD_SCREEN_WIDTH);

	// Mark page as dirty
	LCD_DirtyPages |= 1 << (array_pos / LCD_SCREEN_WIDTH);

	if (color)
		LCD_Buffer[array_pos] |= 1 << (y % LCD_SCREEN_PAGES);
	else
		LCD_Buffer[array_pos] &= 0xFF ^ 1 << (y % LCD_SCREEN_PAGES);
}

/**
 *  -----------------------------------------------------------------------------
 * @brief 		Invert the value of a pixel in the buffer.
 *  -----------------------------------------------------------------------------
 * @param x		Horizontal position
 * @param y		Vertical position
 *  -----------------------------------------------------------------------------
 */
void LCD_InvertPixel(uint8_t x, uint8_t y)
{
	if (x >= LCD_SCREEN_WIDTH || y >= LCD_SCREEN_HEIGHT)
		return;

	uint16_t array_pos = x + ((y / LCD_SCREEN_PAGES) * LCD_SCREEN_WIDTH);

	// Mark page as dirty
	LCD_DirtyPages |= 1 << (array_pos / LCD_SCREEN_WIDTH);

	// Invert pixel
	LCD_Buffer[array_pos] ^= 1U << (y % LCD_SCREEN_PAGES);
}

/**
 * -----------------------------------------------------------------------------
 * @brief 			Write a byte to the LDC buffer
 * -----------------------------------------------------------------------------
 * @param data		Data byte
 * @param col		Column
 * @param page		Page
 * @param mode		Mode ( 0 = OR mode , 1 = overwrite, 2 = AND, 3 = XOR)
 * -----------------------------------------------------------------------------
 */
void LCD_Byte(uint8_t data, uint8_t col, uint8_t page, uint8_t mode)
{
	if (col >= LCD_SCREEN_WIDTH)
		return;

	if (page >= LCD_SCREEN_PAGES)
		return;

	// Mark page as dirty
	LCD_DirtyPages |= 1 << page;

	// Write data to the buffer

	switch (mode)
	{
	// OR mode
	case 0:
		LCD_Buffer[(page * LCD_SCREEN_WIDTH) + col] |= data;
		break;

	// Overwrite mode
	case 1:
		LCD_Buffer[(page * LCD_SCREEN_WIDTH) + col] = data;
		break;

	// AND mode
	case 2:
		LCD_Buffer[(page * LCD_SCREEN_WIDTH) + col] &= data;
		break;

	// XOR mode
	case 3:
		LCD_Buffer[(page * LCD_SCREEN_WIDTH) + col] ^= data;
		break;
	}
}

/**
 * -----------------------------------------------------------------------------
 * @brief 		Send LCD Buffer to Display
 * -----------------------------------------------------------------------------
 */
void LCD_Update(void)
{
	// Do not execute if LCD buffer is locked
	if (SPI1->SR & SPI_SR_BSY)
	{
		LCD_CS(false);
		return;
	}

	// Scan for a dirty page to send to the screen:
	// If the current page is not a dirty page and is less than the number of
	// pages on the LCD, increment the counter
	while (!(LCD_DirtyPages & (1U << gCurrentPage)) && (gCurrentPage < LCD_SCREEN_PAGES))
	{
		gCurrentPage++;
	}

	// If the counter is equal to the number of pages on the LCD,
	// no page should be sent
	if (gCurrentPage == LCD_SCREEN_PAGES)
	{
		gCurrentPage = 0;
		LCD_DirtyPages = 0;
		return;
	}

	// Send page to display
	LCD_SendPage(gCurrentPage);

	// Increment the counter for the next cycle if it is less than (LCD_SCREEN_PAGES - 1),
	// otherwise reset it along with the gDirtyPages variable
	if (gCurrentPage < (LCD_SCREEN_PAGES - 1))
		gCurrentPage++;
	else
	{
		gCurrentPage = 0;
		LCD_DirtyPages = 0;
	}
}

/**
 * -----------------------------------------------------------------------------
 * @brief			Sends a page from the LCD buffer to the display
 * -----------------------------------------------------------------------------
 * @param page 		Page to be sent
 * -----------------------------------------------------------------------------
 */
void LCD_SendPage(uint8_t page)
{
	// Variable to store the send page and column command
	uint8_t gotoPos[3];

	gotoPos[0] = (0xB0 | page);
	gotoPos[1] = LCD_CMD__SET_COL_UPPER;
	gotoPos[2] = LCD_CMD__SET_COL_LOWER;

	// Enable LCD chip select
	LCD_CS(true);

	// Command type transmission
	LCD_RS(false);

	// Transmit page and column position
	SPI_Transmit(SPI1, gotoPos, 3);

	// Data type transmission
	LCD_RS(true);

	// Clear transfer complete flag
	DMA2->LIFCR |= DMA_LISR_TCIF3;

	// Send page data
	DMA2_Stream3->NDTR = (uint32_t) LCD_SCREEN_WIDTH;
	DMA2_Stream3->M0AR = (uint32_t) (&(LCD_Buffer[page * LCD_SCREEN_WIDTH]));
	DMA_Stream_Enable(DMA2_Stream3);
}

/**
 * -----------------------------------------------------------------------------
 * @brief 	Clear LCD Buffer
 * -----------------------------------------------------------------------------
 */
void LCD_ClearBuffer(void)
{
	for (int n = 0; n < (LCD_SCREEN_WIDTH * LCD_SCREEN_HEIGHT) / 8; n++)
		LCD_Buffer[n] = 0x00;
	LCD_DirtyPages = 0xFF;
}

