/**
 ******************************************************************************
 * @file        : system_cx100.h
 * @author      : Estudiez, Ivan
 * @brief       : CX100 System functions and hardware initialization
 * @date        : 01/08/2025
 * @version     : 1.0
 ******************************************************************************
 * Change control:
 * 1.0 - (01/08/2025) Release version
 *
 *
 *
 ******************************************************************************
 */

#ifndef SYSTEM_CX100_H_
#define SYSTEM_CX100_H_

#include <stm32f4xx.h>
#include <stdbool.h>
#include <stdint.h>
#include "rcc.h"

// ----------------------------------------------------------------------------
// Defines
// ----------------------------------------------------------------------------
#define SYSTEM_DEBUG_TIMER				TIM9
#define SYSTEM_DEBUG_TIM_APB_CLK		RCC_APB2_TIM_CLK
#define SYSTEM_DELAY_TIMER				TIM10
#define SYSTEM_DELAY_TIM_APB_CLK		RCC_APB2_TIM_CLK

#define SYSTEM_DEBUG_GPIO				GPIOB
#define SYSTEM_DEBUG_PIN				(10U)

#define SYSTEM_CANTR_GPIO				GPIOB
#define SYSTEM_CANTR_PIN				(12U)

#define SYSTEM_PWRLED_GPIO				GPIOA
#define SYSTEM_PWRLED_PIN				(15U)
#define SYSTEM_MSGLED_GPIO				GPIOB
#define SYSTEM_MSGLED_PIN				(10U)
#define SYSTEM_ERRLED_GPIO				GPIOB
#define SYSTEM_ERRLED_PIN				(9U)

#define SYSTEM_LCD_RED_GPIO				GPIOB
#define SYSTEM_LCD_RED_PIN				(4U)
#define SYSTEM_LCD_GREEN_GPIO			GPIOB
#define SYSTEM_LCD_GREEN_PIN			(5U)
#define SYSTEM_LCD_BLUE_GPIO			GPIOC
#define SYSTEM_LCD_BLUE_PIN				(13U)

#define SYSTEM_LCD_CS_GPIO				GPIOB
#define SYSTEM_LCD_CS_PIN				(1U)

#define SYSTEM_EPROM_CS_GPIO			GPIOB
#define SYSTEM_EPROM_CS_PIN				(0U)

#define SYSTEM_SPI2_CS1_GPIO			GPIOC
#define SYSTEM_SPI2_CS1_PIN				(1U)
#define SYSTEM_SPI2_CS2_GPIO			GPIOC
#define SYSTEM_SPI2_CS2_PIN				(0U)
#define SYSTEM_SPI2_CS3_GPIO			GPIOB
#define SYSTEM_SPI2_CS3_PIN				(2U)

#define SYSTEM_TIM_COUNTER_MODE			(0U)
#define SYSTEM_TIM_FREQUENCY_MODE		(1U)

// ----------------------------------------------------------------------------
// Public functions
// ----------------------------------------------------------------------------
void SYSTEM_Init();
void SYSTEM_PwrLed(bool enable);
void SYSTEM_MsgLed(bool enable);
void SYSTEM_ErrLed(bool enable);
void SYSTEM_LcdGreen(bool enable);
void SYSTEM_LcdRed(bool enable);
void SYSTEM_LcdBlue(bool enable);
void SYSTEM_DelayUs(uint16_t delay);
void SYSTEM_DelayMs(uint16_t delay);
void SYSTEM_TimerConfig(TIM_TypeDef *tim, uint8_t freqMode, uint8_t ccReg);

// ----------------------------------------------------------------------------
// Public variables
// ----------------------------------------------------------------------------

#endif /* SYSTEM_CX100_H_ */
