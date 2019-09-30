/*
 * rtos_timer.h
 *
 *  Created on: Sep 30, 2019
 *      Author: LuisFernando & Cesar Villarreal
 */

#ifndef RTOS_TIMER_H_
#define RTOS_TIMER_H_

/* TODO: insert other definitions and declarations here. */
#define LIMIT_TIME_S		(60U)
#define LIMIT_TIME_M		(60U)
#define LIMIT_TIME_H		(24U)
#define RST_TIME     	(0U)
#define TICKS_SECONDS	(1000U)

#define QUEUE_ELEMENTS	(3)

#define DEFAULT_HOURS	(5)
#define DEFAULT_MINUTES	(59)
#define DEFAULT_SECONDS	(45)

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

#endif /* RTOS_TIMER_H_ */
