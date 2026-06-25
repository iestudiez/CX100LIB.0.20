/**
 ******************************************************************************
 * @file			: gnss.c
 * @author      	: Estudiez, Ivan
 * @brief       	: GNSS
 * @date		  	: 14/05/2025
 * @version			: 1.0
 ******************************************************************************
 *
 *
 *
 *
 *
 ******************************************************************************
 */

#include "gnss.h"
#include "uart.h"
#include "system_cx100.h"
#include "pwrboard.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

// Private definitions
// -----------------------------------------------------------------------------
#define RMC_POS_TIME				(1U)
#define RMC_POS_STATUS				(2U)
#define RMC_POS_LATITUDE			(3U)
#define RMC_POS_LONGITUDE			(5U)
#define RMC_POS_SPEED				(7U)
#define RMC_POS_DATE				(9U)
#define RMC_POS_MODE				(12U)

#define RMC_STR_BUFFER_SIZE			(11)

#define RMC_STR_HEADER_1			"$GPRMC"
#define RMC_STR_HEADER_2			"$GNRMC"

// Public variables
// -----------------------------------------------------------------------------
GnssRmcData_t GNSS_Rmc;
GnssStatus_t GNSS_Status;

// Private variables
// -----------------------------------------------------------------------------
char GNSS_Frame[UART_RX_BUFFER_SIZE];
char gnss_Buffer[RMC_STR_BUFFER_SIZE];

// Private functions
// -----------------------------------------------------------------------------
void GNSS_FrameBuilder(void);
void GNSS_RmcParser(void);
void GNSS_SendCommand(char *msg);
void extractToken(char *const pString);

uint8_t findTokenIdx(uint8_t tokenPos);
uint8_t GNSS_Checksum(const char *sGnssFrame);

/**
 * -----------------------------------------------------------------------------
 * @brief 				GNSS Initialization Routine
 * -----------------------------------------------------------------------------
 * @param refreshRate
 * -----------------------------------------------------------------------------
 */
void GNSS_Init(void)
{
	// Enable power supply for external sensors
	// ----------------------------------------
	PowerBoard.SensorSupply.External = PWR_ON;
	PWRBOARD_Update();

	// Enable GNSS module
	// ------------------
	GNSS_Enable();

	// Reduce output to only RMC
	// -------------------------
	SYSTEM_DelayMs(500);
	GNSS_SendCommand(GNSS_OUTPUT_RMC_ONLY);
	SYSTEM_DelayMs(50);
	GNSS_SendCommand(GNSS_OUTPT_RATE);
}

/**
 * -----------------------------------------------------------------------------
 * @brief 	Update GNSS data
 * -----------------------------------------------------------------------------
 */
void GNSS_Update(void)
{
	static bool runParser;

	// Increment timeout counter
	GNSS_Status.rxCounter++;

	// Check if there is a message available in the UART buffer
	if (GNSS_USART_MODULE->SR & USART_SR_IDLE)
	{
		UART_ClearIdleFlag(GNSS_USART_MODULE);

		// Clear reception counter
		GNSS_Status.rxCounter = 0;
		GNSS_Status.rxSerialOk = true;

		// Build the GNSS_Frame string
		GNSS_FrameBuilder();

		runParser = true;
		return;
	}

	// Check Serial Reception
	if (GNSS_Status.rxCounter >= GNSS_TIMEOUT)
	{
		GNSS_Status.rxCounter = GNSS_TIMEOUT;
		GNSS_Status.rxSerialOk = false;
		GNSS_RmcReset();
		return;
	}

	// Run RMC parser
	if (runParser)
	{
		GNSS_RmcParser();
		runParser = false;
	}
}

/**
 * -----------------------------------------------------------------------------
 * @brief 	Build GNSS frame
 * -----------------------------------------------------------------------------
 */
