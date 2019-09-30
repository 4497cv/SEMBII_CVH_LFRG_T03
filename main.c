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
#include "time.h"
#include "bits.h"
#include "stdint.h"

/*
 * @brief   Application entry point.
 */
/* TODO: insert other structures*/


int main(void)
{

  	/* Init board hardware. */
    BOARD_InitBootPins();
    BOARD_InitBootClocks();
    BOARD_InitBootPeripherals();
  	/* Init FSL debug console. */
    BOARD_InitDebugConsole();
    time_init();

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
