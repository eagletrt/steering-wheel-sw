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
#include "stm32h7xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include "inputs-shared.h"

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

/*!
 * \brief Push an input event to the IPC input queue.
 *
 * \param ev Input event to push.
 *
 * \retval INPUTS_RC_OK if the event was successfully pushed.
 * \retval INPUTS_RC_ERROR if the IPC input queue is full.
 * \retval INPUTS_RC_NOTIFY_ERROR if the HSEM is already taken.
 */
enum InputsReturnCode ipc_input_push(struct InputEvent ev);

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define ENC_FL_CLICK_Pin GPIO_PIN_2
#define ENC_FL_CLICK_GPIO_Port GPIOE
#define ENC_SL_CLICK_Pin GPIO_PIN_8
#define ENC_SL_CLICK_GPIO_Port GPIOI
#define BUTT_BR2_INT_Pin GPIO_PIN_1
#define BUTT_BR2_INT_GPIO_Port GPIOC
#define ENC_SR_A_Pin GPIO_PIN_0
#define ENC_SR_A_GPIO_Port GPIOA
#define ENC_FL_B_Pin GPIO_PIN_1
#define ENC_FL_B_GPIO_Port GPIOA
#define ENC_FL_A_Pin GPIO_PIN_10
#define ENC_FL_A_GPIO_Port GPIOH
#define BUTT_TS_INT_Pin GPIO_PIN_11
#define BUTT_TS_INT_GPIO_Port GPIOH
#define ENC_FR_B_Pin GPIO_PIN_12
#define ENC_FR_B_GPIO_Port GPIOD
#define ENC_FR_A_Pin GPIO_PIN_13
#define ENC_FR_A_GPIO_Port GPIOD
#define PWM_LED_STRIP_Pin GPIO_PIN_6
#define PWM_LED_STRIP_GPIO_Port GPIOC
#define ENC_SL_B_Pin GPIO_PIN_8
#define ENC_SL_B_GPIO_Port GPIOA
#define ENC_SL_A_Pin GPIO_PIN_9
#define ENC_SL_A_GPIO_Port GPIOA
#define BUTT_BR1_INT_Pin GPIO_PIN_0
#define BUTT_BR1_INT_GPIO_Port GPIOI
#define BUTT_BL2_INT_Pin GPIO_PIN_15
#define BUTT_BL2_INT_GPIO_Port GPIOA
#define BUTT_BL1_INT_Pin GPIO_PIN_3
#define BUTT_BL1_INT_GPIO_Port GPIOD
#define BUTT_FR3_INT_Pin GPIO_PIN_4
#define BUTT_FR3_INT_GPIO_Port GPIOD
#define BUTT_FR2_INT_Pin GPIO_PIN_7
#define BUTT_FR2_INT_GPIO_Port GPIOD
#define BUTT_FR1_INT_Pin GPIO_PIN_9
#define BUTT_FR1_INT_GPIO_Port GPIOG
#define BUTT_FL3_INT_Pin GPIO_PIN_10
#define BUTT_FL3_INT_GPIO_Port GPIOG
#define BUTT_FL2_INT_Pin GPIO_PIN_13
#define BUTT_FL2_INT_GPIO_Port GPIOG
#define BUTT_FL1_INT_Pin GPIO_PIN_14
#define BUTT_FL1_INT_GPIO_Port GPIOG
#define ENC_SR_B_Pin GPIO_PIN_3
#define ENC_SR_B_GPIO_Port GPIOB
#define ENC_FR_CLICK_Pin GPIO_PIN_5
#define ENC_FR_CLICK_GPIO_Port GPIOI
#define ENC_SR_CLUCK_Pin GPIO_PIN_6
#define ENC_SR_CLUCK_GPIO_Port GPIOI

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
