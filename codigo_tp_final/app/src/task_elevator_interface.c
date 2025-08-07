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
 * @file   : task_system_interface.c
 * @date   : Set 26, 2023
 * @author : Juan Manuel Cruz <jcruz@fi.uba.ar> <jcruz@frba.utn.edu.ar>
 * @version	v1.0.0
 */

/********************** inclusions *******************************************/
/* Project includes. */
#include <task_elevator_attribute.h>
#include <stdlib.h>
#include "main.h"

/* Demo includes. */
#include "logger.h"
#include "dwt.h"

/* Application & Tasks includes. */
#include "board.h"
#include "app.h"

/********************** macros and definitions *******************************/
#define EVENT_UNDEFINED	(255)
#define MAX_EVENTS		(16)

/********************** internal data declaration ****************************/

/********************** internal functions declaration ***********************/

/********************** internal data definition *****************************/
struct
{
	uint32_t	head;
	uint32_t	tail;
	uint32_t	count;
	task_elevator_ev_t	queue[MAX_EVENTS];
} queue_task_a;

/********************** external data declaration ****************************/

/********************** external functions definition ************************/
void init_queue_event_task_elevator(void)
{
	uint32_t i;

	queue_task_a.head = 0;
	queue_task_a.tail = 0;
	queue_task_a.count = 0;

	for (i = 0; i < MAX_EVENTS; i++)
		queue_task_a.queue[i] = EVENT_UNDEFINED;
}

void put_event_task_elevator(task_elevator_ev_t event)
{
	queue_task_a.count++;
	queue_task_a.queue[queue_task_a.head++] = event;

	if (MAX_EVENTS == queue_task_a.head)
		queue_task_a.head = 0;
}

task_elevator_ev_t get_event_task_elevator(void)

{
	task_elevator_ev_t event;

	queue_task_a.count--;
	event = queue_task_a.queue[queue_task_a.tail];
	queue_task_a.queue[queue_task_a.tail++] = EVENT_UNDEFINED;

	if (MAX_EVENTS == queue_task_a.tail)
		queue_task_a.tail = 0;

	return event;
}

bool any_event_task_elevator(void)
{
  return (queue_task_a.head != queue_task_a.tail);
}




/*New Functions*/

/*returns true if the floor is in the solicited_floor array of self*/
bool is_solicited_floor (task_elevator_dta_t* self, int floor)
{
	unsigned int i= 0;
	bool sentinel = false;
	for(i=0, sentinel = false; i< self->qty_floor && !sentinel ; i++)
	{
		if(self->solicited_floor[i].id_floor == floor && self->solicited_floor[i].solicited )
		{ /*if the floor is in the array and is solicited*/
			sentinel = true;
		}
	}
	return sentinel;
}


/*returns the index of the floor in the solicited_floor array in self*/
unsigned int search_in_solicited_floor_array (task_elevator_dta_t* self, int floor){
	unsigned int index= 0;
	bool sentinel = false;
	for(index=0, sentinel = false; index< self->qty_floor && !sentinel ; index++)
	{
			if(self->solicited_floor[index].id_floor == floor){ /*if index is the position of the floor*/
				sentinel = true;
			}
	}
	return index-1; /*returns the correct index of the solicited_floor*/
}




/*This function sets the current_floor to the given value*/

int get_current_floor(task_elevator_dta_t* self)
{
	return self->current_floor;
}

void set_current_floor (task_elevator_dta_t* self, int current_floor)
{
	self-> current_floor = current_floor;
}

unsigned int get_qty_floor (task_elevator_dta_t* self){
	return self->qty_floor;
}

void set_qty_floor (task_elevator_dta_t* self, unsigned int qty_floor){
	self->qty_floor = qty_floor;
}

void set_flag_menu (task_elevator_dta_t* self, bool flag){
	self->flag = flag;
}
bool get_flag_menu (task_elevator_dta_t* self){
	return self->flag;
}

void elevator_create_solicited_floor_array(task_elevator_dta_t* self){
	floor_t* aux;
	unsigned int i = 0;
	unsigned int j = 0;
	if(self && self->qty_floor){ /*checks if the qty_floor is positive and if self is not a null pointer*/
		if((aux=malloc(sizeof(floor_t)*self->qty_floor))){ /*aux its an array of floor_t with size qty_floor*/
			/*initialize each floor in the array*/
			for(i=0,j = self->min_floor ;i<self->qty_floor && j< (self->min_floor+self->qty_floor); i++, j++){
					aux[i].id_floor = j; /*sets the id_floor from min_floor to min_floor+qty_floor*/
					aux[i].solicited=false; /*initialize the floor as not solicited*/
				}
			free(self->solicited_floor); /*free the previous array*/
			self->solicited_floor = aux; /*assign the new address to the pointer*/
			}
	}
}

/*This function sets the solicited_floor as solicited*/
void put_solicited_floor (task_elevator_dta_t* self, int solicited_floor){
	unsigned int index = 0;
	if(self && self->solicited_floor) /*if the elevator is initialized*/
	{
		index = search_in_solicited_floor_array(self,solicited_floor); /*gets the index of the floor in the solicited_floor array*/
		self->solicited_floor[index].solicited=true; /*sets the new floor as solicited*/
	}

}

/*This function sets the floor as not solicited*/
void eliminate_solicited_floor (task_elevator_dta_t* self, int floor)
{
	unsigned int index = 0;
	if(self && self->solicited_floor) /*if the elevator is initialized*/
	{
		index = search_in_solicited_floor_array(self, floor); /*gets the index of the floor in the solicited_floor array*/
		self->solicited_floor[index].solicited = false; /*sets the floor as not solicited*/
	}
}

/********************** end of file ******************************************/
