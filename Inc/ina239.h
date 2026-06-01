/**
 ******************************************************************************
 * @file        : ina239.h
 * @author      : Estudiez, Ivan
 * @brief       : INA239 Driver
 * @date        : 25/08/2025
 * @version     : 1.0
 ******************************************************************************
 * Change control:
 * 1.0 - (25/08/2025) Release version
 *
 *
 *
 ******************************************************************************
 */

#ifndef INC_INA239_H_
#define INC_INA239_H_

#include <stdint.h>

//=============================================================================
// Device definitions
//=============================================================================
#define INA239_SPI				SPI2					// INA239 SPI peripheral
#define INA239_CS_GPIO			SYSTEM_SPI2_CS3_GPIO	// INA239 CS GPIO
#define INA239_CS_PIN			SYSTEM_SPI2_CS3_PIN		// INA239 CS PIN
#define INA239_CPOL				0U						// INA239 CPOL
#define INA239_CPHA				1U						// INA239 CPHA

// Register definitions
// ----------------------------------------------------------------------------
#define INA239_CONFIG			0x00	// Configuration
#define INA239_ADC_CONFIG		0x04	// ADC Configuration
#define INA239_SHUNT_CAL		0x08	// Shunt Calibration
#define INA239_VSHUNT			0x10	// Shunt Voltage Measurement
#define INA239_VBUS				0x14	// Bus Voltage Measurement
#define INA239_DIETEMP			0x18	// Temperature Measurement
#define INA239_CURRENT			0x1C	// Current Result
#define INA239_POWER			0x20	// Power Result
#define INA239_DIAG_ALRT		0x2C	// Diagnostic Flags Alert
#define INA239_SOVL				0x30	// Shunt Overvoltage Threshold
#define INA239_SUVL				0x34	// Shunt Undervoltage Threshold
#define INA239_BOVL				0x38	// Bus Overvoltage Threshold
#define INA239_BUVL				0x3C	// Bus Undervoltage Threshold
#define INA239_TEMP_LIMIT		0x40	// Temperature Over-Limit Threshold
#define INA239_PWR_LIMIT		0x44	// Power Over-Limit Threshold
#define INA239_MNFR_ID			0xF8	// Manufacturer ID
#define INA239_DEVICE_ID		0xFC	// Device ID

// ============================================================================
// Function prototypes
// ============================================================================
uint8_t INA239_Init(void);
uint8_t INA239_SetupComm(void);
uint16_t INA239_VBus(void);
uint16_t INA239_Temperature(void);
uint16_t INA239_Current(void);
uint16_t INA239_VShunt(void);
uint16_t INA239_Read(uint8_t address);

// ============================================================================
// Public variables
// ============================================================================


// ============================================================================

#endif /* INC_INA239_H_ */
