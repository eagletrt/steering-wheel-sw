/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    gpio.c
  * @brief   This file provides code for the configuration
  *          of all used GPIO pins.
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

/* Includes ------------------------------------------------------------------*/
#include "gpio.h"

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/*----------------------------------------------------------------------------*/
/* Configure GPIO                                                             */
/*----------------------------------------------------------------------------*/
/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

/** Configure pins
*/
void MX_GPIO_Init(void)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOI_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOG_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOH, M4_STATUS_Pin|PTT_ENABLER_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : ENC_BL_1_INT_Pin */
  GPIO_InitStruct.Pin = ENC_BL_1_INT_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(ENC_BL_1_INT_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : ENC_BL_8_Pin */
  GPIO_InitStruct.Pin = ENC_BL_8_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(ENC_BL_8_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : ENC_TL_1_INT_Pin BUTT_BR1_INT_Pin */
  GPIO_InitStruct.Pin = ENC_TL_1_INT_Pin|BUTT_BR1_INT_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOI, &GPIO_InitStruct);

  /*Configure GPIO pins : ENC_TL_2_Pin ENC_TL_4_Pin ENC_TL_8_Pin */
  GPIO_InitStruct.Pin = ENC_TL_2_Pin|ENC_TL_4_Pin|ENC_TL_8_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pin : ENC_TR_1_INT_Pin */
  GPIO_InitStruct.Pin = ENC_TR_1_INT_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(ENC_TR_1_INT_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : ENC_TR_2_Pin ENC_TR_4_Pin ENC_TR_8_Pin */
  GPIO_InitStruct.Pin = ENC_TR_2_Pin|ENC_TR_4_Pin|ENC_TR_8_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);

  /*Configure GPIO pin : BUTT_BR2_INT_Pin */
  GPIO_InitStruct.Pin = BUTT_BR2_INT_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(BUTT_BR2_INT_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : M4_STATUS_Pin PTT_ENABLER_Pin */
  GPIO_InitStruct.Pin = M4_STATUS_Pin|PTT_ENABLER_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOH, &GPIO_InitStruct);

  /*Configure GPIO pin : BUTT_TS_INT_Pin */
  GPIO_InitStruct.Pin = BUTT_TS_INT_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(BUTT_TS_INT_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : BUTT_BL2_INT_Pin */
  GPIO_InitStruct.Pin = BUTT_BL2_INT_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(BUTT_BL2_INT_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : BUTT_BL1_INT_Pin BUTT_FR3_INT_Pin BUTT_FR2_INT_Pin */
  GPIO_InitStruct.Pin = BUTT_BL1_INT_Pin|BUTT_FR3_INT_Pin|BUTT_FR2_INT_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /*Configure GPIO pins : BUTT_FR1_INT_Pin BUTT_FL3_INT_Pin BUTT_FL2_INT_Pin BUTT_FL1_INT_Pin */
  GPIO_InitStruct.Pin = BUTT_FR1_INT_Pin|BUTT_FL3_INT_Pin|BUTT_FL2_INT_Pin|BUTT_FL1_INT_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

  /*Configure GPIO pins : ENC_BR_4_Pin ENC_BR_2_Pin */
  GPIO_InitStruct.Pin = ENC_BR_4_Pin|ENC_BR_2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : ENC_BR1_INT_Pin */
  GPIO_InitStruct.Pin = ENC_BR1_INT_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(ENC_BR1_INT_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : ENC_BR_8_Pin ENC_BL_4_Pin ENC_BL_2_Pin */
  GPIO_InitStruct.Pin = ENC_BR_8_Pin|ENC_BL_4_Pin|ENC_BL_2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOI, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI0_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI0_IRQn);

  HAL_NVIC_SetPriority(EXTI1_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI1_IRQn);

  HAL_NVIC_SetPriority(EXTI2_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI2_IRQn);

  HAL_NVIC_SetPriority(EXTI3_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI3_IRQn);

  HAL_NVIC_SetPriority(EXTI4_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI4_IRQn);

  HAL_NVIC_SetPriority(EXTI9_5_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);

  HAL_NVIC_SetPriority(EXTI15_10_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

}

/* USER CODE BEGIN 2 */

/* USER CODE END 2 */
