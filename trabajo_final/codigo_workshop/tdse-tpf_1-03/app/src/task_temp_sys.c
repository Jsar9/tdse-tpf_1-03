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
 * @file   : task_temp_sys.c
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
#include "task_temp_sys_attribute.h"
#include "task_temp_sys_interface.h"

/********************** macros and definitions *******************************/
#define G_TASK_SYS_CNT_INI			0ul
#define G_TASK_SYS_TICK_CNT_INI		0ul

#define DEL_TEMP_SYS_XX_MIN				0ul
#define DEL_TEMP_SYS_XX_MED				50ul
#define DEL_TEMP_SYS_XX_MAX				500ul


/********************** internal data declaration ****************************/

#define TEMP_SYS_DTA_QTY	(sizeof(task_temp_sys_dta)/sizeof(task_temp_sys_dta_t))

/*Stores the variables of the temp_sys*/
task_temp_sys_dta_t task_temp_sys_dta =
	{		DEL_TEMP_SYS_XX_MIN,
			ST_TEMP_SYS_XX_IDLE,
			EV_TEMP_SYS_XX_IDLE,
			false};

/********************** internal functions declaration ***********************/

/********************** internal data definition *****************************/
const char *p_task_temp_sys 		= "Task Temp temp_sys (Temp temp_sys Statechart)";
const char *p_task_temp_sys_ 		= "Non-Blocking & Update By Time Code";

/********************** external data declaration ****************************/
uint32_t g_task_temp_sys_cnt;
volatile uint32_t g_task_temp_sys_tick_cnt;

/********************** external functions definition ************************/
void task_temp_sys_init(void *parameters)
{
	task_temp_sys_dta_t 	*p_task_temp_sys_dta;
	task_temp_sys_st_t	state;
	task_temp_sys_ev_t	event;
	bool b_event;


	/* Print out: Task Initialized */
	LOGGER_LOG("  %s is running - %s\r\n", GET_NAME(task_temp_sys_init), p_task_temp_sys);
	LOGGER_LOG("  %s is a %s\r\n", GET_NAME(task_temp_sys), p_task_temp_sys_);

	g_task_temp_sys_cnt = G_TASK_SYS_CNT_INI;

	/* Print out: Task execution counter */
	LOGGER_LOG("   %s = %lu\r\n", GET_NAME(g_task_temp_sys_cnt), g_task_temp_sys_cnt);

	init_queue_event_task_temp_sys();

	/* Update Task Actuator Configuration & Data Pointer */
	p_task_temp_sys_dta = &task_temp_sys_dta;

	/* Print out: Task execution FSM */
	state = p_task_temp_sys_dta->state;
	LOGGER_LOG("   %s = %lu", GET_NAME(state), (uint32_t)state);

	event = p_task_temp_sys_dta->event;
	LOGGER_LOG("   %s = %lu", GET_NAME(event), (uint32_t)event);

	b_event = p_task_temp_sys_dta->flag;
	LOGGER_LOG("   %s = %s\r\n", GET_NAME(b_event), (b_event ? "true" : "false"));

	g_task_temp_sys_tick_cnt = G_TASK_SYS_TICK_CNT_INI;
}

