/**
 ******************************************************************************
 * @file        : scheduler.c
 * @author      : Estudiez, Ivan
 * @brief       : Co-operative scheduler to manage system tasks
 * @date        : 12/05/2025
 * @version     : 1.0
 ******************************************************************************
 *
 * Based on the algorithms proposed in the book:
 * Patterns for Time-Triggered Embedded Systems.
 * Michel J. Pont
 *
 ******************************************************************************
 */

#include "scheduler.h"
#include "stm32f4xx.h"
#include <stdint.h>
#include <stdio.h>
#include "systick.h"
#include "tim.h"
#include "global.h"

//#define SCHEDURLER_REPORT_ERRORS

// ----------------------------------------------------------------------------
// Public variables
// ----------------------------------------------------------------------------
uint8_t SCHEDULER_ErrorCode;

// ----------------------------------------------------------------------------
// Private variables
// ----------------------------------------------------------------------------
SCHEDULER_Task_t SCHEDULER_Tasks[SCHEDULER_MAX_TASKS];
uint8_t g_LastErrorCode;

/**
 * ----------------------------------------------------------------------------
 * @brief
 * Scheduler initialization function, prepares scheduler data structures.
 * Must call this function before using the scheduler.
 * ----------------------------------------------------------------------------
 */
void SCHEDULER_Init(void)
{
	for (uint8_t i = 0; i < SCHEDULER_MAX_TASKS; i++)
	{
		SCHEDULER_DeleteTask(i);
	}

	// Reset the error variable
	// SCHEDULER_DeleteTask() will generate an error code,
	// (because the task array is empty)
	SCHEDULER_ErrorCode = 0;
}

/**
 * ----------------------------------------------------------------------------
 * @brief
 * This routine is responsible for scheduling when tasks should be executed.
 * It is called at a rate determined by the TIMER1 settings
 * Important: A task must run in less than one scheduler tick.
 * ----------------------------------------------------------------------------
 */
void SCHEDULER_Update(void)
{
	for (uint8_t index = 0; index < SCHEDULER_MAX_TASKS; index++)
	{
		// Check if there is a task
		if (SCHEDULER_Tasks[index].pTask == NULL)
			continue;

		// Not yet ready to run, just decrement the delay
		if (SCHEDULER_Tasks[index].delay > 0)
		{
			SCHEDULER_Tasks[index].delay -= 1;
			continue;
		}

		// At this point the task is due to run
		SCHEDULER_Tasks[index].runMe += 1;

		// Schedule periodic tasks to run again
		if (SCHEDULER_Tasks[index].period)
			SCHEDULER_Tasks[index].delay = SCHEDULER_Tasks[index].period;
	}
}

/**
 * ----------------------------------------------------------------------------
 * @brief
 * Add a task to be executed at regular intervals, and after user-defined delay
 * ----------------------------------------------------------------------------
 * @param pCallback
 * @param DELAY
 * @param PERIOD
 * @return
 * ----------------------------------------------------------------------------
 */
uint8_t SCHEDULER_AddTask(void (*pCallback)(), const uint16_t DELAY, const uint16_t PERIOD)
{
	uint8_t index = 0;

	// Find the first gap in the array (if there is one)
	while ((SCHEDULER_Tasks[index].pTask != NULL) && (index < SCHEDULER_MAX_TASKS))
	{
		index++;
	}

	// Has the end of the list been reached?
	if (index == SCHEDULER_MAX_TASKS)
	{
		// Task list is full
		// Set the error variable
		SCHEDULER_ErrorCode = SCHEDULER_ERROR_TOO_MANY_TASKS;

		return SCHEDULER_MAX_TASKS;
	}

	// Add the task into the task array
	SCHEDULER_Tasks[index].pTask = pCallback;
	SCHEDULER_Tasks[index].delay = DELAY;
	SCHEDULER_Tasks[index].runMe = 0;

	// TODO: Fix the need to do this calculation to get the correct period times.
	if (PERIOD < 2)
		SCHEDULER_Tasks[index].period = PERIOD;
	else
		SCHEDULER_Tasks[index].period = PERIOD - 1;

	return index;
}

/**
 * -----------------------------------------------------------------------------
 * @brief
 * This is the 'dispatcher' function. When a task (function)
 * is due to run, SCH_Dispatch_Tasks() will run it.
 * -----------------------------------------------------------------------------
 */
void SCHEDULER_DispatchTasks(void)
{
	uint8_t index = 0;

	// Dispatches (runs) the next task (if one is ready)
	for (index = 0; index < SCHEDULER_MAX_TASKS; index++)
	{
		if (SCHEDULER_Tasks[index].runMe > 0)
		{
			// Run the task
			SCHEDULER_Tasks[index].pTask();

			// Reset/ reduce runMe flag
			SCHEDULER_Tasks[index].runMe -= 1;

			// Periodic tasks will automatically run again
			// if this is a "one shot" task, remove it from the array
			if (SCHEDULER_Tasks[index].period == 0)
				SCHEDULER_DeleteTask(index);
		}
	}
	// Report system status
	SCHEDULER_ReportStatus();
}

/**
 * -----------------------------------------------------------------------------
 * @brief
 * -----------------------------------------------------------------------------
 */
void SCHEDULER_Start(void)
{
	TIM_Enable(TIM7);
}

/**
 * -----------------------------------------------------------------------------
 * @brief
 * -----------------------------------------------------------------------------
 */
uint8_t SCHEDULER_DeleteTask(const uint8_t taskId)
{
	uint8_t returnCode;

	if (SCHEDULER_Tasks[taskId].pTask == NULL)
	{
		// No task at this location
		// Set the global error variable
		SCHEDULER_ErrorCode = SCHEDULER_ERROR_CANNOT_DELETE_TASK;
		returnCode = 1;
	}
	else
		returnCode = 0;

	SCHEDULER_Tasks[taskId].pTask = NULL;
	SCHEDULER_Tasks[taskId].delay = 0;
	SCHEDULER_Tasks[taskId].period = 0;
	SCHEDULER_Tasks[taskId].runMe = 0;

	// Return status
	return returnCode;
}

/**
 * -----------------------------------------------------------------------------
 * @brief
 * TIM7 is responsible for executing the scheduler every millisecond and
 * starts the TIM6
 * -----------------------------------------------------------------------------
 */
void TIM7_IRQHandler(void)
{
	// Clear interruption flag
	TIM7->SR &= ~TIM_SR_UIF;

	// Increment global tick counter
	GLOBAL_Ticks++;

	// Call Scheduler
	SCHEDULER_Update();
}

/**
 * -----------------------------------------------------------------------------
 * @brief
 * -----------------------------------------------------------------------------
 */
void SCHEDULER_ReportStatus(void)
{
#ifdef SCHEDURLER_REPORT_ERRORS
	// Only applies if errors are reported
	// Check for a new error code
if(SCHEDULER_ErrorCode != g_LastErrorCode)
{

}
#endif
}
