/**
 ******************************************************************************
 * @file        : adc78h90.h
 * @author      : Estudiez, Ivan
 * @brief       : MCP23S17 Driver
 * @date        : 19/08/2025
 * @version     : 1.0
 ******************************************************************************
 * Change control:
 * 1.0 - (19/08/2025) Release version
 *
 *
 *
 ******************************************************************************
 */

#ifndef INC_ADC78H90_H_
#define INC_ADC78H90_H_

#include <stdint.h>
#include <stdbool.h>
#include <stm32f4xx.h>

//=============================================================================
// Device definitions
//=============================================================================
#define ADC78H90_SPI		SPI2					// ADC78H90 SPI peripheral
#define ADC78H90_CS_GPIO	SYSTEM_SPI2_CS2_GPIO	// ADC78H90 CS GPIO
#define ADC78H90_CS_PIN		SYSTEM_SPI2_CS2_PIN		// ADC78H90 CS PIN
#define ADC78H90_CPOL		1U						// ADC78H90 CPOL
#define ADC78H90_CPHA		1U						// ADC78H90 CPHA
#define ADC78H90_NUMBER_CH	6U						// Number of channels
#define ADC78H90_NUM_SAMPL	10U						// Number of samples per channel

// Register definitions
// -----------------------------------------------------------------------------
#define ADC_AIN1			0x81
#define ADC_AIN2			0x89
#define ADC_AIN3			0x91
#define ADC_AIN4			0x99
#define ADC_AIN5			0xA1
#define ADC_AIN6			0xA9
#define ADC_AIN7			0xB1
#define ADC_AIN8			0xB9

// Function prototypes
// -----------------------------------------------------------------------------
void ADC78H90_Update(void);
void ADC78H90_AverageChannels(void);
void ADC78H90_AddSamples(void);
uint8_t ADC78H90_SetupComm(void);

// Public variables
// -----------------------------------------------------------------------------
extern uint16_t ADC_AvgCh[ADC78H90_NUMBER_CH];

#endif /* INC_ADC78H90_H_ */
