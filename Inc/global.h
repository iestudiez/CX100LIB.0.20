/*
 ******************************************************************************
 * @file        : global.h
 * @author      : Estudiez, Ivan
 * @brief       : Project global variables and functions
 * @date        : 19/05/2025
 * @version     : 1.0
 ******************************************************************************
 *
 *
 *
 *
 *
 ******************************************************************************
 */

#ifndef INC_GLOBAL_H_
#define INC_GLOBAL_H_

#include "stdint.h"
#include "stdbool.h"

// Public variables
// -----------------------------------------------------------------------------
extern uint32_t GLOBAL_Ticks;
extern bool GLOBAL_Blink;
extern bool GLOBAL_Blink2;

// Public methods
// -----------------------------------------------------------------------------
bool GLOBAL_FreqDivider(bool input);

#endif /* INC_GLOBAL_H_ */
