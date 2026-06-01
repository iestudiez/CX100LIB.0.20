/**
 ******************************************************************************
 * @file        : msgbox.h
 * @author      : Ivan.Estudiez
 * @brief       : Message box graphical control
 * @date        : 14 may 2026
 * @version     : 0.
 ******************************************************************************
 */

#ifndef INC_MSGBOX_H_
#define INC_MSGBOX_H_

#include "stdbool.h"
#include "stdint.h"

// Type definitions
// ----------------------------------------------------------------------------
typedef enum GUI_MsgBoxType_e
{
	MSGBOX_WARNING = 0,
	MSGBOX_ERROR = 1
} GUI_MsgBoxType_e;

typedef struct MsgBoxPrivateSettings_t
{
	uint8_t status;
	bool init;
	bool redraw;
	bool singleLine;
	uint8_t txt1Center;
	uint8_t txt2Center;
} MsgBoxPrivateSettings_t;

typedef struct GUI_MsgBox_t
{
	MsgBoxPrivateSettings_t priv;
	bool *pValue;
	char *sTitle;
	GUI_MsgBoxType_e type;
	char *sMsgLine1;
	char *sMsgLine2;
	void *pRet;
} GUI_MsgBox_t;

// Public functions
// ----------------------------------------------------------------------------
void GUI_MsgBox(GUI_MsgBox_t *msgbox);

#endif /* INC_MSGBOX_H_ */
