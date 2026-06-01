/**
 ******************************************************************************
 * @file        : indicators.c
 * @author      : Estudiez, Ivan
 * @brief       : Indicators Library
 * @date        : 07/11/2025
 * @version     : 1.0
 ******************************************************************************
 */

#include <system_cx100.h>
#include "indicators.h"
#include "pwrboard.h"
#include "global.h"

Indicators_t Indicator;

// Global variables
// -----------------------------------------------------------------------------
static bool gBuzzAlarm;

// Function prototypes
// -----------------------------------------------------------------------------
bool ledAction(LedAction_e action);
void updateGlobalVars(void);
void buzzer(void);

/**
 * -----------------------------------------------------------------------------
 * @brief
 * -----------------------------------------------------------------------------
 */
void INDICATORS_Update(void)
{
	updateGlobalVars();

	SYSTEM_PwrLed(ledAction(Indicator.Led.Power));
	SYSTEM_MsgLed(ledAction(Indicator.Led.Message));
	SYSTEM_ErrLed(ledAction(Indicator.Led.Error));
	SYSTEM_LcdRed(ledAction(Indicator.Backlight.Red));
	SYSTEM_LcdGreen(ledAction(Indicator.Backlight.Green));
	SYSTEM_LcdBlue(ledAction(Indicator.Backlight.Blue));
	buzzer();
}

/**
 * -----------------------------------------------------------------------------
 * @brief
 * -----------------------------------------------------------------------------
 */
bool ledAction(LedAction_e action)
{
	if (action == LED_ON)
		return true;

	if (action == LED_BLINK)
		return GLOBAL_Blink;

	return false;
}

/**
 * -----------------------------------------------------------------------------
 * @brief
 * -----------------------------------------------------------------------------
 */
void buzzer(void)
{
	if (Indicator.Buzzer == BUZZER_OFF)
	{
		PowerBoard.Buzzer.Enable = false;
		return;
	}

	if (Indicator.Buzzer == BUZZER_ALARM)
	{
		PowerBoard.Buzzer.Loud = true;
		PowerBoard.Buzzer.Enable = (bool) gBuzzAlarm;
	}

	if (Indicator.Buzzer == BUZZER_BEEP)
	{
		PowerBoard.Buzzer.Loud = false;
		PowerBoard.Buzzer.Enable = true;
		Indicator.Buzzer = BUZZER_OFF;
	}
}

/**
 * -----------------------------------------------------------------------------
 * @brief
 * -----------------------------------------------------------------------------
 */
void updateGlobalVars(void)
{
	static uint8_t buzzState = 0;

	// Switching variables
	GLOBAL_Blink = !GLOBAL_Blink;
	GLOBAL_Blink2 = GLOBAL_FreqDivider(GLOBAL_Blink);

	if (Indicator.Buzzer != BUZZER_ALARM)
		return;

	if (buzzState < 2)
	{
		gBuzzAlarm = true;
		buzzState++;
	}
	else
	{
		gBuzzAlarm = false;
		buzzState = 0;
	}
}

