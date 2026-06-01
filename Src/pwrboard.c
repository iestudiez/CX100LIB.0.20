/**
 ******************************************************************************
 * @file        : pwrboard.c
 * @author      : Estudiez, Ivan
 * @brief       : Power board management
 * @date        : 27/10/2025
 * @version     : 1.0
 ******************************************************************************
 */

#include <stm32f4xx.h>
#include "pwrboard.h"
#include "mcp23s17.h"
#include "adc78h90.h"
#include "ina239.h"
#include "system_cx100.h"
#include "gpio.h"
#include "debug.h"

// ----------------------------------------------------------------------------
// Defines
// ----------------------------------------------------------------------------
#define FREQ_FACTOR_K				(655350U)
#define PWRMON_AVG_SAMPLES			(10U)
#define PWRMON_AVG_INTERVAL			(10U)

// MCP Port-A
#define MCP_BUZZER_SOFT				(0x01)
#define MCP_BUZZER_LOUD				(0x02)
#define MCP_SUPPLY_EXT				(0x04)
#define MCP_SUPPLY_INT				(0x08)
#define MCP_AIN2_ENABLE				(0x10)
#define MCP_MONITOR_ALRT			(0x20)

// MCP Port-B
#define MCP_DIN1_STATUS				(0x04)
#define MCP_DIN2_STATUS				(0x08)
#define MCP_CRLOOP1_EN				(0x10)
#define MCP_DIN1_EN					(0x20)
#define MCP_CRLOOP2_EN				(0x40)
#define MCP_DIN2_EN					(0x80)

typedef struct InputConfigDef_t
{
	TIM_TypeDef *tim;
	uint8_t ccReg;
} InputConfigDef_t;

// ----------------------------------------------------------------------------
// Public variables
// ----------------------------------------------------------------------------
PwrBoardDef_t PowerBoard;

// ----------------------------------------------------------------------------
// Private variables
// ----------------------------------------------------------------------------
uint8_t gMcp_PortA;
uint8_t gMcp_PortB;
uint8_t gPowerMon_Cmd[3];

// @formatter:off
InputConfigDef_t gInputConfig[4] =
{
	{ TIM8, 2 },
	{ TIM3, 1 },
	{ TIM5, 1 },
	{ TIM2, 2 }
};
// @formatter:on

// ----------------------------------------------------------------------------
// Private prototypes
// ----------------------------------------------------------------------------
void PWRBOARD_ProcessInputs(void);
void PWRBOARD_ProcessOutputs(void);
void PWRBOARD_MCP23S17_Update(void);
void PWRBOARD_INA239_Update(void);
void PWRBOARD_ProcessAnalogInputs(void);
void processInput(InputDef_t *pInput, InputConfigDef_t inputConf);
void processBuzzer(void);
void processSensorSupply(void);
void processInputsConfig(void);
void processMcpInputs(void);
void multxReadWrite(void);
void pwrmonAddSample(uint16_t array[], uint16_t sample);
uint16_t pwrmonDoAverage(uint16_t array[]);

/**
 * ----------------------------------------------------------------------------
 * @brief	Initialize the power board
 * ----------------------------------------------------------------------------
 */
void PWRBOARD_Init(void)
{
	// Power monitor initialization
	INA239_Init();

	// Digital I/O expander initialization
	MCP23S17_Init();

	// Timers initialization
	for (uint8_t i = 0; i < 4; i++)
		SYSTEM_TimerConfig(gInputConfig[i].tim, PowerBoard.Freq[i].Mode, gInputConfig[i].ccReg);
}

/**
 * ----------------------------------------------------------------------------
 * @brief	Execute all processes related to the power board.
 * ----------------------------------------------------------------------------
 */
