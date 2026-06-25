/**
 ******************************************************************************
 * @file        : pid.h
 * @author      : Estudiez, Ivan
 * @brief       : PID Library
 * @date        : 01/08/2024
 * @version     : 1.0
 ******************************************************************************
 *
 *
 *
 *
 *
 ******************************************************************************
 */

#ifndef PID_H_
#define PID_H_

#include "stdbool.h"
#include "stdint.h"

typedef struct
{
	int32_t error;
	int32_t accumError;
	int32_t lastError;
	int32_t propTerm;
	int32_t intgTerm;
	int32_t dervTerm;
} PID_PrivateVars_t;

typedef struct PID1000_t
{
	uint32_t *pSetpoint;
	uint32_t *pFeedback;
	uint16_t *pOutput;
	uint32_t offset;
	uint32_t Kp;
	uint32_t Ki;
	uint32_t Kd;
	uint32_t maxIntegral;  	// Maximum percentage of incidence of the integral part on the PWM output (0 to 1000)
	uint8_t allowedErr;		// Allowable error of feedback with respect to the setpoint  (0-30, Max.: 30%)
	uint32_t err;			// When the setpoint value cannot be reached this variable is incremented
	bool *pEnable;
	PID_PrivateVars_t priv;
} PID1000_t;

void PID_Pwm1000(PID1000_t *pid);
void PID_Callback();
void PID_SetCallback(void (*user_api)());

#endif /* PID_H_ */
