/**
 ******************************************************************************
 * @file        : can.h
 * @author      : Estudiez, Ivan
 * @brief       : CAN Bus Library
 * @date        : 05/07/2024
 * @version     : 1.0
 ******************************************************************************
 *
 *
 *
 *
 *
 ******************************************************************************
 */

#ifndef CAN_H_
#define CAN_H_

#include "stm32f4xx.h"
#include <stdbool.h>
#include <stdint.h>

typedef enum
{
	CAN_BR_100K = 100,			// Baud Rate 100K
	CAN_BR_250K = 250,			// Baud Rate 250K
	CAN_BR_500K = 500,			// Baud Rate 500K
	CAN_BR_1M = 1000			// Baud Rate 1M
} BaudrateCAN_e;

typedef enum
{
	STANDARD_FORMAT = 0,		// CAN Message Standard Format
	EXTENDED_FORMAT = 1			// CAN Message Extended Format
} MsgFormat_e;

typedef enum
{
	DATA_FRAME = 0,				// CAN Data Frame
	REMOTE_FRAME = 1			// CAN Remote Frame
} MsgType_e;

typedef enum
{
	CAN_FIFO0 = 0,				// CAN RX FIFO-0
	CAN_FIFO1 = 1				// CAN RX FIFO-1
} Fifo_e;

typedef struct
{
	uint32_t id1;				// Filter Id Register 1
	uint32_t id2;				// Filter Id/Mask Register 2
	bool ide;					// Message Format (Standard/ Extended)
	bool rtr; 					// Message Type	(Date frame/ Remote frame)
	bool maskMode; 				// Filter Bank Mode (Mask mode/ List mode)
	bool fifo;					// (FIFO0/ FIFO1)
} CAN_Filter_t;

typedef struct
{
	uint32_t id;       			// 11 or 29 bit identifier
	uint8_t data[8]; 			// Data field
	uint8_t len;     			// Length of data field in bytes
	MsgFormat_e format;  		// 0 - STANDARD, 1- EXTENDED IDENTIFIER
	MsgType_e type;    			// 0 - DATA FRAME, 1 - REMOTE FRAME
} CAN_Msg_t;

typedef struct
{
	CAN_TypeDef *module;		// bX CAN Module
	BaudrateCAN_e baudrate;		// Baud Rate
	bool nart;					// Non Automatic Retransmission Mode
	bool fmpie0;				// Message Pending Interrupt Enable (FIFO-0)
	bool fmpie1;				// Message Pending Interrupt Enable (FIFO-1)
} CAN_Config_t;

extern CAN_Msg_t CAN_MsgRx;

void CAN_Init(CAN_Config_t hCan);
void CAN_Config_Reset(CAN_Config_t *hCan);
void CAN_Termination(bool canTrm);
void CAN_WriteMsg(CAN_TypeDef *canModule, CAN_Msg_t *msg);
void CAN_ReadMsg(CAN_TypeDef *canModule, CAN_Msg_t *msg);
void CAN_FilterAllowAll(Fifo_e fifo);
void CAN_Add32bitFilter(CAN_Filter_t filter);
void CAN_Disable(void);

#endif /* CAN_H_ */
