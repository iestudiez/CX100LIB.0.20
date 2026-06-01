/**
 ******************************************************************************
 * @file        : rcc.c
 * @author      : Estudiez, Ivan
 * @brief       : RCC
 * @date        : 18/12/2024
 * @version     : 1.0
 ******************************************************************************
 *
 *
 *
 *
 *
 ******************************************************************************
 */

#include <stm32f4xx.h>
#include "rcc.h"

// ============================================================================
// SYSTEM CORE CLOCK HSI 16MHz
// ============================================================================
#ifdef RCC_CORE_CLK_HSI_16MHZ
void RCC_SystemClockConfig(void)
{

}
#endif

// ============================================================================
// SYSTEM CORE CLOCK 64MHz
// ============================================================================
#ifdef RCC_CORE_CLK_64MHZ
void RCC_SystemClockConfig(void)
{
	// 1. Enable HSE and wait for the HSE to become ready
	// --------------------------------------------------
	RCC->CR |= RCC_CR_HSEON;
	while (!(RCC->CR & RCC_CR_HSERDY));

	// 2. Set the POWER ENABLE CLOCK and VOLTAGE REGULATOR
	// ---------------------------------------------------
	RCC->APB1ENR |= RCC_APB1ENR_PWREN;
	PWR->CR |= PWR_CR_VOS;

	// 3. Configure the FLASH PREFETCH and the LATENCY related settings
	// ----------------------------------------------------------------

	// FLASH latency according to table 10 in data sheet (RM0090-STM32F405 Pag80)
	// Device voltage = 3.3V
	// HCLK = 32Mhz
	// LATENCY = 1WS
	FLASH->ACR = FLASH_ACR_ICEN | FLASH_ACR_DCEN | FLASH_ACR_PRFTEN | FLASH_ACR_LATENCY_1WS;

	// 4. Configure the MAIN PLL
	// -------------------------

	// Clear PLLM, PLLN and PLLP registers
	RCC->PLLCFGR &= ~(0x1FFFF);

	// Select PLL source (PLLSRC = 1, HSE)
	RCC->PLLCFGR |= RCC_PLLCFGR_PLLSRC;

	// HSE = 8MHz
	// SYSCLK = 8MHz -> /4 -> x64 -> /2 = 64MHz
	RCC->PLLCFGR |= (4U << RCC_PLLCFGR_PLLM_Pos);	// PLLM = 4
	RCC->PLLCFGR |= (64U << RCC_PLLCFGR_PLLN_Pos); 	// PLLN = 64
	RCC->PLLCFGR |= (0U << RCC_PLLCFGR_PLLP_Pos); 	// PLLP = 2

	// 5. Configure the PRESCALARS HCLK, PCLK1, PCLK2
	// ----------------------------------------------
	// SYSCLK = 64MHz

	// AHB Prescaler
	// HCLK = SYSCLK / 2 = 32MHz
	RCC->CFGR |= RCC_CFGR_HPRE_DIV2;

	// APB1 Prescaler
	// PCLK1 = HCLK / 1 = 32MHz
	RCC->CFGR |= RCC_CFGR_PPRE1_DIV1;

	// APB2 Prescaler
	// PCLK2 = HCLK / 1 = 32MHz
	RCC->CFGR |= RCC_CFGR_PPRE2_DIV1;

	// 6. Enable the PLL and wait for it to become ready
	// -------------------------------------------------
	RCC->CR |= RCC_CR_PLLON;
	while (!(RCC->CR & RCC_CR_PLLRDY));

	// 7. Select the clock source and wait for it to be set
	// ----------------------------------------------------
	RCC->CFGR |= RCC_CFGR_SW_PLL;
	while (!(RCC->CFGR & RCC_CFGR_SWS_PLL));
}
#endif

