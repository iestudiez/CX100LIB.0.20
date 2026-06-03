/**
 ******************************************************************************
 * @file        : list_box.c
 * @author      : Ivan.Estudiez
 * @brief       : List box graphical control
 * @date        : 14 may 2026
 * @version     : 0.
 ******************************************************************************
 */

#include "list_box.h"
#include "string.h"
#include "stdio.h"
#include "keyboard.h"
#include "glcd.h"
#include "fonts.h"
#include "list_box_bmp.h"
#include "gui_utils.h"

// Private functions
// ----------------------------------------------------------------------------
void listBox_Init(GUI_ListBox_t *listBox);
bool listBox_KeyActions(GUI_ListBox_t *listBox);
void listBox_Draw(GUI_ListBox_t *listBox);

/**
 * ----------------------------------------------------------------------------
 * @brief			Draw a vertical scrolling list on the screen.
 * ----------------------------------------------------------------------------
 * @param listBox	Pointer to the structure that contains the parameters
 * 					for building the list box.
 * ----------------------------------------------------------------------------
 */
void GUI_ListBox(GUI_ListBox_t *listBox)
{
	// Exit if state is inactive
	if (listBox->priv.status == GUI_STATUS_DISABLED)
		return;

	// If there was a jump from a previous menu,
	// perform the keyboard reading in the next cycle
	if (listBox->priv.status == GUI_STATUS_JUMP)
	{
		listBox->priv.status = GUI_STATUS_ENABLED;
		return;
	}

	// Initialization
	listBox_Init(listBox);

	// Process keyboard actions
	if (listBox_KeyActions(listBox))
		return;

	// Draw the menu on screen
	if (listBox->priv.redraw)
		listBox_Draw(listBox);
}

/**
 * ----------------------------------------------------------------------------
 * @brief			Initialize list box
 * ----------------------------------------------------------------------------
 * @param listBox	Pointer to ListBox type structure
 * ----------------------------------------------------------------------------
 */
void listBox_Init(GUI_ListBox_t *listBox)
{
	if (listBox->priv.init)
		return;

	listBox->priv.init = true;
	listBox->selectedItem = 0;
	listBox->priv.redraw = true;
	listBox->priv.offset = 0;
	listBox->priv.selRowPosition = 0;

	// Check list box title length
	if (strlen(listBox->sTitle) >= LISTBOX_TITLE_LENGTH)
		listBox->sTitle[LISTBOX_TITLE_LENGTH] = 0;

	// Determine the number of items
	uint8_t qty = 0;

	for (qty = 0; qty < LISTBOX_MAX_NUMBER_ITEMS; qty++)
		if (listBox->item[qty].pInputVar == NULL)
			break;

	listBox->priv.numItems = qty;

	// Initialize input boxes
	for (uint8_t i = 0; i < qty; i++)
	{
		// Check items title length
		if (strlen(listBox->item[i].sTile) >= LISTBOX_ITEM_LABEL_LENGHT)
			listBox->item[i].sTile[LISTBOX_ITEM_LABEL_LENGHT] = 0x00;

		// Update text value
		GUI_InputBox_UpdateValue(&listBox->item[i]);

		// Assign return direction
		listBox->item[i].pRet = listBox;

		// Set position of input box
		listBox->item[i].xPos = LISTBOX_INPUTBOX_XPOS;
		listBox->item[i].yPos = LISTBOX_INPUTBOX_YPOS;
	}
}

/**
 * ----------------------------------------------------------------------------
 * @brief 				Perform keyboard actions
 * ----------------------------------------------------------------------------
 * @param listBox		ListBox type structure
 * @return				False = Continue, True = Exit menu.
 * ----------------------------------------------------------------------------
 */
