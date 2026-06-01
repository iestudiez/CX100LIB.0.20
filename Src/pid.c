/**
 ******************************************************************************
 * @file        : pid.c
 * @author      : Estudiez, Ivan
 * @brief       : PID Library
 * @date        : 01/08/2024
 * @version     : 1.0
 ******************************************************************************
 */

#include "pid.h"
#include <stddef.h>
#include <stdlib.h>

// Defines
// ----------------------------------------------------------------------------
#define PID_MAX_ALLOWED_SETPOINT_ERROR			(30U) // 30%

// Private variables
// ----------------------------------------------------------------------------
void (*pidCallback)();

/**
 * ----------------------------------------------------------------------------
 * @brief 		PID algorithm for speed control with PWM_1000 output
 * ----------------------------------------------------------------------------
 * @param pid	Pointer to structure with PID variables
 * ----------------------------------------------------------------------------
 */
void PID_Pwm1000(PID_Vars_t *pid)
{
	int32_t output;
	int32_t maxAccumErr;
	int32_t errorTolerance;

	if (*pid->pSetpoint == 0 || !*pid->pEnable)
	{
		pid->var.error = 0;
		pid->var.accumError = 0;
		pid->var.lastError = 0;
		pid->err = 0;
		*pid->pOutput = 0;
		return;
	}

	// Check maximum admissible value
	if (pid->maxIntegral > 1000)
		pid->maxIntegral = 1000;

	// Calculate maximum allowable cumulative error
	if (pid->Ki > 0)
		maxAccumErr = (pid->maxIntegral * 100000) / pid->Ki;
	else
		maxAccumErr = 0;

	// Variable error calculation
	pid->var.error = *pid->pSetpoint - *pid->pFeedback;

	// Cumulative error used for the calculation of the integral term
	pid->var.accumError += pid->var.error;

	if (pid->var.accumError > maxAccumErr)
		pid->var.accumError = maxAccumErr;

	pid->var.propTerm = pid->Kp * pid->var.error;
	pid->var.intgTerm = pid->Ki * pid->var.accumError;
	pid->var.dervTerm = pid->Kd * (pid->var.lastError - pid->var.error);

	// Output = proportional term + integral term + derivative term
	output = (pid->var.propTerm + pid->var.intgTerm + pid->var.dervTerm) / 100000;

	// Check output minimum admissible value
	if (output < 0)
		output = 0;

	// Offset max value
	if (pid->offset > 500)
		pid->offset = 500;

	// Adding offset to output
	output += pid->offset;

	// Output maximum value
	if (output > 1000)
		output = 1000;

	// Assign result to output
	*pid->pOutput = (uint16_t) output;

	// Save the value of the error variable for the next iteration
	pid->var.lastError = pid->var.error;

	// Checking the limits of the variable
	if (pid->allowedErr > PID_MAX_ALLOWED_SETPOINT_ERROR)
		pid->allowedErr = PID_MAX_ALLOWED_SETPOINT_ERROR;

	// Deviation value allowed with respect to the setpoint
	errorTolerance = (*pid->pSetpoint * pid->allowedErr) / 100;

	// Increments the error variable if the setpoint value cannot be reached
	if (abs(pid->var.error) > errorTolerance)
		pid->err++;
	else
		pid->err = 0;
}

/**
 * ----------------------------------------------------------------------------
 * @brief 			PID Set callback routine
 * ----------------------------------------------------------------------------
 * @param user_api 	Pointer to user API routine
 * ----------------------------------------------------------------------------
 */
void PID_SetCallback(void (*user_api)())
{
	pidCallback = user_api;
}

/**
 * ----------------------------------------------------------------------------
 * @brief 			PID Callback
 * ----------------------------------------------------------------------------
 */
void PID_Callback()
{
	if (pidCallback != NULL)
		pidCallback();
}
