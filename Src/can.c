/**
 ******************************************************************************
 * @file        : can.c
 * @author      : Estudiez, Ivan
 * @brief       : CAN Bus Library
 * @date        : 05/07/2024
 * @version     : 1.0
 ******************************************************************************
 * Change control:
 * 1.0 - (05/07/2024) Release version
 *
 * TODO: Implement CAN_RX timeout
 *
 ******************************************************************************
 */

#include <system_cx100.h>
#include "can.h"
#include "gpio.h"

/**
 * -----------------------------------------------------------------------------
 * @brief
 * -----------------------------------------------------------------------------
 * @param hCan
 * -----------------------------------------------------------------------------
 */
void CAN_Init(CAN_Config_t hCan)
{
	// Enable clock access for CAN peripheral
	// ------------------------------------------------------------------------
	if (hCan.module == CAN1)
		RCC->APB1ENR |= RCC_APB1ENR_CAN1EN;

	if (hCan.module == CAN2)
		RCC->APB1ENR |= RCC_APB1ENR_CAN2EN;

	// CAN initialization request
	// ------------------------------------------------------------------------
	hCan.module->MCR = CAN_MCR_INRQ;

	// Wait for initialization acknowledge
	while (!(hCan.module->MSR & CAN_MSR_INAK))
	{
	}

	// Set no automatic retransmission
	// ------------------------------------------------------------------------
	if (hCan.nart)
		hCan.module->MCR |= CAN_MCR_NART;
	else
		hCan.module->MCR &= ~CAN_MCR_NART;

	// FIFO message pending interrupt enable
	// ------------------------------------------------------------------------
	if (hCan.fmpie0)
		hCan.module->IER |= CAN_IER_FMPIE0;
	else
		hCan.module->IER &= ~CAN_IER_FMPIE0;

	if (hCan.fmpie1)
		hCan.module->IER |= CAN_IER_FMPIE1;
	else
		hCan.module->IER &= ~CAN_IER_FMPIE1;

	// Configure CAN network bit rate
	// ------------------------------------------------------------------------

	// Reset register
	// --------------------------------------------
	hCan.module->BTR = 0x00;

	// Bit timing for APB1 frequency of 32MHz or 16MHz
	// -----------------------------------------------
	if ((RCC_APB1_CLK == 32000000U) || (RCC_APB1_CLK == 16000000U))
	{
		// Bit Segment_1
		// --------------------------------------------
		// Bit Segment 1 = 13 - 1
		hCan.module->BTR |= 12U << CAN_BTR_TS1_Pos;

		// Bit Segment_2
		// --------------------------------------------
		// Bit Segment 2 = 2 - 1
		hCan.module->BTR |= 1U << CAN_BTR_TS2_Pos;

		// Re-synchronization segment = 4 - 1
		hCan.module->BTR |= 3U << CAN_BTR_SJW_Pos;

		// Baud rate pre-scaler
		// --------------------------------------------
		// Bit width = (1 + Segment_1 + Segment_2 = 16)
		hCan.module->BTR |= (((RCC_APB1_CLK / (hCan.baudrate * 1000 * 16)) - 1) & 0x01FF) << CAN_BTR_BRP_Pos;
	}

	// Bit timing for APB1 frequency of 42MHz
	// --------------------------------------
	if (RCC_APB1_CLK == 42000000U)
	{
		// Bit Segment_1
		// --------------------------------------------
		// Bit Segment 1 = 16 - 1
		hCan.module->BTR |= 15U << CAN_BTR_TS1_Pos;

		// Bit Segment_2
		// --------------------------------------------
		// Bit Segment 2 = 4 - 1
		hCan.module->BTR |= 3U << CAN_BTR_TS2_Pos;

		// Re-synchronization segment = 4 - 1
		hCan.module->BTR |= 3U << CAN_BTR_SJW_Pos;

		// Baud rate pre-scaler
		// --------------------------------------------
		// Bit width = (1 + Segment_1 + Segment_2 = 21)
		hCan.module->BTR |= (((RCC_APB1_CLK / (hCan.baudrate * 1000 * 21)) - 1) & 0x01FF) << CAN_BTR_BRP_Pos;
	}

	// (Initialization request) Enter normal operation mode
	// by resetting the INRQ bit
	// --------------------------------------------
	hCan.module->MCR &= ~(CAN_MCR_INRQ);
}

/**
 * -----------------------------------------------------------------------------
 * @brief
 * -----------------------------------------------------------------------------
 * @param hCan
 * -----------------------------------------------------------------------------
 */
void CAN_Config_Reset(CAN_Config_t *hCan)
{
	hCan->module = CAN1;
	hCan->fmpie0 = false;
	hCan->fmpie1 = false;
	hCan->nart = true;
	hCan->baudrate = 250;
}

/**
 * -----------------------------------------------------------------------------
 * @brief
 * -----------------------------------------------------------------------------
 * @param canModule
 * @param msg
 * -----------------------------------------------------------------------------
 */
