/**
 ******************************************************************************
 * @file        : global.c
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

#include "global.h"

// Public variables
// -----------------------------------------------------------------------------
uint32_t GLOBAL_Ticks = 0;
bool GLOBAL_Blink = false;
bool GLOBAL_Blink2 = false;

/**
 * -----------------------------------------------------------------------------
 * @brief			Divide the switching frequency of the input variable by 2
 * -----------------------------------------------------------------------------
 * @param input		Switching variable
 * @return			Input frequency divided by 2
 * -----------------------------------------------------------------------------
 */
bool GLOBAL_FreqDivider(bool input)
{
	static bool prevState;

	prevState = prevState ^ input;
	return prevState;
}
