/**
 ******************************************************************************
 * @file        : rcc.h
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

#ifndef INC_RCC_H_
#define INC_RCC_H_

// ============================================================================
// SYSTEM CORE CLOCK SELECTION
// ----------------------------------------------------------------------------
//#define RCC_CORE_CLK_HSI_16MHZ
//#define RCC_CORE_CLK_64MHZ
//#define RCC_CORE_CLK_168MHZ
#define RCC_CORE_CLK_168MHZ_HCLK_168MHZ
// ============================================================================

// ============================================================================
// SYSTEM CORE CLOCK HSI 16MHz
// ----------------------------------------------------------------------------
#ifdef RCC_CORE_CLK_HSI_16MHZ
#define RCC_SYSTEM_CLK			16000000U

// AHB Buses Clock
#define RCC_HCLK_CLK			16000000U
#define RCC_APB1_CLK			16000000U
#define RCC_APB2_CLK			16000000U

// Timers Clock
#define RCC_SYSTICK_CLK			16000000U
#define RCC_APB1_TIM_CLK		16000000U
#define RCC_APB2_TIM_CLK		16000000U
#endif
// ============================================================================

// ============================================================================
// SYSTEM CORE CLOCK 64MHz
// ----------------------------------------------------------------------------
#ifdef RCC_CORE_CLK_64MHZ
#define RCC_SYSTEM_CLK			64000000U

// AHB Buses Clock
#define RCC_HCLK_CLK			32000000U
#define RCC_APB1_CLK			32000000U
#define RCC_APB2_CLK			32000000U

// Timers Clock
#define RCC_SYSTICK_CLK			32000000U
#define RCC_APB1_TIM_CLK		32000000U
#define RCC_APB2_TIM_CLK		32000000U
#endif
// ============================================================================


// ============================================================================
// SYSTEM CORE CLOCK 168MHz - HCLK 84MHz
// ----------------------------------------------------------------------------
#ifdef RCC_CORE_CLK_168MHZ
#define RCC_SYSTEM_CLK			168000000U

// AHB Buses Clock
#define RCC_HCLK_CLK			84000000U
#define RCC_APB1_CLK			42000000U
#define RCC_APB2_CLK			42000000U

// Timers Clock
#define RCC_SYSTICK_CLK			84000000U
#define RCC_APB1_TIM_CLK		84000000U
#define RCC_APB2_TIM_CLK		84000000U
#endif
// ============================================================================

// ============================================================================
// SYSTEM CORE CLOCK 168MHz - HCLK 168MHz
// ----------------------------------------------------------------------------
#ifdef RCC_CORE_CLK_168MHZ_HCLK_168MHZ
#define RCC_SYSTEM_CLK			168000000U

// AHB Buses Clock
#define RCC_HCLK_CLK			168000000U
#define RCC_APB1_CLK			42000000U
#define RCC_APB2_CLK			42000000U

// Timers Clock
#define RCC_SYSTICK_CLK			168000000U
#define RCC_APB1_TIM_CLK		84000000U
#define RCC_APB2_TIM_CLK		84000000U
#endif
// ============================================================================

void RCC_SystemClockConfig(void);

#endif /* INC_RCC_H_ */
