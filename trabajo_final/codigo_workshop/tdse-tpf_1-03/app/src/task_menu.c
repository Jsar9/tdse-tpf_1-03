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
 * @file   : task_menu.c
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
#include "task_menu_attribute.h"
#include "task_menu_interface.h"
#include "display.h"
#include "flash_store.h"

/********************** macros and definitions *******************************/
#define G_TASK_MEN_CNT_INI			0ul
#define G_TASK_MEN_TICK_CNT_INI		0ul

#define DEL_MEN_XX_MIN				0ul
#define DEL_MEN_XX_MED				50ul
#define DEL_MEN_XX_MAX				500ul

/********************** internal data declaration ****************************/

#define MENU_DTA_QTY	(sizeof(task_menu_dta)/sizeof(task_menu_dta_t))


/* Initializes the menu data*/
task_menu_dta_t task_menu_dta =
	{		DEL_MEN_XX_MIN,
			ST_MAIN_MENU,
			EV_MEN_ENT_IDLE,
			false,
			INITIAL_ID_MENU,
			INITIAL_PARAMETER,
			INITIAL_MENU_LOW_TEMP,
			INITIAL_MENU_HIGH_TEMP,
			INITIAL_MENU_CL_TEMP,
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

/********************** internal data definition *****************************/
const char *p_task_menu 		= "Task Menu (Interactive Menu)";
const char *p_task_menu_ 		= "Non-Blocking & Update By Time Code";

/********************** external data declaration ****************************/
uint32_t g_task_menu_cnt;
volatile uint32_t g_task_menu_tick_cnt;

/********************** external functions definition ************************/

void task_menu_init(void *parameters)
{
	task_menu_dta_t *p_task_menu_dta;
	task_menu_st_t	state;
	task_menu_ev_t	event;
	bool b_event;

	// initialize the shared data using parameters
	shared_temperature_dta_t* p_shared_temperature_dta = (shared_temperature_dta_t *) parameters;

	/* Print out: Task Initialized */
	LOGGER_LOG("  %s is running - %s\r\n", GET_NAME(task_menu_init), p_task_menu);
	LOGGER_LOG("  %s is a %s\r\n", GET_NAME(task_menu), p_task_menu_);

	g_task_menu_cnt = G_TASK_MEN_CNT_INI;

	/* Print out: Task execution counter */
	LOGGER_LOG("   %s = %lu\r\n", GET_NAME(g_task_menu_cnt), g_task_menu_cnt);

	init_queue_event_task_menu();

	/* Update Task Actuator Configuration & Data Pointer */
	p_task_menu_dta = &task_menu_dta;

	/* Print out: Task execution FSM */
	state = p_task_menu_dta->state;
	LOGGER_LOG("   %s = %lu", GET_NAME(state), (uint32_t)state);

	event = p_task_menu_dta->event;
	LOGGER_LOG("   %s = %lu", GET_NAME(event), (uint32_t)event);

	b_event = p_task_menu_dta->flag;
	LOGGER_LOG("   %s = %s\r\n", GET_NAME(b_event), (b_event ? "true" : "false"));

	cycle_counter_init();
	cycle_counter_reset();

	displayInit( DISPLAY_CONNECTION_GPIO_4BITS );

    displayCharPositionWrite(0, 0);
	displayStringWrite("TdSE - Trabajo Final");

	displayCharPositionWrite(0, 1);
	displayStringWrite("Test Nro: ");

	g_task_menu_tick_cnt = G_TASK_MEN_TICK_CNT_INI;


	/********************************* INITIALIZATION FROM FLASH MEMORY *************************/

	// initialize the shared parameters using the previous data stored in flash memory

	// aux variables to check if there's data stored in flash memory before to use it
	float aux_low_temp, aux_high_temp, aux_cl_temp;

	// reading from flash memory
	flash_read_page(SELECTED_PAGE, FLASH_SLOT_INDEX_LOW_TEMP, &aux_low_temp);
	flash_read_page(SELECTED_PAGE, FLASH_SLOT_INDEX_HIGH_TEMP, &aux_high_temp);
	flash_read_page(SELECTED_PAGE, FLASH_SLOT_INDEX_CL_TEMP, &aux_cl_temp);


	// checks the first stored value, to see if data its correct (avoiding nan errors or values out of range)
	// if at least one data is invalid, the menu sets default values in each variable
	    if (is_valid(aux_low_temp) || is_valid(aux_high_temp) || is_valid(aux_cl_temp))
	    {

	    	// print information status when flash memory has not valid data
	        LOGGER_LOG("Invalid data in flash memory - loading default configuration\r\n");

	        // sets initial values
	        p_shared_temperature_dta->low_temp  = INITIAL_MENU_LOW_TEMP;
	        p_shared_temperature_dta->high_temp = INITIAL_MENU_HIGH_TEMP;
	        p_shared_temperature_dta->cl_temp   = INITIAL_MENU_CL_TEMP;

	    }
	    else
	    {
	    	// if there's valid information stored in flash memory, it will be loaded in shared_temperature_dta and the menu structure
	        LOGGER_LOG("loading data from flash memory\r\n");

	        p_shared_temperature_dta->low_temp  = aux_low_temp;
	        p_shared_temperature_dta->high_temp = aux_high_temp;
	        p_shared_temperature_dta->cl_temp   = aux_cl_temp;


	        p_task_menu_dta -> low_temp = aux_low_temp;
	        p_task_menu_dta -> high_temp = aux_high_temp;
	        p_task_menu_dta -> cl_temp = aux_cl_temp;
	    }




}

void task_menu_update(void *parameters)
{
	task_menu_dta_t *p_task_menu_dta;
	bool b_time_update_required = false;
	char menu_str[8];

	//Initialize the pointer to temperature_dta
	shared_temperature_dta_t* p_shared_temperature_dta = (shared_temperature_dta_t* )parameters;

	/* Update Task Menu Counter */
	g_task_menu_cnt++;

	/* Protect shared resource (g_task_menu_tick) */
	__asm("CPSID i");	/* disable interrupts*/
    if (G_TASK_MEN_TICK_CNT_INI < g_task_menu_tick_cnt)
    {
    	g_task_menu_tick_cnt--;
    	b_time_update_required = true;
    }
    __asm("CPSIE i");	/* enable interrupts*/

    while (b_time_update_required)
    {
		/* Protect shared resource (g_task_menu_tick) */
		__asm("CPSID i");	/* disable interrupts*/
		if (G_TASK_MEN_TICK_CNT_INI < g_task_menu_tick_cnt)
		{
			g_task_menu_tick_cnt--;
			b_time_update_required = true;
		}
		else
		{
			b_time_update_required = false;
		}
		__asm("CPSIE i");	/* enable interrupts*/

    	/* Update Task Menu Data Pointer */
		p_task_menu_dta = &task_menu_dta;

    	if (DEL_MEN_XX_MIN < p_task_menu_dta->tick)
		{
			p_task_menu_dta->tick--;
		}
		else
		{
			snprintf(menu_str, sizeof(menu_str), "%lu", (g_task_menu_cnt/1000ul));
			displayCharPositionWrite(10, 1);
			displayStringWrite(menu_str);

			p_task_menu_dta->tick = DEL_MEN_XX_MAX;

			if (true == any_event_task_menu())
			{
				p_task_menu_dta->flag = true;
				p_task_menu_dta->event = get_event_task_menu();
			}


			switch (p_task_menu_dta->state)
			{
				case ST_MAIN_MENU:

					// actions - next
					if(EV_MEN_NEX_ACTIVE == p_task_menu_dta->event && p_task_menu_dta->id_mode < QTY_MODES)
					{
						p_task_menu_dta->id_mode++;
					}

					if(EV_MEN_NEX_ACTIVE == p_task_menu_dta->event && p_task_menu_dta->id_mode >= QTY_MODES)
					{
						p_task_menu_dta->id_mode = 0;
					}

					// actions - enter
					if (EV_MEN_ENT_ACTIVE == p_task_menu_dta->event && p_task_menu_dta->id_mode == ID_NORMAL_MODE )
					{
						p_task_menu_dta->state = ST_NORMAL_MODE;
						put_event_task_system(EV_SYS_XX_ACTIVE); //turns on the system during normal mode
					}

					if (EV_MEN_ENT_ACTIVE == p_task_menu_dta->event && p_task_menu_dta->id_mode == ID_SETUP_MODE )
					{
						p_task_menu_dta->state = ST_SETUP_MODE;
						put_event_task_system(EV_SYS_XX_IDLE); //turns off the system during setup mode

						// stores in menu_dta structure the current configuration values
						p_task_menu_dta->low_temp= p_shared_temperature_dta -> low_temp;
						p_task_menu_dta->high_temp= p_shared_temperature_dta -> high_temp ;
						p_task_menu_dta->cl_temp= p_shared_temperature_dta -> cl_temp;
					}

					// actions - esc
					if(EV_MEN_ESC_ACTIVE == p_task_menu_dta->event)
					{
						p_task_menu_dta->state = ST_MAIN_MENU;
					}

					break;

				case ST_NORMAL_MODE:

					// actions - esc
					if(EV_MEN_ESC_ACTIVE == p_task_menu_dta->event)
					{
						p_task_menu_dta->state = ST_MAIN_MENU;
						put_event_task_system(EV_SYS_XX_IDLE);  //turns off the system
					}

					if((EV_MEN_ESC_IDLE == p_task_menu_dta->event) || (EV_MEN_ENT_IDLE == p_task_menu_dta->event) || (EV_MEN_NEX_IDLE == p_task_menu_dta->event))
					{
						// SI NO SE PRESIONÓ NINGÚN BOTÓN:
						//se debe imprimir por display la temperatura leída del ADC (ya configurado, los shared data son los temperature_dta)p_temperature_dta->temp;
					}


					break;

				case ST_SETUP_MODE:


					// actions - next
					if(EV_MEN_NEX_ACTIVE == p_task_menu_dta->event &&  p_task_menu_dta->parameter < QTY_PARAMETERS)
					{
						p_task_menu_dta->parameter++ ;
					}

					if(EV_MEN_NEX_ACTIVE == p_task_menu_dta->event && p_task_menu_dta->parameter >= QTY_PARAMETERS )
					{
						p_task_menu_dta->parameter = 0 ;
					}


					// actions - enter
					if (EV_MEN_ENT_ACTIVE == p_task_menu_dta->event && p_task_menu_dta->parameter == ID_LOW_TEMP_PARAMETER)
					{
						p_task_menu_dta->state = ST_LOW_TEMP;
					}

					if (EV_MEN_ENT_ACTIVE == p_task_menu_dta->event && p_task_menu_dta->parameter == ID_HIGH_TEMP_PARAMETER)
					{
						p_task_menu_dta->state = ST_HIGH_TEMP;
					}

					if (EV_MEN_ENT_ACTIVE == p_task_menu_dta->event && p_task_menu_dta->parameter == ID_CL_TEMP_PARAMETER)
					{
						p_task_menu_dta->state = ST_CL_TEMP;
					}


					// actions - esc
					if(EV_MEN_ESC_ACTIVE == p_task_menu_dta->event )
					{
						if(p_task_menu_dta->save_data_required == true)
						{
							//save data if there is at least one change in the configuration data

							//erase the SELECTED_PAGE
							flash_erase_page(SELECTED_PAGE);

							//save the data in flash memory, using fixed addresses for each variable
							flash_write_page(SELECTED_PAGE, FLASH_SLOT_INDEX_LOW_TEMP, &(p_task_menu_dta->low_temp));
							flash_write_page(SELECTED_PAGE, FLASH_SLOT_INDEX_HIGH_TEMP, &(p_task_menu_dta->high_temp));
							flash_write_page(SELECTED_PAGE, FLASH_SLOT_INDEX_CL_TEMP, &(p_task_menu_dta->cl_temp));

							//update the shared_temperature_dta structure to avoid reading from flash memory
							p_shared_temperature_dta->low_temp = p_task_menu_dta->low_temp;
							p_shared_temperature_dta->high_temp = p_task_menu_dta->high_temp;
							p_shared_temperature_dta->cl_temp = p_task_menu_dta->cl_temp;

							p_task_menu_dta->save_data_required = false;
						}

						p_task_menu_dta->state = ST_MAIN_MENU;
					}

					break;

				case ST_LOW_TEMP :

					// actions - next (suma)
					if(EV_MEN_NEX_ACTIVE == p_task_menu_dta->event &&  p_task_menu_dta->low_temp < MAX_TEMP_VALUE  )
					{
						p_task_menu_dta->low_temp ++ ;
					}
					if(EV_MEN_NEX_ACTIVE == p_task_menu_dta->event &&  p_task_menu_dta->low_temp >= MAX_TEMP_VALUE )
					{
						p_task_menu_dta->low_temp = MIN_TEMP_VALUE ;
					}

					// actions - esc (resta)
					if(EV_MEN_ESC_ACTIVE == p_task_menu_dta->event &&  p_task_menu_dta->low_temp > MIN_TEMP_VALUE  )
					{
						p_task_menu_dta->low_temp --;
					}
					if(EV_MEN_ESC_ACTIVE == p_task_menu_dta->event &&  p_task_menu_dta->low_temp <= MIN_TEMP_VALUE  )
					{
						p_task_menu_dta->low_temp = MAX_TEMP_VALUE;
					}


					// actions - enter
					if(EV_MEN_ENT_ACTIVE == p_task_menu_dta->event )
					{
						// indicates that must be saved the configuration values
						p_task_menu_dta->save_data_required=true;

						// returns to setup mode
						p_task_menu_dta->state = ST_SETUP_MODE;

					}

					break;

				case ST_HIGH_TEMP :data
					// actions - next (suma)
					if(EV_MEN_NEX_ACTIVE == p_task_menu_dta->event &&  p_task_menu_dta->high_temp < MAX_TEMP_VALUE  )
					{
						p_task_menu_dta->high_temp ++ ;
					}
					if(EV_MEN_NEX_ACTIVE == p_task_menu_dta->event &&  p_task_menu_dta->high_temp >= MAX_TEMP_VALUE )
					{
						p_task_menu_dta->high_temp = MIN_TEMP_VALUE ;
					}

					// actions - esc (resta)
					if(EV_MEN_ESC_ACTIVE == p_task_menu_dta->event &&  p_task_menu_dta->high_temp > MIN_TEMP_VALUE  )
					{
						p_task_menu_dta->high_temp --;
					}
					if(EV_MEN_ESC_ACTIVE == p_task_menu_dta->event &&  p_task_menu_dta->high_temp <= MIN_TEMP_VALUE  )
					{
						p_task_menu_dta->high_temp = MAX_TEMP_VALUE;
					}


					// actions - enter
					if(EV_MEN_ENT_ACTIVE == p_task_menu_dta->event )
					{
						// indicates that must be saved the configuration values
						p_task_menu_dta->save_data_required=true;

						// returns to setup mode
						p_task_menu_dta->state = ST_SETUP_MODE;
					}

					break;

				case ST_CL_TEMP :
					// actions - next (suma)
					if(EV_MEN_NEX_ACTIVE == p_task_menu_dta->event &&  p_task_menu_dta->cl_temp < MAX_TEMP_VALUE  )
					{
						p_task_menu_dta->cl_temp ++ ;
					}
					if(EV_MEN_NEX_ACTIVE == p_task_menu_dta->event &&  p_task_menu_dta->cl_temp >= MAX_TEMP_VALUE )
					{
						p_task_menu_dta->cl_temp = MIN_TEMP_VALUE ;
					}

					// actions - esc (resta)
					if(EV_MEN_ESC_ACTIVE == p_task_menu_dta->event &&  p_task_menu_dta->cl_temp > MIN_TEMP_VALUE  )
					{
						p_task_menu_dta->cl_temp --;
					}
					if(EV_MEN_ESC_ACTIVE == p_task_menu_dta->event &&  p_task_menu_dta->cl_temp <= MIN_TEMP_VALUE  )
					{
						p_task_menu_dta->cl_temp = MAX_TEMP_VALUE;
					}


					// actions - enter
					if(EV_MEN_ENT_ACTIVE == p_task_menu_dta->event )
					{
						// indicates that must be saved the configuration values
						p_task_menu_dta->save_data_required=true;

						// returns to setup mode
						p_task_menu_dta->state = ST_SETUP_MODE;
					}

					break;

				default:

					p_task_menu_dta->tick  = DEL_MEN_XX_MIN;
					p_task_menu_dta->state = ST_MEN_XX_IDLE;
					p_task_menu_dta->event = EV_MEN_ENT_IDLE;
					p_task_menu_dta->flag  = false;

					break;
			}
		}
	}
}

/********************** end of file ******************************************/
