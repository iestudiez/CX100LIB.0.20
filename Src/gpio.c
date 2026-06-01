/**
 ******************************************************************************
 * @file		: gpio.c
 * @author  	: Estudiez, Ivan
 * @brief   	: STM32F405 GPIO LIB
 * @date		: 11/08/2025
 * @version		: 1.1
 ******************************************************************************
 * Change control:
 * 1.0 - (16/01/2024) Release version
 * 1.1 - (11/08/2025) Changes in Pull-Up Pull-Down section
 *
 *
 ******************************************************************************
 */

#include "gpio.h"

// ----------------------------------------------------------------------------
// GPIO_Init()
// -----------
// Initialize GPIO pin
// ----------------------------------------------------------------------------
void GPIO_Init(GPIO_Config_t gpio)
{
	// Check if pin number is greater than 15
	if (gpio.pin > 15)
		return;

	// ---------------------------
	// Enable clock access to GPIO
	// ---------------------------
	if (gpio.port == GPIOA)
		RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;

	else if (gpio.port == GPIOB)
		RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;

	else if (gpio.port == GPIOC)
		RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN;

	else if (gpio.port == GPIOD)
		RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN;

	else if (gpio.port == GPIOE)
		RCC->AHB1ENR |= RCC_AHB1ENR_GPIOEEN;

	// ---------------------------
	// Configure selected pin mode
	// ---------------------------
	switch (gpio.mode)
	{
	case INPUT:
		gpio.port->MODER &= ~(0b11 << (gpio.pin * 2));
		break;

	case OUTPUT:
		// Mode register
		gpio.port->MODER &= ~(0b11 << (gpio.pin * 2));
		gpio.port->MODER |= (0b01 << (gpio.pin * 2));

		// Output type
		if (gpio.otype == OPEN_DRAIN)
			gpio.port->OTYPER |= (0b1 << gpio.pin);
		else
			gpio.port->OTYPER &= ~(0b1 << gpio.pin);

		// Configure Speed
		gpio.port->OSPEEDR &= ~(0b11 << (gpio.pin * 2));
		gpio.port->OSPEEDR |= (gpio.speed << (gpio.pin * 2));
		break;

	case AFUNCTION:
		gpio.port->MODER &= ~(0b11 << (gpio.pin * 2));
		gpio.port->MODER |= (0b10 << (gpio.pin * 2));
		break;

	case ANALOG:
		gpio.port->MODER |= (0b11 << (gpio.pin * 2));
	}

	// --------------------------------------
	// Configure Pull Up - Pull Down Register
	// --------------------------------------

	if (gpio.pupd != NO_PULL_RESISTOR)
	{
		// Reset configuration
		gpio.port->PUPDR &= ~(0b11 << gpio.pin * 2);

		// Set Pull Up Resistor
		if (gpio.pupd == PULL_UP_RESISTOR)
			gpio.port->PUPDR |= (0b01 << gpio.pin * 2);

		// Set Pull Down Resistor
		if (gpio.pupd == PULL_DOWN_RESISTOR)
			gpio.port->PUPDR |= (0b10 << gpio.pin * 2);
	}

	// -------------------------
	// Configure selected pin AF
	// -------------------------
	if (gpio.mode != AFUNCTION)
		return;

	// Configure Alternate Function Register Low
	if (gpio.pin < 8)
	{
		gpio.port->AFR[0] &= ~(0b1111 << (gpio.pin * 4));
		gpio.port->AFR[0] |= (gpio.af << (gpio.pin * 4));
		return;
	}

	// Configure Alternate Function Register High
	uint8_t pin = gpio.pin - 8;
	gpio.port->AFR[1] &= ~(0b1111 << (pin * 4));
	gpio.port->AFR[1] |= (gpio.af << (pin * 4));
}

// ----------------------------------------------------------------------------
// GPIO_Config_Reset()
// -------------------
// Clear GPIO configuration structure
// ----------------------------------------------------------------------------
void GPIO_Config_Reset(GPIO_Config_t *gpio)
{
	gpio->port = GPIOA;
	gpio->pin = 0;
	gpio->mode = INPUT;
	gpio->af = AF0;
	gpio->speed = LOW_SPEED;
	gpio->otype = PUSH_PULL;
	gpio->pupd = NO_PULL_RESISTOR;
}

// ----------------------------------------------------------------------------
// GPIO_Write()
// ------------
// Change the state of output pin
// ----------------------------------------------------------------------------
void GPIO_Write(GPIO_TypeDef *gpio_port, uint8_t pin, bool pinState)
{
	if (pinState)
	{
		gpio_port->BSRR |= (1U << pin);
		return;
	}
	gpio_port->BSRR |= (1U << (pin + 16));
}

// ----------------------------------------------------------------------------
// GPIO_Read()
// -----------
// Read the state of input pin
// ----------------------------------------------------------------------------
bool GPIO_Read(GPIO_TypeDef *gpio_port, uint8_t pin)
{
	if (gpio_port->IDR & (1U << pin))
		return true;
	else
		return false;
}
