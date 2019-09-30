/*
 * rtos_timer.c
 *
 *  Created on: Sep 30, 2019
 *      Author: LuisFernando & Cesar Villarreal
 */


void task_seconds()
{
	const TickType_t xPeriod = pdMS_TO_TICKS(TICKS_SECONDS);
	TickType_t xLastWakeTime = xTaskGetTickCount();

	uint8_t seconds = DEFAULT_SECONDS;
	time_msg_t *timer_alarm_queue;

	for(;;)
	{
		vTaskDelayUntil(&xLastWakeTime, xPeriod);
		seconds++;
		if(LIMIT_TIME_S == seconds)
		{
			seconds = RST_TIME;
			xSemaphoreGive(minutes_semaphore);
		}
		/*asignamos espacio de memoria de acuerdo al tamaño de time_msg_t*/
		timer_alarm_queue = pvPortMalloc(sizeof(time_msg_t));
		timer_alarm_queue->time_type = seconds_type;
		timer_alarm_queue->value = seconds;
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
	uint8_t hours_t = DEFAULT_HOURS;
	time_msg_t *timer_alarm_queue;

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
	uint8_t hours_t = DEFAULT_HOURS;
	uint8_t minutes_t = DEFAULT_MINUTES;
	uint8_t seconds_t = DEFAULT_SECONDS;

	for(;;)
	{
		xQueueReceive(xQueue,&timer_alarm_queue,portMAX_DELAY);

	}
}