void task_temp_sys_update(void *parameters)
{
	task_temp_sys_dta_t *p_task_temp_sys_dta;
	bool b_time_update_required = false;

	/*****************/

	//Initialize an auxiliar variable used for adc_read convertion
	float v_aux= 0;

	//Initialize the pointer to temperature_dta
	shared_temperature_t* p_shared_temperature_dta = (shared_temperature_t* )parameters;


	/*****************/

	/* Update Task temp_sys Counter */
	g_task_temp_sys_cnt++;

	/* Protect shared resource (g_task_temp_sys_tick) */
	__asm("CPSID i");	/* disable interrupts*/
    if (G_TASK_SYS_TICK_CNT_INI < g_task_temp_sys_tick_cnt)
    {
    	g_task_temp_sys_tick_cnt--;
    	b_time_update_required = true;
    }
    __asm("CPSIE i");	/* enable interrupts*/

    while (b_time_update_required)
    {
		/* Protect shared resource (g_task_temp_sys_tick) */
		__asm("CPSID i");	/* disable interrupts*/
		if (G_TASK_SYS_TICK_CNT_INI < g_task_temp_sys_tick_cnt)
		{
			g_task_temp_sys_tick_cnt--;
			b_time_update_required = true;
		}
		else
		{
			b_time_update_required = false;
		}
		__asm("CPSIE i");	/* enable interrupts*/

    	/* Update Task temp_sys Data Pointer */
		p_task_temp_sys_dta = &task_temp_sys_dta;

		if (true == any_event_task_temp_sys())
		{
			p_task_temp_sys_dta->flag = true; //the flag indicates the presence of any event in the event list
			p_task_temp_sys_dta->event = get_event_task_temp_sys();
		}






		switch (p_task_temp_sys_dta->state)
		{
			case ST_TEMP_SYS_XX_ACTIVE:

				// **************** EV_SYS_XX_ACTIVE
				if ( (p_task_temp_sys_dta->flag) && (EV_SYS_XX_ACTIVE == p_task_temp_sys_dta->event) )
				{
					if(p_shared_temperature_dta->adc_end_of_conversion == true)
					{
						/***************************	TEMPERATURE CONVERTION	********************/
								// update the previous temperature
								p_shared_temperature_dta->previous_temp = p_shared_temperature_dta->current_temp;

								// update the v_aux calculating the read voltage, considering 3.3V reference and a 12 bits lecture from ADC
								v_aux = (3.3 * p_shared_temperature_dta->adc_read) / 4095;

								// converts voltage to temperature considering LM35 sensor (10mV / °C) and update the current temperature
								p_shared_temperature_dta->current_temp = v_aux / 0.01;

								// sets false in the adc_end_of_conversion flag because the data was used
								p_shared_temperature_dta -> adc_end_of_conversion = false;

						/****************************************************************************/
					}

					if(p_shared_temperature_dta->previous_temp > p_shared_temperature_dta->current_temp)
					{
						put_event_task_system(EV_SYS_TEMP_DECREASING);
					}

					if(p_shared_temperature_dta->current_temp > p_shared_temperature_dta->previous_temp)
					{
						put_event_task_system(EV_SYS_TEMP_INCREASING);
					}

					// cancel the flag event
					p_task_temp_sys_dta->flag = false;

					// keep in active mode
					p_task_temp_sys_dta->state = ST_TEMP_SYS_XX_ACTIVE;
				}



				// **************** EV_SYS_XX_IDLE
				if ((p_task_temp_sys_dta->flag) && (EV_SYS_XX_IDLE == p_task_temp_sys_dta->event))
				{
					p_task_temp_sys_dta->state = ST_TEMP_SYS_XX_IDLE;

					p_task_temp_sys_dta->flag = false;
				}


				break;

			case ST_TEMP_SYS_XX_IDLE:

				// **************** EV_SYS_XX_ACTIVE
				if ((p_task_temp_sys_dta->flag) && (EV_SYS_XX_ACTIVE == p_task_temp_sys_dta->event))
				{
					p_task_temp_sys_dta->flag = false;
					p_task_temp_sys_dta->state = ST_TEMP_SYS_XX_ACTIVE;
				}

				// **************** EV_SYS_XX_IDLE
				if((p_task_temp_sys_dta->flag) && (EV_SYS_XX_ACTIVE == p_task_temp_sys_dta->event))
				{
					p_task_temp_sys_dta->state = ST_TEMP_SYS_XX_IDLE;
					p_task_temp_sys_dta->flag = false;
				}

				break;



			default:

				p_task_temp_sys_dta->state = ST_SYS_XX_IDLE;

				break;
		}
	}
}

/********************** end of file ******************************************/
