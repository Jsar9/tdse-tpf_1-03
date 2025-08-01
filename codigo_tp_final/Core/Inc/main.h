/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define B1_Pin GPIO_PIN_13
#define B1_GPIO_Port GPIOC
#define B1_EXTI_IRQn EXTI15_10_IRQn
#define USART_TX_Pin GPIO_PIN_2
#define USART_TX_GPIO_Port GPIOA
#define USART_RX_Pin GPIO_PIN_3
#define USART_RX_GPIO_Port GPIOA
#define LD2_Pin GPIO_PIN_5
#define LD2_GPIO_Port GPIOA
#define D12_Pin GPIO_PIN_6
#define D12_GPIO_Port GPIOA
#define D11_Pin GPIO_PIN_7
#define D11_GPIO_Port GPIOA
#define D6_Pin GPIO_PIN_10
#define D6_GPIO_Port GPIOB
#define BTN_SETUP_Pin GPIO_PIN_12
#define BTN_SETUP_GPIO_Port GPIOB
#define LED_WARNING_Pin GPIO_PIN_13
#define LED_WARNING_GPIO_Port GPIOB
#define BTN_F3_Pin GPIO_PIN_14
#define BTN_F3_GPIO_Port GPIOB
#define BTN_F2_Pin GPIO_PIN_15
#define BTN_F2_GPIO_Port GPIOB
#define BTN_F1_Pin GPIO_PIN_6
#define BTN_F1_GPIO_Port GPIOC
#define D9_Pin GPIO_PIN_7
#define D9_GPIO_Port GPIOC
#define BTN_DOOR_Pin GPIO_PIN_8
#define BTN_DOOR_GPIO_Port GPIOC
#define BTN_PERSON_Pin GPIO_PIN_9
#define BTN_PERSON_GPIO_Port GPIOC
#define D7_Pin GPIO_PIN_8
#define D7_GPIO_Port GPIOA
#define D8_Pin GPIO_PIN_9
#define D8_GPIO_Port GPIOA
#define D2_Pin GPIO_PIN_10
#define D2_GPIO_Port GPIOA
#define LED_DOWN_Pin GPIO_PIN_11
#define LED_DOWN_GPIO_Port GPIOA
#define LED_UP_Pin GPIO_PIN_12
#define LED_UP_GPIO_Port GPIOA
#define TMS_Pin GPIO_PIN_13
#define TMS_GPIO_Port GPIOA
#define TCK_Pin GPIO_PIN_14
#define TCK_GPIO_Port GPIOA
#define BTN_CALL_F3_Pin GPIO_PIN_10
#define BTN_CALL_F3_GPIO_Port GPIOC
#define BTN_CALL_F2_Pin GPIO_PIN_11
#define BTN_CALL_F2_GPIO_Port GPIOC
#define BTN_CALL_F1_Pin GPIO_PIN_12
#define BTN_CALL_F1_GPIO_Port GPIOC
#define SWO_Pin GPIO_PIN_3
#define SWO_GPIO_Port GPIOB
#define D5_Pin GPIO_PIN_4
#define D5_GPIO_Port GPIOB
#define D4_Pin GPIO_PIN_5
#define D4_GPIO_Port GPIOB
#define D10_Pin GPIO_PIN_6
#define D10_GPIO_Port GPIOB
#define BTN_CLOSE_DOOR_Pin GPIO_PIN_8
#define BTN_CLOSE_DOOR_GPIO_Port GPIOB
#define BTN_OPEN_DOOR_Pin GPIO_PIN_9
#define BTN_OPEN_DOOR_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
