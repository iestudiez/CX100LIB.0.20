/**
 ******************************************************************************
 * @file			: uart.h
 * @author        	: Estudiez, Ivan
 * @brief         	: STM32F407 UART - DMA
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

#ifndef UART_H_
#define UART_H_

#include "stm32f4xx.h"
#include <stdint.h>
#include <stdbool.h>
#include "gpio.h"
#include "dma.h"

#define UART_DEFAULT_MODULE			USART2
#define UART_RX_BUFFER_SIZE			100
#define UART_TX_BUFFER_SIZE			80

typedef enum
{
	BR_9600 = 9600,
	BR_14400 = 14400,
	BR_19200 = 19200,
	BR_28800 = 28800,
	BR_38400 = 38400,
	BR_57600 = 57600,
	BR_115200 = 115200
} Baudrate_e;

typedef struct uart_s
{
	USART_TypeDef *module;
	Baudrate_e baudrate;
	uint32_t periph_clk;
	bool dma_tx;
	bool dma_rx;
	bool dir_tx;
	bool dir_rx;
	bool tcie;
	bool txeie;
	bool rxneie;
	bool idleie;
} UART_Config_t;

extern char UART_RxBuffer[UART_RX_BUFFER_SIZE];
extern char UART_TxBuffer[UART_TX_BUFFER_SIZE];

int __io_putchar(int ch);
void UART_Init(UART_Config_t uart);
void UART_Enable(USART_TypeDef *uart);
void UART_Disable(USART_TypeDef *uart);
void UART_Config_Reset(UART_Config_t *uart);
void UART_SetBaudrate(UART_Config_t uart);
void UART_Putchar(int ch);
char UART_Read(void);
void UART_ClearTcFlag(USART_TypeDef *module);
void UART_ClearIdleFlag(USART_TypeDef *module);

#endif /* UART_H_ */