bool listBox_KeyActions(GUI_ListBox_t *listBox)
{
	// Escape action
	// ------------------------------------------------------------------------
	if (KEYBOARD_Event.Esc == 1)
	{
		// Disable the current control execution state
		listBox->priv.status = GUI_STATUS_DISABLED;

		// Give execution control to the return address
		*(uint8_t*) listBox->pRet = GUI_STATUS_JUMP;

		// Set the redraw flag for the next cycle
		listBox->priv.redraw = true;
		return true;
	}

	// Enter action
	// ------------------------------------------------------------------------
	if (KEYBOARD_Event.Enter == 1)
	{
		// Disable the current control execution state
		listBox->priv.status = GUI_STATUS_DISABLED;

		// Give execution control to the selected input box
		listBox->item[listBox->selectedItem].priv.status = GUI_STATUS_JUMP;

		// Point the system's default input box to the selected input box
		GUI_InputBox_DefaultPointer = &listBox->item[listBox->selectedItem];

		// Set the redraw flag for the next cycle
		listBox->priv.redraw = true;
		return true;
	}

	// Calculate the position of the selected row
	// and the offset of the listBox items
	// ------------------------------------------------------------------------
	if ((KEYBOARD_Event.N5_Down == 1) && (listBox->selectedItem < (listBox->priv.numItems - 1)))
	{
		if (listBox->priv.selRowPosition >= (LISTBOX_ITEMS_ON_SCREEN - 1))
			listBox->priv.offset++;
		else
			listBox->priv.selRowPosition++;

		listBox->selectedItem++;
		listBox->priv.redraw = true;
	}

	if ((KEYBOARD_Event.N2_Up == 1) && (listBox->selectedItem > 0))
	{
		if (listBox->priv.selRowPosition > 0)
			listBox->priv.selRowPosition--;
		else
			listBox->priv.offset--;

		listBox->selectedItem--;
		listBox->priv.redraw = true;
	}

	return false;
}

/**
 * ----------------------------------------------------------------------------
 * @brief				Draw list box on screen
 * ----------------------------------------------------------------------------
 * @param listBox		Pointer to ListBox type structure
 * ----------------------------------------------------------------------------
 */
void listBox_Draw(GUI_ListBox_t *listBox)
{
	const uint8_t SELECTED_ITEM_TOP = 0;
	const uint8_t LEFT_JUSTIFICATION = 2;
	const uint8_t ITEM_Y_POS = 10;
	const uint8_t ITEM_Y_PITCH = 11;
	const uint8_t RIGHT_JUSTIFICATION = 125;
	const uint8_t SELECTED_ROW_X1 = 1;
	const uint8_t SELECTED_ROW_X2 = 126;
	const uint8_t SELECTED_ROW_HEIGTH = 9;

	uint8_t txtCenter;
	char txtSelectedItem[4];

	// Draw list box frame
	GLCD_ScreenBitmap(ListBox_Screen_128x64);

	// Draw selected item number in the top bar
	// ------------------------------------------------------------------------
	sprintf(txtSelectedItem, "%d", listBox->selectedItem + 1);
	txtCenter = 64 - (GLCD_TextWidth(txtSelectedItem, Font_System1, 0) / 2);
	GLCD_DrawText(txtSelectedItem, txtCenter, SELECTED_ITEM_TOP, Font_System1, 0, 0);

	// Draw list box items and values
	// ------------------------------------------------------------------------
	uint8_t itemYPos;
	uint8_t valueXPos;
	char *stringToDisplay;

	for (uint8_t ix = 0; ix < LISTBOX_ITEMS_ON_SCREEN; ix++)
	{
		// Draw items
		itemYPos = ix * ITEM_Y_PITCH + ITEM_Y_POS;
		stringToDisplay = listBox->item[ix + listBox->priv.offset].sTile;
		GLCD_DrawText(stringToDisplay, LEFT_JUSTIFICATION, itemYPos, Font_System1, 0, 1);

		// Draw values
		GUI_InputBox_UpdateValue(&listBox->item[ix + listBox->priv.offset]);
		stringToDisplay = listBox->item[ix + listBox->priv.offset].priv.txtValue;
		valueXPos = RIGHT_JUSTIFICATION - GLCD_TextWidth(stringToDisplay, Font_System1, 0);
		GLCD_DrawText(stringToDisplay, valueXPos, itemYPos, Font_System1, 0, 1);
	}

	// Draw the selected row
	// ------------------------------------------------------------------------
	uint8_t selectedRowY1 = listBox->priv.selRowPosition * ITEM_Y_PITCH + ITEM_Y_POS - 1;
	uint8_t selectedRowY2 = selectedRowY1 + SELECTED_ROW_HEIGTH;
	GLCD_DrawBox(SELECTED_ROW_X1, selectedRowY1, SELECTED_ROW_X2, selectedRowY2, GLCD_BOXTYPE_REVERSE);

	// The drawing of the list box has already been completed.
	listBox->priv.redraw = false;
}

