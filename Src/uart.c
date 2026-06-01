/**
 ******************************************************************************
 * @file			: uart.c
 * @author      	: Estudiez, Ivan
 * @brief       	: STM32F407 UART
 * @date		  	: 30/01/2024
 * @version			: 1.0
 ******************************************************************************
 *
 *
 *
 *
 *
 ******************************************************************************
 */

#include "uart.h"
#include "rcc.h"
#include <stddef.h>
#include <stdio.h>

// ============================================================================
// Public variables
// ============================================================================
char UART_RxBuffer[UART_RX_BUFFER_SIZE];
char UART_TxBuffer[UART_TX_BUFFER_SIZE];

// ============================================================================
// Private variables
// ============================================================================
// Dummy variable to avoid GCC warning "unused variable"
uint8_t gDummy __attribute__ ((unused));

// ============================================================================
// Function Prototypes
// ============================================================================
void (*uartCallback)(void);

// ----------------------------------------------------------------------------
// __io_putchar()
// --------------
// Redirect standard output
// ----------------------------------------------------------------------------
int __io_putchar(int ch)
{
	UART_Putchar(ch);
	return ch;
}

// ----------------------------------------------------------------------------
// UART Initialization
// ----------------------------------------------------------------------------
void UART_Init(UART_Config_t uart)
{
	// Enable clock access to module
	// -----------------------------
	if (uart.module == USART1)
		RCC->APB2ENR |= RCC_APB2ENR_USART1EN;
	else if (uart.module == USART2)
		RCC->APB1ENR |= RCC_APB1ENR_USART2EN;
	else if (uart.module == USART3)
		RCC->APB1ENR |= RCC_APB1ENR_USART3EN;
	else if (uart.module == UART4)
		RCC->APB1ENR |= RCC_APB1ENR_UART4EN;
	else if (uart.module == UART5)
		RCC->APB1ENR |= RCC_APB1ENR_UART5EN;
	else if (uart.module == USART6)
		RCC->APB2ENR |= RCC_APB2ENR_USART6EN;

	// Clear CR1 register
	// ------------------
	uart.module->CR1 = 0;

	// Set Baudrate
	// ------------
	UART_SetBaudrate(uart);

	// DMA Section
	// -------------
	// Enable DMA TX
	// -------------
	if (uart.dma_tx)
		uart.module->CR3 |= USART_CR3_DMAT;
	else
		uart.module->CR3 &= ~USART_CR3_DMAT;

	// Enable DMA RX
	// -------------
	if (uart.dma_rx)
		uart.module->CR3 |= USART_CR3_DMAR;
	else
		uart.module->CR3 &= ~USART_CR3_DMAR;

	// Set transfer direction
	// ----------------------
	// Enable TX
	// ---------
	if (uart.dir_tx)
		uart.module->CR1 |= USART_CR1_TE;
	else
		uart.module->CR1 &= ~USART_CR1_TE;
	// Enable RX
	// ---------
	if (uart.dir_rx)
		uart.module->CR1 |= USART_CR1_RE;
	else
		uart.module->CR1 &= ~USART_CR1_RE;

	// Clear TC flag
	// -------------
	uart.module->SR &= ~USART_SR_TC;

	// Enable Transfer Complete Interrupt
	// ----------------------------------
	if (uart.tcie)
		uart.module->CR1 |= USART_CR1_TCIE;
	else
		uart.module->CR1 &= ~USART_CR1_TCIE;

	// Enable RXNE interrupt
	// ---------------------
	if (uart.rxneie)
		uart.module->CR1 |= USART_CR1_RXNEIE;
	else
		uart.module->CR1 &= ~USART_CR1_RXNEIE;

	// Enable idle line detection interrupt
	// ------------------------------------
	if (uart.idleie)
		uart.module->CR1 |= USART_CR1_IDLEIE;
	else
		uart.module->CR1 &= ~USART_CR1_IDLEIE;
}

// ----------------------------------------------------------------------------
// Enable UART module
// ----------------------------------------------------------------------------
void UART_Enable(USART_TypeDef *uart)
{
	uart->CR1 |= USART_CR1_UE;
}

// ----------------------------------------------------------------------------
// Disable UART module
// ----------------------------------------------------------------------------
void UART_Disable(USART_TypeDef *uart)
{
	uart->CR1 &= ~USART_CR1_UE;
}

// ----------------------------------------------------------------------------
// UART_Putchar()
// ----------------------------------------------------------------------------
void UART_Putchar(int ch)
{
	// Make sure the transmit data register is empty
	while (!(UART_DEFAULT_MODULE->SR & USART_SR_TXE))
		;

	// Write to transmit data register
	UART_DEFAULT_MODULE->DR = (ch & 0xFF);
}

// ----------------------------------------------------------------------------
// UART_Read()
// ----------------------------------------------------------------------------
char UART_Read(void)
{
	// Make sure the receive data register is not empty
	while (!(UART_DEFAULT_MODULE->SR & USART_SR_RXNE))
		;

	// Return data
	return UART_DEFAULT_MODULE->DR;
}

// ----------------------------------------------------------------------------
// UART_SetBaudrate()
// ----------------------------------------------------------------------------
void UART_SetBaudrate(UART_Config_t uart)
{
	uint16_t uartdiv = uart.periph_clk / uart.baudrate;

	uart.module->BRR = ((uartdiv / 16) << USART_BRR_DIV_Mantissa_Pos);
	uart.module->BRR |= ((uartdiv % 16) << USART_BRR_DIV_Fraction_Pos);
}

// ----------------------------------------------------------------------------
// UART_Config_Reset()
// -------------------
// Configuration variable reset
// ----------------------------------------------------------------------------
void UART_Config_Reset(UART_Config_t *uart)
{
	uart->baudrate = BR_9600;
	uart->module = NULL;
	uart->dir_tx = false;
	uart->dir_rx = false;
	uart->dma_tx = false;
	uart->dma_rx = false;
	uart->tcie = false;
	uart->rxneie = false;
	uart->txeie = false;
	uart->idleie = false;
}

// ----------------------------------------------------------------------------
// Clear Transmission Complete Flag
// ----------------------------------------------------------------------------
void UART_ClearTcFlag(USART_TypeDef *module)
{
	module->SR &= ~USART_SR_TC;
}

// ----------------------------------------------------------------------------
// Clear Idle Line Flag
// ----------------------------------------------------------------------------
void UART_ClearIdleFlag(USART_TypeDef *module)
{
	// Sequence to clear the IDLE flag
	gDummy = module->SR;
	gDummy = module->DR;
}

