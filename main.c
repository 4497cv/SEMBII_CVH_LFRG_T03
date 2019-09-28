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
#define LIMIT_TIME		(60U)
#define LIMIT_HOUR		(24U)
#define RST_TIME     	(0U)
#define TICKS_SECONDS	(1000U)

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

void task_seconds()
{
	const TickType_t xPeriod = pdMS_TO_TICKS(TICKS_SECONDS);
	TickType_t xLastWakeTime = xTaskGetTickCount();

	uint8_t seconds = DEFAULT_SECONDS;
	time_msg_t *time_queue;

	for(;;)
	{
		vTaskDelayUntil(&xLastWakeTime, xPeriod);
		seconds++;
		if(LIMIT_TIME == seconds)
		{
			seconds = RST_TIME;
			xSemaphoreGive(minutes_semaphore);
		}
	}
}

int main(void) {

  	/* Init board hardware. */
    BOARD_InitBootPins();
    BOARD_InitBootClocks();
    BOARD_InitBootPeripherals();
  	/* Init FSL debug console. */
    BOARD_InitDebugConsole();

    PRINTF("Hello World\n");

    /* Force the counter to be placed into memory. */
    volatile static int i = 0 ;
    /* Enter an infinite loop, just incrementing a counter. */
    while(1) {
        i++ ;
        /* 'Dummy' NOP to allow source level single stepping of
            tight while() loop */
        __asm volatile ("nop");
    }
    return 0 ;
}
