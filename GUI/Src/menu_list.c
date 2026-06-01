/**
 ******************************************************************************
 * @file        : menu_list.c
 * @author      : Estudiez, Ivan
 * @brief       : Vertical scroll menu
 * @date		: 07/05/2026
 * @version		: 1.0
 ******************************************************************************
 */

#include "stddef.h"
#include "stdio.h"
#include "string.h"
#include "menu_list.h"
#include "keyboard.h"
#include "gui_utils.h"
#include "glcd.h"
#include "fonts.h"
#include "menu_list_bmp.h"
#include "symbols_bmp.h"

// Private functions
// ----------------------------------------------------------------------------
void menuList_Init(GUI_MenuList_t *menu);
bool menuList_KeyActions(GUI_MenuList_t *menu);
void menuList_Draw(GUI_MenuList_t *menu);

/**
 * ----------------------------------------------------------------------------
 * @brief			Draw a vertical scrolling menu on the screen.
 *----------------------------------------------------------------------------
 * @param menu		Pointer to the structure that contains the parameters
 * 					for building the menu
 * ----------------------------------------------------------------------------
 */
void GUI_MenuList(GUI_MenuList_t *menu)
{
	// Exit if state is inactive
	if (menu->priv.status == GUI_STATUS_DISABLED)
		return;

	// If there was a jump from a previous menu,
	// perform the keyboard reading in the next cycle
	if (menu->priv.status == GUI_STATUS_JUMP)
	{
		menu->priv.status = GUI_STATUS_ENABLED;
		return;
	}

	// Initialization
	menuList_Init(menu);

	// Process keyboard actions
	if (menuList_KeyActions(menu))
		return;

	// Draw the menu on screen
	if (menu->priv.redraw)
		menuList_Draw(menu);
}

/**
 * ----------------------------------------------------------------------------
 * @brief 			Menu initialization
 * ----------------------------------------------------------------------------
 * @param menu		Pointer to MenuList type structure
 * ----------------------------------------------------------------------------
 */
void menuList_Init(GUI_MenuList_t *menu)
{
	if (menu->priv.init)
		return;

	menu->priv.init = true;
	menu->selectedItem = 0;
	menu->priv.redraw = true;
	menu->priv.offset = 0;
	menu->priv.arrowPosition = 0;
	menu->sTitle[14] = 0;

	// Put string termination character
	for (uint8_t i = 0; i < MENULIST_MAX_NUMBER_ITEMS; i++)
		if (strlen(menu->item[i].label) >= MENULIST_ITEM_LABEL_LENGHT)
			menu->item[i].label[MENULIST_ITEM_LABEL_LENGHT] = 0x00;

	// Determine the number of items
	uint8_t q = 0;
	for (q = 0; q < MENULIST_MAX_NUMBER_ITEMS; q++)
		if (menu->item[q].label[0] == 0x00)
			break;

	menu->priv.numItems = q;
}

/**
 * ----------------------------------------------------------------------------
 * @brief 			Perform keyboard actions
 * ----------------------------------------------------------------------------
 * @param menu		Pointer to MenuList type structure
 * @return			False = Continue, True = Exit menu.
 * ----------------------------------------------------------------------------
 */
bool menuList_KeyActions(GUI_MenuList_t *menu)
{
	// Escape action
	if (KEYBOARD_Event.Esc == 1)
	{
		menu->priv.status = GUI_STATUS_DISABLED;
		*(uint8_t*) menu->pRet = GUI_STATUS_JUMP;
		menu->priv.redraw = true;
		return true;
	}

	// Enter action
	if (KEYBOARD_Event.Enter == 1)
	{
		if (menu->item[menu->selectedItem].run != NULL)
		{
			menu->priv.status = GUI_STATUS_DISABLED;
			*(uint8_t*) menu->item[menu->selectedItem].run = GUI_STATUS_JUMP;
		}
		menu->priv.redraw = true;
		return true;
	}

	// Calculate the position of the selection arrow and the offset of the menu items
	if ((KEYBOARD_Event.N5_Down == 1) && (menu->selectedItem < (menu->priv.numItems - 1)))
	{
		if (menu->priv.arrowPosition >= (MENULIST_ITEMS_ON_SCREEN - 1))
			menu->priv.offset++;
		else
			menu->priv.arrowPosition++;

		menu->selectedItem++;
		menu->priv.redraw = true;
	}

	if ((KEYBOARD_Event.N2_Up == 1) && (menu->selectedItem > 0))
	{
		if (menu->priv.arrowPosition > 0)
			menu->priv.arrowPosition--;
		else
			menu->priv.offset--;

		menu->selectedItem--;
		menu->priv.redraw = true;
	}

	return false;
}

/**----------------------------------------------------------------------------
 * @brief 			Draw menu list on screen
 * ----------------------------------------------------------------------------
 * @param menu		Pointer to MenuList type structure
 * ----------------------------------------------------------------------------
 */
void menuList_Draw(GUI_MenuList_t *menu)
{
	const uint8_t TITLE_Y_POS = 1;
	const uint8_t ITEM_X_POS = 12;
	const uint8_t ITEM_Y_POS = 12;
	const uint8_t ITEM_Y_PITCH = 10;
	const uint8_t ARROW_X_POS = 4;
	const uint8_t ARROW_Y_OFFSET = 12;
	const uint8_t ARROW_Y_PITCH = 10;
	const uint8_t SEL_ITEM_X_POS = 118;
	const uint8_t SEL_ITEM_Y_POS = 1;

	uint8_t selItemPos = SEL_ITEM_X_POS;
	char txtSelectedItem[4];

	GLCD_ScreenBitmap(MenuItem_Screen_128x64);

	// Title bar text
	GLCD_DrawText(menu->sTitle, ITEM_X_POS, TITLE_Y_POS, Font_System1, 0, 0);

	// Draw items text
	for (uint8_t ix = 0; ix < MENULIST_ITEMS_ON_SCREEN; ix++)
		GLCD_DrawText(menu->item[ix + menu->priv.offset].label, ITEM_X_POS, ix * ITEM_Y_PITCH + ITEM_Y_POS, Font_System1, 0, 1);

	// Draw arrow
	GLCD_PutBitmap(ARROW_X_POS, menu->priv.arrowPosition * ARROW_Y_PITCH + ARROW_Y_OFFSET, Symbols_ArrowRight_4x8, 0);

	// Draw selected item number in the top bar
	if (menu->selectedItem == 9)
		selItemPos -= 2;
	sprintf(txtSelectedItem, "%d", menu->selectedItem + 1);
	GLCD_DrawText(txtSelectedItem, selItemPos, SEL_ITEM_Y_POS, Font_System1, 0, 0);

	// The drawing of the menu list has already been completed.
	menu->priv.redraw = false;
}
