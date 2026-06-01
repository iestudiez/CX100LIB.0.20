/**
 ******************************************************************************
 * @file		: gpio.h
 * @author      : Estudiez, Ivan
 * @brief       : STM32F405 GPIO LIB
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

#ifndef GPIO_H_
#define GPIO_H_

#include "stm32f4xx.h"
#include <stdint.h>
#include <stdbool.h>

typedef enum
{
	LOW_SPEED = 0,
	MEDIUM_SPEED = 1,
	HIGH_SPEED = 3,
	VERY_HIGH_SPEED = 4
} OutputSpeed_e;

typedef enum
{
	INPUT = 0,
	OUTPUT = 1,
	AFUNCTION = 2,
	ANALOG = 3
} Mode_e;

typedef enum
{
	PUSH_PULL = 0,
	OPEN_DRAIN = 1
} Otype_e;

typedef enum
{
	NO_PULL_RESISTOR = 0,
	PULL_UP_RESISTOR = 1,
	PULL_DOWN_RESISTOR = 2
}PupdRegister_e;

typedef enum
{
	AF0 = 0,
	AF1 = 1,
	AF2 = 2,
	AF3 = 3,
	AF4 = 4,
	AF5 = 5,
	AF6 = 6,
	AF7 = 7,
	AF8 = 8,
	AF9 = 9,
	AF10 = 10,
	AF11 = 11,
	AF12 = 12,
	AF13 = 13,
	AF14 = 14,
	AF15 = 15
} AF_e;

typedef struct
{
	GPIO_TypeDef *port;
	uint8_t pin;
	Mode_e mode;
	AF_e af;
	OutputSpeed_e speed;
	Otype_e otype;
	PupdRegister_e pupd;
} GPIO_Config_t;

void GPIO_Init(GPIO_Config_t gpio);
void GPIO_Config_Reset(GPIO_Config_t *gpio);
void GPIO_Write(GPIO_TypeDef *gpio_port, uint8_t pin, bool pin_state);
bool GPIO_Read(GPIO_TypeDef *gpio_port, uint8_t pin);

#endif /* GPIO_H_ */
