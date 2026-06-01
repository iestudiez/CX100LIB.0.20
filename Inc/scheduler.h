/*
 ******************************************************************************
 * @file        : scheduler.h
 * @author      : Estudiez, Ivan
 * @brief       : Co-operative scheduler to manage system tasks
 * @date        : 12/05/2025
 * @version     : 1.0
 ******************************************************************************
 *
 *
 *
 *
 *
 ******************************************************************************
 */

#ifndef SCHEDULER_H_
#define SCHEDULER_H_

#include <stdbool.h>
#include <stdint.h>
#include "systick.h"

#define SCHEDULER_MAX_TASKS						10

// Error definitions
// -----------------------------------------------
#define SCHEDULER_ERROR_TOO_MANY_TASKS			01
#define SCHEDULER_ERROR_CANNOT_DELETE_TASK		02

typedef struct
{
	void (*pTask)();
	int16_t delay;
	int16_t period;
	int16_t runMe;
} SCHEDULER_Task_t;

// ============================================================================
// Public methods
// ============================================================================
void SCHEDULER_Init(void);
void SCHEDULER_Start(void);
void SCHEDULER_Update(void);
void SCHEDULER_DispatchTasks(void);
void SCHEDULER_ReportStatus(void);

// ============================================================================
// Public variables
// ============================================================================
uint8_t SCHEDULER_AddTask(void (*pCallback)(), const uint16_t DELAY, const uint16_t PERIOD);
uint8_t SCHEDULER_DeleteTask(uint8_t taskId);

extern uint8_t SCHEDULER_ErrorCode;

#endif /* SCHEDULER_H_ */
