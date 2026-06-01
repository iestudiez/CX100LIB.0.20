/**
 ******************************************************************************
 * @file        : spi.c
 * @author      : Estudiez, Ivan
 * @brief       : STM32F407 SPI Library
 * @date        : 14/02/2024
 * @version     : 1.0
 ******************************************************************************
 *
 *
 *
 *
 *
 ******************************************************************************
 */

#include <spi.h>

// ============================================================================
// Public variables
// ============================================================================

// ============================================================================
// Private variables
// ============================================================================

void SPI_Init(SPI_Config_t spi)
{
	// Enable clock access
	// -------------------
	if (spi.module == SPI1)
		RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;
	else if (spi.module == SPI2)
		RCC->APB1ENR |= RCC_APB1ENR_SPI2EN;
	else if (spi.module == SPI3)
		RCC->APB1ENR |= RCC_APB1ENR_SPI3EN;

	// Polarity and Phase configuration
	// --------------------------------
	if (spi.cpol)
		spi.module->CR1 |= SPI_CR1_CPOL;
	else
		spi.module->CR1 &= ~SPI_CR1_CPOL;

	if (spi.cpha)
		spi.module->CR1 |= SPI_CR1_CPHA;
	else
		spi.module->CR1 &= ~SPI_CR1_CPHA;

	// RX Only
	// -------
	if (spi.rxonly)
		spi.module->CR1 |= SPI_CR1_RXONLY;
	else
		spi.module->CR1 &= ~SPI_CR1_RXONLY;

	// LSB First
	// ---------
	if (spi.lsbfrst)
		spi.module->CR1 |= SPI_CR1_LSBFIRST;
	else
		spi.module->CR1 &= ~SPI_CR1_LSBFIRST;

	// Master Mode Selection
	// ---------------------
	if (spi.mstr)
		spi.module->CR1 |= SPI_CR1_MSTR;
	else
		spi.module->CR1 &= ~SPI_CR1_MSTR;

	// Set Data Frame Format
	// ---------------------
	if (spi.dff)
		spi.module->CR1 |= SPI_CR1_DFF;
	else
		spi.module->CR1 &= ~SPI_CR1_DFF;

	// Set Software Slave Management
	// -----------------------------
	if (spi.ssm)
		spi.module->CR1 |= SPI_CR1_SSM;
	else
		spi.module->CR1 &= ~SPI_CR1_SSM;

	// Internal Slave Select
	// ---------------------
	if (spi.ssi)
		spi.module->CR1 |= SPI_CR1_SSI;
	else
		spi.module->CR1 &= ~SPI_CR1_SSI;

	// DMA configuration
	// -----------------
	if (spi.txdma)
		spi.module->CR2 |= SPI_CR2_TXDMAEN;
	else
		spi.module->CR2 &= ~ SPI_CR2_TXDMAEN;

	// Baudrate
	// --------
	spi.module->CR1 &= ~(SPI_CR1_BR_Msk);
	spi.module->CR1 |= (spi.br << SPI_CR1_BR_Pos);
}

void SPI_Config_Reset(SPI_Config_t *spiConf)
{
	spiConf->br = CLK_DIV_4;
	spiConf->cpol = false;
	spiConf->cpha = false;
	spiConf->rxonly = false;
	spiConf->lsbfrst = false;
	spiConf->mstr = true;
	spiConf->dff = false;
	spiConf->ssm = false;
	spiConf->ssi = false;
	spiConf->txdma = false;
	spiConf->rxdma = false;
}

void SPI_Enable(SPI_TypeDef *spi)
{
	spi->CR1 |= SPI_CR1_SPE;
}

void SPI_Disable(SPI_TypeDef *spi)
{
	spi->CR1 &= ~SPI_CR1_SPE;
}

// SPI Transmit Routine
void SPI_Transmit(SPI_TypeDef *spi, uint8_t *data, uint32_t size)
{
	uint32_t i = 0;

	while (i < size)
	{
		// Wait until TXE is set
		while (!(spi->SR & (SPI_SR_TXE)))
			;
		// Write the data to the register
		spi->DR = data[i];
		i++;
	}

	// Wait until TXE is set
	while (!(spi->SR & (SPI_SR_TXE)))
		;

	// Wait for BUSY flag to reset
	while (spi->SR & (SPI_SR_BSY))
		;

	// Clear OVR flag
	i = spi->DR;
	i = spi->SR;
}

// SPI Receive Routine
void SPI_Receive(SPI_TypeDef *spi, uint8_t *data, uint32_t size)
{
	while (size)
	{
		// Send dummy data
		spi->DR = 0;

		// Wait for RXNE flag to be set
		while (!(spi->SR & (SPI_SR_RXNE)))
			;

		// Read data from data register
		*data++ = (spi->DR);
		size--;
	}
}

void SPI_Select(GPIO_TypeDef *gpio, uint8_t pin, bool enable)
{
	if (enable)
		gpio->ODR &= ~(1U << pin);
	else
		gpio->ODR |= (1U << pin);
}

