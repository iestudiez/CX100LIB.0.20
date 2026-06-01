/**
 ******************************************************************************
 * @file		: tim.c
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

#include "tim.h"
#include "gpio.h"

void sysClk_Enable(TIM_Config_t tim);
void tch_Init(TIM_Config_t tim);
void tch_Enable(TIM_Config_t tim);
void sclr_Init(TIM_Config_t tim);
void mms_Init(TIM_Config_t tim);
void sm_Init(TIM_Config_t tim);
void dier_Init(TIM_Config_t tim);
void tch_Moe(TIM_Config_t tim);

void TIM_Init(TIM_Config_t tim)
{
	// Enable clock access
	sysClk_Enable(tim);

	// Set timer pre-scaler and auto-reload register
	sclr_Init(tim);

	// Configure master mode
	mms_Init(tim);

	// Channels and Slave Mode Initialization
	tch_Init(tim);

	// Master Output Enable
	tch_Moe(tim);

	// Channels DMA and Interrupt Configuration
	dier_Init(tim);

	// Enable Channels
	tch_Enable(tim);
}

// ----------------------------------------------------------------------------
// TIM_Enable_CH()
// ---------------
// Enable/disable timers channels
// ----------------------------------------------------------------------------
void TIM_ChEnable(TIM_TypeDef *tim, CH_e ch, bool enable)
{
	if (enable)
		tim->CCER |= (1UL << (ch * 4));
	else
		tim->CCER &= ~(1UL << (ch * 4));
}

// Timer Enable
// ----------------------------------------------------------------------------
void TIM_Enable(TIM_TypeDef *tim)
{
	// Enable timer
	tim->CR1 |= TIM_CR1_CEN;
}

// Timer Disable
// ----------------------------------------------------------------------------
void TIM_Disable(TIM_TypeDef *tim)
{
	tim->CR1 &= ~TIM_CR1_CEN;
}

// ----------------------------------------------------------------------------
// TIM_Update_Generation()
// -----------------------
// Timer Update Generation
// ----------------------------------------------------------------------------
void TIM_Update_Generation(TIM_TypeDef *tim)
{
	tim->EGR |= TIM_EGR_UG;
}

// Timer configuration variable reset
// ----------------------------------------------------------------------------
void TIM_Config_Reset(TIM_Config_t *tim)
{
	tim->arr = 0xFFFFFFFF;
	tim->psc = 0;
	tim->arpe = false;
	tim->masterMode = MMS_RESET;
	tim->msm = false;
	tim->slaveModeSelection = SMS_DISABLED;
	tim->triggerSelection = TS_ITR0;
	tim->cc1Selection = CCS_OUTPUT;
	tim->cc2Selection = CCS_OUTPUT;
	tim->cc3Selection = CCS_OUTPUT;
	tim->cc4Selection = CCS_OUTPUT;
	tim->oc1Mode = OCM_FROZEN;
	tim->oc2Mode = OCM_FROZEN;
	tim->oc3Mode = OCM_FROZEN;
	tim->oc4Mode = OCM_FROZEN;
	tim->cc1p = false;
	tim->cc1np = false;
	tim->cc2p = false;
	tim->cc2np = false;
	tim->cc3p = false;
	tim->cc3np = false;
	tim->cc4p = false;
	tim->cc4np = false;
	tim->cc1ien = false;
	tim->cc2ien = false;
	tim->cc3ien = false;
	tim->cc4ien = false;
	tim->uiEnable = false;
	tim->masterOutputEnable = false;
}

// System Clock Enable Section
// ----------------------------------------------------------------------------
void sysClk_Enable(TIM_Config_t tim)
{
	if (tim.module == TIM1)
		RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;

	else if (tim.module == TIM2)
		RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;

	else if (tim.module == TIM3)
		RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;

	else if (tim.module == TIM4)
		RCC->APB1ENR |= RCC_APB1ENR_TIM4EN;

	else if (tim.module == TIM5)
		RCC->APB1ENR |= RCC_APB1ENR_TIM5EN;

	else if (tim.module == TIM6)
		RCC->APB1ENR |= RCC_APB1ENR_TIM6EN;

	else if (tim.module == TIM7)
		RCC->APB1ENR |= RCC_APB1ENR_TIM7EN;

	else if (tim.module == TIM8)
		RCC->APB2ENR |= RCC_APB2ENR_TIM8EN;

	else if (tim.module == TIM9)
		RCC->APB2ENR |= RCC_APB2ENR_TIM9EN;

	else if (tim.module == TIM12)
		RCC->APB1ENR |= RCC_APB1ENR_TIM12EN;

	else if (tim.module == TIM10)
		RCC->APB2ENR |= RCC_APB2ENR_TIM10EN;

	else if (tim.module == TIM11)
		RCC->APB2ENR |= RCC_APB2ENR_TIM11EN;

	else if (tim.module == TIM13)
		RCC->APB1ENR |= RCC_APB1ENR_TIM13EN;

	else if (tim.module == TIM14)
		RCC->APB1ENR |= RCC_APB1ENR_TIM14EN;
}

// Set timer pre-scaler and auto-reload register
// ----------------------------------------------------------------------------
void sclr_Init(TIM_Config_t tim)
{
	// Auto-reload pre-load enable
	if (tim.arpe)
		tim.module->CR1 |= TIM_CR1_ARPE;
	else
		tim.module->CR1 &= ~TIM_CR1_ARPE;

	// Set timer pre-scaler and auto-reload register
	if ((tim.module == TIM2) | (tim.module == TIM3) | (tim.module == TIM4)
			| (tim.module == TIM5))
	{
		// Set Timer pre-scaler register
		tim.module->PSC = (uint16_t) tim.psc;
		// Set Timer auto-reload register
		tim.module->ARR = (uint32_t) tim.arr;
	}
	else
	{
		// Set Timer pre-scaler register
		tim.module->PSC = (uint16_t) tim.psc;
		// Set Timer auto-reload register
		tim.module->ARR = (uint16_t) tim.arr;
	}
}

// Channels and Slave Mode Initialization
// ----------------------------------------------------------------------------
void tch_Init(TIM_Config_t tim)
{
	// TIM6 - TIM7
	// -----------
	if ((tim.module == TIM6) | (tim.module == TIM7))
		return;

	// TIM10 - TIM11 - TIM13 - TIM14
	// -----------------------------
	// Capture/compare selection - CH1
	tim.module->CCMR1 &= ~TIM_CCMR1_CC1S_Msk;
	tim.module->CCMR1 |= (tim.cc1Selection << TIM_CCMR1_CC1S_Pos);

	// Output/compare mode - CH1
	tim.module->CCMR1 &= ~TIM_CCMR1_OC1M_Msk;
	tim.module->CCMR1 |= (tim.oc1Mode << TIM_CCMR1_OC1M_Pos);

	// Output/compare polarity -CH1
	if (tim.cc1p)
		tim.module->CCER |= TIM_CCER_CC1P;
	else
		tim.module->CCER &= ~TIM_CCER_CC1P;

	// Output compare complementary polarity -CH1
	if (tim.cc1np)
		tim.module->CCER |= TIM_CCER_CC1NP;
	else
		tim.module->CCER &= ~TIM_CCER_CC1NP;

	if ((tim.module == TIM10) | (tim.module == TIM11) | (tim.module == TIM13)
			| (tim.module == TIM14))
		return;

	// TIM9 - TIM12
	// ------------
	// Capture/compare selection - CH2
	tim.module->CCMR1 &= ~TIM_CCMR1_CC2S_Msk;
	tim.module->CCMR1 |= (tim.cc2Selection << TIM_CCMR1_CC2S_Pos);

	// Output/compare mode - CH2
	tim.module->CCMR1 &= ~TIM_CCMR1_OC2M_Msk;
	tim.module->CCMR1 |= (tim.oc2Mode << TIM_CCMR1_OC2M_Pos);

	// Output/compare polarity -CH2
	if (tim.cc2p)
		tim.module->CCER |= TIM_CCER_CC2P;
	else
		tim.module->CCER &= ~TIM_CCER_CC2P;

	// Output compare complementary polarity - CH2
	if (tim.cc2np)
		tim.module->CCER |= TIM_CCER_CC2NP;
	else
		tim.module->CCER &= ~TIM_CCER_CC2NP;

	if ((tim.module == TIM9) | (tim.module == TIM12))
		return;

	// TIM1 - TIM2 - TIM2 - TIM3 - TIM5 - TIM5 - TIM8
	// ----------------------------------------------
	// Capture/compare selection - CH3 and CH4
	tim.module->CCMR2 &= ~(TIM_CCMR2_CC3S_Msk | TIM_CCMR2_CC4S_Msk);
	tim.module->CCMR2 |= (tim.cc3Selection << TIM_CCMR2_CC3S_Pos);
	tim.module->CCMR2 |= (tim.cc4Selection << TIM_CCMR2_CC4S_Pos);

	// Output/compare mode - CH3 and CH4
	tim.module->CCMR2 &= ~(TIM_CCMR2_OC3M_Msk | TIM_CCMR2_OC4M_Msk);
	tim.module->CCMR2 |= (tim.oc3Mode << TIM_CCMR2_OC3M_Pos);
	tim.module->CCMR2 |= (tim.oc4Mode << TIM_CCMR2_OC4M_Pos);

	// Output/compare polarity - CH3
	if (tim.cc3p)
		tim.module->CCER |= TIM_CCER_CC3P;
	else
		tim.module->CCER &= ~TIM_CCER_CC3P;

	// Output compare complementary polarity - CH3
	if (tim.cc3np)
		tim.module->CCER |= TIM_CCER_CC3NP;
	else
		tim.module->CCER &= ~TIM_CCER_CC3NP;

	// Output/compare polarity - CH4
	if (tim.cc4p)
		tim.module->CCER |= TIM_CCER_CC4P;
	else
		tim.module->CCER &= ~TIM_CCER_CC4P;

	// Output compare complementary polarity - CH4
	if (tim.cc4np)
		tim.module->CCER |= TIM_CCER_CC4NP;
	else
		tim.module->CCER &= ~TIM_CCER_CC4NP;

	// Slave mode configuration
	sm_Init(tim);
}

// Enable channels
// ----------------------------------------------------------------------------
void tch_Enable(TIM_Config_t tim)
{
	if ((tim.oc1Mode) | (tim.cc1Selection))
		tim.module->CCER |= TIM_CCER_CC1E;
	else
		tim.module->CCER &= ~TIM_CCER_CC1E;

	if ((tim.oc2Mode) | (tim.cc2Selection))
		tim.module->CCER |= TIM_CCER_CC2E;
	else
		tim.module->CCER &= ~TIM_CCER_CC2E;

	if ((tim.oc3Mode) | (tim.cc3Selection))
		tim.module->CCER |= TIM_CCER_CC3E;
	else
		tim.module->CCER &= ~TIM_CCER_CC3E;

	if ((tim.oc4Mode) | (tim.cc4Selection))
		tim.module->CCER |= TIM_CCER_CC4E;
	else
		tim.module->CCER &= ~TIM_CCER_CC4E;
}

// Master Mode Selection
// ----------------------------------------------------------------------------
void mms_Init(TIM_Config_t tim)
{
	if ((tim.module == TIM9) | (tim.module == TIM10) | (tim.module == TIM11)
			| (tim.module == TIM12) | (tim.module == TIM13)
			| (tim.module == TIM14))
		return;
	tim.module->CR2 &= ~(0b111 << TIM_CR2_MMS_Pos);
	tim.module->CR2 |= (tim.masterMode << TIM_CR2_MMS_Pos);
}

// Slave Mode Configuration Section
// ----------------------------------------------------------------------------
void sm_Init(TIM_Config_t tim)
{
	tim.module->SMCR &= ~((0b111 << TIM_SMCR_SMS_Pos)
			| (0b111 << TIM_SMCR_TS_Pos));
	tim.module->SMCR |= (tim.slaveModeSelection << TIM_SMCR_SMS_Pos);
	tim.module->SMCR |= (tim.triggerSelection << TIM_SMCR_TS_Pos);

	// Master/Slave mode
	if (tim.msm)
		tim.module->SMCR |= TIM_SMCR_MSM;
	else
		tim.module->SMCR &= ~TIM_SMCR_MSM;
}

// DMA and Interrupt Configuration Section
// ----------------------------------------------------------------------------
void dier_Init(TIM_Config_t tim)
{
	if (tim.uiEnable)
		tim.module->DIER |= TIM_DIER_UIE;

	if (tim.cc1ien)
		tim.module->DIER |= TIM_DIER_CC1IE;
	else
		tim.module->DIER &= ~TIM_DIER_CC1IE;

	if (tim.cc2ien)
		tim.module->DIER |= TIM_DIER_CC2IE;
	else
		tim.module->DIER &= ~TIM_DIER_CC2IE;

	if (tim.cc3ien)
		tim.module->DIER |= TIM_DIER_CC3IE;
	else
		tim.module->DIER &= ~TIM_DIER_CC3IE;

	if (tim.cc4ien)
		tim.module->DIER |= TIM_DIER_CC4IE;
	else
		tim.module->DIER &= ~TIM_DIER_CC4IE;
}

// Main Output Enable (only needed for TIM1 and TIM8)
// ----------------------------------------------------------------------------
void tch_Moe(TIM_Config_t tim)
{
	// The MOE bit in the BDTR register must be set
	// so that the outputs of timers TIM1 and TIM8 are activated.
	if ((tim.module == TIM1) || (tim.module == TIM8))
		tim.module-> BDTR = TIM_BDTR_MOE;
}
