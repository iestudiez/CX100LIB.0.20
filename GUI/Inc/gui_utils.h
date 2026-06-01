/**
 ******************************************************************************
 * @file        : utils.h
 * @author      : Ivan.Estudiez
 * @brief       : GUI utilities files
 * @date        : 28 may 2026
 * @version     : 0.
 ******************************************************************************
 */

#ifndef GUI_INC_GUI_UTILS_H_
#define GUI_INC_GUI_UTILS_H_

#include "stdint.h"

// Execution status of the graphics components
// ----------------------------------------------------------------------------
#define	GUI_STATUS_DISABLED					0x00
#define	GUI_STATUS_ENABLED					0x01
#define GUI_STATUS_JUMP						0x02

// Public functions
// ----------------------------------------------------------------------------
void GUI_DecimalPoint(char *textVal, uint8_t pos);
void GUI_TransferControl(uint8_t *pSourceCtrl, void *pDestCtrl);

#endif /* GUI_INC_GUI_UTILS_H_ */
