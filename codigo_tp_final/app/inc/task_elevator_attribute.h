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
 * @file   : task_elevator_attribute.h
 * @date   : Set 26, 2023
 * @author : Juan Manuel Cruz <jcruz@fi.uba.ar> <jcruz@frba.utn.edu.ar>
 * @version	v1.0.0
 */

#ifndef TASK_INC_TASK_ELEVATOR_ATTRIBUTE_H_
#define TASK_INC_TASK_ELEVATOR_ATTRIBUTE_H_

#include <stdint.h>   // Para uint32_t, uint8_t, etc.
#include <stdbool.h>  // Para bool, true, false


/********************** CPP guard ********************************************/
#ifdef __cplusplus
extern "C" {
#endif

/********************** inclusions *******************************************/

/********************** macros ***********************************************/

/********************** typedef **********************************************/
/* elevator Statechart - State Transition Table */
/* 	------------------------+-----------------------+-----------------------+-----------------------+------------------------
 * 	| Current               | Event                 |                       | Next                  |                       |
 * 	| State                 | (Parameters)          | [Guard]               | State                 | Actions               |
 * 	|=======================+=======================+=======================+=======================+=======================|
 * 	| ST_SYS_XX_IDLE        | EV_SYS_XX_ACTIVE      |                       | ST_SYS_XX_ACTIVE      | put_event_t.._actuator|
 * 	|                       |                       |                       |                       | (event, identifier)   |
 * 	|-----------------------+-----------------------+-----------------------+-----------------------+-----------------------|
 * 	| ST_SYS_XX_ACTIVE      | EV_SYS_XX_IDLE        |                       | ST_SYS_XX_IDLE        | put_event_t.._actuator|
 * 	|                       |                       |                       |                       | (event, identifier)   |
 * 	------------------------+-----------------------+-----------------------+-----------------------+------------------------
 */

/* Events to excite Task elevator */
typedef enum task_elevator_ev {EV_SYS_BTN_FLOOR_PRESSED, EV_SYS_BTN_FLOOR_UNPRESSED, EV_SYS_CLOSE_DOOR_BTN,
							EV_SYS_OPEN_DOOR_BTN, EV_SYS_BTN_DOOR_UNPRESSED, EV_SYS_SENSOR_DOOR_ACTIVATED,
							EV_SYS_SENSOR_DOOR_DESACTIVATED, EV_SYS_IR_P, EV_SYS_EXTERN_BTN_FLOOR} task_elevator_ev_t;

/* State of Task elevator */
typedef enum task_elevator_st {ST_SYS_IDLE, ST_SYS_OPEN_DOOR, ST_SYS_CLOSING_DOOR, ST_SYS_CLOSED_DOOR, ST_SYS_MOVING_UP,
							ST_SYS_MOVING_DOWN, ST_SYS_OPENING_DOOR} task_elevator_st_t;

typedef struct floor_s {
	int id_floor;
	bool solicited;
} floor_t;

typedef struct
{
	uint32_t			tick;
	task_elevator_st_t	state;
	task_elevator_ev_t	event;
	bool 				flag; /*This flag indicates if the normal mode is active*/
	unsigned int 		qty_floor; /*It will be set in setup mode*/
	unsigned int 		max_people; /*stores the max qty of people admitted*/
	unsigned int 		current_people; /*stores the current qty of people inside the elevator*/
	int 				current_floor; /*Stores the current_floor value*/
	int					min_floor; /*stores the number of the lower floor*/
	int 				initial_floor; /*stores the number of the initial floor*/
	floor_t*			solicited_floor; /*Its an array of floor_t with size qty_floor*/
} task_elevator_dta_t;

/********************** external data declaration ****************************/
extern task_elevator_dta_t task_elevator_dta;

/********************** external functions declaration ***********************/

/********************** End of CPP guard *************************************/
#ifdef __cplusplus
}
#endif

#endif /* TASK_INC_TASK_elevator_ATTRIBUTE_H_ */

/********************** end of file ******************************************/
