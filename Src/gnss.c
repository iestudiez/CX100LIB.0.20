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
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

// Private definitions
// -----------------------------------------------------------------------------

// Public variables
// -----------------------------------------------------------------------------
GnssVtgData_t GNSS_Vtg;
GnssRmcData_t GNSS_Rmc;
uint32_t GNSS_Timeout;

// Private variables
// -----------------------------------------------------------------------------
const char *gHeader_VTG = "$GPVTG";
const char *gHeader_GGA = "$GPGGA";
const char *gHeader_RMC = "$GNRMC";
char GNSS_Frame[UART_RX_BUFFER_SIZE];

// Private functions
// -----------------------------------------------------------------------------
void GNSS_FrameBuilder(void);
void GNSS_RmcUpdate(void);
void GNSS_VtgUpdate(void);
void GNSS_GgaUpdate(void);
void GNSS_SendCommand(char *msg);
uint8_t GNSS_Checksum(const char *sGnssFrame);

/**
 * -----------------------------------------------------------------------------
 * @brief 				GNSS Initialization Routine
 * -----------------------------------------------------------------------------
 * @param refreshRate
 * -----------------------------------------------------------------------------
 */
void GNSS_Init(GnssRefresh_e refreshRate)
{
	// Enable GNSS module
	// ------------------
	GNSS_Enable();
	SYSTEM_DelayMs(100);

	// Reduce output to only RMC
	// -------------------------
	GNSS_SendCommand("\r\n");
	SYSTEM_DelayMs(50);
	GNSS_SendCommand("$PMTK314,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*29\r\n");
	SYSTEM_DelayMs(50);

	switch (refreshRate)
	{
	case GNSS_REFRESH_RATE_1HZ:
		GNSS_SendCommand("$PMTK220,1000*1F\r\n");
		break;

	case GNSS_REFRESH_RATE_2HZ:
		GNSS_SendCommand("$PMTK220,500*2B\r\n");
		break;

	case GNSS_REFRESH_RATE_5HZ:
		GNSS_SendCommand("$PMTK220,200*2C\r\n");
		break;

	case GNSS_REFRESH_RATE_10HZ:
		GNSS_SendCommand("$PMTK220,100*2F\r\n");
		break;
	}
}

/**
 * -----------------------------------------------------------------------------
 * @brief 	Update GNSS data
 * -----------------------------------------------------------------------------
 */
void GNSS_Update(void)
{
	// -------------------------------------------------------------------------
	// GNSS_FrameBuilder(), GNSS_VtgUpdate() and GNSS_GgaUpdate() functions are
	// executed only once for each time the GNSS_Update() task is called.
	// To ensure the execution of the GNSS_Update() task within one millisecond,
	// which is the update period of the task scheduler
	// -------------------------------------------------------------------------

	static char compHeader[7];
	uint8_t compResult;

	// Increment timeout counter
	GNSS_Timeout++;

	// Check if there is a message available in the UART buffer
	// --------------------------------------------------------
	if (USART2->SR & USART_SR_IDLE)
	{
		UART_ClearIdleFlag(USART2);

		// Clear timeout counter
		GNSS_Timeout = 0;

		// Build the GNSS_Frame string
		GNSS_FrameBuilder();

		// Store header in a comparison variable
		strncpy(compHeader, GNSS_Frame, 6);
		compHeader[6] = 0;

		return;
	}

	// Identify if it is a RMC type message and process it
	// ---------------------------------------------------
	compResult = strcmp(compHeader, gHeader_RMC);
	if (compResult == 0)
	{
		GNSS_RmcUpdate();
		return;
	}
}

/**
 * -----------------------------------------------------------------------------
 * @brief 	RMC Update
 * -----------------------------------------------------------------------------
 */
void GNSS_RmcUpdate(void)
{
	char speedStr[8];
	uint16_t speedKnot;
	uint8_t i = 0;
	uint8_t j = 0;
	uint8_t charCnt = 0;

	// Search for the beginning of the speed substring
	// -----------------------------------------------
	while (charCnt < 7)
	{
		if (GNSS_Frame[i] == ',')
			charCnt++;
		i++;
	}

	// Extract speed substring
	// -----------------------
	while (GNSS_Frame[i] != ',')
	{
		if (GNSS_Frame[i] != '.') // discard decimal point
		{
			speedStr[j] = GNSS_Frame[i];
			i++;
			j++;
		}
		else
			i++;
	}

	// Conversion
	// ----------
	speedStr[j] = 0x00; // termination character
	speedKnot = (uint16_t) atoi(speedStr);
	GNSS_Rmc.speed = (uint16_t) ((uint32_t) (speedKnot * 185 / 100));

	// Extract RMC Mode
	// ----------------
	while (charCnt < 12)
	{
		if (GNSS_Frame[i] == ',')
			charCnt++;
		i++;
	}
	GNSS_Rmc.mode = GNSS_Frame[i];
}

/**
 * -----------------------------------------------------------------------------
 * @brief 	VTG Update
 * -----------------------------------------------------------------------------
 */
void GNSS_VtgUpdate(void)
{
	char speedStr[8];
	uint8_t i = 0;
	uint8_t j = 0;
	uint8_t charCnt = 0;

	// Search for the beginning of the speed substring
	// -----------------------------------------------
	while (charCnt < 7)
	{
		if (GNSS_Frame[i] == ',')
			charCnt++;
		i++;
	}

	// Extract speed substring
	// -----------------------
	while (GNSS_Frame[i] != ',')
	{
		if (GNSS_Frame[i] != '.') // discard decimal point
		{
			speedStr[j] = GNSS_Frame[i];
			i++;
			j++;
		}
		else
			i++;
	}

	// Conversion
	// ----------
	speedStr[j] = 0x00; // termination character
	GNSS_Vtg.speed = (uint16_t) atoi(speedStr);

	// Extract VTG Mode
	// ----------------
	GNSS_Vtg.mode = GNSS_Frame[i + 3];
}

/**
 * -----------------------------------------------------------------------------
 * @brief 	GGA Update
 * -----------------------------------------------------------------------------
 */
void GNSS_GgaUpdate(void)
{
	char satUsedStr[8];
	uint8_t charCnt = 0;
	uint8_t i = 0;
	uint8_t j = 0;

	// Search for the beginning of the satellites used substring
	// ---------------------------------------------------------
	while (charCnt < 6)
	{
		if (GNSS_Frame[i] == ',')
			charCnt++;
		i++;
	}

	// Extract satellites used substring
	// ---------------------------------
	while (GNSS_Frame[i] != ',')
	{
		satUsedStr[j] = GNSS_Frame[i];
		i++;
		j++;
	}

	// Conversion
	// ----------
	satUsedStr[j] = 0x00; // termination character
	GNSS_Vtg.satellites = (uint8_t) atoi(satUsedStr);
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
 * @param gnssFrame
 * @return
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
	UART_Enable(UART_DEFAULT_MODULE);
}

/**
 * -----------------------------------------------------------------------------
 * @brief 	Disable GNSS Module
 * -----------------------------------------------------------------------------
 */
void GNSS_Disable(void)
{
	UART_Disable(UART_DEFAULT_MODULE);
}

