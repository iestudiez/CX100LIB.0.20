/*
 ******************************************************************************
 * @file        : lcd.h
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

#ifndef LCD_H_
#define LCD_H_

#include <stdbool.h>
#include <stdint.h>

// LCD Pin Definitions
// -----------------------------------------------------------------------------
#define LCD_ST7565_CS_PORT				GPIOB
#define LCD_ST7565_CS_PIN				1U
#define LCD_ST7565_RS_PORT				GPIOC
#define LCD_ST7565_RS_PIN				5U
#define LCD_ST7565_RST_PORT				GPIOC
#define LCD_ST7565_RST_PIN				4U

// LCD Properties
// -----------------------------------------------------------------------------
#define LCD_SCREEN_WIDTH				128
#define LCD_SCREEN_HEIGHT				64
#define LCD_SCREEN_PAGES				8
#define LCD_SCREEN_RAM					1024

// LCD Commands
// -----------------------------------------------------------------------------
#define LCD_CMD__DISPLAY_OFF			0xAE
#define LCD_CMD__DISPLAY_ON				0xAF
#define LCD_CMD__DISPLAY_START			0x40
#define LCD_CMD__SET_COL_UPPER			0x10
#define LCD_CMD__SET_COL_LOWER			0x00
#define LCD_CMD__DISPLAY_REVERSE		0xA7
#define LCD_CMD__DISPLAY_NORMAL			0xA6
#define LCD_CMD__ADC_NORMAL				0xA0
#define LCD_CMD__ADC_REVERSE			0xA1
#define LCD_CMD__SET_CONTRAST			0x81
#define LCD_CMD__RES_RATIO				0x20

// LCD Constants
// -----------------------------------------------------------------------------
#define LCD_WRITE_MODE_OR				0x00
#define LCD_WRITE_MODE_OVERWRITE		0x01
#define LCD_WRITE_MODE_AND				0x02
#define LCD_WRITE_MODE_XOR				0x03

// LCD Low Level Functions
// -----------------------------------------------------------------------------
void LCD_Init(void);
void LCD_CS(bool s);
void LCD_Data(uint8_t d);
void LCD_Cmd(uint8_t c);
void LCD_Reset(bool rst);
void LCD_Clear(void);
void LCD_SetPage(uint8_t pAdr);
void LCD_SetColumn(uint8_t cAdr);
void LCD_Contrast(uint8_t resRatio, uint8_t contrast);
void LCD_Pixel(uint8_t x, uint8_t y, bool colour);
void LCD_Byte(uint8_t data, uint8_t x, uint8_t page, uint8_t mode);
void LCD_InvertPixel(uint8_t x, uint8_t y);
void LCD_Update(void);
void LCD_ClearBuffer(void);
void LCD_Backlight(bool isOn);


// Public variables
// -----------------------------------------------------------------------------
extern uint8_t LCD_Buffer[LCD_SCREEN_WIDTH * LCD_SCREEN_HEIGHT / LCD_SCREEN_PAGES];
extern uint8_t LCD_DirtyPages;

#endif /* LCD_H_ */
