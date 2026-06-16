/**
 ******************************************************************************
 * @file        : mcp23s17.c
 * @author      : Estudiez, Ivan
 * @brief       : MCP23S17 Driver
 * @date        : 15/08/2025
 * @version     : 1.0
 ******************************************************************************
 * Change control:
 * 1.0 - (15/08/2025) Release version
 *
 *
 *
 ******************************************************************************
 */

#include "mcp23s17.h"
#include "spi.h"
#include "system_cx100.h"

#define MCP23S17_WRITE_OP			0x40
#define MCP23S17_READ_OP			0x41

// Public variables
// ----------------------------------------------------------------------------

// Private variables
// ----------------------------------------------------------------------------
uint8_t mcpCmd[3];

/**
 * -----------------------------------------------------------------------------
 * @brief 	MCP23S17 Initialization
 * -----------------------------------------------------------------------------
 * @return	Error code
 * -----------------------------------------------------------------------------
 */
uint8_t MCP23S17_Init(void)
{
	// SPI peripheral configuration for MCP23S17
	if (MCP23S17_SetupComm())
		return 1;

	// Enable MCP23S17 device (Set reset pin)
	GPIO_Write(MCP23S17_RST_GPIO, MCP23S17_RST_PIN, 1);

	// SPI MODE: 1, 1
	if (MCP23S17_CPOL && MCP23S17_CPHA)
	{
		GPIO_Write(MCP23S17_CS_GPIO, MCP23S17_CS_PIN, 0);
		SYSTEM_DelayUs(5);
		GPIO_Write(MCP23S17_CS_GPIO, MCP23S17_CS_PIN, 1);
	}

	// Configure MCP23S17 GPIOA port
	// -----------------------------
	mcpCmd[0] = MCP23S17_WRITE_OP;
	mcpCmd[1] = IODIRA_ADDR;
	mcpCmd[2] = MCP23S17_IODIRA;

	// Enable CS pin
	GPIO_Write(MCP23S17_CS_GPIO, MCP23S17_CS_PIN, 0);

	// Send Configuration Command
	SPI_Transmit(MCP23S17_SPI, mcpCmd, 3);

	// Configure MCP23S17 GPIOB port
	// -----------------------------
	mcpCmd[0] = MCP23S17_WRITE_OP;
	mcpCmd[1] = IODIRB_ADDR;
	mcpCmd[2] = MCP23S17_IODIRB;

	// Send Configuration Command
	SPI_Transmit(MCP23S17_SPI, mcpCmd, 3);

	// Disable CS pin
	GPIO_Write(MCP23S17_CS_GPIO, MCP23S17_CS_PIN, 1);

	return 0;
}

/**
 * -----------------------------------------------------------------------------
 * @brief	MCP23S17 SPI communication setup
 * -----------------------------------------------------------------------------
 * @return	Error code
 * -----------------------------------------------------------------------------
 */
uint8_t MCP23S17_SetupComm(void)
{
	// Exit if SPI2 peripheral is busy
	if (MCP23S17_SPI->SR & SPI_SR_BSY)
		return 1;

	SPI_Disable(MCP23S17_SPI);

	// Configure CPOL and CPHA
	MCP23S17_SPI->CR1 &= ~(0x03);
	MCP23S17_SPI->CR1 |= (MCP23S17_CPOL << SPI_CR1_CPOL_Pos);
	MCP23S17_SPI->CR1 |= (MCP23S17_CPHA << SPI_CR1_CPHA_Pos);

	// 8-bit Mode
	MCP23S17_SPI->CR1 &= ~(SPI_CR1_DFF);

	SPI_Enable(MCP23S17_SPI);

	return 0;
}

/**
 * -----------------------------------------------------------------------------
 * @brief 			Write a register
 * -----------------------------------------------------------------------------
 * @param address	Register address
 * @param data		Data to be written
 * -----------------------------------------------------------------------------
 */
void MCP23S17_Write(uint8_t address, uint8_t data)
{
	// MCP23S17 Write Operation
	mcpCmd[0] = MCP23S17_WRITE_OP;
	mcpCmd[1] = address;
	mcpCmd[2] = data;

	// Enable CS pin
	GPIO_Write(MCP23S17_CS_GPIO, MCP23S17_CS_PIN, 0);

	// Send Configuration Command
	SPI_Transmit(MCP23S17_SPI, mcpCmd, 3);

	// Disable CS pin
	GPIO_Write(MCP23S17_CS_GPIO, MCP23S17_CS_PIN, 1);
}

/**
 * -----------------------------------------------------------------------------
 * @brief 			Read a register
 * -----------------------------------------------------------------------------
 * @param address	Register address
 * @param pData		Pointer to storage variable
 * -----------------------------------------------------------------------------
 */
void MCP23S17_Read(uint8_t address, uint8_t *pData)
{
	// MCP23S17 Read Operation
	mcpCmd[0] = MCP23S17_READ_OP;
	mcpCmd[1] = address;

	// Enable CS pin
	GPIO_Write(MCP23S17_CS_GPIO, MCP23S17_CS_PIN, 0);

	// Send Configuration Command
	SPI_Transmit(MCP23S17_SPI, mcpCmd, 2);

	// Read MCP23S17 address
	SPI_Receive(MCP23S17_SPI, pData, 1);

	// Disable CS pin
	GPIO_Write(MCP23S17_CS_GPIO, MCP23S17_CS_PIN, 1);
}
