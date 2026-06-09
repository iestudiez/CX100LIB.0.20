/**
 ******************************************************************************
 * @file        : system_cx100.c
 * @author      : Estudiez, Ivan
 * @brief       : CX100 System functions and hardware initialization
 * @date        : 01/08/2025
 * @version     : 1.0
 ******************************************************************************
 * Change control:
 * 1.0 - (01/08/2025) Release version
 *
 *
 *
 ******************************************************************************
 */

#include <system_cx100.h>
#include "gpio.h"
#include "tim.h"
#include "adc.h"
#include "dma.h"
#include "spi.h"
#include "can.h"
#include "lcd.h"
#include "uart.h"
#include "rcc.h"

// Definitions
// ----------------------------------------------------------------------------

// Private variables
// ----------------------------------------------------------------------------
GPIO_Config_t gpioConf;
TIM_Config_t timerConf;
ADC_Config_t adcConf;
DMA_Config_t dmaConf;
SPI_Config_t spiConf;
CAN_Config_t canConf;
UART_Config_t uartConf;

void SYSTEM_Init()
{
	// ************************************************************************
	// * RCC Configuration		                                    		  *
	// ************************************************************************
	RCC_SystemClockConfig();

	// ************************************************************************
	// * FPU Section			                                    		  *
	// ************************************************************************
	// Set CP10 and CP11 Full Access
	SCB->CPACR |= ((3UL << 20U) | (3UL << 22U));

	//************************************************************************
	//* Keyboard Configuration Section
	//* CX100 Compatibility: OK.
	//************************************************************************

	// PB14 (KEYBD_1 COLUMN1 - Output)
	GPIO_Config_Reset(&gpioConf);
	gpioConf.port = GPIOB;
	gpioConf.pin = 14;
	gpioConf.mode = OUTPUT;
	gpioConf.otype = PUSH_PULL;
	GPIO_Init(gpioConf);

	// PB15 (KEYBD_2 COLUMN2 - Output)
	GPIO_Config_Reset(&gpioConf);
	gpioConf.port = GPIOB;
	gpioConf.pin = 15;
	gpioConf.mode = OUTPUT;
	gpioConf.otype = PUSH_PULL;
	GPIO_Init(gpioConf);

	// PC8 (KEYBD_3 COLUMN3 - Output)
	GPIO_Config_Reset(&gpioConf);
	gpioConf.port = GPIOC;
	gpioConf.pin = 8;
	gpioConf.mode = OUTPUT;
	gpioConf.otype = PUSH_PULL;
	GPIO_Init(gpioConf);

	// PC9 (KEYBD_4 ROW1 - Input Pull Down)
	GPIO_Config_Reset(&gpioConf);
	gpioConf.port = GPIOC;
	gpioConf.pin = 9;
	gpioConf.mode = INPUT;
	gpioConf.pupd = PULL_DOWN_RESISTOR;
	GPIO_Init(gpioConf);

	// PC10 (KEYBD_5 ROW2 - Input Pull Down)
	GPIO_Config_Reset(&gpioConf);
	gpioConf.port = GPIOC;
	gpioConf.pin = 10;
	gpioConf.mode = INPUT;
	gpioConf.pupd = PULL_DOWN_RESISTOR;
	GPIO_Init(gpioConf);

	// PC11 (KEYBD_6 ROW3 - Input Pull Down)
	GPIO_Config_Reset(&gpioConf);
	gpioConf.port = GPIOC;
	gpioConf.pin = 11;
	gpioConf.mode = INPUT;
	gpioConf.pupd = PULL_DOWN_RESISTOR;
	GPIO_Init(gpioConf);

	// PC12 (KEYBD_7 ROW4 - Input Pull Down)
	GPIO_Config_Reset(&gpioConf);
	gpioConf.port = GPIOC;
	gpioConf.pin = 12;
	gpioConf.mode = INPUT;
	gpioConf.pupd = PULL_DOWN_RESISTOR;
	GPIO_Init(gpioConf);

	// PD2 (KEYBD_8 ROW5 - Input Pull Down)
	GPIO_Config_Reset(&gpioConf);
	gpioConf.port = GPIOD;
	gpioConf.pin = 2;
	gpioConf.mode = INPUT;
	gpioConf.pupd = PULL_DOWN_RESISTOR;
	GPIO_Init(gpioConf);

	// ************************************************************************
	// * Display Section                                                      *
	// * CX100 Compatibility: OK.   										  *
	// ************************************************************************

	// PB4 (Red Backlight - Output)
	GPIO_Config_Reset(&gpioConf);
	gpioConf.port = SYSTEM_LCD_RED_GPIO;
	gpioConf.pin = SYSTEM_LCD_RED_PIN;
	gpioConf.mode = OUTPUT;
	gpioConf.otype = PUSH_PULL;
	GPIO_Init(gpioConf);

	// PB5 (Green Backlight - Output)
	GPIO_Config_Reset(&gpioConf);
	gpioConf.port = SYSTEM_LCD_GREEN_GPIO;
	gpioConf.pin = SYSTEM_LCD_GREEN_PIN;
	gpioConf.mode = OUTPUT;
	gpioConf.otype = PUSH_PULL;
	GPIO_Init(gpioConf);

	// PB6 (Blue Backlight - Output)
	GPIO_Config_Reset(&gpioConf);
	gpioConf.port = SYSTEM_LCD_BLUE_GPIO;
	gpioConf.pin = SYSTEM_LCD_BLUE_PIN;
	gpioConf.mode = OUTPUT;
	gpioConf.otype = PUSH_PULL;
	GPIO_Init(gpioConf);

	// SPI1_CS2 (Display Chip Select - Output)
	GPIO_Config_Reset(&gpioConf);
	gpioConf.port = SYSTEM_LCD_CS_GPIO;
	gpioConf.pin = SYSTEM_LCD_CS_PIN;
	gpioConf.mode = OUTPUT;
	gpioConf.otype = OPEN_DRAIN;
	gpioConf.pupd = PULL_UP_RESISTOR;
	GPIO_Init(gpioConf);

	// PC4 LCD_RST (Display Reset - Output)
	GPIO_Config_Reset(&gpioConf);
	gpioConf.port = GPIOC;
	gpioConf.pin = 4;
	gpioConf.mode = OUTPUT;
	gpioConf.otype = PUSH_PULL;
	GPIO_Init(gpioConf);

	// PC5 LCD_A0 (Display Register Select /A0 - Output)
	GPIO_Config_Reset(&gpioConf);
	gpioConf.port = GPIOC;
	gpioConf.pin = 5;
	gpioConf.mode = OUTPUT;
	gpioConf.otype = PUSH_PULL;
	GPIO_Init(gpioConf);

	// ************************************************************************
	// * EEPROM Chip Select                                                   *
	// * CX100 Compatibility: OK											  *
	// ************************************************************************

	// Microchip 25LC640A SPI1_CS1 (Chip Select Pin - Output)
	GPIO_Config_Reset(&gpioConf);
	gpioConf.port = SYSTEM_EPROM_CS_GPIO;
	gpioConf.pin = SYSTEM_EPROM_CS_PIN;
	gpioConf.mode = OUTPUT;
	gpioConf.otype = OPEN_DRAIN;
	gpioConf.pupd = PULL_UP_RESISTOR;
	GPIO_Init(gpioConf);

	// ************************************************************************
	// * SPI1 Initialization                                                  *
	// * CX100 Compatibility: OK.   										  *
	// ************************************************************************

	// Initialize Chip Select Pins (CS)
	// -----------------------------------------------------------------------
	// In open-drain mode, a "0" in the output register activates
	// the NMOS transistor.
	// -----------------------------------------------------------------------
	GPIO_Write(SYSTEM_EPROM_CS_GPIO, SYSTEM_EPROM_CS_PIN, 1);
	GPIO_Write(SYSTEM_LCD_CS_GPIO, SYSTEM_LCD_CS_PIN, 1);

	// PA5 SPI1_SCK (Alternate Function SPI clock)
	GPIO_Config_Reset(&gpioConf);
	gpioConf.port = GPIOA;
	gpioConf.pin = 5;
	gpioConf.mode = AFUNCTION;
	gpioConf.af = AF5;
	GPIO_Init(gpioConf);

	// PA6 SPI1_MISO (Alternate Function SPI Serial Input)
	GPIO_Config_Reset(&gpioConf);
	gpioConf.port = GPIOA;
	gpioConf.pin = 6;
	gpioConf.mode = AFUNCTION;
	gpioConf.af = AF5;
	GPIO_Init(gpioConf);

	// PA7 SPI1_MOSI (Alternate Function SPI Serial Output)
	GPIO_Config_Reset(&gpioConf);
	gpioConf.port = GPIOA;
	gpioConf.pin = 7;
	gpioConf.mode = AFUNCTION;
	gpioConf.af = AF5;
	GPIO_Init(gpioConf);

	// SPI Configuration
	// -----------------
	// SPI1 Module
	SPI_Config_Reset(&spiConf);
	spiConf.module = SPI1;
	spiConf.br = CLK_DIV_4;
	spiConf.cpol = true;
	spiConf.cpha = true;
	spiConf.rxonly = false;
	spiConf.lsbfrst = false;
	spiConf.mstr = true;	// 8-bit data format
	spiConf.dff = false;	// Slave Select Mode (Software management ssm-bit=1)
	spiConf.ssm = true;
	spiConf.ssi = true;
	spiConf.txdma = true;
	SPI_Init(spiConf);
	SPI_Enable(SPI1);

	// ************************************************************************
	// * DMA2 for SPI1 Initialization                                         *
	// * CX100 Compatibility: OK.											  *
	// ************************************************************************

	// Configure DMA2 Stream3 - Channel 3 (TX)
	// ---------------------------------------
	DMA_Config_Reset(&dmaConf);

	dmaConf.stream = DMA2_Stream3;
	dmaConf.ch = 3;
	dmaConf.par = (uint32_t) (&(SPI1->DR));
	dmaConf.msize = MSIZE_BYTE;
	dmaConf.dir = MEM_TO_PERIPH;
	dmaConf.ndtr = LCD_SCREEN_WIDTH;
	dmaConf.minc = true;
	dmaConf.circ = false;
	dmaConf.tcie = false;
	DMA_Init(dmaConf);

	// Enable DMA2_Stream3
	DMA_Stream_Enable(DMA2_Stream3);

	// ************************************************************************
	// * SPI2 Initialization                                                  *
	// * CX100 Compatibility: OK.											  *
	// ************************************************************************

	// SPI2 Chip Select 1
	GPIO_Config_Reset(&gpioConf);
	gpioConf.port = SYSTEM_SPI2_CS1_GPIO;
	gpioConf.pin = SYSTEM_SPI2_CS1_PIN;
	gpioConf.mode = OUTPUT;
	gpioConf.otype = PUSH_PULL;
	GPIO_Init(gpioConf);

	// SPI2 Chip Select 2
	GPIO_Config_Reset(&gpioConf);
	gpioConf.port = SYSTEM_SPI2_CS2_GPIO;
	gpioConf.pin = SYSTEM_SPI2_CS2_PIN;
	gpioConf.mode = OUTPUT;
	gpioConf.otype = PUSH_PULL;
	GPIO_Init(gpioConf);

	// SPI2 Chip Select 3
	GPIO_Config_Reset(&gpioConf);
	gpioConf.port = SYSTEM_SPI2_CS3_GPIO;
	gpioConf.pin = SYSTEM_SPI2_CS3_PIN;
	gpioConf.mode = OUTPUT;
	gpioConf.otype = PUSH_PULL;
	GPIO_Init(gpioConf);

	// PB11 PRST (Power Board Reset)
	GPIO_Config_Reset(&gpioConf);
	gpioConf.port = GPIOB;
	gpioConf.pin = 11;
	gpioConf.mode = OUTPUT;
	gpioConf.otype = PUSH_PULL;
	GPIO_Init(gpioConf);

	// Initialize SPI2 CS pins
	GPIO_Write(SYSTEM_SPI2_CS1_GPIO, SYSTEM_SPI2_CS1_PIN, 1);
	GPIO_Write(SYSTEM_SPI2_CS2_GPIO, SYSTEM_SPI2_CS2_PIN, 1);
	GPIO_Write(SYSTEM_SPI2_CS3_GPIO, SYSTEM_SPI2_CS3_PIN, 1);

	// PB13 SPI2_SCK (Alternate Function SPI clock)
	GPIO_Config_Reset(&gpioConf);
	gpioConf.port = GPIOB;
	gpioConf.pin = 13;
	gpioConf.mode = AFUNCTION;
	gpioConf.af = AF5;
	GPIO_Init(gpioConf);

	// PC2 SPI2_MISO (Alternate Function SPI Serial Input)
	GPIO_Config_Reset(&gpioConf);
	gpioConf.port = GPIOC;
	gpioConf.pin = 2;
	gpioConf.mode = AFUNCTION;
	gpioConf.af = AF5;
	GPIO_Init(gpioConf);

	// PC3 SPI2_MOSI (Alternate Function SPI Serial Output)
	GPIO_Config_Reset(&gpioConf);
	gpioConf.port = GPIOC;
	gpioConf.pin = 3;
	gpioConf.mode = AFUNCTION;
	gpioConf.af = AF5;
	GPIO_Init(gpioConf);

	// SPI Configuration
	// -----------------
	// SPI2 Module
	SPI_Config_Reset(&spiConf);
	spiConf.module = SPI2;
	spiConf.br = CLK_DIV_16;
	spiConf.cpol = 0;
	spiConf.cpha = 0;
	spiConf.rxonly = false;
	spiConf.lsbfrst = false;
	spiConf.mstr = true;
	spiConf.dff = false;
	spiConf.ssm = true;
	spiConf.ssi = true;
	spiConf.txdma = true;
	SPI_Init(spiConf);
	SPI_Enable(SPI2);

	// ************************************************************************
	// * UART1 Initialization                                                 *
	// ************************************************************************

	// PB7 UART1_RX (Alternate Function UART1 RX)
	// ------------------------------------------
	GPIO_Config_Reset(&gpioConf);
	gpioConf.port = GPIOB;
	gpioConf.pin = 7;
	gpioConf.mode = AFUNCTION;
	gpioConf.af = AF7;
	GPIO_Init(gpioConf);

	// PB6 UART1_TX (Alternate Function UART1 TX)
	// ------------------------------------------
	GPIO_Config_Reset(&gpioConf);
	gpioConf.port = GPIOB;
	gpioConf.pin = 6;
	gpioConf.mode = AFUNCTION;
	gpioConf.af = AF7;
	GPIO_Init(gpioConf);

	// USART Configuration
	// -------------------
	// UART1 Module
	UART_Config_Reset(&uartConf);
	uartConf.module = USART1;
	uartConf.periph_clk = (uint32_t) RCC_APB2_CLK;
	uartConf.baudrate = BR_9600;
	uartConf.dir_tx = true;
	uartConf.dir_rx = true;
	uartConf.rxneie = false;
	uartConf.tcie = false;
	uartConf.idleie = false;
	UART_Init(uartConf);

	// Enable Module
	// -------------
	UART_Enable(USART1);

	// ************************************************************************
	// * UART2 Initialization                                                 *
	// ************************************************************************

	// PA2 UART2_RX (Alternate Function UART2 RX)
	// -----------------------------------------
	GPIO_Config_Reset(&gpioConf);
	gpioConf.port = GPIOA;
	gpioConf.pin = 2;
	gpioConf.mode = AFUNCTION;
	gpioConf.af = AF7;
	GPIO_Init(gpioConf);

	// PA3 UART2_TX (Alternate Function UART2 TX)
	// ----------------------------------------
	GPIO_Config_Reset(&gpioConf);
	gpioConf.port = GPIOA;
	gpioConf.pin = 3;
	gpioConf.mode = AFUNCTION;
	gpioConf.af = AF7;
	GPIO_Init(gpioConf);

	// UART Configuration
	// ------------------
	// UART2 Module
	UART_Config_Reset(&uartConf);
	uartConf.module = USART2;
	uartConf.periph_clk = (uint32_t) RCC_APB1_CLK;
	uartConf.baudrate = BR_9600;
	uartConf.dir_tx = true;
	uartConf.dir_rx = true;
	uartConf.dma_tx = true;
	uartConf.dma_rx = true;
	uartConf.rxneie = false;
	uartConf.tcie = false;
	uartConf.idleie = false;
	UART_Init(uartConf);

	// Enable Module
	// -------------
	// Enable the module in the GNSS section
	// UART_Enable(USART2);

	// ************************************************************************
	// * DMA1 for USART2 Initialization										  *
	// * CX100 Compatibility: OK.											  *
	// ************************************************************************

	// Configure DMA1 Stream5 - Channel 4 (RX)
	// ---------------------------------------
	DMA_Config_Reset(&dmaConf);
	dmaConf.stream = DMA1_Stream5;
	dmaConf.ch = 4;
	dmaConf.par = (uint32_t) (&(USART2->DR));
	dmaConf.m0ar = (uint32_t) &UART_RxBuffer;
	dmaConf.msize = MSIZE_BYTE;
	dmaConf.dir = PERIPH_TO_MEM;
	dmaConf.ndtr = UART_RX_BUFFER_SIZE;
	dmaConf.minc = true;
	dmaConf.circ = true;
	dmaConf.tcie = false;
	DMA_Init(dmaConf);

	// Enable DMA1_Stream5
	DMA_Stream_Enable(DMA1_Stream5);

	// Configure DMA1 Stream6 - Channel 4 (TX)
	// ---------------------------------------
	DMA_Config_Reset(&dmaConf);
	dmaConf.stream = DMA1_Stream6;
	dmaConf.ch = 4;
	dmaConf.par = (uint32_t) (&(USART2->DR));
	dmaConf.m0ar = (uint32_t) &UART_TxBuffer;
	dmaConf.msize = MSIZE_BYTE;
	dmaConf.dir = MEM_TO_PERIPH;
	dmaConf.ndtr = UART_TX_BUFFER_SIZE;
	dmaConf.minc = true;
	dmaConf.circ = false;
	dmaConf.tcie = false;
	DMA_Init(dmaConf);

	// Enable DMA1_Stream6
	// DMA transfer is enabled whenever a transmission is needed.
	// DMA_Stream_Enable(DMA1_Stream6);

	// ************************************************************************
	// * DMA2: Memory to memory transfer									  *
	// * CX100 Compatibility: OK.											  *
	// ************************************************************************

	// Configure DMA2 Stream0
	// ---------------------------------------
	DMA_Config_Reset(&dmaConf);

	dmaConf.stream = DMA2_Stream0;
	dmaConf.ch = 0;
	dmaConf.par = (uint32_t) LCD_Buffer;
	dmaConf.msize = MSIZE_BYTE;
	dmaConf.psize = MSIZE_BYTE;
	dmaConf.dir = MEM_TO_MEM;
	dmaConf.dmdis = true;
	dmaConf.fth = FIFO_FULL;
	dmaConf.minc = true;
	dmaConf.pinc = true;
	dmaConf.circ = false;
	dmaConf.tcie = false;
	DMA_Init(dmaConf);

	// ************************************************************************
	// * CAN1 Initialization                                                  *
	// * CX100 Compatibility: OK.											  *
	// ************************************************************************

	// PB8 CAN_RX (Alternate Function CAN RX)
	GPIO_Config_Reset(&gpioConf);
	gpioConf.port = GPIOB;
	gpioConf.pin = 8;
	gpioConf.mode = AFUNCTION;
	gpioConf.af = AF9;
	GPIO_Init(gpioConf);

	// PA12 CAN_TX (Alternate Function CAN TX)
	GPIO_Config_Reset(&gpioConf);
	gpioConf.port = GPIOA;
	gpioConf.pin = 12;
	gpioConf.mode = AFUNCTION;
	gpioConf.af = AF9;
	GPIO_Init(gpioConf);

	// PB12 CAN TERMINATION (Output - Open Drain)
	GPIO_Config_Reset(&gpioConf);
	gpioConf.port = GPIOB;
	gpioConf.pin = 12;
	gpioConf.mode = OUTPUT;
	gpioConf.otype = OPEN_DRAIN;
	GPIO_Init(gpioConf);

	// PB12 Initialization
	// (CAN Termination disable)
	GPIO_Write(SYSTEM_CANTR_GPIO, SYSTEM_CANTR_PIN, 1);

	// CAN Configuration
	// -----------------
	// CAN1 Module
	CAN_Config_Reset(&canConf);
	canConf.module = CAN1;
	canConf.nart = true;
	canConf.baudrate = CAN_BR_250K;
	canConf.fmpie0 = false;
	CAN_Init(canConf);

	// ************************************************************************
	// * LEDs Section                                                         *
	// * CX100 Compatibility: OK											  *
	// ************************************************************************

	// Power LED "Green" - Output
	GPIO_Config_Reset(&gpioConf);
	gpioConf.port = SYSTEM_PWRLED_GPIO;
	gpioConf.pin = SYSTEM_PWRLED_PIN;
	gpioConf.mode = OUTPUT;
	gpioConf.otype = PUSH_PULL;
	GPIO_Init(gpioConf);

	// Message LED "Blue" - Output
	GPIO_Config_Reset(&gpioConf);
	gpioConf.port = SYSTEM_MSGLED_GPIO;
	gpioConf.pin = SYSTEM_MSGLED_PIN;
	gpioConf.mode = OUTPUT;
	gpioConf.otype = PUSH_PULL;
	GPIO_Init(gpioConf);

	// Error LED "Red" - Output
	GPIO_Config_Reset(&gpioConf);
	gpioConf.port = SYSTEM_ERRLED_GPIO;
	gpioConf.pin = SYSTEM_ERRLED_PIN;
	gpioConf.mode = OUTPUT;
	gpioConf.otype = PUSH_PULL;
	GPIO_Init(gpioConf);

	// ************************************************************************
	// * Frequency Inputs Section                                             *
	// * CX100 Compatibility: OK.											  *
	// ************************************************************************

	// PC7 (FREQ IN1 - Alternate Function Input Capture)
	// TIM8_CH2 - TI2FP2
	GPIO_Config_Reset(&gpioConf);
	gpioConf.port = GPIOC;
	gpioConf.pin = 7;
	gpioConf.mode = AFUNCTION;
	gpioConf.af = AF3;
	GPIO_Init(gpioConf);

	// PC6 (FREQ IN2 - Alternate Function Input Capture)
	// TIM3_CH1 - TI1FP1
	GPIO_Config_Reset(&gpioConf);
	gpioConf.port = GPIOC;
	gpioConf.pin = 6;
	gpioConf.mode = AFUNCTION;
	gpioConf.af = AF2;
	GPIO_Init(gpioConf);

	// PA0 (FREQ IN3 - Alternate Function Input Capture)
	// TIM5_CH1 - TI1FP1
	GPIO_Config_Reset(&gpioConf);
	gpioConf.port = GPIOA;
	gpioConf.pin = 0;
	gpioConf.mode = AFUNCTION;
	gpioConf.af = AF2;
	GPIO_Init(gpioConf);

	// PA1 (FREQ IN4 - Alternate Function Input Capture)
	// TIM2_CH2 - TI2FP2
	GPIO_Config_Reset(&gpioConf);
	gpioConf.port = GPIOA;
	gpioConf.pin = 1;
	gpioConf.mode = AFUNCTION;
	gpioConf.af = AF1;
	GPIO_Init(gpioConf);

	// TIM2 Configuration
	// -------------------
	SYSTEM_TimerConfig(TIM2, SYSTEM_TIM_FREQUENCY_MODE, 2);

	// TIM3 Configuration
	// -------------------
	SYSTEM_TimerConfig(TIM3, SYSTEM_TIM_FREQUENCY_MODE, 1);

	// TIM5 Configuration
	// -------------------
	SYSTEM_TimerConfig(TIM5, SYSTEM_TIM_FREQUENCY_MODE, 1);

	// TIM8 Configuration
	// -------------------
	SYSTEM_TimerConfig(TIM8, SYSTEM_TIM_FREQUENCY_MODE, 2);

	// ************************************************************************
	// * Power Outputs Section                                                *
	// * 																	  *
	// ************************************************************************

	// GPIOs Configuration
	// -------------------
	// PA8 TIM1_CH1
	// PWM Group 4 (PG4)
	GPIO_Config_Reset(&gpioConf);
	gpioConf.port = GPIOA;
	gpioConf.pin = 8;
	gpioConf.mode = AFUNCTION;
	gpioConf.af = AF1;
	GPIO_Init(gpioConf);

	// PA9 TIM1_CH2
	// PWM Group 3 (PG3)
	GPIO_Config_Reset(&gpioConf);
	gpioConf.port = GPIOA;
	gpioConf.pin = 9;
	gpioConf.mode = AFUNCTION;
	gpioConf.af = AF1;
	GPIO_Init(gpioConf);

	// PA10 TIM1_CH3
	// PWM Group 2 (PG2)
	GPIO_Config_Reset(&gpioConf);
	gpioConf.port = GPIOA;
	gpioConf.pin = 10;
	gpioConf.mode = AFUNCTION;
	gpioConf.af = AF1;
	GPIO_Init(gpioConf);

	// PA11 TIM1_CH4
	// PWM Group 1 (PG1)
	GPIO_Config_Reset(&gpioConf);
	gpioConf.port = GPIOA;
	gpioConf.pin = 11;
	gpioConf.mode = AFUNCTION;
	gpioConf.af = AF1;
	GPIO_Init(gpioConf);

	// Timer Configuration
	// -------------------
	// Timer 1 (PWM 1000 - 100 Hz)
	TIM_Config_Reset(&timerConf);
	timerConf.module = TIM1;
	timerConf.arpe = true;
	timerConf.psc = (RCC_APB2_TIM_CLK / 100000) - 1;
	timerConf.arr = 1000 - 1;
	timerConf.oc1Mode = OCM_PWM1;
	timerConf.oc2Mode = OCM_PWM1;
	timerConf.oc3Mode = OCM_PWM1;
	timerConf.oc4Mode = OCM_PWM1;
	// Necessary for TIM1 and TIM8 to activate their outputs
	timerConf.masterOutputEnable = true;
	TIM_Init(timerConf);

	// Timer Enable
	TIM_Enable(TIM1);

	// ************************************************************************
	// * Scheduler and Dispatcher Timer Section                               *
	// * 																	  *
	// ************************************************************************

	// TIM7
	// Update event each millisecond
	TIM_Config_Reset(&timerConf);
	timerConf.module = TIM7;
	timerConf.arpe = false;
	timerConf.psc = (RCC_APB1_TIM_CLK / 100000U) - 1;
	timerConf.arr = 100 - 1;
	timerConf.uiEnable = true;
	TIM_Init(timerConf);

	// Enable interruptions
	NVIC_EnableIRQ(TIM7_IRQn);

	// ************************************************************************
	// * Debug Timer Section				                                  *
	// * 																	  *
	// ************************************************************************

	// Debug Timer (defined in SYSTEM_DEBUG_TIMER)
	// -------------------------------------------
	// Counter in micro-seconds
	TIM_Config_Reset(&timerConf);
	timerConf.module = SYSTEM_DEBUG_TIMER;
	timerConf.arpe = true;
	timerConf.psc = (SYSTEM_DEBUG_TIM_APB_CLK / 1000000U) - 1;
	timerConf.arr = 0xFFFFFFFF;
	TIM_Init(timerConf);

	// ************************************************************************
	// * Delay Timer Section			                               		  *
	// *  																	  *
	// ************************************************************************
	// Delay Timer (defined in SYSTEM_DELAY_TIMER)
	// -------------------------------------------
	// Counter in micro-seconds
	TIM_Config_Reset(&timerConf);
	timerConf.module = SYSTEM_DELAY_TIMER;
	timerConf.arpe = true;
	timerConf.psc = (SYSTEM_DELAY_TIM_APB_CLK / 1000000U) - 1;
	TIM_Init(timerConf);
}

