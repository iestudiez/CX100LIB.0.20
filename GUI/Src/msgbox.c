/**
 ******************************************************************************
 * @file        : msgbox.c
 * @author      : Ivan.Estudiez
 * @brief       : Message box graphical control
 * @date        : 14 may 2026
 * @version     : 0.
 ******************************************************************************
 */

#include "msgbox.h"
#include "msgbox_bmp.h"
#include "keyboard.h"
#include "gui_utils.h"
#include "glcd.h"
#include "fonts.h"
#include "stddef.h"

// Macros and definitions
// ----------------------------------------------------------------------------
#define MSGBOX_XPOS						13
#define MSGBOX_YPOS						13
#define MSGBOX_WIDTH					100
#define MSGBOX_HEIGHT					40
#define MSGBOX_LINES_XPOS				72
#define MSGBOX_LINE1_YPOS				(MSGBOX_YPOS + 6)
#define MSGBOX_LINE2_YPOS				(MSGBOX_YPOS + 16)
#define MSGBOX_SINGLE_LINE_YPOS			(MSGBOX_YPOS + 12)

#define MSGBOX_DELETE_X1				(MSGBOX_XPOS + 1)
#define MSGBOX_DELETE_Y1				(MSGBOX_YPOS + 1)
#define MSGBOX_DELETE_X2				(MSGBOX_XPOS + MSGBOX_WIDTH - 1)
#define MSGBOX_DELETE_Y2				(MSGBOX_YPOS + MSGBOX_HEIGHT - 1)

#define MSGBOX_FONT						(Font_System1)
#define MSGBOX_FONT_SPACE				0

// Private functions
// ----------------------------------------------------------------------------
void msgbox_Init(GUI_MsgBox_t *msgbox);
void msgbox_Draw(GUI_MsgBox_t *msgbox);
bool msgbox_KeyActions(GUI_MsgBox_t *msgbox);

/**
 * ----------------------------------------------------------------------------
 * @brief			Draw a message box on the screen.
 * ----------------------------------------------------------------------------
 * @param msgbox	Pointer to the structure that contains the parameters
 * 					for building the message box.
 * ----------------------------------------------------------------------------
 */
void GUI_MsgBox(GUI_MsgBox_t *msgbox)
{
	// Exit if state is inactive
	if (msgbox->priv.status == GUI_STATUS_DISABLED)
		return;

	// If there was a jump from a previous menu,
	// perform the keyboard reading in the next cycle
	if (msgbox->priv.status == GUI_STATUS_JUMP)
	{
		msgbox->priv.status = GUI_STATUS_ENABLED;
		return;
	}

	// Perform message box initialization
	if (msgbox->priv.init == false)
		msgbox_Init(msgbox);

	// Draw message box
	if (msgbox->priv.redraw)
		msgbox_Draw(msgbox);

	// Process keyboard actions
	if (msgbox_KeyActions(msgbox))
		return;
}

/**
 * ----------------------------------------------------------------------------
 * @brief			Perform message box initialization
 * ----------------------------------------------------------------------------
 * @param msgbox	Pointer to MsgBox type structure
 * ----------------------------------------------------------------------------
 */
void msgbox_Init(GUI_MsgBox_t *msgbox)
{
	// Set initialization flag
	msgbox->priv.init = true;

	// Set redraw flag
	msgbox->priv.redraw = true;

	// Determine text line 1 justification
	msgbox->priv.txt1Center = MSGBOX_LINES_XPOS - (GLCD_TextWidth(msgbox->sMsgLine1, MSGBOX_FONT, MSGBOX_FONT_SPACE) / 2);

	if (msgbox->sMsgLine2[0] == 0x00)
	{
		msgbox->priv.singleLine = true;
		return;
	}

	// Determine text line 2 justification
	msgbox->priv.singleLine = false;
	msgbox->priv.txt2Center = MSGBOX_LINES_XPOS - (GLCD_TextWidth(msgbox->sMsgLine2, MSGBOX_FONT, MSGBOX_FONT_SPACE) / 2);
}

/**
 * ----------------------------------------------------------------------------
 * @brief 				Perform keyboard actions
 * ----------------------------------------------------------------------------
 * @param msgbox		Pointer to MsgBox type structure
 * @return				False = Continue, True = Exit current control.
 * ----------------------------------------------------------------------------
 */
bool msgbox_KeyActions(GUI_MsgBox_t *msgbox)
{

	// Escape action
	if (KEYBOARD_Event.Esc == 1)
	{
		// Disable the current control execution state
		msgbox->priv.status = GUI_STATUS_DISABLED;

		// Give execution control to the return address
		*(uint8_t*) msgbox->pRet = GUI_STATUS_JUMP;

		// Set redraw flag
		msgbox->priv.redraw = true;

		// Set the value of the boolean variable
		if (msgbox->pValue != NULL)
			*msgbox->pValue = false;

		// A key has been pressed
		return true;
	}

	// Enter action
	if (KEYBOARD_Event.Enter == 1)
	{
		// Disable the current control execution state
		msgbox->priv.status = GUI_STATUS_DISABLED;

		// Give execution control to the return address
		*(uint8_t*) msgbox->pRet = GUI_STATUS_JUMP;

		// Set redraw flag
		msgbox->priv.redraw = true;

		// Set the value of the boolean variable
		if (msgbox->pValue != NULL)
			*msgbox->pValue = true;

		// A key has been pressed
		return true;
	}

	// Returns false if no key has been pressed
	return false;
}
/**
 * ----------------------------------------------------------------------------
 * @brief			Perform message box initialization
 * ----------------------------------------------------------------------------
 * @param msgbox	Pointer to MsgBox type structure
 * ----------------------------------------------------------------------------
 */
void msgbox_Draw(GUI_MsgBox_t *msgbox)
{
	// Reset redraw flag
	msgbox->priv.redraw = false;

	// Delete box
	GLCD_DrawBox(MSGBOX_DELETE_X1, MSGBOX_DELETE_Y1, MSGBOX_DELETE_X2, MSGBOX_DELETE_Y2, 0);

	// Draw message box
	switch (msgbox->type)
	{
	case MSGBOX_WARNING:
		GLCD_PutBitmap(MSGBOX_XPOS, MSGBOX_YPOS, MsgboxWarning_Bitmap100x40, 0);
		break;

	case MSGBOX_ERROR:
		GLCD_PutBitmap(MSGBOX_XPOS, MSGBOX_YPOS, MsgboxError_Bitmap100x40, 0);
		break;
	}

	// Print a single line message
	if (msgbox->priv.singleLine)
	{
		GLCD_DrawText(msgbox->sMsgLine1, msgbox->priv.txt1Center, MSGBOX_SINGLE_LINE_YPOS, MSGBOX_FONT, MSGBOX_FONT_SPACE, 1);
		return;
	}

	// Print a double line message
	GLCD_DrawText(msgbox->sMsgLine1, msgbox->priv.txt1Center, MSGBOX_LINE1_YPOS, MSGBOX_FONT, MSGBOX_FONT_SPACE, 1);
	GLCD_DrawText(msgbox->sMsgLine2, msgbox->priv.txt2Center, MSGBOX_LINE2_YPOS, MSGBOX_FONT, MSGBOX_FONT_SPACE, 1);
}