void GNSS_FrameBuilder(void)
{
	static uint8_t oldPos = 0;
	uint8_t pos;
	uint8_t i;
	uint8_t nReceivedChars;

// Set position pointer
	pos = UART_RX_BUFFER_SIZE - DMA1_Stream5->NDTR;

	if (pos > oldPos)
	{
		/**
		 * Processing is done in "linear" mode.
		 *
		 * Application processing is fast with single data block,
		 * length is simply calculated by subtracting pointers
		 *
		 * [   0   ]
		 * [   1   ] <- oldPos  |------------------------------------|
		 * [   2   ]            |                                    |
		 * [   3   ]            | Single block (len = pos - oldPos)  |
		 * [   4   ]            |                                    |
		 * [   5   ]            |------------------------------------|
		 * [   6   ] <- pos
		 * [   7   ]
		 * [ N - 1 ]
		 */

		nReceivedChars = pos - oldPos;

		for (i = 0; i < nReceivedChars; i++)
			GNSS_Frame[i] = UART_RxBuffer[oldPos + i];
	}
	else
	{
		/**
		 * Processing is done in "overflow" mode..
		 *
		 * Application must process data twice,
		 * since there are 2 linear memory blocks to handle
		 *
		 * [   0   ]            |---------------------------------|
		 * [   1   ]            | Second block (len = pos)        |
		 * [   2   ]            |---------------------------------|
		 * [   3   ] <- pos
		 * [   4   ] <- oldPos  |---------------------------------|
		 * [   5   ]            |                                 |
		 * [   6   ]            | First block (len = N - oldPos)  |
		 * [   7   ]            |                                 |
		 * [ N - 1 ]            |---------------------------------|
		 */

		nReceivedChars = UART_RX_BUFFER_SIZE - oldPos;

		// First block
		for (i = 0; i < nReceivedChars; i++)
			GNSS_Frame[i] = UART_RxBuffer[oldPos + i];

		// Second block
		for (i = 0; i < pos; i++)
			GNSS_Frame[i + nReceivedChars] = UART_RxBuffer[i];
	}
	oldPos = pos;
}

/**
 * -----------------------------------------------------------------------------
 * @brief 	RMC Parser
 * -----------------------------------------------------------------------------
 */
void GNSS_RmcParser(void)
{
	uint32_t intValue;
	uint8_t idx = 0;
	bool chkHeader = false;

	// Check if is a RMC message
	// ------------------------------------------------------------------------
	strncpy(gnss_Buffer, GNSS_Frame, 6);
	gnss_Buffer[6] = '\0';

	if (strcmp(gnss_Buffer, RMC_STR_HEADER_1) == 0)
		chkHeader = true;

	if (strcmp(gnss_Buffer, RMC_STR_HEADER_2) == 0)
		chkHeader = true;

	if (chkHeader == false)
	{
		GNSS_RmcReset();
		return;
	}

	// RMC Time Section
	// ------------------------------------------------------------------------
	idx = findTokenIdx(RMC_POS_TIME);

	// Extract time substring
	extractToken(&GNSS_Frame[idx]);

	// Conversion
	GNSS_Rmc.time = (uint32_t) (atoi(gnss_Buffer) / 1000);

	// RMC Status Section
	// ------------------------------------------------------------------------
	idx = findTokenIdx(RMC_POS_STATUS);

	// Conversion
	if (GNSS_Frame[idx] == 'A')
		GNSS_Rmc.status = true;
	else
		GNSS_Rmc.status = false;

	// RMC Latitude Section
	// ------------------------------------------------------------------------
	idx = findTokenIdx(RMC_POS_LATITUDE);

	// Extract latitude substring
	extractToken(&GNSS_Frame[idx]);

	// Conversion
	GNSS_Rmc.latitude = (uint32_t) atoi(gnss_Buffer);

	// RMC Longitude Section
	// ------------------------------------------------------------------------
	idx = findTokenIdx(RMC_POS_LONGITUDE);

	// Extract longitude substring
	extractToken(&GNSS_Frame[idx]);

	// Conversion
	GNSS_Rmc.longitude = (uint32_t) atoi(gnss_Buffer);

	// RMC Speed Section
	// ------------------------------------------------------------------------
	idx = findTokenIdx(RMC_POS_SPEED);

	// Extract speed substring
	extractToken(&GNSS_Frame[idx]);

	// Conversion
	intValue = (uint32_t) atoi(gnss_Buffer);
	GNSS_Rmc.speed = (uint16_t) ((uint32_t) (intValue * 185 / 100));

	// RMC Date Section
	// ------------------------------------------------------------------------
	idx = findTokenIdx(RMC_POS_DATE);

	// Extract date substring
	extractToken(&GNSS_Frame[idx]);

	// Conversion
	GNSS_Rmc.date = (uint32_t) atoi(gnss_Buffer);

	// RMC Mode Section
	// ------------------------------------------------------------------------
	idx = findTokenIdx(RMC_POS_MODE);
	GNSS_Rmc.mode = GNSS_Frame[idx];
}

