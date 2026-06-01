/*
 ******************************************************************************
 * @file        : keyboard.h
 * @author      : Estudiez, Ivan
 * @brief       : Keyboard
 * @date        : 19/07/2024
 * @version     : 1.0
 ******************************************************************************
 *
 *
 *
 *
 *
 ******************************************************************************
 */
#ifndef KEYBOARD_H_
#define KEYBOARD_H_

#include <stdint.h>

typedef enum
{
	KEY_NONE = 0,
	KEY_N1 = 1,
	KEY_N2_UP = 2,
	KEY_N3 = 3,
	KEY_N4_LEFT = 4,
	KEY_N5_DOWN = 5,
	KEY_N6_RIGHT = 6,
	KEY_N7 = 7,
	KEY_N8 = 8,
	KEY_N9 = 9,
	KEY_N0 = 10,
	KEY_ESC = 11,
	KEY_ENTER = 12,
	KEY_PWR = 13,
	KEY_BACK = 14
} KeyboardKeys_e;

typedef struct
{
	uint8_t N0;
	uint8_t N1;
	uint8_t N2_Up;
	uint8_t N3;
	uint8_t N4_Left;
	uint8_t N5_Down;
	uint8_t N6_Right;
	uint8_t N7;
	uint8_t N8;
	uint8_t N9;
	uint8_t Enter;
	uint8_t Esc;
	uint8_t Pwr;
	uint8_t Bckspc;
} KeyEventsDef_t;

extern KeyEventsDef_t KEYBOARD_Event;

void KEYBOARD_Update(void);
void KEYBOARD_Scan(void);
void KEYBOARD_AssingEvent(void);

#endif /* KEYBOARD_H_ */
