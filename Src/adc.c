/**
 ******************************************************************************
 * @file		: adc.c
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

#include "adc.h"

void ADC_Init(ADC_Config_t adc)
{
	// Enable clock access to ADC module
	// ---------------------------------
	if (adc.module == ADC1)
		RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;

	if (adc.module == ADC2)
		RCC->APB2ENR |= RCC_APB2ENR_ADC2EN;

	if (adc.module == ADC3)
		RCC->APB2ENR |= RCC_APB2ENR_ADC3EN;

	// Clear sequencer registers
	adc.module->SQR1 = 0;
	adc.module->SQR2 = 0;
	adc.module->SQR3 = 0;

	// Set sequence length
	adc.module->SQR1 &= ~(0b1111 << ADC_SQR1_L_Pos);
	if (adc.len > 0)
		adc.module->SQR1 |= ((0x0F & (adc.len - 1)) << ADC_SQR1_L_Pos);

	// Fill sequence registers with the selected channels
	// --------------------------------------------------
	for (int i = 0; i < 6; i++)
	{
		adc.module->SQR3 |= (adc.sequence[i] << (i * 5));
	}
	for (int i = 0; i < 6; i++)
	{
		adc.module->SQR2 |= (adc.sequence[i + 5] << (i * 5));
	}
	for (int i = 0; i < 4; i++)
	{
		adc.module->SQR1 |= (adc.sequence[i + 10] << (i * 5));
	}

	// Enable scan mode
	if (adc.scan)
		adc.module->CR1 |= ADC_CR1_SCAN;

	// Select to use DMA
	if (adc.dma)
		adc.module->CR2 |= ADC_CR2_DMA;

	// Enable continuous mode
	if (adc.cont)
		adc.module->CR2 |= ADC_CR2_CONT;

	// DMA disable selection (for single ADC mode)
	//--------------------------------------------
	if (adc.dds)
		adc.module->CR2 |= ADC_CR2_DDS;
	// If DMA and continuous mode are selected, enable DDS mode
	// (DMA requests are emitted as long as data are converted)
	if (adc.cont && adc.dma)
		adc.module->CR2 |= ADC_CR2_DDS;

	// External trigger enable for regular channels
	adc.module->CR2 &= ~(0b11 << ADC_CR2_EXTEN_Pos);
	adc.module->CR2 |= (adc.exten << ADC_CR2_EXTEN_Pos);

	// External event select for a regular group
	adc.module->CR2 &= ~(0b1111 << ADC_CR2_EXTSEL_Pos);
	adc.module->CR2 |= (adc.extsel << ADC_CR2_EXTSEL_Pos);
}

// Enable ADC
// ------------------------------
void ADC_Enable(ADC_TypeDef *adc)
{
	adc->CR2 |= ADC_CR2_ADON;
}

// Disable ADC
// ------------------------------
void ADC_Disable(ADC_TypeDef *adc)
{
	adc->CR2 &= ~ADC_CR2_ADON;
}

// ADC Software Start
void ADC_Start(ADC_TypeDef *adc)
{
	adc->CR2 |= ADC_CR2_SWSTART;
}

// ADC Configuration Structure Reset
void ADC_Config_Reset(ADC_Config_t *adc)
{

	adc->len = 1;
	adc->scan = false;
	adc->dma = false;
	adc->dds = false;
	adc->cont = false;
	for (int i = 0; i < 16; i++)
		adc->sequence[i] = ADC_IN0;
}