// ****************************************************************************
// * LEDs Section                                                             *
// *  																		  *
// ****************************************************************************

/**
 * ----------------------------------------------------------------------------
 * @brief 			Power LED (Green)
 * ----------------------------------------------------------------------------
 * @param enable	Turn LED on/off
 * ----------------------------------------------------------------------------
 */
void SYSTEM_PwrLed(bool enable)
{
	if (enable)
		SYSTEM_PWRLED_GPIO->ODR |= (1U << SYSTEM_PWRLED_PIN);
	else
		SYSTEM_PWRLED_GPIO->ODR &= ~(1U << SYSTEM_PWRLED_PIN);
}

/**
 * ----------------------------------------------------------------------------
 * @brief 			Message LED (Blue)
 * ----------------------------------------------------------------------------
 * @param enable	Turn LED on/off
 * ----------------------------------------------------------------------------
 */
void SYSTEM_MsgLed(bool enable)
{
	if (enable)
		SYSTEM_MSGLED_GPIO->ODR |= (1U << SYSTEM_MSGLED_PIN);
	else
		SYSTEM_MSGLED_GPIO->ODR &= ~(1U << SYSTEM_MSGLED_PIN);
}

/**
 * ----------------------------------------------------------------------------
 * @brief 			Error LED (Red)
 * ----------------------------------------------------------------------------
 * @param enable	Turn LED on/off
 * ----------------------------------------------------------------------------
 */