// ============================================================================
// SYSTEM CORE CLOCK 168MHz - HCLK 84MHz
// ============================================================================
#ifdef RCC_CORE_CLK_168MHZ
void RCC_SystemClockConfig(void)
{
	// 1. Enable HSE and wait for the HSE to become ready
	// --------------------------------------------------
	RCC->CR |= RCC_CR_HSEON;
	while (!(RCC->CR & RCC_CR_HSERDY));

	// 2. Set the POWER ENABLE CLOCK and VOLTAGE REGULATOR
	// ---------------------------------------------------
	RCC->APB1ENR |= RCC_APB1ENR_PWREN;
	PWR->CR |= PWR_CR_VOS;

	// 3. Configure the FLASH PREFETCH and the LATENCY related settings
	// ----------------------------------------------------------------

	// FLASH latency according to table 10 in data sheet (RM0090-STM32F405 Pag80)
	// Device voltage = 3.3V
	// HCLK = 84Mhz
	// LATENCY = 2WS
	FLASH->ACR = FLASH_ACR_ICEN | FLASH_ACR_DCEN | FLASH_ACR_PRFTEN | FLASH_ACR_LATENCY_2WS;

	// 4. Configure the MAIN PLL
	// -------------------------

	// Clear PLLM, PLLN and PLLP registers
	RCC->PLLCFGR &= ~(0x1FFFF);

	// Select PLL source (PLLSRC = 1, HSE)
	RCC->PLLCFGR |= RCC_PLLCFGR_PLLSRC;

	// HSE = 8MHz
	// SYSCLK = 8MHz -> /4 -> x168 -> /2 = 168MHz
	RCC->PLLCFGR |= (4U << RCC_PLLCFGR_PLLM_Pos);		// PLLM = 4
	RCC->PLLCFGR |= (168U << RCC_PLLCFGR_PLLN_Pos); 	// PLLN = 168
	RCC->PLLCFGR |= (0U << RCC_PLLCFGR_PLLP_Pos); 		// PLLP = 2

	// 5. Configure the PRESCALARS HCLK, PCLK1, PCLK2
	// ----------------------------------------------
	// SYSCLK = 168MHz

	// AHB Prescaler
	// HCLK = SYSCLK / 2 = 84MHz
	RCC->CFGR |= RCC_CFGR_HPRE_DIV2;

	// APB1 Prescaler
	// PCLK1 = HCLK / 2 = 42MHz
	RCC->CFGR |= RCC_CFGR_PPRE1_DIV2;

	// APB2 Prescaler
	// PCLK2 = HCLK / 2 = 42MHz
	RCC->CFGR |= RCC_CFGR_PPRE2_DIV2;

	// 6. Enable the PLL and wait for it to become ready
	// -------------------------------------------------
	RCC->CR |= RCC_CR_PLLON;
	while (!(RCC->CR & RCC_CR_PLLRDY));

	// 7. Select the clock source and wait for it to be set
	// ----------------------------------------------------
	RCC->CFGR |= RCC_CFGR_SW_PLL;
	while (!(RCC->CFGR & RCC_CFGR_SWS_PLL));
}
#endif

// ============================================================================
// SYSTEM CORE CLOCK 168MHz - HCLK 168MHz
// ============================================================================
#ifdef RCC_CORE_CLK_168MHZ_HCLK_168MHZ
void RCC_SystemClockConfig(void)
{
	// 1. Enable HSE and wait for the HSE to become ready
	// --------------------------------------------------
	RCC->CR |= RCC_CR_HSEON;
	while (!(RCC->CR & RCC_CR_HSERDY));

	// 2. Set the POWER ENABLE CLOCK and VOLTAGE REGULATOR
	// ---------------------------------------------------
	RCC->APB1ENR |= RCC_APB1ENR_PWREN;
	PWR->CR |= PWR_CR_VOS;

	// 3. Configure the FLASH PREFETCH and the LATENCY related settings
	// ----------------------------------------------------------------

	// FLASH latency according to table 10 in data sheet (RM0090-STM32F405 Pag80)
	// Device voltage = 3.3V
	// HCLK = 168Mhz
	// LATENCY = 5WS
	FLASH->ACR = FLASH_ACR_ICEN | FLASH_ACR_DCEN | FLASH_ACR_PRFTEN | FLASH_ACR_LATENCY_5WS;

	// 4. Configure the MAIN PLL
	// -------------------------

	// Clear PLLM, PLLN and PLLP registers
	RCC->PLLCFGR &= ~(0x1FFFF);

	// Select PLL source (PLLSRC = 1, HSE)
	RCC->PLLCFGR |= RCC_PLLCFGR_PLLSRC;

	// HSE = 8MHz
	// SYSCLK = 8MHz -> /4 -> x168 -> /2 = 168MHz
	RCC->PLLCFGR |= (4U << RCC_PLLCFGR_PLLM_Pos);		// PLLM = 4
	RCC->PLLCFGR |= (168U << RCC_PLLCFGR_PLLN_Pos); 	// PLLN = 168
	RCC->PLLCFGR |= (0U << RCC_PLLCFGR_PLLP_Pos); 		// PLLP = 2

	// 5. Configure the PRESCALARS HCLK, PCLK1, PCLK2
	// ----------------------------------------------
	// SYSCLK = 168MHz

	// AHB Prescaler
	// HCLK = SYSCLK / 1 = 168MHz
	RCC->CFGR |= RCC_CFGR_HPRE_DIV1;

	// APB1 Prescaler
	// PCLK1 = HCLK / 4 = 42MHz
	RCC->CFGR |= RCC_CFGR_PPRE1_DIV4;

	// APB2 Prescaler
	// PCLK2 = HCLK / 4 = 42MHz
	RCC->CFGR |= RCC_CFGR_PPRE2_DIV4;

	// 6. Enable the PLL and wait for it to become ready
	// -------------------------------------------------
	RCC->CR |= RCC_CR_PLLON;
	while (!(RCC->CR & RCC_CR_PLLRDY));

	// 7. Select the clock source and wait for it to be set
	// ----------------------------------------------------
	RCC->CFGR |= RCC_CFGR_SW_PLL;
	while (!(RCC->CFGR & RCC_CFGR_SWS_PLL));
}
#endif