/**
 * -----------------------------------------------------------------------------
 * @brief 			Send a GNSS command
 * -----------------------------------------------------------------------------
 * @param msg		Message to send
 * -----------------------------------------------------------------------------
 */
void GNSS_SendCommand(char *msg)
{
	sprintf(UART_TxBuffer, msg);
	DMA1_Stream6->NDTR = strlen(UART_TxBuffer);
	DMA_Stream_Enable(DMA1_Stream6);

	// Wait for DMA1_Stream6 transmission complete
	while (!((DMA1->HISR) & DMA_HISR_TCIF6))
	{
	}
	// Clear transfer complete flag
	DMA1->HIFCR |= DMA_HIFCR_CTCIF6;
}

/**
 * -----------------------------------------------------------------------------
 * @brief 				Perform the GNSS frame checksum
 * -----------------------------------------------------------------------------
 * @param gnssFrame		Pointer to GNSS frame
 * @return				Checksum
 * -----------------------------------------------------------------------------
 */
uint8_t GNSS_Checksum(const char *sGnssFrame)
{
	uint8_t checksum = 0;

	assert(sGnssFrame);

	// Start at index 1 to skip the leading '$'
	for (uint16_t i = 1; (sGnssFrame[i] != '*') && (sGnssFrame[i] != '\0'); i++)
		checksum ^= (uint8_t) sGnssFrame[i];

	return checksum;
}

/**
 * -----------------------------------------------------------------------------
 * @brief 	Enable GNSS Module
 * -----------------------------------------------------------------------------
 */
void GNSS_Enable(void)
{
	UART_Enable(GNSS_USART_MODULE);
}

/**
 * -----------------------------------------------------------------------------
 * @brief 	Disable GNSS Module
 * -----------------------------------------------------------------------------
 */
void GNSS_Disable(void)
{
	UART_Disable(GNSS_USART_MODULE);
}

/**
 * -----------------------------------------------------------------------------
 * @brief 				Get an integer from a string of characters
 * -----------------------------------------------------------------------------
 * @param pString		Pointer to a string with a float
 * -----------------------------------------------------------------------------
 */
void extractToken(char *const pString)
{
	char *pFloat = pString;
	char *pInt = gnss_Buffer;
	uint8_t limit = 0;

	// Scan the string
	while ((*pFloat != ',') && (limit < RMC_STR_BUFFER_SIZE))
	{
		// Buffer boundary check
		limit++;

		// Search for dot character
		if (*pFloat != '.')
		{
			*pInt = *pFloat;
			pFloat++;
			pInt++;
		}
		else
			pFloat++;
	}
	*pInt = '\0'; // Add termination character
}

/**
 * -----------------------------------------------------------------------------
 * @brief 				Finding the array index of a token
 * -----------------------------------------------------------------------------
 * @param tokenPos		Position of the requested token
 * @return				Array index of the token found
 *  -----------------------------------------------------------------------------
 */
uint8_t findTokenIdx(uint8_t tokenPos)
{
	uint8_t idx = 0;
	uint8_t cnt = 0;

	while ((cnt < tokenPos) && (idx < UART_RX_BUFFER_SIZE))
	{
		if (GNSS_Frame[idx] == ',')
			cnt++;
		idx++;
	}

	return idx;
}

/**
 * -----------------------------------------------------------------------------
 * @brief
 * -----------------------------------------------------------------------------
 */
void GNSS_RmcReset(void)
{
	GNSS_Rmc.status = false;
	GNSS_Rmc.date = 0;
	GNSS_Rmc.latitude = 0;
	GNSS_Rmc.longitude = 0;
	GNSS_Rmc.time = 0;
	GNSS_Rmc.speed = 0;
	GNSS_Rmc.mode = GNSS_RMC_MODE_N;
}