void PWRBOARD_Update(void)
{
	// Configuration and board control
	PWRBOARD_MCP23S17_Update();

	// Process frequency/ counter inputs
	PWRBOARD_ProcessInputs();

	// Process analog module
	ADC78H90_Update();
	ADC78H90_AddSamples();
	ADC78H90_AverageChannels();

	// Process analog inputs
	PWRBOARD_ProcessAnalogInputs();

	// Process power monitor
	PWRBOARD_INA239_Update();

	// Process power outputs
	PWRBOARD_ProcessOutputs();
}

// *****************************************************************************
// * Frequency Inputs Section												   *
// * STM32F405																   *
// *****************************************************************************

/**
 * ----------------------------------------------------------------------------
 * @brief		Process all digital frequency inputs of the system
 * ----------------------------------------------------------------------------
 */
void PWRBOARD_ProcessInputs(void)
{
	static InputMode_e prevModeIn[4];

	for (uint8_t i = 0; i < 4; i++)
		if (prevModeIn[i] != PowerBoard.Freq[i].Mode)
		{
			prevModeIn[i] = PowerBoard.Freq[i].Mode;
			SYSTEM_TimerConfig(gInputConfig[i].tim, PowerBoard.Freq[i].Mode, gInputConfig[i].ccReg);
		}

	for (uint8_t i = 0; i < 4; i++)
		processInput(&PowerBoard.Freq[i], gInputConfig[i]);
}

/**
 * -----------------------------------------------------------------------------
 * @brief				Process a single digital input
 * -----------------------------------------------------------------------------
 * @param pInput		Pointer to input type structure
 * @param inputConf		Array containing the configuration of all frequency inputs
 * -----------------------------------------------------------------------------
 */
void processInput(InputDef_t *pInput, InputConfigDef_t inputConf)
{
	volatile uint32_t *pCCR;

	if (pInput->Reset)
	{
		pInput->Counter = 0;
		pInput->Freq = 0;
		pInput->Rpm = 0;
		pInput->Timeout = 0;
		inputConf.tim->CNT = 0;
		pInput->Reset = false;
		return;
	}

	if (pInput->Mode == INPUT_COUNTER_MODE)
	{
		pInput->Counter = inputConf.tim->CNT;
		pInput->Freq = 0;
		pInput->Rpm = 0;
		return;
	}

	// Determine which capture and compare record to use
	if (inputConf.ccReg == 1)
		pCCR = &inputConf.tim->CCR1;
	else
		pCCR = &inputConf.tim->CCR2;

	if ((*pCCR > 0) && (inputConf.tim->SR & TIM_SR_TIF_Msk))
	{
		// Clear interrupt flag
		inputConf.tim->SR &= ~TIM_SR_TIF_Msk;

		// Clear timeout
		pInput->Timeout = 0;

		// Compute frequency ( Hz x 10)
		pInput->Freq = (FREQ_FACTOR_K / *pCCR);

		// Compute revolutions (RPM x 10)
		if (pInput->Div > 0)
			pInput->Rpm = (pInput->Freq * 60) / (pInput->Div);

		return;
	}

	pInput->Timeout++;

	if (pInput->Timeout >= PWRBOARD_FREQ_TIMEOUT)
	{
		pInput->Freq = 0;
		pInput->Rpm = 0;
		pInput->Timeout = PWRBOARD_FREQ_TIMEOUT;
	}
}

/**
 * -----------------------------------------------------------------------------
 * @brief	Processes PWM outputs
 * -----------------------------------------------------------------------------
 */
void PWRBOARD_ProcessOutputs(void)
{
	TIM1->CCR1 = PowerBoard.Out[0].DutyCycle;
	TIM1->CCR2 = PowerBoard.Out[1].DutyCycle;
	TIM1->CCR3 = PowerBoard.Out[2].DutyCycle;

	if (PowerBoard.InputsConfig.Ain1En)
		TIM1->CCR4 = 0;
	else
		TIM1->CCR4 = PowerBoard.Out[3].DutyCycle;

	// Assign the measured currents to the outputs
	PowerBoard.Out[0].Current = ADC_AvgCh[3];
	PowerBoard.Out[1].Current = ADC_AvgCh[2];
	PowerBoard.Out[2].Current = ADC_AvgCh[1];
	PowerBoard.Out[3].Current = ADC_AvgCh[0];
}

