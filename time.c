/*
	\file     time.c
	\brief    This is the source file for the time module.
	
	\authors: César Villarreal Hernández, ie707560
	          José Luis Rodríguez Gutiérrez,ie705694

	\date	  30/09/2019
 */

#include <time.h>

/*
 * De acuerdo a las diapositivas nombramos a los distintos semaforos, mutex y eventos.
 */
//handle by which the semaphore can be referenced
static SemaphoreHandle_t minutes_semaphore;
static SemaphoreHandle_t hours_semaphore;
static SemaphoreHandle_t g_mutex1;
// variable type to store event groups
static EventGroupHandle_t g_time_events;
//that references the queue it created
static QueueHandle_t xQueue;

static timer_alarm_t alarm = {ALARM_HR_VAL, ALARM_MIN_VAL, ALARM_SEC_VAL};

static const Keymap_t key_map[TABLE_SIZE]=
{
	{ZERO,        ZERO,  ZERO},
	{ONE,         ZERO,  ONE},
	{TWO,         ZERO,  TWO},
	{THREE,       ZERO,  THREE},
	{FOUR,        ZERO,  FOUR},
	{FIVE,        ZERO,  FIVE},
	{SIX,         ZERO,  SIX},
	{SEVEN,       ZERO,  SEVEN},
	{EIGHT,       ZERO,  EIGHT},
	{NINE,        ZERO,  NINE},
	{TEN,         ONE,   ZERO},
	{ELEVEN,      ONE,   ONE},
	{TWELVE,      ONE,   TWO},
	{THIRTEEN,    ONE,   THREE},
	{FOURTEEN,    ONE,   FOUR},
	{FIFTHTEEN,   ONE,   FIVE},
	{SIXTEEN,     ONE,   SIX},
	{SEVENTEEN,   ONE,   SEVEN},
	{EIGHTEEN,    ONE,   EIGHT},
	{NINETEEN,    ONE,   NINE},
	{TWENTY,      TWO,   ZERO},
	{TWENTYONE,   TWO,   ONE},
	{TWENTYTWO,   TWO,   TWO},
	{TWENTYTHREE, TWO,   THREE},
	{TWENTYFOUR,  TWO,   FOUR},
	{TWENTYFIVE,  TWO,   FIVE},
	{TWENTYSIX,   TWO,   SIX},
	{TWENTYSEVEN, TWO,   SEVEN},
	{TWENTYEIGHT, TWO,   EIGHT},
	{TWENTYNINE,  TWO,   NINE},
	{THIRTY,      THREE, ZERO},
	{THIRTYONE,   THREE, ONE},
	{THIRTYTWO, THREE, TWO},
	{THIRTYTHREE, THREE, THREE},
	{THIRTYFOUR,  THREE, FOUR},
	{THIRTYFIVE,  THREE, FIVE},
	{THIRTYSIX,   THREE, SIX},
	{THIRTYSEVEN, THREE, SEVEN},
	{THIRTYEIGHT, THREE, EIGHT},
	{THIRTYNINE,  THREE, NINE},
	{FOURTY,      FOUR,  ZERO},
	{FOURTYONE,   FOUR,  ONE},
	{FOURTYTWO,  FOUR,  TWO},
	{FOURTYTHREE,  FOUR,  THREE},
	{FOURTYFOUR,  FOUR,  FOUR},
	{FOURTYFIVE,  FOUR,  FIVE},
	{FOURTYSIX,   FOUR,  SIX},
	{FOURTYSEVEN, FOUR,  SEVEN},
	{FOURTYEIGHT, FOUR,  EIGHT},
	{FOURTYNINE,  FOUR,  NINE},
	{FIFTY,       FIVE,  ZERO},
	{FIFTYONE,    FIVE,  ONE},
	{FIFTYFIVE,   FIVE,  TWO},
	{FIFTYFIVE,   FIVE,  THREE},
	{FIFTYFOUR,   FIVE,  FOUR},
	{FIFTYFIVE,   FIVE,  FIVE},
	{FIFTYSIX,    FIVE,  SIX},
	{FIFTYSEVEN,  FIVE,  SEVEN},
	{FIFTYEIGHT,  FIVE,  EIGHT},
	{FIFTYNINE,   FIVE,  NINE}
};

uint8_t get_table_value(uint8_t unit, uint8_t decimal)
{
	uint8_t ret_val; //output val
	uint8_t i; //counter

	ret_val = FALSE; //default value; false is returned if NAVN

	for(i = ZERO; (TABLE_SIZE-1) > i ; i++)
	{
		/** Verify if the given units and decimals correspond to 0 - 59 **/
		if((key_map[i].unit == unit) && (key_map[i].decimal == decimal))
		{
			ret_val = key_map[i].nval; //merge units and decimals
		}
	}

	return ret_val;
}

uint8_t get_table_value_unit(uint8_t value)
{
	uint8_t ret_val; //output val
	uint8_t i; //counter

	ret_val = FALSE; //default value; false is returned if NAVN

	for(i = ZERO; (TABLE_SIZE-1) > i ; i++)
	{
		/** Verify if the given units and decimals correspond to 0 - 59 **/
		if(key_map[i].nval == value)
		{
			ret_val = key_map[i].unit; //merge units and decimals
		}
	}

	return ret_val;
}