void SYSTEM_ErrLed(bool enable)
{
	if (enable)
		SYSTEM_ERRLED_GPIO->ODR |= (1U << SYSTEM_ERRLED_PIN);
	else
		SYSTEM_ERRLED_GPIO->ODR &= ~(1U << SYSTEM_ERRLED_PIN);
}

// ****************************************************************************
// * Display Section                                                     	  *
// *  																		  *
// ****************************************************************************

/**
 * ----------------------------------------------------------------------------
 * @brief 			Green backlight of the LCD screen
 * ----------------------------------------------------------------------------
 * @param enable	Enable backlight
 * ----------------------------------------------------------------------------
 */
void SYSTEM_LcdGreen(bool enable)
{
	if (enable)
		SYSTEM_LCD_GREEN_GPIO->ODR |= (1U << SYSTEM_LCD_GREEN_PIN);
	else
		SYSTEM_LCD_GREEN_GPIO->ODR &= ~(1U << SYSTEM_LCD_GREEN_PIN);
}

/**
 * ----------------------------------------------------------------------------
 * @brief 			Red backlight of the LCD screen
 * ----------------------------------------------------------------------------
 * @param enable	Enable backlight
 * ----------------------------------------------------------------------------
 */
void SYSTEM_LcdRed(bool enable)
{
	if (enable)
		SYSTEM_LCD_RED_GPIO->ODR |= (1U << SYSTEM_LCD_RED_PIN);
	else
		SYSTEM_LCD_RED_GPIO->ODR &= ~(1U << SYSTEM_LCD_RED_PIN);
}

