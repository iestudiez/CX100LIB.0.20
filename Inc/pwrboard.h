/**
 ******************************************************************************
 * @file        : pwrboard.h
 * @author      : Estudiez, Ivan
 * @brief       : Power board management
 * @date        : 27/10/2025
 * @version     : 1.0
 ******************************************************************************
 */

#ifndef INC_PWRBOARD_H_
#define INC_PWRBOARD_H_

#include <stdbool.h>

// ----------------------------------------------------------------------------
// Defines
// ----------------------------------------------------------------------------
#define PWRBOARD_FREQ_TIMEOUT					(200U)  // 2000 micro-sec
#define PWR_OFF									(0U)
#define PWR_ON									(1U)

// ----------------------------------------------------------------------------
// Type Definitions
// ----------------------------------------------------------------------------
typedef enum InputMode_e
{
	INPUT_COUNTER_MODE = 0,
	INPUT_FREQUENCY_MODE = 1
} InputMode_e;

typedef struct InputDef_t
{
	InputMode_e Mode;
	uint32_t Counter;
	uint32_t Freq;
	uint32_t Rpm;
	uint16_t Div;
	uint16_t Timeout;
	bool Reset;
} InputDef_t;

typedef struct BoardStatusDef_t
{
	uint16_t VBus;
	uint16_t Current;
	uint16_t Temp;
	uint16_t Power;
	bool Alert;
} BoardStatusDef_t;

typedef struct PwrSupplyDef_t
{
	bool External;
	bool Internal;
} PwrSupplyDef_t;

typedef struct OutputDef_t
{
	uint16_t DutyCycle;
	uint16_t Current;
} OutputDef_t;

typedef struct BuzzerDef_t
{
	bool Loud;
	bool Enable;
} BuzzerDef_t;

typedef struct InputConfDef_t
{
	bool Ain1En;
	bool CurrLoop0En;
	bool CurrLoop1En;
	bool Din0En;
	bool Din1En;
} InputConfDef_t;

typedef struct PwrBoardDef_t
{
	InputDef_t Freq[4];
	bool Din[2];
	uint16_t Ain[2];
	OutputDef_t Out[4];
	BoardStatusDef_t Status;
	PwrSupplyDef_t SensorSupply;
	BuzzerDef_t Buzzer;
	InputConfDef_t InputsConfig;
} PwrBoardDef_t;

// ----------------------------------------------------------------------------
// Public variables
// ----------------------------------------------------------------------------
extern PwrBoardDef_t PowerBoard;

// ----------------------------------------------------------------------------
// Public functions
// ----------------------------------------------------------------------------
void PWRBOARD_Init(void);
void PWRBOARD_Update(void);

#endif /* INC_PWRBOARD_H_ */