uint8_t get_table_value_decimal(uint8_t value)
{
	uint8_t ret_val; //output val
	uint8_t i; //counter

	ret_val = FALSE; //default value; false is returned if NAVN

	for(i = ZERO; (TABLE_SIZE-1) > i ; i++)
	{
		/** Verify if the given units and decimals correspond to 0 - 59 **/
		if(key_map[i].nval == value)
		{
			ret_val = key_map[i].decimal; //merge units and decimals
		}
	}

	return ret_val;
}

void task_seconds()
{
	uint8_t seconds;
	time_msg_t *timer_alarm_queue;
	TickType_t xLastWakeTime;
	TickType_t xPeriod;

	/* define second's task period */
	xPeriod = pdMS_TO_TICKS(TICKS_SECONDS);
	/* retrieve the number of ticks */
	xLastWakeTime = xTaskGetTickCount();
	/* set second's default value */
	seconds = DEFAULT_SECONDS;

	/*asignamos espacio de memoria de acuerdo al tamaño de time_msg_t*/
	timer_alarm_queue = pvPortMalloc(sizeof(time_msg_t));

	for(;;)
	{
		/* task delay generation */
		vTaskDelayUntil(&xLastWakeTime, xPeriod);

		if(LIMIT_TIME_S == seconds)
		{
			/* reset second's value */
			seconds = RST_TIME;
			/* release minutes semaphore */
			xSemaphoreGive(minutes_semaphore);
		}
		else
		{
			/* do-nothing */
		}

		timer_alarm_queue->time_type = seconds_type;
		timer_alarm_queue->value = seconds;
		xQueueSend(xQueue,&timer_alarm_queue,portMAX_DELAY);

		/* increase the number of seconds */
		seconds++;
	}
}

void task_minutes()
{
	
	uint8_t minutes_t;
	time_msg_t *timer_alarm_queue;


	/* setting default hour value */
	minutes_t = DEFAULT_MINUTES;
	/* allocate memory space for minutes queue*/
	timer_alarm_queue = pvPortMalloc(sizeof(time_msg_t));

	for(;;)
	{
		/* wait for the minutes semaphore to be released */
		xSemaphoreTake(minutes_semaphore, portMAX_DELAY);
		
		if(LIMIT_TIME_M == minutes_t)
		{
			/* reset minute's value */
			minutes_t = RST_TIME;
			/* release hours semaphore */
			xSemaphoreGive(hours_semaphore);
		}
		else
		{
			/* do-nothing */
		}
		
		/* set queue's element time format */
		timer_alarm_queue->time_type = minutes_type;
		/* set queue's element time value */
		timer_alarm_queue->value = minutes_t;
		/* send time queue */
		xQueueSend(xQueue, &timer_alarm_queue, portMAX_DELAY);
		/* increase the number of minutes */
		minutes_t++;
	}
}

void task_hours()
{
	uint8_t hours_t;
	time_msg_t *timer_alarm_queue;

	/* setting default hour value */
 	hours_t = DEFAULT_HOURS;

	/*asignamos espacio de memoria de acuerdo al tamaño de time_msg_t*/
	timer_alarm_queue = pvPortMalloc(sizeof(time_msg_t));

	for(;;)
	{
		/* wait for the hours semaphore to be released */
		xSemaphoreTake(hours_semaphore,portMAX_DELAY);
		
		if(LIMIT_TIME_H == hours_t)
		{
			/* reset hour's value*/
			hours_t = RST_TIME;
			/* release hours semaphore*/
			xSemaphoreGive(hours_semaphore);

		}
		else
		{
			/* do-nothing */
		}
		
		/* set queue's element time format */
		timer_alarm_queue->time_type = hours_type;
		/* set queue's element time value */
		timer_alarm_queue->value = hours_t;
		/* send time queue */
		xQueueSend(xQueue,&timer_alarm_queue,portMAX_DELAY);
		/* increase the number of hours */
		hours_t++;
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

		PRINTF("%i%i:%i%i:%i%i\r\n", get_table_value_unit(hours_t),
									 get_table_value_decimal(hours_t),
									 get_table_value_unit(minutes_t),
									 get_table_value_decimal(minutes_t),
									 get_table_value_unit(seconds_t),
									 get_table_value_decimal(seconds_t));
		xSemaphoreGive(g_mutex1);
		check_alarm(hours_t, minutes_t, seconds_t);
	}
}

void task_alarm()
{
	EventBits_t events_flag;

	for(;;)
	{

	events_flag = xEventGroupGetBits(g_time_events);

	if(events_flag == (bit_0 | bit_1 | bit_2))
	{
		PRINTF("WAKE UP! \n \r");
		xEventGroupClearBits(g_time_events, (bit_0 | bit_1 | bit_2));
	}
	else
	{

	}
	}

}

void time_init()
{
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
}

void check_alarm(uint8_t hours, uint8_t minutes, uint8_t seconds)
{
	if((alarm.hours == hours) &&
	   (alarm.minutes == minutes) &&
	   (alarm.seconds == seconds))
	{
		xEventGroupSetBits(g_time_events, bit_0 | bit_1 | bit_2);
	}
	else
	{

	}
}