void CAN_WriteMsg(CAN_TypeDef *canModule, CAN_Msg_t *msg)
{
	uint8_t mailboxNumber;

	// Select a empty transmit mailbox
	// -------------------------------
	if (canModule->TSR & CAN_TSR_TME0_Msk)
		mailboxNumber = 0;
	else if (canModule->TSR & CAN_TSR_TME1_Msk)
		mailboxNumber = 1;
	else if (canModule->TSR & CAN_TSR_TME2_Msk)
		mailboxNumber = 2;
	else
		// No mailbox is available, do not transmit the message
		return;

	// Set the format message type
	if (msg->format == EXTENDED_FORMAT)
	{
		canModule->sTxMailBox[mailboxNumber].TIR = (uint32_t) (msg->id << 3);
		canModule->sTxMailBox[mailboxNumber].TIR |= CAN_TI0R_IDE;
	}
	else
	{
		canModule->sTxMailBox[mailboxNumber].TIR = (uint32_t) (msg->id << 21);
		canModule->sTxMailBox[mailboxNumber].TIR &= ~(CAN_TI0R_IDE);
	}

	// Message type (Data frame / remote frame)
	if (msg->type == DATA_FRAME)
		canModule->sTxMailBox[mailboxNumber].TIR &= ~(CAN_TI0R_RTR);
	else
		canModule->sTxMailBox[mailboxNumber].TIR |= CAN_TI0R_RTR;

	// Store data in mailbox
	canModule->sTxMailBox[mailboxNumber].TDLR = (((uint32_t) msg->data[3] << 24) | ((uint32_t) msg->data[2] << 16) | ((uint32_t) msg->data[1] << 8) | ((uint32_t) msg->data[0]));

	canModule->sTxMailBox[mailboxNumber].TDHR = (((uint32_t) msg->data[7] << 24) | ((uint32_t) msg->data[6] << 16) | ((uint32_t) msg->data[5] << 8) | ((uint32_t) msg->data[4]));

	// Setup length
	canModule->sTxMailBox[mailboxNumber].TDTR &= ~(CAN_TDT0R_DLC);
	canModule->sTxMailBox[mailboxNumber].TDTR |= (msg->len & CAN_TDT0R_DLC);

	// transmit message
	canModule->sTxMailBox[mailboxNumber].TIR |= CAN_TI0R_TXRQ;
}

/**
 * -----------------------------------------------------------------------------
 * @brief
 * -----------------------------------------------------------------------------
 * @param canTrm
 * -----------------------------------------------------------------------------
 */
void CAN_Termination(bool canTrm)
{
	if (canTrm)
		GPIO_Write(SYSTEM_CANTR_GPIO, SYSTEM_CANTR_PIN, false);
	else
		GPIO_Write(SYSTEM_CANTR_GPIO, SYSTEM_CANTR_PIN, true);
}

/**
 * -----------------------------------------------------------------------------
 * @brief
 * -----------------------------------------------------------------------------
 * @param canModule
 * @param msg
 * -----------------------------------------------------------------------------
 */
void CAN_ReadMsg(CAN_TypeDef *canModule, CAN_Msg_t *msg)
{
	Fifo_e fifo;

	// Check if there are pending messages in FIFO-0 or FIFO-1, otherwise exit
	if (CAN1->RF0R & CAN_RF0R_FMP0_Msk)
		fifo = CAN_FIFO0;
	else if (CAN1->RF1R & CAN_RF1R_FMP1_Msk)
		fifo = CAN_FIFO1;
	else
		return;

	// CAN format message
	// ------------------
	if ((canModule->sFIFOMailBox[fifo].RIR & CAN_RI0R_IDE) == 0)
	{
		// Standard ID
		msg->format = STANDARD_FORMAT;
		msg->id = (uint32_t) 0x000007FF & ((canModule->sFIFOMailBox[fifo].RIR >> 21));
	}
	else
	{
		// Extended ID
		msg->format = EXTENDED_FORMAT;
		msg->id = (uint32_t) 0x1FFFFFFF & ((canModule->sFIFOMailBox[fifo].RIR >> 3));
	}

	// Read message type information
	// -----------------------------
	if ((canModule->sFIFOMailBox[fifo].RIR & CAN_RI0R_RTR) == 0)
	{
		msg->type = DATA_FRAME;
	}
	else
	{
		msg->type = REMOTE_FRAME;
	}

	// Read length (number of received bytes)
	msg->len = (uint32_t) 0x0000000F & canModule->sFIFOMailBox[fifo].RDTR;

	// Read data bytes
	msg->data[0] = (uint32_t) 0x000000FF & (canModule->sFIFOMailBox[fifo].RDLR);
	msg->data[1] = (uint32_t) 0x000000FF & (canModule->sFIFOMailBox[fifo].RDLR >> 8);
	msg->data[2] = (uint32_t) 0x000000FF & (canModule->sFIFOMailBox[fifo].RDLR >> 16);
	msg->data[3] = (uint32_t) 0x000000FF & (canModule->sFIFOMailBox[fifo].RDLR >> 24);

	msg->data[4] = (uint32_t) 0x000000FF & (canModule->sFIFOMailBox[fifo].RDHR);
	msg->data[5] = (uint32_t) 0x000000FF & (canModule->sFIFOMailBox[fifo].RDHR >> 8);
	msg->data[6] = (uint32_t) 0x000000FF & (canModule->sFIFOMailBox[fifo].RDHR >> 16);
	msg->data[7] = (uint32_t) 0x000000FF & (canModule->sFIFOMailBox[fifo].RDHR >> 24);

	// Release FIFO output mailbox
	if (fifo == CAN_FIFO0)
		CAN1->RF0R |= CAN_RF0R_RFOM0;
	else
		CAN1->RF1R |= CAN_RF1R_RFOM1;
}

