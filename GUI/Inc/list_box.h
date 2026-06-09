/**
 ******************************************************************************
 * @file        : list_box.h
 * @author      : Ivan.Estudiez
 * @brief       : 
 * @date        : 14 may 2026
 * @version     : 0.
 ******************************************************************************
 */
#ifndef INC_LIST_BOX_H_
#define INC_LIST_BOX_H_

#include "stdint.h"
#include "stdbool.h"
#include "input_box.h"
#include "stddef.h"

// Pre-processor definitions and macros
// ----------------------------------------------------------------------------
#define LISTBOX_TITLE_LENGTH					20
#define	LISTBOX_MAX_NUMBER_ITEMS				8
#define	LISTBOX_ITEM_LABEL_LENGHT				20
#define	LISTBOX_ITEMS_ON_SCREEN					5
#define LISTBOX_INPUTBOX_XPOS					27
#define LISTBOX_INPUTBOX_YPOS					8

// Type definitions
// ----------------------------------------------------------------------------

typedef struct ListPrivateSettings_t
{
	uint8_t status;
	bool redraw;
	bool init;
	uint8_t selRowPosition;
	uint8_t numItems;
	uint8_t offset;
} GUI_ListPrivateSettings_t;

typedef struct ListBox_t
{
	GUI_ListPrivateSettings_t priv;
	char *sTitle;
	uint8_t selectedItem;
	GUI_InputBox_t item[LISTBOX_MAX_NUMBER_ITEMS];
	void *pRet;
} GUI_ListBox_t;

void GUI_ListBox(GUI_ListBox_t *listBox);

#endif /* INC_LIST_BOX_H_ */
