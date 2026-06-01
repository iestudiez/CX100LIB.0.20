/**
 ******************************************************************************
 * @file		: adc.h
 * @author      : Estudiez, Ivan
 * @brief       : STM32F405 ADC LIB
 * @date		: 18/01/2024
 * @version		: 1.0
 ******************************************************************************
 *
 *
 *
 *
 *
 ******************************************************************************
 */
#ifndef ADC_H_
#define ADC_H_

#include "stm32f4xx.h"
#include <stdint.h>
#include <stdbool.h>
#include "gpio.h"

typedef enum
{
	ADC_IN0 = 0,
	ADC_IN1 = 1,
	ADC_IN2 = 2,
	ADC_IN3 = 3,
	ADC_IN4 = 4,
	ADC_IN5 = 5,
	ADC_IN6 = 6,
	ADC_IN7 = 7,
	ADC_IN8 = 8,
	ADC_IN9 = 9,
	ADC_IN10 = 10,
	ADC_IN11 = 11,
	ADC_IN12 = 12,
	ADC_IN13 = 13,
	ADC_IN14 = 14,
	ADC_IN15 = 15,
	ADC_IN16 = 16,
	ADC_IN17 = 17,
	ADC_IN18 = 18,
} Channels_Enum;

typedef enum
{
	TRG_DISABLED = 0,
	TRG_RISING_EDGE = 1,
	TRG_FALLING_EDGE = 2,
	TRG_BOTH_EDGES = 3
} Exten_Enum;

typedef enum
{
	TIM1_CC1_EVENT = 0,
	TIM1_CC2_EVENT = 1,
	TIM1_CC3_EVENT = 2,
	TIM2_CC2_EVENT = 3,
	TIM2_CC3_EVENT = 4,
	TIM2_CC4_EVENT = 5,
	TIM2_TRGO_EVENT = 6,
	TIM3_CC1_EVENT = 7,
	TIM3_TRGO_EVENT = 8,
	TIM4_CC4_EVENT = 9,
	TIM5_CC1_EVENT = 10,
	TIM5_CC2_EVENT = 11,
	TIM5_CC3_EVENT = 12,
	TIM8_CC1_EVENT = 13,
	TIM8_TRGO_EVENT = 14,
	EXTI_LINE11 = 15
}Extsel_Enum;

typedef struct
{
	ADC_TypeDef *module;
	uint8_t len;
	Channels_Enum sequence[15];
	bool scan;
	bool dma;
	bool dds;
	bool cont;
	Exten_Enum exten;
	Extsel_Enum extsel;
} ADC_Config_t;

void ADC_Init(ADC_Config_t adc);
void ADC_Enable(ADC_TypeDef *adc);
void ADC_Disable(ADC_TypeDef *adc);
void ADC_Start(ADC_TypeDef *adc);
void ADC_Config_Reset(ADC_Config_t *adc);

#endif /* ADC_H_ */
