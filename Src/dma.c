/**
 ******************************************************************************
 * @file		: dma.c
 * @author      : Estudiez, Ivan
 * @brief       : STM32F405 DMA Library
 * @date		: 17/01/2024
 * @version		: 1.0
 ******************************************************************************
 *
 *
 *
 *
 *
 ******************************************************************************
 */
#include "dma.h"

// DMA Initialization Routine
// ----------------------------------------------------------------------------
void DMA_Init(DMA_Config_t dma)
{
	DMA_TypeDef *dma_module;
	Stream_e dma_stream;

	// Determine the DMA module and its respective Stream
	// --------------------------------------------------
	if (dma.stream == DMA1_Stream0)
	{
		dma_module = DMA1;
		dma_stream = STREAM_0;
	}
	else if (dma.stream == DMA1_Stream1)
	{
		dma_module = DMA1;
		dma_stream = STREAM_1;
	}
	else if (dma.stream == DMA1_Stream2)
	{
		dma_module = DMA1;
		dma_stream = STREAM_2;
	}
	else if (dma.stream == DMA1_Stream3)
	{
		dma_module = DMA1;
		dma_stream = STREAM_3;
	}
	else if (dma.stream == DMA1_Stream4)
	{
		dma_module = DMA1;
		dma_stream = STREAM_4;
	}
	else if (dma.stream == DMA1_Stream5)
	{
		dma_module = DMA1;
		dma_stream = STREAM_5;
	}
	else if (dma.stream == DMA1_Stream6)
	{
		dma_module = DMA1;
		dma_stream = STREAM_6;
	}
	else if (dma.stream == DMA1_Stream7)
	{
		dma_module = DMA1;
		dma_stream = STREAM_7;
	}
	else if (dma.stream == DMA2_Stream0)
	{
		dma_module = DMA2;
		dma_stream = STREAM_0;
	}
	else if (dma.stream == DMA2_Stream1)
	{
		dma_module = DMA2;
		dma_stream = STREAM_1;
	}
	else if (dma.stream == DMA2_Stream2)
	{
		dma_module = DMA2;
		dma_stream = STREAM_2;
	}
	else if (dma.stream == DMA2_Stream3)
	{
		dma_module = DMA2;
		dma_stream = STREAM_3;
	}
	else if (dma.stream == DMA2_Stream4)
	{
		dma_module = DMA2;
		dma_stream = STREAM_4;
	}
	else if (dma.stream == DMA2_Stream5)
	{
		dma_module = DMA2;
		dma_stream = STREAM_5;
	}
	else if (dma.stream == DMA2_Stream6)
	{
		dma_module = DMA2;
		dma_stream = STREAM_6;
	}
	else if (dma.stream == DMA2_Stream7)
	{
		dma_module = DMA2;
		dma_stream = STREAM_7;
	}

	// Enable clock access to DMA module
	// ---------------------------------
	if (dma_module == DMA1)
		RCC->AHB1ENR |= RCC_AHB1ENR_DMA1EN;
	if (dma_module == DMA2)
		RCC->AHB1ENR |= RCC_AHB1ENR_DMA2EN;

	// Disable DMA stream
	// ------------------
	dma.stream->CR &= ~DMA_SxCR_EN;

	// Wait until stream is disabled
	while (dma.stream->CR & DMA_SxCR_EN)
	{
	}

	// Clear interrupt flags
	// ---------------------
	DMA_Clear_IntFlags(dma_module, dma_stream);

	// Configure DMA parameters
	// ------------------------

	// Select Channel
	dma.stream->CR &= ~(DMA_SxCR_CHSEL_Msk);
	dma.stream->CR |= (dma.ch << DMA_SxCR_CHSEL_Pos);

	// Set MSIZE (memory data size)
	dma.stream->CR &= ~(DMA_SxCR_MSIZE_Msk);
	dma.stream->CR |= (dma.msize << DMA_SxCR_MSIZE_Pos);

	// Set PSIZE (peripheral data size)
	dma.stream->CR &= ~(DMA_SxCR_PSIZE_Msk);
	dma.stream->CR |= (dma.psize << DMA_SxCR_PSIZE_Pos);

	// Memory increment
	if (dma.minc)
		dma.stream->CR |= DMA_SxCR_MINC;
	else
		dma.stream->CR &= ~DMA_SxCR_MINC;

	// Peripheral increment
	if (dma.pinc)
		dma.stream->CR |= DMA_SxCR_PINC;
	else
		dma.stream->CR &= ~DMA_SxCR_PINC;

	// Select transfer direction
	dma.stream->CR &= ~(DMA_SxCR_DIR_Msk);
	dma.stream->CR |= (dma.dir << DMA_SxCR_DIR_Pos);

	// Set peripheral address
	// (In memory to memory transfers the source address)
	dma.stream->PAR = dma.par;

	// Set memory address
	dma.stream->M0AR = dma.m0ar;

	// Set memory address
	dma.stream->M1AR = dma.m1ar;

	// Set number of transfers
	dma.stream->NDTR = dma.ndtr;

	// Set circular mode
	if (dma.circ)
		dma.stream->CR |= DMA_SxCR_CIRC;
	else
		dma.stream->CR &= ~DMA_SxCR_CIRC;

	// Transfer complete interrupt
	if (dma.tcie)
		dma.stream->CR |= DMA_SxCR_TCIE;
	else
		dma.stream->CR &= ~DMA_SxCR_TCIE;

	//Transfer error interrupt
	if (dma.teie)
		dma.stream->CR |= DMA_SxCR_TEIE;
	else
		dma.stream->CR &= ~DMA_SxCR_TEIE;

	// DMA Direct mode disable
	if (dma.dmdis)
		dma.stream->FCR |= DMA_SxFCR_DMDIS;
	else
		dma.stream->FCR &= ~DMA_SxFCR_DMDIS;

	// FIFO threshold selection
	if (dma.dmdis)
	{
		dma.stream->FCR &= ~(DMA_SxFCR_FTH_Msk);
		dma.stream->FCR |= (dma.fth << DMA_SxFCR_FTH_Pos);
	}
}

