/*
 ******************************************************************************
 * @file        : systick.c
 * @author      : Estudiez, Ivan
 * @brief       : STM32F405 SysTick
 * @date        : 12/05/2025
 * @version     : 1.0
 ******************************************************************************
 *
 *
 *
 *
 *
 ******************************************************************************
 */

#include "stm32f4xx.h"
#include "systick.h"
#include "scheduler.h"
#include "rcc.h"
#include <stdint.h>
#include <system_cx100.h>

#define SYSTICK_LOAD_VALUE          (RCC_SYSTICK_CLK / 1000U)

// ============================================================================
// Public variables
// ============================================================================
uint32_t SYSTICK_Ticks;

// ============================================================================
// Private variables
// ============================================================================

void SYSTICK_DelayMs(uint16_t delay)
{
	// Reload with number of clocks per millisecond
	SysTick->LOAD = SYSTICK_LOAD_VALUE;

	// Clear SysTick current value register
	SysTick->VAL = 0;

	// Enable SysTick and select internal clock source
	SysTick->CTRL = SysTick_CTRL_ENABLE_Msk | SysTick_CTRL_CLKSOURCE_Msk;

	for (int i = 0; i < delay; i++)
	{
		// Wait until the COUNTFLAG is set
		while (!(SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk))
		{
		}
	}

	// Stop SysTick
	SysTick->CTRL = 0;
}

// ----------------------------------------------------------------------------
// SYSTICK_Config()
// ----------------
// Systick Configuration and Interrupt Enabling
// ----------------------------------------------------------------------------
void SYSTICK_Config(uint16_t ms)
{
	SysTick_Config((RCC_SYSTICK_CLK / 1000U) * ms);
}

// ----------------------------------------------------------------------------
// SYSTICK_Handler()
// ----------------
// SYSTICK Interrupt Service Routine (ISR)
// ----------------------------------------------------------------------------
void SysTick_Handler(void)
{
	NVIC_ClearPendingIRQ(SysTick_IRQn);

	// Update ticks
	SYSTICK_Ticks++;
}

