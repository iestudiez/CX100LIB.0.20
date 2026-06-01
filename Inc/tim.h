/**
 ******************************************************************************
 * @file		: tim.h
 * @author      : Estudiez, Ivan
 * @brief       : STM32F405 TIMERS LIB
 * @date		: 15/08/2025
 * @version		: 1.3
 ******************************************************************************
 * Change control:
 * 1.0 - (24/01/2024) Release version
 * 1.3 - (15/08/2025) Added Master Output Enable bit (MOE) for TIM1 and TIM8
 *
 *
 ******************************************************************************
 */

#ifndef TIM_H_
#define TIM_H_

#include <stdint.h>
#include <stdbool.h>
#include "stm32f4xx.h"

// Master Mode Selection
typedef enum
{
	MMS_RESET = 0,
	MMS_ENABLE = 1,
	MMS_UPDATE = 2,
	MMS_COMP_PULSE = 3,
	MMS_COMP_OC1REF = 4,
	MMS_COMP_OC2REF = 5,
	MMS_COMP_OC3REF = 6,
	MMS_COMP_OC4REF = 7,
} MMS_e;

// Slave Mode Selection
typedef enum
{
	SMS_DISABLED = 0,
	SMS_ENCODER_MODE1 = 1,
	SMS_ENCODER_MODE2 = 2,
	SMS_ENCODER_MODE3 = 3,
	SMS_RESET_MODE = 4,
	SMS_GATED_MODE = 5,
	SMS_TRIGGER_MODE = 6,
	SMS_EXTCLOCK_MODE = 7
} SMS_e;

// Trigger Selection
typedef enum
{
	TS_ITR0 = 0,
	TS_ITR1 = 1,
	TS_ITR2 = 2,
	TS_ITR3 = 3,
	TS_TI1F_ED = 4,
	TS_TI1FP1 = 5,
	TS_TI2FP2 = 6,
	TS_ETRF = 7
} TS_e;

// Output Compare Mode
typedef enum
{
	OCM_FROZEN = 0,
	OCM_ACTIVE = 1,
	OCM_INACTIVE = 2,
	OCM_TOGGLE = 3,
	OCM_FORCE_LOW = 4,
	OCM_FORCE_HIGH = 5,
	OCM_PWM1 = 6,
	OCM_PWM2 = 7
} OCM_e;

// Capture/Compare Selection
typedef enum
{
	CCS_OUTPUT = 0,
	CCS_INPUT_FIRST_TI = 1,
	CCS_INPUT_SECOND_TI = 2,
	CCS_INPUT_TRC = 3
} CCS_e;

// Timer Channel
typedef enum
{
	TIM_CH1 = 0,
	TIM_CH2 = 1,
	TIM_CH3 = 2,
	TIM_CH4 = 3
} CH_e;

//

typedef struct
{
	TIM_TypeDef *module;
	uint16_t psc;
	uint32_t arr;
	bool arpe;
	MMS_e masterMode;
	TS_e triggerSelection;
	SMS_e slaveModeSelection;
	bool msm;
	CCS_e cc1Selection;
	CCS_e cc2Selection;
	CCS_e cc3Selection;
	CCS_e cc4Selection;
	OCM_e oc1Mode;
	OCM_e oc2Mode;
	OCM_e oc3Mode;
	OCM_e oc4Mode;
	bool cc1p;
	bool cc1np;
	bool cc2p;
	bool cc2np;
	bool cc3p;
	bool cc3np;
	bool cc4p;
	bool cc4np;
	bool cc1ien;
	bool cc2ien;
	bool cc3ien;
	bool cc4ien;
	bool uiEnable;
	bool masterOutputEnable;
} TIM_Config_t;

void TIM_Init(TIM_Config_t tim);
void TIM_Enable(TIM_TypeDef *tim);
void TIM_Disable(TIM_TypeDef *tim);
void TIM_Config_Reset(TIM_Config_t *tim);
void TIM_ChEnable(TIM_TypeDef *tim, CH_e ch, bool enable);
void TIM_Update_Generation(TIM_TypeDef *tim);

#endif /* TIM_H_ */
