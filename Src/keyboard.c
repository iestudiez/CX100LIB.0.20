/*
 ******************************************************************************
 * @file        : keyboard.c
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

#include "keyboard.h"
#include "scheduler.h"
#include "indicators.h"
#include "gpio.h"

// Definitions
// -----------------------------------------------------------------------------
#define COLUMN1_GPIO			GPIOB
#define COLUMN1_PIN				(14U)
#define COLUMN2_GPIO			GPIOB
#define COLUMN2_PIN				(15U)
#define COLUMN3_GPIO			GPIOC
#define COLUMN3_PIN				(8U)
#define ROW1_GPIO				GPIOC
#define ROW1_PIN				(9U)
#define ROW2_GPIO				GPIOC
#define ROW2_PIN				(10U)
#define ROW3_GPIO				GPIOC
#define ROW3_PIN				(11U)
#define ROW4_GPIO				GPIOC
#define ROW4_PIN				(12U)
#define ROW5_GPIO				GPIOD
#define ROW5_PIN				(2U)

// Private variables
// -----------------------------------------------------------------------------
KeyEventsDef_t KEYBOARD_Event;
KeyboardKeys_e gKeyPressed;

// Prototypes
// -----------------------------------------------------------------------------
void keyBeep();

/**
 * -----------------------------------------------------------------------------
 * @brief
 * -----------------------------------------------------------------------------
 */
void KEYBOARD_Update(void)
{
	KEYBOARD_Scan();
	KEYBOARD_AssingEvent();
}

/**
 * -----------------------------------------------------------------------------
 * @brief
 * -----------------------------------------------------------------------------
 */
void KEYBOARD_Scan(void)
{
	gKeyPressed = KEY_NONE;

	// ------------------------------------------------
	// Scan Column 1
	// ------------------------------------------------
	GPIO_Write(COLUMN1_GPIO, COLUMN1_PIN, true);

	if (GPIO_Read(ROW5_GPIO, ROW5_PIN))
		gKeyPressed = KEY_ESC;

	if (GPIO_Read(ROW4_GPIO, ROW4_PIN))
		gKeyPressed = KEY_N1;

	if (GPIO_Read(ROW3_GPIO, ROW3_PIN))
		gKeyPressed = KEY_N4_LEFT;

	if (GPIO_Read(ROW2_GPIO, ROW2_PIN))
		gKeyPressed = KEY_N7;

	if (GPIO_Read(ROW1_GPIO, ROW1_PIN))
		gKeyPressed = KEY_PWR;

	GPIO_Write(COLUMN1_GPIO, COLUMN1_PIN, false);

	// ------------------------------------------------
	// Scan Column 2
	// ------------------------------------------------
	GPIO_Write(COLUMN2_GPIO, COLUMN2_PIN, true);

	if (GPIO_Read(ROW4_GPIO, ROW4_PIN))
		gKeyPressed = KEY_N2_UP;

	if (GPIO_Read(ROW3_GPIO, ROW3_PIN))
		gKeyPressed = KEY_N5_DOWN;

	if (GPIO_Read(ROW2_GPIO, ROW2_PIN))
		gKeyPressed = KEY_N8;

	if (GPIO_Read(ROW1_GPIO, ROW1_PIN))
		gKeyPressed = KEY_N0;

	GPIO_Write(COLUMN2_GPIO, COLUMN2_PIN, false);

	// ------------------------------------------------
	// Scan Column 3
	// ------------------------------------------------
	GPIO_Write(COLUMN3_GPIO, COLUMN3_PIN, true);

	if (GPIO_Read(ROW5_GPIO, ROW5_PIN))
		gKeyPressed = KEY_ENTER;

	if (GPIO_Read(ROW4_GPIO, ROW4_PIN))
		gKeyPressed = KEY_N3;

	if (GPIO_Read(ROW3_GPIO, ROW3_PIN))
		gKeyPressed = KEY_N6_RIGHT;

	if (GPIO_Read(ROW2_GPIO, ROW2_PIN))
		gKeyPressed = KEY_N9;

	if (GPIO_Read(ROW1_GPIO, ROW1_PIN))
		gKeyPressed = KEY_BACK;

	GPIO_Write(COLUMN3_GPIO, COLUMN3_PIN, false);
}

