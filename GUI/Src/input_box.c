/**
 ******************************************************************************
 * @file        : input_box.c
 * @author      : Ivan.Estudiez
 * @brief       : 
 * @date        : 11 may 2026
 * @version     : 0.1
 ******************************************************************************
 */

#include "gui_utils.h"
#include "stddef.h"
#include "input_box.h"
#include "keyboard.h"
#include "string.h"
#include "stdio.h"
#include "glcd.h"
#include "fonts.h"
#include "input_box_bmp.h"

// Defines
// ----------------------------------------------------------------------------
#define INPUTBOX_MIN_NUM_DIGITS				3
#define INPUTBOX_MAX_NUM_DIGITS				6
#define INPUTBOX_WIDTH						73
#define INPUTBOX_HEIGHT						26
#define INPUTBOX_TEXT_WIDTH					72
#define INPUTBOX_TEXT_HEIGHT				14

// Public variables
// ----------------------------------------------------------------------------
GUI_InputBox_t *GUI_InputBox_DefaultPointer;

// Private functions
// ----------------------------------------------------------------------------
void inputBox_Init(GUI_InputBox_t *inputBox);
void inputBox_UpdateTxt(GUI_InputBox_t *inputBox);
void inputBox_DrawFrame(GUI_InputBox_t *inputBox);
void inputBox_DrawText(GUI_InputBox_t *inputBox);
bool inputBox_KeyActions(GUI_InputBox_t *inputBox);

/**
 * ----------------------------------------------------------------------------
 * @brief				Displays a numeric input box on the screen
 * ----------------------------------------------------------------------------
 * @param inputBox		InputBox type structure
 * ----------------------------------------------------------------------------
 */
void GUI_InputBox(GUI_InputBox_t *inputBox)
{
	// Exit if state is inactive
	if (inputBox->priv.status == GUI_STATUS_DISABLED)
		return;

	// Initialization
	inputBox_Init(inputBox);

	// If there was a jump from a previous menu,
	// perform the keyboard reading in the next cycle
	if (inputBox->priv.status == GUI_STATUS_JUMP)
	{
		inputBox->priv.status = GUI_STATUS_ENABLED;

		// Read the input value and draw the frame
		GUI_InputBox_UpdateValue(inputBox);

		// Display changes on screen
		inputBox_DrawFrame(inputBox);
		inputBox_DrawText(inputBox);

		return;
	}

	// Process keyboard actions
	if (inputBox_KeyActions(inputBox))
		return;

	// Display changes on screen
	inputBox_UpdateTxt(inputBox);
	inputBox_DrawFrame(inputBox);
	inputBox_DrawText(inputBox);
}

/**
 * ----------------------------------------------------------------------------
 * @brief				Input box initialization
 * ----------------------------------------------------------------------------
 * @param inputBox		InputBox type structure
 * ----------------------------------------------------------------------------
 */
void inputBox_Init(GUI_InputBox_t *inputBox)
{
	if (inputBox->priv.init)
		return;

	inputBox->priv.init = true;

	// Check number of digits
	if (inputBox->numDigits < INPUTBOX_MIN_NUM_DIGITS)
		inputBox->numDigits = INPUTBOX_MIN_NUM_DIGITS;

	if (inputBox->numDigits > INPUTBOX_MAX_NUM_DIGITS)
		inputBox->numDigits = INPUTBOX_MAX_NUM_DIGITS;
}

/**
 * ----------------------------------------------------------------------------
 * @brief				Read the input value and draw the frame.
 * ----------------------------------------------------------------------------
 * @param inputBox		InputBox type structure
 * ----------------------------------------------------------------------------
 */
void GUI_InputBox_UpdateValue(GUI_InputBox_t *inputBox)
{
	// Read variable value
	switch (inputBox->dataType)
	{
	case GUI_INT32:
		inputBox->priv.value = *(int32_t*) inputBox->pInputVar;
		break;

	case GUI_INT16:
		inputBox->priv.value = *(int16_t*) inputBox->pInputVar;
		break;

	case GUI_INT8:
		inputBox->priv.value = *(int8_t*) inputBox->pInputVar;
		break;

	case GUI_UINT16:
		inputBox->priv.value = *(uint16_t*) inputBox->pInputVar;
		break;

	case GUI_UINT8:
		inputBox->priv.value = *(uint8_t*) inputBox->pInputVar;
		break;
	}

	// Update numeric variable to text
	inputBox_UpdateTxt(inputBox);
}

/**
 * ----------------------------------------------------------------------------
 * @brief				Update text value of input box
 * ----------------------------------------------------------------------------
 * @param inputBox		InputBox type structure
 * ----------------------------------------------------------------------------
 */
void inputBox_UpdateTxt(GUI_InputBox_t *inputBox)
{
	// Update text value
	sprintf(inputBox->priv.txtValue, "%d", (int) inputBox->priv.value);

	// Determine the number of digits
	inputBox->priv.txtLen = (uint8_t) strlen(inputBox->priv.txtValue);

	// Add decimal point
	GUI_DecimalPoint(inputBox->priv.txtValue, inputBox->decimalPos);
}

/**
 * ----------------------------------------------------------------------------
 * @brief 				Perform keyboard actions
 * ----------------------------------------------------------------------------
 * @param inputBox		InputBox type structure
 * @return				False = Continue, True = Exit menu.
 * ----------------------------------------------------------------------------
 */
