/*
 * Copyright (c) 2023 Juan Manuel Cruz <jcruz@fi.uba.ar> <jcruz@frba.utn.edu.ar>.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 *
 * @file   : task_system.c
 * @date   : Set 26, 2023
 * @author : Juan Manuel Cruz <jcruz@fi.uba.ar> <jcruz@frba.utn.edu.ar>
 * @version	v1.0.0
 */

/********************** inclusions *******************************************/
/* Project includes. */
#include "main.h"

/* Demo includes. */
#include "logger.h"
#include "dwt.h"

/* Application & Tasks includes. */
#include "board.h"
#include "app.h"
#include "task_system_attribute.h"
#include "task_system_interface.h"
#include "task_actuator_attribute.h"
#include "task_actuator_interface.h"

/********************** macros and definitions *******************************/
#define G_TASK_SYS_CNT_INI			0ul
#define G_TASK_SYS_TICK_CNT_INI		0ul

#define DEL_SYS_XX_MIN				0ul
#define DEL_SYS_XX_MED				50ul
#define DEL_SYS_XX_MAX				500ul


/********************** internal data declaration ****************************/

#define SYSTEM_DTA_QTY	(sizeof(task_system_dta)/sizeof(task_system_dta_t))

/*Stores the variables of the system*/
task_system_dta_t task_system_dta =
	{		DEL_SYS_XX_MIN,
			ST_SYS_XX_IDLE,
			EV_SYS_XX_IDLE,
			false};

/********************** internal functions declaration ***********************/


/*
 * Its used to verify the obtained data from flash memory
 *
 * returns true for invalid data
 */
bool is_invalid(float val) {
    return (isnan(val) || val > MAX_TEMP_VALUE || val < MIN_TEMP_VALUE);
}

/*
 * Reads data from flash memory, using defined macros in flash_store.h for addressing
 * 	can be modified to read from any other kind of memory storage
 *
 */

void read_data (void* low_temp, void* high_temp, void* cl_temp)
{
		// reading from flash memory
		flash_read(SELECTED_PAGE, FLASH_SLOT_INDEX_LOW_TEMP, low_temp);
		flash_read(SELECTED_PAGE, FLASH_SLOT_INDEX_HIGH_TEMP, high_temp);
		flash_read(SELECTED_PAGE, FLASH_SLOT_INDEX_CL_TEMP, cl_temp);
}


/*
 * Saves given data in flash memory, using defined macros in flash_store.h for addressing.
 * 	can be modified to read from any other kind of memory storage
 *
 */

void save_data(void* low_temp, void* high_temp, void* cl_temp)
{
	// avoids null pointer assignment
	if (low_temp && high_temp && cl_temp)
	{
		//erase the SELECTED_PAGE
		flash_erase_page(SELECTED_PAGE);

		//save the data in flash memory, using fixed addresses for each variable
		flash_write(SELECTED_PAGE, FLASH_SLOT_INDEX_LOW_TEMP, low_temp);
		flash_write(SELECTED_PAGE, FLASH_SLOT_INDEX_HIGH_TEMP, high_temp);
		flash_write(SELECTED_PAGE, FLASH_SLOT_INDEX_CL_TEMP, cl_temp);
	}
}



/********************** internal data definition *****************************/
const char *p_task_system 		= "Task System (System Statechart)";
const char *p_task_system_ 		= "Non-Blocking & Update By Time Code";

/********************** external data declaration ****************************/
uint32_t g_task_system_cnt;
volatile uint32_t g_task_system_tick_cnt;

/********************** external functions definition ************************/
void task_system_init(void *parameters)
{
	task_system_dta_t 	*p_task_system_dta;
	task_system_st_t	state;
	task_system_ev_t	event;
	bool b_event;

	//Initialize the pointer to temperature_dta
	shared_temperature_dta_t* p_shared_temperature_dta = (shared_temperature_dta_t* )parameters;


	/* Print out: Task Initialized */
	LOGGER_LOG("  %s is running - %s\r\n", GET_NAME(task_system_init), p_task_system);
	LOGGER_LOG("  %s is a %s\r\n", GET_NAME(task_system), p_task_system_);

	g_task_system_cnt = G_TASK_SYS_CNT_INI;

	/* Print out: Task execution counter */
	LOGGER_LOG("   %s = %lu\r\n", GET_NAME(g_task_system_cnt), g_task_system_cnt);

	init_queue_event_task_system();

	/* Update Task Actuator Configuration & Data Pointer */
	p_task_system_dta = &task_system_dta;

	/* Print out: Task execution FSM */
	state = p_task_system_dta->state;
	LOGGER_LOG("   %s = %lu", GET_NAME(state), (uint32_t)state);

	event = p_task_system_dta->event;
	LOGGER_LOG("   %s = %lu", GET_NAME(event), (uint32_t)event);

	b_event = p_task_system_dta->flag;
	LOGGER_LOG("   %s = %s\r\n", GET_NAME(b_event), (b_event ? "true" : "false"));

	g_task_system_tick_cnt = G_TASK_SYS_TICK_CNT_INI;

	/*********************** START CONFIGURATION DATA ***********************/

	float aux_low_temp;
	float aux_high_temp;
	float aux_cl_temp;

	read_data(&aux_low_temp,&aux_high_temp, &aux_cl_temp);

	// if at least one saved data is invalid, the system sets default values in each variable
	if (is_invalid(aux_low_temp) || is_invalid(aux_high_temp) || is_invalid(aux_cl_temp))
	    {
	    	// print information status when flash memory has not valid data
	        LOGGER_LOG("Invalid stored data - loading default configuration\r\n");

	        // sets initial values
	        p_shared_temperature_dta->low_temp  = INITIAL_LOW_TEMP;
	        p_shared_temperature_dta->high_temp = INITIAL_HIGH_TEMP;
	        p_shared_temperature_dta->cl_temp   = INITIAL_CL_TEMP;

	    }
	    else
	    {
	    	// if there's valid information stored in flash memory, it will be loaded in shared_temperature_dta
	        LOGGER_LOG("loading stored data\r\n");

	        p_shared_temperature_dta->low_temp  = aux_low_temp;
	        p_shared_temperature_dta->high_temp = aux_high_temp;
	        p_shared_temperature_dta->cl_temp   = aux_cl_temp;
	    }

	/*********************** FINISHES CONFIGURATION DATA ***********************/

}

