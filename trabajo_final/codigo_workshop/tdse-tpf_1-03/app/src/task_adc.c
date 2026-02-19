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
 * @file   : task_adc.c
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

/********************** macros and definitions *******************************/

// the number of samples that will be used for mean calculations
#define NUM_SAMPLES 100

// the time interval in milliseconds used to update the current_temp value
#define UPDATE_INTERVAL_MS 5000

// the percentage of data that will be discarded, avoiding outliers
#define DISCARD_PERCENTAGE 10


// initial value for adc tick counter
#define G_TASK_ADC_TICK_CNT_INI		0ul


/********************** internal data declaration ****************************/


/********************** internal functions declaration ***********************/
HAL_StatusTypeDef ADC_Poll_Read(uint16_t *value);

//this function sorts from min to max a uint16_t array
void sort_array (uint16_t* array, uint16_t size);

/********************** internal data definition *****************************/
const char *p_task_adc 		= "Task ADC";

/********************** external data declaration *****************************/

extern ADC_HandleTypeDef hadc1;

volatile uint32_t g_task_adc_tick_cnt;

/********************** external functions definition ************************/
void task_adc_init(void *parameters)
{
	shared_temperature_dta_t *p_shared_temperature_dta = (shared_temperature_dta_t *) parameters;

	/* Print out: Task Initialized */
	LOGGER_LOG("  %s is running - %s\r\n", GET_NAME(task_adc_init), p_task_adc);

	/*************** BEGIN HARDWARE CALIBRATION ***************/
	// Stops ADC
	HAL_ADC_Stop(&hadc1);

	// Calibrate ADC
	if (HAL_ADCEx_Calibration_Start(&hadc1) != HAL_OK)
	{
	    LOGGER_LOG("Error ADC calibration\r\n");
	}
	/*************** END HARDWARE CALIBRATION ***************/

	p_shared_temperature_dta->adc_end_of_conversion = false;

	//initialize the adc ticks
	g_task_adc_tick_cnt = G_TASK_ADC_TICK_CNT_INI;

}

void task_adc_update(void *parameters)
{

	//Initialize the pointer to shared_temperature_dta
	shared_temperature_dta_t * p_shared_temperature_dta = (shared_temperature_dta_t *) parameters;

	// local variables
	uint16_t adc_samples[NUM_SAMPLES];
	uint16_t sample_val;

	// It's a secondary flag for adc conversion
	bool error_flag = false;

	// calculate the amount of samples to discard
	uint16_t discard_count = (NUM_SAMPLES * DISCARD_PERCENTAGE) / 100;

	// calculate the number of valid samples, considering the discarded values
	uint16_t valid_samples = NUM_SAMPLES - (2 * discard_count);

	// initialize the raw value
	uint32_t sum_raw = 0;

	// current amount ticks is bigger than update_interval
	if ( g_task_adc_tick_cnt >= UPDATE_INTERVAL_MS)
	{

		//reset the adc tick counter
		g_task_adc_tick_cnt = 0;

		// stores the adc raw values
		for (int i = 0; i < NUM_SAMPLES; i++)
		{

			if (HAL_OK == ADC_Poll_Read(&sample_val))
			{
				adc_samples[i] = sample_val;
			}

			else
			{
				LOGGER_LOG("error en lectura ADC\r\n");
				error_flag = true;
				break;
			}

			// waits 1ms to for sensor stabilization
			HAL_Delay(1);

		}

		// if there's no error in the adc value read
		if (!error_flag)
		{

			// sorts array
			sort_array(adc_samples, NUM_SAMPLES);

			// calculate the raw sum of the adc_samples
			for (int i = discard_count; i < (NUM_SAMPLES - discard_count); i++)
			{
				sum_raw += adc_samples[i];
			}

			//stores the mean of the raw samples in adc_read
			p_shared_temperature_dta->adc_read = (uint16_t)(sum_raw / valid_samples);

			p_shared_temperature_dta->adc_end_of_conversion = true;
		}

	}

}



//	Requests start of conversion, waits until conversion done
HAL_StatusTypeDef ADC_Poll_Read(uint16_t *value) {
	HAL_StatusTypeDef res;

	res=HAL_ADC_Start(&hadc1);
	if ( HAL_OK==res ) {
		res=HAL_ADC_PollForConversion(&hadc1, 0);
		if ( HAL_OK==res ) {
			*value = HAL_ADC_GetValue(&hadc1);
		}
	}
	return res;
}


void sort_array (uint16_t* arr, uint16_t arr_size) {

	//sorts the given array using insertion sort method

    for (uint16_t i = 1; i < arr_size; i++) {
        uint16_t key = arr[i];
        int16_t j = i - 1;

        while (j >= 0 && arr[j] > key) {
            arr[j + 1] = arr[j];
            j = j - 1;
        }
        arr[j + 1] = key;
    }
}


/********************** end of file ******************************************/
