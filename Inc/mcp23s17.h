/**
 ******************************************************************************
 * @file        : mcp23s17.h
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

#ifndef INC_MCP23S17_H_
#define INC_MCP23S17_H_

#include <stdint.h>
#include <stdbool.h>
#include "system_cx100.h"
#include <stm32f4xx.h>

//=============================================================================
// Device definitions
//=============================================================================
#define MCP23S17_SPI			SPI2
#define MCP23S17_CS_GPIO		SYSTEM_SPI2_CS1_GPIO
#define MCP23S17_CS_PIN			SYSTEM_SPI2_CS1_PIN
#define MCP23S17_RST_GPIO		GPIOB
#define MCP23S17_RST_PIN		11U
#define MCP23S17_CPOL			0U
#define MCP23S17_CPHA			0U
#define MCP23S17_IODIRA			0x0F
#define MCP23S17_IODIRB			0xE0

//=============================================================================
// All Registers and there Address if BANK = 0
//=============================================================================

#define IODIRA_ADDR		0x00       // Controls the direction of the data I/O on Port A
#define IODIRB_ADDR		0x01       // Controls the direction of the data I/O on Port B
#define IPOLA_ADDR    	0x02       // Configure the polarity on the corresponding GPIO (Port A)
#define IPOLB_ADDR    	0x03       // Configure the polarity on the corresponding GPIO (Port B)
#define GPINTENA_ADDR 	0x04       // Controls the interrupt-on change feature for each pin for Port A
#define GPINTENB_ADDR 	0x05       // Controls the interrupt-on change feature for each pin for Port B
#define DEFVALA_ADDR  	0x06       // The default comparison value if the INTCONA is set to "1" for Port A
#define DEFVALB_ADDR  	0x07       // The default comparison value if the INTCONA is set to "1" for Port B
#define INTCONA_ADDR  	0x08       // Controls how the associated pin value is compared for the interrupt-on-change feature for Port A
#define INTCONB_ADDR  	0x09       // Controls how the associated pin value is compared for the interrupt-on-change feature for Port B
#define IOCON_ADDR    	0x0A       // Contains several bits for configuring the device
#define GPPUA_ADDR    	0x0C       // Controls the pull-up resistors for the port pins for port A
#define GPPUB_ADDR    	0x0D       // Controls the pull-up resistors for the port pins for port B
#define INTFA_ADDR    	0x0E       // READ ONLY: reflects the interrupt condition on port A pins of any pin that is enabled for interrupts via the GPINTEN register.
#define INTFB_ADDR    	0x0F       // READ ONLY: reflects the interrupt condition on port B pins of any pin that is enabled for interrupts via the GPINTEN register.
#define INTCAPA_ADDR  	0x10       // READ ONLY: captures the GPIO port A value at the time the interrupt occurred
#define INTCAPB_ADDR  	0x11       // READ ONLY: captures the GPIO port B value at the time the interrupt occurred
#define GPIOA_ADDR    	0x12       // Reflects the value on the port A (doing write function it only read input)
#define GPIOB_ADDR    	0x13       // Reflects the value on the port B (doing write function it only read input)
#define OLATA_ADDR    	0x14       // A write to this register modifies the output latches that modifies the pins configured as outputs for Port A
#define OLATB_ADDR    	0x15       // A write to this register modifies the output latches that modifies the pins configured as outputs for Port B

// ============================================================================
// Function prototypes
// ============================================================================
uint8_t MCP23S17_Init(void);
uint8_t MCP23S17_SetupComm(void);
void MCP23S17_Write(uint8_t address, uint8_t data);
void MCP23S17_Read(uint8_t address, uint8_t *data);

// ============================================================================
// Public variables
// ============================================================================
#endif /* INC_MCP23S17_H_ */
