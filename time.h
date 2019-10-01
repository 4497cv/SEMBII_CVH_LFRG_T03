/*
	\file     time.h
	\brief    This is the header file for the time module.
	
	\authors: César Villarreal Hernández, ie707560
	          Luis Fernando Rodríguez Gutiérrez,ie705694

	\date	  30/09/2019
 */

#ifndef TIME_H_
#define TIME_H_


/* TODO: insert other include files here. */
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "queue.h"
#include "event_groups.h"
#include "time.h"
#include "bits.h"
#include "stdint.h"
#include "fsl_debug_console.h"

/* TODO: insert other definitions and declarations here. */
#define LIMIT_TIME_S	(60U)
#define LIMIT_TIME_M	(60U)
#define LIMIT_TIME_H    (24U)
#define RST_TIME     	(0U)
#define TICKS_SECONDS	(1000U)
#define QUEUE_ELEMENTS	(3U)
#define DEFAULT_HOURS	(12U)
#define DEFAULT_MINUTES	(00U)
#define DEFAULT_SECONDS	(00U)

#define ALARM_HR_VAL    (12U)
#define ALARM_MIN_VAL   (00U)
#define ALARM_SEC_VAL   (15U)

#define FALSE (0U)

#define ZERO        0
#define ONE         1
#define TWO         2
#define THREE       3
#define FOUR        4
#define FIVE        5
#define SIX         6
#define SEVEN       7
#define EIGHT       8
#define NINE        9
#define TEN         10
#define ELEVEN      11
#define TWELVE      12
#define THIRTEEN    13
#define FOURTEEN    14
#define FIFTHTEEN   15
#define SIXTEEN     16
#define SEVENTEEN   17
#define EIGHTEEN    18
#define NINETEEN    19
#define TWENTY      20
#define TWENTYONE   21
#define TWENTYTWO   22
#define TWENTYTHREE 23
#define TWENTYFOUR  24
#define TWENTYFIVE  25
#define TWENTYSIX   26
#define TWENTYSEVEN 27
#define TWENTYEIGHT 28
#define TWENTYNINE  29
#define THIRTY      30
#define THIRTYONE   31
#define THIRTYTWO   32
#define THIRTYTHREE 33
#define THIRTYFOUR  34
#define THIRTYFIVE  35
#define THIRTYSIX 	36
#define THIRTYSEVEN 37
#define THIRTYEIGHT 38
#define THIRTYNINE 	39
#define FOURTY      40
#define FOURTYONE   41
#define FOURTYTWO   42
#define FOURTYTHREE 43
#define FOURTYFOUR  44
#define FOURTYFIVE  45
#define FOURTYSIX   46
#define FOURTYSEVEN 47
#define FOURTYEIGHT 48
#define FOURTYNINE  49
#define FIFTY       50
#define FIFTYONE    51
#define FIFTYTWO    52
#define FIFTYTHREE  53
#define FIFTYFOUR   54
#define FIFTYFIVE   55
#define FIFTYSIX    56
#define FIFTYSEVEN  57
#define FIFTYEIGHT  58
#define FIFTYNINE   59

#define TABLE_SIZE  60
#define ASCII_CONV  48

#define RS_MASK     (0x0F)
#define BIT8_MASK   (0x10)

#define HOUR_CAP    0
#define MIN_CAP     1
#define SEC_CAP     2
#define DAY_CAP     3
#define MONTH_CAP   4
#define YEAR_CAP    5

typedef enum
{
	MENU,
	SETHOUR,
	SETDATE,
	READHOUR,
	READDATE,
	WRITEMEM,
	SHOWMEM,
	RETURN
}	Time_type;

typedef struct
{
	uint8_t nval;
   	uint8_t unit;
   	uint8_t decimal;
} 	Keymap_t;


typedef enum
{
	seconds_type,
    minutes_type,
	hours_type
} time_types_t;

typedef struct
{
	uint8_t hours;
	uint8_t minutes;
	uint8_t seconds;
} timer_alarm_t;

typedef struct
{
	time_types_t time_type;
    uint8_t value;
} time_msg_t;

/*
 	 \brief	     This function retrieves time table value. The time input must be divided in units and decimals.
 	 \param[in]  uint8_t unit, uint8_t decimal
 	 \return     uint8_t
 */
uint8_t get_table_value(uint8_t unit, uint8_t decimal);

/*
 	 \brief	     This function retrieves time table unit value
 	 \param[in]  uint8_t unit, uint8_t decimal
 	 \return     uint8_t
 */
uint8_t get_table_value_unit(uint8_t value);

/*
 	 \brief	     This function retrieves time table decimal value
 	 \param[in]  uint8_t unit, uint8_t decimal
 	 \return     uint8_t
 */
uint8_t get_table_value_decimal(uint8_t value);

/*
 	 \brief	     Timer intialization
 	 \param[in]  void
 	 \return     void
 */
void time_init(void);

/*
 	 \brief	     This function handles clock's seconds
 	 \param[in]  void
 	 \return     void
 */
void task_seconds(void);

/*
 	 \brief	     This function handles clock's minutes
 	 \param[in]  void
 	 \return     void
 */
void task_minutes(void);

/*
 	 \brief	     This function handles clock's hours
 	 \param[in]  void
 	 \return     void
 */
void task_hours(void);

/*
 	 \brief	     This function handles clock's alarm
 	 \param[in]  void
 	 \return     void
 */
void task_alarm();

/*
 	 \brief	     This function handles the event group for the alarm
 	 \param[in]  void
 	 \return     void
 */
void check_alarm(uint8_t hours, uint8_t minutes, uint8_t seconds);

/*
 	 \brief	     This function handles terminal output
 	 \param[in]  void
 	 \return     void
 */
void task_print_terminal(void);

#endif /* TIME_H_ */
