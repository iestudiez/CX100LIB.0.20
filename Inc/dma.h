/**
 ******************************************************************************
 * @file		: dma.h
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

#ifndef DMA_H_
#define DMA_H_

#include "stm32f4xx.h"
#include <stdint.h>
#include <stdbool.h>
#include "gpio.h"

typedef enum
{
  MSIZE_BYTE = 0,
  MSIZE_HALF_WORD = 1,
  MSIZE_WORD = 2
} DataSize_e;

typedef enum
{
  PERIPH_TO_MEM = 0,
  MEM_TO_PERIPH = 1,
  MEM_TO_MEM = 2
} Direction_e;

typedef enum
{
  DMA_CH0 = 0,
  DMA_CH1 = 1,
  DMA_CH2 = 2,
  DMA_CH3 = 3,
  DMA_CH4 = 4,
  DMA_CH5 = 5,
  DMA_CH6 = 6,
  DMA_CH7 = 7,
} DmaCh_e;

typedef enum
{
  FIFO_1_4 = 0,
  FIFO_1_2 = 1,
  FIFO_3_4 = 2,
  FIFO_FULL = 3
} FifoTh_e;

typedef enum
{
  STREAM_0 = 0,
  STREAM_1 = 1,
  STREAM_2 = 2,
  STREAM_3 = 3,
  STREAM_4 = 4,
  STREAM_5 = 5,
  STREAM_6 = 6,
  STREAM_7 = 7,
} Stream_e;

typedef struct
{
  DMA_Stream_TypeDef* stream;
  DmaCh_e ch;
  uint32_t par;
  uint32_t m0ar;
  uint32_t m1ar;
  uint16_t ndtr;
  DataSize_e msize;
  DataSize_e psize;
  bool minc;
  bool pinc;
  bool circ;
  Direction_e dir;
  bool tcie;
  bool teie;
  bool dmdis;
  FifoTh_e fth;
} DMA_Config_t;

void DMA_Init(DMA_Config_t dma);
void DMA_Stream_Reset(DMA_Stream_TypeDef *dma_stream);
void DMA_Config_Reset(DMA_Config_t *dma);
void DMA_Stream_Enable(DMA_Stream_TypeDef *dma_stream);
void DMA_Stream_Disable(DMA_Stream_TypeDef *dma_stream);

// DMA Stream clear interrupts flags
void DMA_Clear_IntFlags(DMA_TypeDef *dma, Stream_e stream);

#endif /* DMA_H_ */