// *****************************************************************************
// * Analog Inputs Section													   *
// * ADC78H90																   *
// *****************************************************************************

void PWRBOARD_ProcessAnalogInputs(void)
{
	PowerBoard.Ain[0] = ADC_AvgCh[5];
	PowerBoard.Ain[1] = ADC_AvgCh[4];
}

// *****************************************************************************
// * Digital I/O Expander Section											   *
// * MCP23S17																   *
// *****************************************************************************

/**
 * -----------------------------------------------------------------------------
 * @brief	Update the status of the GPIOs of the MCP23S17 chip
 * -----------------------------------------------------------------------------
 */
void PWRBOARD_MCP23S17_Update(void)
{
	// Process the outputs of the MCP23S17 chip
	processBuzzer();
	processSensorSupply();
	processInputsConfig();

	// Perform MCP23S17 write and read operations
	multxReadWrite();

	// Process the inputs of the MCP23S17 chip
	processMcpInputs();
}

/**
 * -----------------------------------------------------------------------------
 * @brief	Activate the buzzer at high or low volume
 * -----------------------------------------------------------------------------
 */
void processBuzzer(void)
{
	// Clear buzzer flags
	gMcp_PortB &= ~(MCP_BUZZER_SOFT | MCP_BUZZER_LOUD);

	if (!PowerBoard.Buzzer.Enable)
		return;

	// Set buzzer loud
	if (PowerBoard.Buzzer.Loud)
	{
		gMcp_PortB |= MCP_BUZZER_LOUD;
		return;
	}

	// Set buzzer soft
	gMcp_PortB |= MCP_BUZZER_SOFT;
}

/**
 * -----------------------------------------------------------------------------
 * @brief	Activate power supply for digital inputs
 * -----------------------------------------------------------------------------
 */
void processSensorSupply(void)
{
	// External power supply
	if (PowerBoard.SensorSupply.External)
		gMcp_PortB |= MCP_SUPPLY_EXT;
	else
		gMcp_PortB &= ~MCP_SUPPLY_EXT;

	// Internal power supply
	if (PowerBoard.SensorSupply.Internal)
		gMcp_PortB |= MCP_SUPPLY_INT;
	else
		gMcp_PortB &= ~MCP_SUPPLY_INT;
}

/**
 * -----------------------------------------------------------------------------
 * @brief 	Perform inputs configuration
 * -----------------------------------------------------------------------------
 */
void processInputsConfig(void)
{
	// Enable Analog Input 2
	if (PowerBoard.InputsConfig.Ain1En)
		gMcp_PortB |= MCP_AIN2_ENABLE;
	else
		gMcp_PortB &= ~MCP_AIN2_ENABLE;

	// Enable 4-20mA current loop in Analog Input 1
	if (PowerBoard.InputsConfig.CurrLoop0En)
		gMcp_PortA |= MCP_CRLOOP1_EN;
	else
		gMcp_PortA &= ~MCP_CRLOOP1_EN;

	// Enable 4-20mA current loop in Analog Input 2
	if (PowerBoard.InputsConfig.CurrLoop1En)
		gMcp_PortA |= MCP_CRLOOP2_EN;
	else
		gMcp_PortA &= ~MCP_CRLOOP2_EN;

	// Enable power supply at DIN1
	if (PowerBoard.InputsConfig.Din0En)
		gMcp_PortA |= MCP_DIN1_EN;
	else
		gMcp_PortA &= ~MCP_DIN1_EN;

	// Enable power supply at DIN2
	if (PowerBoard.InputsConfig.Din1En)
		gMcp_PortA |= MCP_DIN2_EN;
	else
		gMcp_PortA &= ~MCP_DIN2_EN;
}

/**
 * -----------------------------------------------------------------------------
 * @brief	Read digital inputs from MCP23S17
 * -----------------------------------------------------------------------------
 */