void KEYBOARD_AssingEvent(void)
{
	// Key pressed beep
	// -------------------------------------------------
	keyBeep();

	// Key 0 pressed
	// -------------------------------------------------
	if (gKeyPressed == KEY_N0)
		KEYBOARD_Event.N0++;
	else
		KEYBOARD_Event.N0 = 0;

	// Key 1 pressed
	// -------------------------------------------------
	if (gKeyPressed == KEY_N1)
		KEYBOARD_Event.N1++;
	else
		KEYBOARD_Event.N1 = 0;

	// Key 2 pressed
	// -------------------------------------------------
	if (gKeyPressed == KEY_N2_UP)
		KEYBOARD_Event.N2_Up++;
	else
		KEYBOARD_Event.N2_Up = 0;

	// Key 3 pressed
	// -------------------------------------------------
	if (gKeyPressed == KEY_N3)
		KEYBOARD_Event.N3++;
	else
		KEYBOARD_Event.N3 = 0;

	// Key 4 pressed
	// -------------------------------------------------
	if (gKeyPressed == KEY_N4_LEFT)
		KEYBOARD_Event.N4_Left++;
	else
		KEYBOARD_Event.N4_Left = 0;

	// Key 5 pressed
	// -------------------------------------------------
	if (gKeyPressed == KEY_N5_DOWN)
		KEYBOARD_Event.N5_Down++;
	else
		KEYBOARD_Event.N5_Down = 0;

	// Key 6 pressed
	// -------------------------------------------------
	if (gKeyPressed == KEY_N6_RIGHT)
		KEYBOARD_Event.N6_Right++;
	else
		KEYBOARD_Event.N6_Right = 0;

	// Key 7 pressed
	// -------------------------------------------------
	if (gKeyPressed == KEY_N7)
		KEYBOARD_Event.N7++;
	else
		KEYBOARD_Event.N7 = 0;

	// Key 8 pressed
	// -------------------------------------------------
	if (gKeyPressed == KEY_N8)
		KEYBOARD_Event.N8++;
	else
		KEYBOARD_Event.N8 = 0;

	// Key 9 pressed
	// -------------------------------------------------
	if (gKeyPressed == KEY_N9)
		KEYBOARD_Event.N9++;
	else
		KEYBOARD_Event.N9 = 0;

	// Key Escape pressed
	// -------------------------------------------------
	if (gKeyPressed == KEY_ESC)
		KEYBOARD_Event.Esc++;
	else
		KEYBOARD_Event.Esc = 0;

	// Key Enter pressed
	// -------------------------------------------------
	if (gKeyPressed == KEY_ENTER)
		KEYBOARD_Event.Enter++;
	else
		KEYBOARD_Event.Enter = 0;

	// Key Power pressed
	// -------------------------------------------------
	if (gKeyPressed == KEY_PWR)
		KEYBOARD_Event.Pwr++;
	else
		KEYBOARD_Event.Pwr = 0;

	// Key Backspace pressed
	// -------------------------------------------------
	if (gKeyPressed == KEY_BACK)
		KEYBOARD_Event.Bckspc++;
	else
		KEYBOARD_Event.Bckspc = 0;
}

/**
 * -----------------------------------------------------------------------------
 * @brief
 * -----------------------------------------------------------------------------
 */
void keyBeep()
{
	static uint8_t cnt = 0;

	// Exit if the alarm is sounding
	if(Indicator.Buzzer == BUZZER_ALARM)
		return;

	if (gKeyPressed)
		cnt++;
	else
		cnt = 0;

	if (cnt == 1)
		Indicator.Buzzer = BUZZER_BEEP;
}