void task_system_update(void *parameters)
{
	task_system_dta_t *p_task_system_dta;
	bool b_time_update_required = false;

	//Initialize the pointer to temperature_dta
	shared_temperature_t* p_shared_temperature_dta = (shared_temperature_t* )parameters;

	/* Update Task System Counter */
	g_task_system_cnt++;

	/* Protect shared resource (g_task_system_tick) */
	__asm("CPSID i");	/* disable interrupts*/
    if (G_TASK_SYS_TICK_CNT_INI < g_task_system_tick_cnt)
    {
    	g_task_system_tick_cnt--;
    	b_time_update_required = true;
    }
    __asm("CPSIE i");	/* enable interrupts*/

    while (b_time_update_required)
    {
		/* Protect shared resource (g_task_system_tick) */
		__asm("CPSID i");	/* disable interrupts*/
		if (G_TASK_SYS_TICK_CNT_INI < g_task_system_tick_cnt)
		{
			g_task_system_tick_cnt--;
			b_time_update_required = true;
		}
		else
		{
			b_time_update_required = false;
		}
		__asm("CPSIE i");	/* enable interrupts*/

    	/* Update Task System Data Pointer */
		p_task_system_dta = &task_system_dta;

		if (true == any_event_task_system())
		{
			p_task_system_dta->flag = true; // indicates that there's at least one event in the event list
			p_task_system_dta->event = get_event_task_system(); //takes the event
		}


		switch (p_task_system_dta->state)
		{
			case ST_SYS_XX_ACTIVE:

				if(p_task_system_dta->flag == true)
				{

					if (p_task_system_dta->event == EV_SYS_XX_IDLE )
					{
						p_task_system_dta->state = ST_SYS_XX_IDLE;

						// turns off all leds
						put_event_task_actuator(EV_LED_XX_OFF,ID_LED_GREEN);
						put_event_task_actuator(EV_LED_XX_OFF,ID_LED_YELLOW);
						put_event_task_actuator(EV_LED_XX_OFF,ID_LED_RED);
						put_event_task_actuator(EV_LED_XX_OFF,ID_LED_CL);
					}



					// EACH STATE DECIDES WHAT TO DO CONSIDERING THE CURRENT_TEMP VALUE
					if(p_task_system_dta->event == EV_SYS_TEMP_INCREASING || p_task_system_dta->event == EV_SYS_TEMP_DECREASING )
					{

						if(p_shared_temperature_dta->current_temp <= p_shared_temperature_dta->low_temp)
						{
							p_task_system_dta->state = ST_SYS_LOW_TEMP;
						}


						// if (current_temp > low_temp && current_temp < high_temp )
						if( (p_shared_temperature_dta->current_temp > p_shared_temperature_dta->low_temp) && (p_shared_temperature_dta->current_temp < p_shared_temperature_dta->high_temp) )
						{
							p_task_system_dta->state = ST_SYS_MID_TEMP;
						}


						// if (current_temp > high_temp )
						if( p_shared_temperature_dta->current_temp >= p_shared_temperature_dta->high_temp)
						{
							p_task_system_dta->state = ST_SYS_HIGH_TEMP;
						}
					}


					p_task_system_dta->flag = false;
				}

				break;




















			case ST_SYS_XX_IDLE:

				if (p_task_system_dta->flag == true)
				{

					if (p_task_system_dta->event == EV_SYS_XX_IDLE)
					{
						p_task_system_dta->state = ST_SYS_XX_IDLE;
					}


					if(p_task_system_dta->event == EV_SYS_XX_ACTIVE)
					{
						p_task_system_dta->state = ST_SYS_XX_ACTIVE;
					}

					// only during IDLE state, the configuration data can be saved
					if(p_task_system_dta->event = EV_SYS_SAVE_CONFIG)
					{
						save_data(&(p_shared_temperature_dta->low_temp),&(p_shared_temperature_dta->high_temp),&(p_shared_temperature_dta->cl_temp));

						p_task_system_dta->state = ST_SYS_XX_IDLE;
					}



					p_task_system_dta->flag = false;
				}

				break;









				// CORREGIR Y TERMINAR
			case ST_SYS_LOW_TEMP:


				if((p_task_system_dta->flag) && (EV_SYS_ == p_task_system_dta->event))
				{
					/*Resta terminar la máquina de estados*/
				}


				/*transition cases*/
				if((p_task_system_dta->flag) && (p_shared_temperature_dta->current_temp > p_shared_temperature_dta->low_temp) && (p_shared_temperature_dta->current_temp < p_shared_temperature_dta->high_temp))
				{
					p_task_system_dta -> state = ST_SYS_MID_TEMP;
				}


				break;




			default:

				p_task_system_dta->state = ST_SYS_XX_IDLE;

				break;
		}
	}
}

/********************** end of file ******************************************/