/**
 * ----------------------------------------------------------------------------
 * @brief 			Blue backlight of the LCD screen
 * ----------------------------------------------------------------------------
 * @param enable	Enable backlight
 * ----------------------------------------------------------------------------
 */
void SYSTEM_LcdBlue(bool enable)
{
	if (enable)
		SYSTEM_LCD_BLUE_GPIO->ODR |= (1U << SYSTEM_LCD_BLUE_PIN);
	else
		SYSTEM_LCD_BLUE_GPIO->ODR &= ~(1U << SYSTEM_LCD_BLUE_PIN);
}

// ****************************************************************************
// * Delay Timer Section                                                      *
// *  																		  *
// ****************************************************************************

/**
 * ----------------------------------------------------------------------------
 * @brief 			Generates a delay with blocking code
 * ----------------------------------------------------------------------------
 * @param delay		Delay in micro-seconds
 * ----------------------------------------------------------------------------
 */
void SYSTEM_DelayUs(uint16_t delay)
{
	if (delay == 0)
		return;

	SYSTEM_DELAY_TIMER->ARR = delay - 1;
	SYSTEM_DELAY_TIMER->CNT = 0;

	// Generate a update event after load ARR register
	SYSTEM_DELAY_TIMER->EGR |= TIM_EGR_UG;

	// Clear update interruption flag (UIF)
	SYSTEM_DELAY_TIMER->SR &= ~TIM_SR_UIF;

	TIM_Enable(SYSTEM_DELAY_TIMER);

	while (!(SYSTEM_DELAY_TIMER->SR & TIM_SR_UIF))
		;

	TIM_Disable(SYSTEM_DELAY_TIMER);
}