// DMA Stream Reset
// ----------------------------------------------------------------------------
void DMA_Stream_Reset(DMA_Stream_TypeDef *dma_stream)
{
	// Disable DMA stream
	// ------------------
	dma_stream->CR = 0x00;

	// Wait until stream is disabled
	while (dma_stream->CR & DMA_SxCR_EN)
	{
	}

	dma_stream->FCR = 0x00000021;
}

// DMA Stream clear interrupts flags
// ----------------------------------------------------------------------------
void DMA_Clear_IntFlags(DMA_TypeDef *dma, Stream_e stream)
{
	if (stream == STREAM_0)
		dma->LIFCR |= (DMA_LIFCR_CDMEIF0 | DMA_LIFCR_CTEIF0 | DMA_LIFCR_CTCIF0);
	else if (stream == STREAM_1)
		dma->LIFCR |= (DMA_LIFCR_CDMEIF1 | DMA_LIFCR_CTEIF1 | DMA_LIFCR_CTCIF1);
	else if (stream == STREAM_2)
		dma->LIFCR |= (DMA_LIFCR_CDMEIF2 | DMA_LIFCR_CTEIF2 | DMA_LIFCR_CTCIF2);
	else if (stream == STREAM_3)
		dma->LIFCR |= (DMA_LIFCR_CDMEIF3 | DMA_LIFCR_CTEIF3 | DMA_LIFCR_CTCIF2);
	else if (stream == STREAM_4)
		dma->HIFCR |= (DMA_HIFCR_CDMEIF4 | DMA_HIFCR_CTEIF4 | DMA_HIFCR_CTCIF4);
	else if (stream == STREAM_5)
		dma->HIFCR |= (DMA_HIFCR_CDMEIF5 | DMA_HIFCR_CTEIF5 | DMA_HIFCR_CTCIF5);
	else if (stream == STREAM_6)
		dma->HIFCR |= (DMA_HIFCR_CDMEIF6 | DMA_HIFCR_CTEIF6 | DMA_HIFCR_CTCIF6);
	else if (stream == STREAM_7)
		dma->HIFCR |= (DMA_HIFCR_CDMEIF7 | DMA_HIFCR_CTEIF7 | DMA_HIFCR_CTCIF7);
}

// DMA Configuration Structure Reset
// ----------------------------------------------------------------------------
void DMA_Config_Reset(DMA_Config_t *dma)
{
	dma->ch = 0;
	dma->par = 0;
	dma->m0ar = 0;
	dma->m1ar = 0;
	dma->ndtr = 0;
	dma->dir = 0;
	dma->msize = MSIZE_BYTE;
	dma->psize = MSIZE_BYTE;
	dma->circ = false;
	dma->minc = false;
	dma->pinc = false;
	dma->tcie = false;
	dma->teie = false;
	dma->dmdis = false;
}

// DMA Stream Enable
// ----------------------------------------------------------------------------
void DMA_Stream_Enable(DMA_Stream_TypeDef *dma_stream)
{
	dma_stream->CR |= DMA_SxCR_EN;
}

// DMA Stream Disable
// ----------------------------------------------------------------------------
void DMA_Stream_Disable(DMA_Stream_TypeDef *dma_stream)
{
	dma_stream->CR &= ~DMA_SxCR_EN;

	// Wait until stream is disabled
	while (dma_stream->CR & DMA_SxCR_EN)
	{
	}
}
