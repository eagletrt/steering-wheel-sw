/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    dac.h
  * @brief   This file contains all the function prototypes for
  *          the dac.c file
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
#ifndef __DAC_H__
#define __DAC_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

extern DAC_HandleTypeDef hdac1;

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

void MX_DAC1_Init(void);

/* USER CODE BEGIN Prototypes */

/*!
 * \brief Set DAC value to 0 and starts it
 */
void backlight_init(void);

/*!
 * \brief Set DAC value to \c percent of the maximum value
 *
 * \param percent Value between 0.0 and 1.0
 */
void backlight_set_percent(float percent);

/*!
 * \brief Set DAC value to 0
 */
void backlight_off(void);

/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif

#endif /* __DAC_H__ */
