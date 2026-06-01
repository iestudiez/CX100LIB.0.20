/**
 ******************************************************************************
 * @file        : menu_list.h
 * @author      : Estudiez, Ivan
 * @brief       : Vertical scroll menu
 * @date		: 07/05/2026
 * @version		: 1.0
 ******************************************************************************
 */

#ifndef INC_MENU_LIST_H_
#define INC_MENU_LIST_H_

#include "stdint.h"
#include "stdbool.h"

// Pre-processor definitions and macros
// ----------------------------------------------------------------------------
#define	MENULIST_MAX_NUMBER_ITEMS				10
#define	MENULIST_ITEM_LABEL_LENGHT				20
#define	MENULIST_ITEMS_ON_SCREEN				5

// Type definitions
// ----------------------------------------------------------------------------
typedef struct MenuItem_t
{
	char *label;
	void *run;
} MenuItem_t;

typedef struct MenuPrivateSettings_t
{
	uint8_t status;
	bool redraw;
	bool init;
	uint8_t arrowPosition;
	uint8_t numItems;
	uint8_t offset;
}GUI_MenuPrivateSettings_t;

typedef struct MenuList_t
{
	GUI_MenuPrivateSettings_t priv;
	char *sTitle;
	uint8_t selectedItem;
	MenuItem_t item[MENULIST_MAX_NUMBER_ITEMS];
	void *pRet;
} GUI_MenuList_t;

// Public functions
// ----------------------------------------------------------------------------
void GUI_MenuList(GUI_MenuList_t *menu);

#endif /* INC_MENU_LIST_H_ */
