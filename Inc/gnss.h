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

typedef enum GnssRefresh_e
{
	GNSS_REFRESH_RATE_1HZ = 1,
	GNSS_REFRESH_RATE_2HZ = 2,
	GNSS_REFRESH_RATE_5HZ = 3,
	GNSS_REFRESH_RATE_10HZ = 4
} GnssRefresh_e;

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
}GnssRmcData_t;

// Public definitions
// -----------------------------------------------------------------------------
#define GNSS_TIMEOUT_LIMIT				(1500U)

// Public variables
// -----------------------------------------------------------------------------
extern GnssVtgData_t GNSS_Vtg;
extern GnssRmcData_t GNSS_Rmc;
extern uint32_t GNSS_Timeout;

// Public functions
// -----------------------------------------------------------------------------
void GNSS_Init(GnssRefresh_e refreshRate);
void GNSS_Update(void);
void GNSS_Disable(void);
void GNSS_Enable(void);

#endif /* INC_GNSS_H_ */
