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

typedef enum GnssVtgMode_e
{
	GNSS_VTG_MODE_A = 65, // Autonomous
	GNSS_VTG_MODE_D = 68, // DGPS
	GNSS_VTG_MODE_E = 69, // DR
	GNSS_VTG_MODE_N = 78, // Data not valid
	GNSS_VTG_MODE_R = 82, // Coarse Position
	GNSS_VTG_MODE_S = 83  // Simulator
} GnssVtgMode_e;

typedef enum GnssRmcMode_e
{
	GNSS_RMC_MODE_A = 65, // Autonomous
	GNSS_RMC_MODE_D = 68, // DGPS
	GNSS_RMC_MODE_E = 69, // DR
	GNSS_RMC_MODE_N = 78, // Data not valid
	GNSS_RMC_MODE_R = 82, // Coarse Position
	GNSS_RMC_MODE_S = 83  // Simulator
} GnssRmcMode_e;

typedef struct GnssVtgData_t
{
	uint16_t speed;
	GnssVtgMode_e mode;
	uint8_t satellites;
} GnssVtgData_t;

typedef struct GnssRmcData_t
{
	uint16_t speed;
	uint32_t time;
	uint32_t date;
	GnssRmcMode_e mode;
} GnssRmcData_t;

// Public definitions
// -----------------------------------------------------------------------------
#define	GNSS_OUTPUT_RATE_1HZ 			("$PMTK220,1000*1F\r\n")
#define	GNSS_OUTPUT_RATE_2HZ 			("$PMTK220,500*2B\r\n")
#define	GNSS_OUTPUT_RATE_5HZ 			("$PMTK220,200*2C\r\n")
#define	GNSS_OUTPUT_RATE_10HZ 			("$PMTK220,100*2F\r\n")
#define GNSS_OUTPUT_RMC_ONLY			("$PMTK314,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*29\r\n")
#define GNSS_OUTPT_RATE					GNSS_OUTPUT_RATE_2HZ

#define GNSS_TIMEOUT_LIMIT				(1500U)
#define GNSS_USART_MODULE				USART2

// Public variables
// -----------------------------------------------------------------------------
extern GnssVtgData_t GNSS_Vtg;
extern GnssRmcData_t GNSS_Rmc;
extern uint32_t GNSS_Timeout;

// Public functions
// -----------------------------------------------------------------------------
void GNSS_Init(void);
void GNSS_Update(void);
void GNSS_Disable(void);
void GNSS_Enable(void);

#endif /* INC_GNSS_H_ */
