/**
 ******************************************************************************
 * @file        : utils.c
 * @author      : Ivan.Estudiez
 * @brief       : GUI utilities files
 * @date        : 28 may 2026
 * @version     : 0.
 ******************************************************************************
 */

#include "string.h"
#include "stdint.h"
#include "gui_utils.h"


/**
 * ----------------------------------------------------------------------------
 * @brief			Add the decimal point to the number in text format.
 * ----------------------------------------------------------------------------
 * @param textVal	Number in text format
 * @param pos		Position of the decimal point
 * ----------------------------------------------------------------------------
 */
void GUI_DecimalPoint(char *textVal, uint8_t pos)
{
	uint8_t len = strlen(textVal);
	uint8_t fill = 0;
	uint8_t i, j = 0;

	if (pos == 0)
		return;

	if (pos >= len)
	{
		fill = pos - len + 1;
		len += pos + 1;
	}

	char strTemp[len + 2];
	char strSrc[len + 1];
	strcpy(strSrc, textVal);

	// Fill with zeros if necessary
	if (fill)
	{
		for (i = 0; i < fill; i++)
			strSrc[i] = '0';
		strSrc[i] = '\0';
		strcat(strSrc, textVal);
		len = strlen(strSrc);
	}

	pos = len - pos;

	// Insert decimal point
	for (i = 0; i < len; i++)
	{
		if (i == pos)
			strTemp[j++] = '.';
		strTemp[j++] = strSrc[i];
	}

	// Null-terminate the string
	strTemp[j] = '\0';

	strcpy(textVal, strTemp);
}

/**
 * ----------------------------------------------------------------------------
 * @brief				Transfers execution control of the current
 * 						graphics component.
 * ----------------------------------------------------------------------------
 * @param pSourceCtrl	Current graphic control
 * @param pDestCtrl		Target Graphic Component
 * ----------------------------------------------------------------------------
 */
void GUI_TransferControl(uint8_t *pSourceCtrl, void *pDestCtrl)
{
	*pSourceCtrl = GUI_STATUS_DISABLED;
	*(uint8_t*) pDestCtrl = GUI_STATUS_JUMP;
}
