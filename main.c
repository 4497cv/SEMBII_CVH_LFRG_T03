/*
 * Copyright 2016-2019 NXP
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of NXP Semiconductor, Inc. nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
 
/**
 * @file    timer_alarm_Project.c
 * @brief   Application entry point.
 */
#include <stdio.h>
#include "board.h"
#include "peripherals.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "MK64F12.h"
#include "fsl_debug_console.h"
/* TODO: insert other include files here. */
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "queue.h"
#include "event_groups.h"
/* TODO: insert other definitions and declarations here. */
#define LIMIT_TIME_S		(60U)
#define LIMIT_TIME_M		(60U)
#define LIMIT_TIME_H		(24U)
#define RST_TIME     	(0U)
#define TICKS_SECONDS	(1000U)

#define QUEUE_ELEMENTS	(3U)

#define DEFAULT_HOURS	(5U)
#define DEFAULT_MINUTES	(59U)
#define DEFAULT_SECONDS	(45U)

#define FALSE (0U)

/*
 * De acuerdo a las diapositivas nombramos a los distintos semaforos, mutex y eventos.
 */
//handle by which the semaphore can be referenced
SemaphoreHandle_t minutes_semaphore;
SemaphoreHandle_t hours_semaphore;
SemaphoreHandle_t g_mutex1;
SemaphoreHandle_t g_mutex2;
// variable type to store event groups
EventGroupHandle_t g_time_events;
//that references the queue it created
QueueHandle_t xQueue;
/*
 * @brief   Application entry point.
 */
/* TODO: insert other structures*/

typedef enum{seconds_type, minutes_type, hours_type} time_types_t;

typedef struct
{
	uint8_t seconds;
	uint8_t minutes;
	uint8_t hours;
} timer_alarm_t;

typedef struct
{
	time_types_t time_type;
    uint8_t value;
} time_msg_t;

void task_seconds(void);
void task_minutes(void);
void task_hours(void);

void task_print_terminal(void);

int main(void)
{

  	/* Init board hardware. */
    BOARD_InitBootPins();
    BOARD_InitBootClocks();
    BOARD_InitBootPeripherals();
  	/* Init FSL debug console. */
    BOARD_InitDebugConsole();
	
	/* semaphore's mutex instantiation */
    g_mutex1 = xSemaphoreCreateMutex();
    /* minutes semaphore instantiation */
    minutes_semaphore = xSemaphoreCreateBinary();
	/* hours semaphore instantiation */
    hours_semaphore = xSemaphoreCreateBinary();
    /* time event instantiation **/
    g_time_events = xEventGroupCreate();
    /* time queue instantiation*/
    xQueue = xQueueCreate(QUEUE_ELEMENTS, sizeof(time_msg_t*));

    /* seconds task creation */
    xTaskCreate(task_seconds, "t_sec", configMINIMAL_STACK_SIZE, NULL, configMAX_PRIORITIES-1, NULL);
	/* minutes task creation */
    xTaskCreate(task_minutes, "t_min", configMINIMAL_STACK_SIZE, NULL, configMAX_PRIORITIES-1, NULL);
	/* hours task creation */
    xTaskCreate(task_hours, "t_hr", configMINIMAL_STACK_SIZE, NULL, configMAX_PRIORITIES-1, NULL);
	/* task for the terminal creation */
    xTaskCreate(task_print_terminal, "terminal_print", configMINIMAL_STACK_SIZE, NULL, configMAX_PRIORITIES-1, NULL);

    /* start scheduler */
    vTaskStartScheduler();

    for(;;)
    {

    }

    return 0;
}

void task_seconds()
{
	uint8_t seconds;
	time_msg_t *timer_alarm_queue;
	TickType_t xLastWakeTime;
	const TickType_t xPeriod;

	/* define second's task period */
	xPeriod = pdMS_TO_TICKS(TICKS_SECONDS);
	/* retrieve the number of ticks */
	xLastWakeTime = xTaskGetTickCount();
	/* set second's default value */
	seconds = DEFAULT_SECONDS;

	for(;;)
	{
		/* generate a task delay */
		vTaskDelayUntil(&xLastWakeTime, xPeriod);

		if(LIMIT_TIME_S == seconds)
		{
			seconds = RST_TIME;
			xSemaphoreGive(minutes_semaphore);
		}
		else
		{

		}
		
		/*asignamos espacio de memoria de acuerdo al tamaño de time_msg_t*/
		timer_alarm_queue = pvPortMalloc(sizeof(time_msg_t));
		timer_alarm_queue->time_type = seconds_type;
		timer_alarm_queue->value = seconds;
		/* increase the number of seconds */
		seconds++;

		/*
		 * #define portMAX_DELAY ( TickType_t ) 0xffffffffUL
		 */
		xQueueSend(xQueue,&timer_alarm_queue,portMAX_DELAY);
	}
}

void task_minutes()
{
	uint8_t minutes_t = DEFAULT_MINUTES;
	time_msg_t *timer_alarm_queue;

	for(;;)
	{
		xSemaphoreTake(minutes_semaphore,portMAX_DELAY);
		minutes_t++;
		if(LIMIT_TIME_M == minutes_t)
		{
			minutes_t = RST_TIME;
			xSemaphoreGive(hours_semaphore);
		}
		/*asignamos espacio de memoria de acuerdo al tamaño de time_msg_t*/
		timer_alarm_queue = pvPortMalloc(sizeof(time_msg_t));
		timer_alarm_queue->time_type = minutes_type;
		timer_alarm_queue->value = minutes_t;

		xQueueSend(xQueue,&timer_alarm_queue,portMAX_DELAY);
	}
}

void task_hours()
{
	uint8_t hours_t;
	time_msg_t *timer_alarm_queue;

 	hours_t = DEFAULT_HOURS;

	for(;;)
	{
		xSemaphoreTake(hours_semaphore,portMAX_DELAY);
		hours_t++;
		if(LIMIT_TIME_H == hours_t)
		{
			hours_t = RST_TIME;
			xSemaphoreGive(hours_semaphore);
		}
		/*asignamos espacio de memoria de acuerdo al tamaño de time_msg_t*/
		timer_alarm_queue = pvPortMalloc(sizeof(time_msg_t));
		timer_alarm_queue->time_type = hours_type;
		timer_alarm_queue->value = hours_t;

		xQueueSend(xQueue,&timer_alarm_queue,portMAX_DELAY);
	}
}

void task_print_terminal()
{
	time_msg_t *timer_alarm_queue;
	uint8_t hours_t;
	uint8_t minutes_t;
	uint8_t seconds_t;

	hours_t = DEFAULT_HOURS;	
	minutes_t = DEFAULT_MINUTES;
	seconds_t = DEFAULT_SECONDS;

	for(;;)
	{
		do
		{
			xQueueReceive(xQueue, &timer_alarm_queue,portMAX_DELAY);
			switch(timer_alarm_queue->time_type)
			{
			case (seconds_type):
					seconds_t = timer_alarm_queue->value;
					break;
			case (minutes_type):
					minutes_t = timer_alarm_queue->value;
					if(FALSE == minutes_t)
					{
						hours_t = RST_TIME;
					}
					break;
			case (hours_type):
					hours_t = timer_alarm_queue->value;
					break;
			default:
					break;
			}
		}while(FALSE != uxQueueMessagesWaiting(xQueue));

		xSemaphoreTake(g_mutex1,portMAX_DELAY);
		PRINTF("%i:%i:%i\r\n", hours_t, minutes_t, seconds_t);
		xSemaphoreGive(g_mutex1);
	}
}