bool inputBox_KeyActions(GUI_InputBox_t *inputBox)
{

	// Escape action
	if (KEYBOARD_Event.Esc == 1)
	{
		inputBox->priv.status = GUI_STATUS_DISABLED;
		*(uint8_t*) inputBox->pRet = GUI_STATUS_JUMP;
		return true;
	}

	// Enter action
	if (KEYBOARD_Event.Enter == 1)
	{
		// Check maximum value
		if (inputBox->priv.value > inputBox->maxValue)
			inputBox->priv.value = inputBox->maxValue;

		// Check minimum value
		if (inputBox->priv.value < inputBox->minValue)
			inputBox->priv.value = inputBox->minValue;

		// Perform data conversion
		switch (inputBox->dataType)
		{
		case GUI_INT32:
			*(int32_t*) inputBox->pInputVar = (int32_t) inputBox->priv.value;
			break;

		case GUI_INT16:
			*(int16_t*) inputBox->pInputVar = (int16_t) inputBox->priv.value;
			break;

		case GUI_INT8:
			*(int8_t*) inputBox->pInputVar = (int8_t) inputBox->priv.value;
			break;

		case GUI_UINT16:
			*(uint16_t*) inputBox->pInputVar = (uint16_t) inputBox->priv.value;
			break;

		case GUI_UINT8:
			*(uint8_t*) inputBox->pInputVar = (uint8_t) inputBox->priv.value;
			break;
		}

		// Disable the current control execution state
		inputBox->priv.status = GUI_STATUS_DISABLED;

		// Give execution control to the return address
		*(uint8_t*) inputBox->pRet = GUI_STATUS_JUMP;
		return true;
	}

	// Backspace
	if (KEYBOARD_Event.Bckspc == 1 || KEYBOARD_Event.Bckspc > 3)
	{
		inputBox->priv.value /= 10;
		return false;
	}

	// Check number of digits
	if (inputBox->priv.txtLen >= inputBox->numDigits)
		return true;

	// Key 0
	if (KEYBOARD_Event.N0 == 1)
	{
		inputBox->priv.value *= 10;
		return false;
	}

	// Key 1
	if (KEYBOARD_Event.N1 == 1)
	{
		inputBox->priv.value = inputBox->priv.value * 10 + 1;
		return false;
	}

	// Key 2
	if (KEYBOARD_Event.N2_Up == 1)
	{
		inputBox->priv.value = inputBox->priv.value * 10 + 2;
		return false;
	}

	// Key 3
	if (KEYBOARD_Event.N3 == 1)
	{
		inputBox->priv.value = inputBox->priv.value * 10 + 3;
		return false;
	}

	// Key 4
	if (KEYBOARD_Event.N4_Left == 1)
	{
		inputBox->priv.value = inputBox->priv.value * 10 + 4;
		return false;
	}

	// Key 5
	if (KEYBOARD_Event.N5_Down == 1)
	{
		inputBox->priv.value = inputBox->priv.value * 10 + 5;
		return false;
	}

	// Key 6
	if (KEYBOARD_Event.N6_Right == 1)
	{
		inputBox->priv.value = inputBox->priv.value * 10 + 6;
		return false;
	}

	// Key 7
	if (KEYBOARD_Event.N7 == 1)
	{
		inputBox->priv.value = inputBox->priv.value * 10 + 7;
		return false;
	}

	// Key 8
	if (KEYBOARD_Event.N8 == 1)
	{
		inputBox->priv.value = inputBox->priv.value * 10 + 8;
		return false;
	}

	// Key 9
	if (KEYBOARD_Event.N9 == 1)
	{
		inputBox->priv.value = inputBox->priv.value * 10 + 9;
		return false;
	}

	return true;
}

/**
 * ----------------------------------------------------------------------------
 * @brief				Draw Frame
 * ----------------------------------------------------------------------------
 * @param inputBox		InputBox type structure
 * ----------------------------------------------------------------------------
 */
void inputBox_DrawFrame(GUI_InputBox_t *inputBox)
{
	GLCD_DrawBox(inputBox->xPos + 1, inputBox->yPos + 1, inputBox->xPos + INPUTBOX_WIDTH, inputBox->yPos + INPUTBOX_HEIGHT, 0);
	GLCD_PutBitmap(inputBox->xPos, inputBox->yPos, InputBox_Bitmap74x27, 0);
}

/**
 * ----------------------------------------------------------------------------
 * @brief
 * ----------------------------------------------------------------------------
 * @param inputBox		InputBox type structure
 * ----------------------------------------------------------------------------
 */
void inputBox_DrawText(GUI_InputBox_t *inputBox)
{
	// Value and unit position
	uint8_t len;
	uint8_t valueRightJustif;
	uint8_t unitLeftJustif;
	const uint8_t TEXT_SPACER = 2;

	// Delete box coordinates
	uint8_t x1 = inputBox->xPos + 1;
	uint8_t y1 = inputBox->yPos + 2;
	uint8_t x2 = inputBox->xPos + INPUTBOX_TEXT_WIDTH;
	uint8_t y2 = inputBox->yPos + INPUTBOX_TEXT_HEIGHT;

	// Delete text box
	GLCD_DrawBox(x1, y1, x2, y2, 0);

	// Determine the position of the units text, justified to the left.
	len = GLCD_TextWidth(inputBox->sUnit, Font_System1, 0);
	unitLeftJustif = x2 - len;

	// Determine the length of the text
	len = GLCD_TextWidth(inputBox->priv.txtValue, Font_Fixedsys, 1);
	valueRightJustif = unitLeftJustif - len - TEXT_SPACER;

	// Draw text
	GLCD_DrawText(inputBox->priv.txtValue, valueRightJustif, y1, Font_Fixedsys, 1, 0);

	// Show engineering units
	GLCD_DrawText(inputBox->sUnit, unitLeftJustif, y1 + 3, Font_System1, 0, 0);
}