/**
 * -----------------------------------------------------------------------------
 * @brief 			Accept all messages
 * -----------------------------------------------------------------------------
 * @param fifo		CAN FIFO selection
 * -----------------------------------------------------------------------------
 */
void CAN_FilterAllowAll(Fifo_e fifo)
{
	// Initialize filter mode
	CAN1->FMR |= CAN_FMR_FINIT;

	// Deactivate filter
	CAN1->FA1R &= ~(CAN_FA1R_FACT0);

	// Set filter scale
	CAN1->FS1R |= CAN_FS1R_FSC0;

	// Set filter mode (mask/ list)
	CAN1->FM1R &= ~(CAN_FS1R_FSC0);

	CAN1->sFilterRegister[0].FR1 = 0x0;
	CAN1->sFilterRegister[0].FR2 = 0x0;

	if (fifo == CAN_FIFO0)
		// Assign filter to FIFO-0
		CAN1->FFA1R &= ~(CAN_FFA1R_FFA0);
	else
		// Assign filter to FIFO-1
		CAN1->FFA1R |= CAN_FFA1R_FFA0;

	// Activate filter
	CAN1->FA1R |= CAN_FA1R_FACT0;

	// reset Initialization mode for filter banks
	CAN1->FMR &= ~(CAN_FMR_FINIT);
}

/**
 * -----------------------------------------------------------------------------
 * @brief 			Adds a 32-bit scale filter to filters bank
 * -----------------------------------------------------------------------------
 * @param filter
 * -----------------------------------------------------------------------------
 */
void CAN_Add32bitFilter(CAN_Filter_t filter)
{
	static uint8_t CanFilterBank = 0;

	// Check if Filter Memory is full
	if (CanFilterBank > 27)
		return;

	// Initialize filter
	CAN1->FMR |= CAN_FMR_FINIT;

	// Deactivate filter
	CAN1->FA1R &= ~(1U << CanFilterBank);

	// Set Scale
	CAN1->FS1R |= (1U << CanFilterBank);

	// Set Mode
	if (filter.maskMode)
		CAN1->FM1R &= ~(1U << CanFilterBank);
	else
		CAN1->FM1R |= (1U << CanFilterBank);

	// Reset Filters Bank
	CAN1->sFilterRegister[CanFilterBank].FR1 = 0;
	CAN1->sFilterRegister[CanFilterBank].FR2 = 0;

	// Set Standard or Extended Format
	if (filter.ide)
	{
		CAN1->sFilterRegister[CanFilterBank].FR1 |= (uint32_t) (filter.id1 << 3) | 4;
		CAN1->sFilterRegister[CanFilterBank].FR2 |= (uint32_t) (filter.id2 << 3) | 4;
		CAN1->sFilterRegister[CanFilterBank].FR1 |= (1U << 2U);
		CAN1->sFilterRegister[CanFilterBank].FR2 |= (1U << 2U);
	}
	else
	{
		CAN1->sFilterRegister[CanFilterBank].FR1 |= (uint32_t) (filter.id1 << 21);
		CAN1->sFilterRegister[CanFilterBank].FR2 |= (uint32_t) (filter.id2 << 21);
		CAN1->sFilterRegister[CanFilterBank].FR1 &= ~(1U << 2U);
		if (filter.maskMode)
			CAN1->sFilterRegister[CanFilterBank].FR2 |= (1U << 2U);
		else
			CAN1->sFilterRegister[CanFilterBank].FR2 &= ~(1U << 2U);
	}

	if (filter.rtr)
	{
		CAN1->sFilterRegister[CanFilterBank].FR1 |= 2;
		CAN1->sFilterRegister[CanFilterBank].FR2 |= 2;
	}

	// Assign filter to FIFO
	if (filter.fifo)
		CAN1->FFA1R |= (uint32_t) (1 << CanFilterBank);
	else
		CAN1->FFA1R &= ~(uint32_t) (1 << CanFilterBank);

	// Activate filter
	CAN1->FA1R |= (uint32_t) (1 << CanFilterBank);

	// Exit filter initialization mode
	CAN1->FMR &= ~(CAN_FMR_FINIT);
	CanFilterBank++;
}

