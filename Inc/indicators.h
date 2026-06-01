/**
 ******************************************************************************
 * @file        : indicators.h
 * @author      : Estudiez, Ivan
 * @brief       : Indicators Library
 * @date        : 07/11/2025
 * @version     : 1.0
 ******************************************************************************
 *
 *
 *
 *
 *
 ******************************************************************************
 */

#ifndef INDICATORS_H_
#define INDICATORS_H_
#include <stdbool.h>

typedef enum LedAction_e
{
	LED_OFF = 0,
	LED_ON = 1,
	LED_BLINK = 2
} LedAction_e;

typedef enum BuzzerAction_e
{
	BUZZER_OFF = 0,
	BUZZER_BEEP = 1,
	BUZZER_DOUBLE_BEEP = 2,
	BUZZER_CONTINUOUS = 3,
	BUZZER_BEEP_LOUD = 4,
	BUZZER_ALARM = 5
} BuzzerAction_e;

typedef struct LedIndicator_t
{
	LedAction_e Power;
	LedAction_e Message;
	LedAction_e Error;

} LedIndicator_t;

typedef struct LcdIndicator_t
{
	LedAction_e Red;
	LedAction_e Green;
	LedAction_e Blue;
} LcdIndicator_t;

typedef struct Indicators_t
{
	LedIndicator_t Led;
	LcdIndicator_t Backlight;
	BuzzerAction_e Buzzer;
} Indicators_t;

extern Indicators_t Indicator;
void INDICATORS_Update(void);

#endif /* INDICATORS_H_ */
