/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    tim.h
  * @brief   This file contains all the function prototypes for
  *          the tim.c file
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
#ifndef __TIM_H__
#define __TIM_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */
#include "leds.h"

/* USER CODE END Includes */

extern TIM_HandleTypeDef htim1;

extern TIM_HandleTypeDef htim2;

extern TIM_HandleTypeDef htim3;

extern TIM_HandleTypeDef htim4;

extern TIM_HandleTypeDef htim5;

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

void MX_TIM1_Init(void);
void MX_TIM2_Init(void);
void MX_TIM3_Init(void);
void MX_TIM4_Init(void);
void MX_TIM5_Init(void);

void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);

/* USER CODE BEGIN Prototypes */

/*!
 * \brief Transmits a buffer of duty cicles to the WS2812B LEDs using PWM on TIM3.
 *
 * \param buffer A pointer to an array of duty cycles representing the data to be transmitted to the WS2812B LEDs. Each duty cycle corresponds to a specific color and brightness for the LEDs.
 * \param size The number of duty cycles in the buffer.
 *
 * \retval WS2812B_RC_OK if the transmission was successful.
 * \retval WS2812B_RC_NULL_POINTER if the buffer pointer is NULL.
 * \retval WS2812B_RC_TRANSMISSION_ERROR if there was an error during transmission.
 * \retval WS2812B_RC_BUSY if the LEDs are currently busy with another transmission.
 */
enum LedsReturnCode tim_leds_transmit(const enum WS2812BDutyCycle *buffer, uint16_t size);

/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif

#endif /* __TIM_H__ */
