/**
 ******************************************************************************
 * @file			: gnss.h
 * @author      	: Estudiez, Ivan
 * @brief       	: GNSS
 * @date		  	: 06/01/2025
 * @version			: 1.0
 ******************************************************************************
 *
 *
 *
 *
 *
 ******************************************************************************
 */

#ifndef INC_GNSS_H_
#define INC_GNSS_H_

#include <stdint.h>
#include <stdbool.h>

typedef enum GnssRmcMode_e
{
	GNSS_RMC_MODE_A = 65, // Autonomous
	GNSS_RMC_MODE_D = 68, // DGPS
	GNSS_RMC_MODE_E = 69, // DR
	GNSS_RMC_MODE_N = 78, // Data not valid
} GnssRmcMode_e;

typedef struct GnssStatus_t
{
	uint32_t rxCounter;
	bool rxSerialOk;
}GnssStatus_t;

typedef struct GnssRmcData_t
{
	uint32_t time;
	bool status;
	uint32_t latitude;
	uint32_t longitude;
	uint16_t speed;
	uint32_t date;
	GnssRmcMode_e mode;
	uint8_t checksum;
} GnssRmcData_t;

// Public definitions
// -----------------------------------------------------------------------------
#define	GNSS_OUTPUT_RATE_1HZ 			("$PMTK220,1000*1F\r\n")
#define	GNSS_OUTPUT_RATE_2HZ 			("$PMTK220,500*2B\r\n")
#define	GNSS_OUTPUT_RATE_5HZ 			("$PMTK220,200*2C\r\n")
#define	GNSS_OUTPUT_RATE_10HZ 			("$PMTK220,100*2F\r\n")
#define GNSS_OUTPUT_RMC_ONLY			("$PMTK314,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*29\r\n")
#define GNSS_OUTPT_RATE					GNSS_OUTPUT_RATE_5HZ

#define GNSS_TIMEOUT					(500U)
#define GNSS_USART_MODULE				USART2

// Public variables
// -----------------------------------------------------------------------------
extern GnssRmcData_t GNSS_Rmc;
extern GnssStatus_t GNSS_Status;

// Public functions
// -----------------------------------------------------------------------------
void GNSS_Init(void);
void GNSS_Update(void);
void GNSS_Disable(void);
void GNSS_Enable(void);
void GNSS_RmcReset(void);

#endif /* INC_GNSS_H_ */