void processMcpInputs(void)
{
	PowerBoard.Din[0] = (bool) (gMcp_PortA & MCP_DIN1_STATUS);
	PowerBoard.Din[1] = (bool) (gMcp_PortA & MCP_DIN2_STATUS);
	PowerBoard.Status.Alert = (bool) (gMcp_PortB & MCP_MONITOR_ALRT);
}

/**
 * -----------------------------------------------------------------------------
 * @brief	MCP23S17 Write and read operations
 * -----------------------------------------------------------------------------
 */
void multxReadWrite(void)
{
	// SPI peripheral configuration for MCP23S17
	if (MCP23S17_SetupComm())
		return;

	// Write MCP23S17 output latches
	MCP23S17_Write(OLATA_ADDR, gMcp_PortA);
	MCP23S17_Write(OLATB_ADDR, gMcp_PortB);

	// Read MCP23S17 port status
	MCP23S17_Read(GPIOA_ADDR, &gMcp_PortA);
	MCP23S17_Read(GPIOB_ADDR, &gMcp_PortB);
}

// *****************************************************************************
// * Power Monitor Section													   *
// * INA239																	   *
// *****************************************************************************

/**
 * -----------------------------------------------------------------------------
 * @brief	Power monitor status update
 * -----------------------------------------------------------------------------
 */
void PWRBOARD_INA239_Update(void)
{
	static uint16_t aCurrent[PWRMON_AVG_SAMPLES];
	static uint16_t aVbus[PWRMON_AVG_SAMPLES];
	static uint8_t pwrmonInterval = 0;

	// Configure SPI communications
	if (INA239_SetupComm())
		return;

	// Enable INA239
	GPIO_Write(INA239_CS_GPIO, INA239_CS_PIN, 0);

	// Establish the interval at which voltage and current readings are taken
	if (pwrmonInterval < PWRMON_AVG_INTERVAL)
		pwrmonInterval++;
	else
	{
		pwrmonAddSample(aVbus, INA239_VBus());
		pwrmonAddSample(aCurrent, INA239_VShunt());
		pwrmonInterval = 0;
	}

	// Read bus voltage
	PowerBoard.Status.VBus = pwrmonDoAverage(aVbus);

	// Read shunt current
	PowerBoard.Status.Current = (uint16_t) ((pwrmonDoAverage(aCurrent) * 25) / 100);

	// Read die temperature
	PowerBoard.Status.Temp = INA239_Temperature();

	// Calculate the system's power consumption (Watt x 100)
	PowerBoard.Status.Power = (uint16_t) ((PowerBoard.Status.VBus * PowerBoard.Status.Current) / 10000);

	// Disable INA239
	GPIO_Write(INA239_CS_GPIO, INA239_CS_PIN, 1);
}

/**
 * -----------------------------------------------------------------------------
 * @brief			Add a sample to the power monitor array
 * -----------------------------------------------------------------------------
 * @param array		Array containing samples of the power monitor readings
 * @param sample	Sample to add
 * -----------------------------------------------------------------------------
 */
void pwrmonAddSample(uint16_t array[], uint16_t sample)
{
	for (uint8_t i = 0; i < PWRMON_AVG_SAMPLES - 1; i++)
		array[i] = array[i + 1];

	array[PWRMON_AVG_SAMPLES - 1] = sample;
}

/**
 * -----------------------------------------------------------------------------
 * @brief			Calculate the average of the values ​​contained in the array
 * -----------------------------------------------------------------------------
 * @param array		Array containing samples of the power monitor readings
 * @return			Result of the average calculation
 * -----------------------------------------------------------------------------
 */
uint16_t pwrmonDoAverage(uint16_t array[])
{
	uint32_t sum = 0;

	for (uint8_t i = 0; i < PWRMON_AVG_SAMPLES; i++)
		sum += array[i];

	return (uint16_t) (sum / PWRMON_AVG_SAMPLES);
}