/**
 * ----------------------------------------------------------------------------
 * @brief 			Generates a delay with blocking code
 * ----------------------------------------------------------------------------
 * @param delay		Delay in milli-seconds
 * ----------------------------------------------------------------------------
 */
void SYSTEM_DelayMs(uint16_t delay)
{
	for (uint16_t i = 0; i < delay; i++)
		SYSTEM_DelayUs(1000);
}

// ****************************************************************************
// * Timers Configuration Section                                             *
// *  																		  *
// ****************************************************************************

/**
 * ----------------------------------------------------------------------------
 * @brief
 * ----------------------------------------------------------------------------
 * @param tim		Selected timer
 * @param freqMode	Frequency or counter mode
 * @param ccReg		Capture/ compare register (1 or 2)
 * ----------------------------------------------------------------------------
 */
void SYSTEM_TimerConfig(TIM_TypeDef *tim, uint8_t freqMode, uint8_t ccReg)
{
	// Disable Timer
	TIM_Disable(tim);

	// Timer Configuration
	// -------------------
	TIM_Config_Reset(&timerConf);
	timerConf.module = tim;

	// Auto Reload Register Value
	timerConf.arr = 0xFFFF;

	// TI1FP1 Selection and Polarity
	if (ccReg == 1)
	{
		timerConf.cc1Selection = CCS_INPUT_FIRST_TI;
		timerConf.cc1p = false;
		timerConf.cc1np = false;

		// Trigger Selection
		timerConf.triggerSelection = TS_TI1FP1;
	}
	else
	{
		timerConf.cc2Selection = CCS_INPUT_FIRST_TI;
		timerConf.cc2p = false;
		timerConf.cc2np = false;

		// Trigger Selection
		timerConf.triggerSelection = TS_TI2FP2;
	}

	// Select timer mode (frequency meter/counter)
	if (freqMode)
	{
		// PSC = RCC_APB1_TIM_CLK / 65536 ~= 65536 counts per second
		timerConf.psc = (uint16_t) (RCC_APB1_TIM_CLK / 0xFFFF) - 1;

		// Slave Mode Selection
		timerConf.slaveModeSelection = SMS_RESET_MODE;
	}
	else
	{
		// Prescaler division = 1
		timerConf.psc = 0;

		// Reset counter register
		timerConf.module->CNT = 0;

		// Slave Mode Selection
		timerConf.slaveModeSelection = SMS_EXTCLOCK_MODE;
	}

	// Perform Configuration
	TIM_Init(timerConf);

	// Enable timer
	TIM_Enable(tim);

	// Generate an update event
	timerConf.module->EGR |= TIM_EGR_UG;
}
