/**
 ******************************************************************************
 * @file        : input_box.h
 * @author      : Ivan.Estudiez
 * @brief       : 
 * @date        : 11 may 2026
 * @version     : 0.1
 ******************************************************************************
 */
#ifndef INC_INPUT_BOX_H_
#define INC_INPUT_BOX_H_

#include "stdint.h"
#include "stdbool.h"

// Pre-processor definitions and macros
// ----------------------------------------------------------------------------

// Type definitions
// ----------------------------------------------------------------------------
typedef enum GUI_DataType_e
{
	GUI_INT32 = 0,
	GUI_INT16 = 1,
	GUI_INT8 = 3,
	GUI_UINT16 = 5,
	GUI_UINT8 = 6
} GUI_DataType_e;

typedef struct IBoxPrivateSettings_t
{
	uint8_t status;
	int32_t value;
	char txtValue[10];
	uint8_t txtLen;
	bool init;
} GUI_IBoxPrivateSettings_t;

typedef struct InputBox_t
{
	GUI_IBoxPrivateSettings_t priv;
	char *sTile;
	void *pInputVar;
	GUI_DataType_e dataType;
	int32_t maxValue;
	int32_t minValue;
	uint8_t decimalPos;
	uint8_t numDigits;
	uint8_t xPos;
	uint8_t yPos;
	char *sUnit;
	void *pRet;
} GUI_InputBox_t;

// Public variables
// ----------------------------------------------------------------------------
extern GUI_InputBox_t *GUI_InputBox_DefaultPointer;

// Public functions
// ----------------------------------------------------------------------------
void GUI_InputBox(GUI_InputBox_t *inputBox);
void GUI_InputBox_UpdateValue(GUI_InputBox_t *inputBox);

#endif /* INC_INPUT_BOX_H_ */
