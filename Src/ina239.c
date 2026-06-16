/**
 ******************************************************************************
 * @file        : ina239.c
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

#include "ina239.h"
#include "spi.h"
#include "system_cx100.h"

// Local Definitions
// -----------------------------------------------------------------------------
#define INA239_WRITE_OP			(0U << 0U)
#define INA239_READ_OP			(1U << 0U)
#define CONFIG_ADCRANGE			(1U << 4U)		// Range = 40.96 mV
#define ADC_CONFIG_MODE			(0xF << 12U)	// Continuous bus, shunt, temperature
#define ADC_CONFIG_VBUSCT		(0x5 << 9U)		// VBus conversion time
#define ADC_CONFIG_VSHCT		(0x5 << 6U)		// Shunt conversion time
#define ADC_CONFIG_VTCT			(0x5 << 3U)		// Temperature conversion time
#define ADC_CONFIG_AVG			(0x3 << 0U)		// ADC averaging count
#define SHUNT_CAL				(4096U)			// Shunt calibration value

// Public variables
// -----------------------------------------------------------------------------

// Private variables
// -----------------------------------------------------------------------------
uint8_t ina239Cmd[3];
uint8_t ina239Data[3];

/**
 * -----------------------------------------------------------------------------
 * @brief 	INA239 Initialization and configuration routine
 * -----------------------------------------------------------------------------
 * @return	Error code
 * -----------------------------------------------------------------------------
 */
uint8_t INA239_Init(void)
{
	uint16_t wrData;

	// INA239 Communications setup
	if (INA239_SetupComm())
		return 1;

	// Enable INA239
	GPIO_Write(INA239_CS_GPIO, INA239_CS_PIN, 0);

	// Device configuration register
	wrData = 0;
	wrData |= CONFIG_ADCRANGE;

	ina239Cmd[0] = INA239_CONFIG;
	ina239Cmd[1] = (uint8_t) (wrData >> 8U);
	ina239Cmd[2] = (uint8_t) (wrData & 0xFF);

	// Send command
	SPI_Transmit(INA239_SPI, ina239Cmd, 3);

	// ADC Configuration register
	wrData = 0;
	wrData |= ADC_CONFIG_MODE | ADC_CONFIG_VBUSCT | ADC_CONFIG_VSHCT
			| ADC_CONFIG_VTCT | ADC_CONFIG_AVG;

	ina239Cmd[0] = (INA239_ADC_CONFIG);
	ina239Cmd[1] = (uint8_t) (wrData >> 8U);
	ina239Cmd[2] = (uint8_t) (wrData & 0xFF);

	// Send command
	SPI_Transmit(INA239_SPI, ina239Cmd, 3);

	// Shunt calibration
	// SHUNT_CAL = 819.2x10^6 x 1mA (CURRENT_LSB) x 5mOmh (RShunt) = 4096
	wrData = SHUNT_CAL;

	ina239Cmd[0] = (INA239_SHUNT_CAL);
	ina239Cmd[1] = (uint8_t) (wrData >> 8U);
	ina239Cmd[2] = (uint8_t) (wrData & 0xFF);

	// Send command
	SPI_Transmit(INA239_SPI, ina239Cmd, 3);

	// Disable INA239
	GPIO_Write(INA239_CS_GPIO, INA239_CS_PIN, 1);

	return 0;
}

/**
 * -----------------------------------------------------------------------------
 * @brief	INA239 Communications setup
 * -----------------------------------------------------------------------------
 * @return	Error code
 */
uint8_t INA239_SetupComm(void)
{
	// Exit if SPI2 peripheral is busy
	if (INA239_SPI->SR & SPI_SR_BSY)
		return 1;

	SPI_Disable(INA239_SPI);

	// Configure CPOL and CPHA
	INA239_SPI->CR1 &= ~(0x03);
	INA239_SPI->CR1 |= (INA239_CPOL << SPI_CR1_CPOL_Pos);
	INA239_SPI->CR1 |= (INA239_CPHA << SPI_CR1_CPHA_Pos);

	// 8-bit Mode
	INA239_SPI->CR1 &= ~(SPI_CR1_DFF);

	SPI_Enable(INA239_SPI);

	return 0;
}

/**
 * -----------------------------------------------------------------------------
 * @brief 	Read bus voltage
 * -----------------------------------------------------------------------------
 * @return	Voltage [mV]
 * -----------------------------------------------------------------------------
 */
uint16_t INA239_VBus(void)
{
	const uint32_t convFactor = 3125;
	uint32_t retData;

	ina239Cmd[0] = INA239_VBUS | INA239_READ_OP;
	SPI_Transmit(INA239_SPI, ina239Cmd, 1);
	SPI_Receive(INA239_SPI, ina239Data, 2);
	retData = 0;
	retData = ina239Data[0] << 8U;
	retData |= ina239Data[1];

	retData = (retData * convFactor) / 1000;

	return (uint16_t) retData;
}

/**
 * -----------------------------------------------------------------------------
 * @brief 	Internal die temperature
 * -----------------------------------------------------------------------------
 * @return	Temperature [°C] x 10
 * -----------------------------------------------------------------------------
 */
uint16_t INA239_Temperature(void)
{
	const uint32_t convFactor = 125;
	uint32_t retData;

	ina239Cmd[0] = INA239_DIETEMP | INA239_READ_OP;
	SPI_Transmit(INA239_SPI, ina239Cmd, 1);
	SPI_Receive(INA239_SPI, ina239Data, 2);
	retData = 0;
	retData = ina239Data[0] << 8U;
	retData |= ina239Data[1];
	retData = retData >> 4U;

	retData = (retData * convFactor) / 100;

	return (uint16_t) retData;
}

/**
 * -----------------------------------------------------------------------------
 * @brief 	Current through the shunt resistor
 * -----------------------------------------------------------------------------
 * @return	Current [mA]
 * -----------------------------------------------------------------------------
 */
uint16_t INA239_Current(void)
{
	uint32_t retData;

	ina239Cmd[0] = INA239_CURRENT | INA239_READ_OP;
	SPI_Transmit(INA239_SPI, ina239Cmd, 1);
	SPI_Receive(INA239_SPI, ina239Data, 2);
	retData = 0;
	retData = ina239Data[0] << 8U;
	retData |= ina239Data[1];

	return (uint16_t) retData;
}

/**
 * -----------------------------------------------------------------------------
 * @brief	Voltage through the shunt resistor
 * -----------------------------------------------------------------------------
 * @return	Volts (value x 0.125 uV)
 * -----------------------------------------------------------------------------
 */
uint16_t INA239_VShunt(void)
{
	uint32_t retData;

	ina239Cmd[0] = INA239_VSHUNT | INA239_READ_OP;
	SPI_Transmit(INA239_SPI, ina239Cmd, 1);
	SPI_Receive(INA239_SPI, ina239Data, 2);
	retData = 0;
	retData = ina239Data[0] << 8U;
	retData |= ina239Data[1];

	return (uint16_t) retData;
}

/**
 * -----------------------------------------------------------------------------
 * @brief 			Reads a register
 * -----------------------------------------------------------------------------
 * @param address	Register address
 * @return			Register value
 * -----------------------------------------------------------------------------
 */
uint16_t INA239_Read(uint8_t address)
{
	uint32_t retData;

	ina239Cmd[0] = address | INA239_READ_OP;
	SPI_Transmit(INA239_SPI, ina239Cmd, 1);
	SPI_Receive(INA239_SPI, ina239Data, 2);
	retData = 0;
	retData = ina239Data[0] << 8U;
	retData |= ina239Data[1];

	return (uint16_t) retData;
}
