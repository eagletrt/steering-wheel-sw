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

#include <stdbool.h>
#include "inputs-api.h"
#include "inputs-shared.h"

/* USER CODE END 0 */

/*----------------------------------------------------------------------------*/
/* Configure GPIO                                                             */
/*----------------------------------------------------------------------------*/
/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

/** Configure pins
*/
void MX_GPIO_Init(void) {

    GPIO_InitTypeDef GPIO_InitStruct = { 0 };

    /* GPIO Ports Clock Enable */
    __HAL_RCC_GPIOE_CLK_ENABLE();
    __HAL_RCC_GPIOI_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOH_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOD_CLK_ENABLE();
    __HAL_RCC_GPIOG_CLK_ENABLE();

    /*Configure GPIO pin : ENC_FL_SW_Pin */
    GPIO_InitStruct.Pin = ENC_FL_SW_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(ENC_FL_SW_GPIO_Port, &GPIO_InitStruct);

    /*Configure GPIO pins : ENC_SL_SW_Pin BUTT_BR1_Pin ENC_FR_SW_Pin ENC_SR_SW_Pin */
    GPIO_InitStruct.Pin = ENC_SL_SW_Pin | BUTT_BR1_Pin | ENC_FR_SW_Pin | ENC_SR_SW_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOI, &GPIO_InitStruct);

    /*Configure GPIO pin : BUTT_BR2_Pin */
    GPIO_InitStruct.Pin = BUTT_BR2_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(BUTT_BR2_GPIO_Port, &GPIO_InitStruct);

    /*Configure GPIO pin : BUTT_TS_Pin */
    GPIO_InitStruct.Pin = BUTT_TS_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(BUTT_TS_GPIO_Port, &GPIO_InitStruct);

    /*Configure GPIO pin : BUTT_BL2_Pin */
    GPIO_InitStruct.Pin = BUTT_BL2_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(BUTT_BL2_GPIO_Port, &GPIO_InitStruct);

    /*Configure GPIO pins : BUTT_BL1_Pin BUTT_FR3_Pin BUTT_FR2_Pin */
    GPIO_InitStruct.Pin = BUTT_BL1_Pin | BUTT_FR3_Pin | BUTT_FR2_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

    /*Configure GPIO pins : BUTT_FR1_Pin BUTT_FL3_Pin BUTT_FL2_Pin BUTT_FL1_Pin */
    GPIO_InitStruct.Pin = BUTT_FR1_Pin | BUTT_FL3_Pin | BUTT_FL2_Pin | BUTT_FL1_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

    /* EXTI interrupt init*/
    HAL_NVIC_SetPriority(EXTI0_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(EXTI0_IRQn);

    HAL_NVIC_SetPriority(EXTI1_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(EXTI1_IRQn);

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

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
    enum InputsSharedButtonID button_id = 0;
    bool button_state = false;
    uint32_t timestamp = HAL_GetTick();
    switch (GPIO_Pin) {
        case ENC_FL_SW_Pin:
            button_id = INPUTS_SHARED_BUTTON_ID_KNOB_PUSH_FRONT_LEFT;
            button_state = (bool)HAL_GPIO_ReadPin(ENC_FL_SW_GPIO_Port, ENC_FL_SW_Pin);
            break;

        case ENC_SL_SW_Pin:
            button_id = INPUTS_SHARED_BUTTON_ID_KNOB_PUSH_SIDE_LEFT;
            button_state = (bool)HAL_GPIO_ReadPin(ENC_SL_SW_GPIO_Port, ENC_SL_SW_Pin);
            break;

        case BUTT_BR2_Pin:
            button_id = INPUTS_SHARED_BUTTON_ID_PADDLE_BOTTOM_RIGHT;
            button_state = (bool)HAL_GPIO_ReadPin(BUTT_BR2_GPIO_Port, BUTT_BR2_Pin);
            break;

        case BUTT_TS_Pin:
            button_id = INPUTS_SHARED_BUTTON_ID_TS_ON;
            button_state = (bool)HAL_GPIO_ReadPin(BUTT_TS_GPIO_Port, BUTT_TS_Pin);
            break;

        case BUTT_BR1_Pin:
            button_id = INPUTS_SHARED_BUTTON_ID_PADDLE_TOP_RIGHT;
            button_state = (bool)HAL_GPIO_ReadPin(BUTT_BR1_GPIO_Port, BUTT_BR1_Pin);
            break;

        case BUTT_BL2_Pin:
            button_id = INPUTS_SHARED_BUTTON_ID_PADDLE_BOTTOM_LEFT;
            button_state = (bool)HAL_GPIO_ReadPin(BUTT_BL2_GPIO_Port, BUTT_BL2_Pin);
            break;

        case BUTT_BL1_Pin:
            button_id = INPUTS_SHARED_BUTTON_ID_PADDLE_TOP_LEFT;
            button_state = (bool)HAL_GPIO_ReadPin(BUTT_BL1_GPIO_Port, BUTT_BL1_Pin);
            break;

        case BUTT_FR3_Pin:
            button_id = INPUTS_SHARED_BUTTON_ID_BOTTOM_RIGHT;
            button_state = (bool)HAL_GPIO_ReadPin(BUTT_FR3_GPIO_Port, BUTT_FR3_Pin);
            break;

        case BUTT_FR2_Pin:
            button_id = INPUTS_SHARED_BUTTON_ID_TOP_RIGHT_2;
            button_state = (bool)HAL_GPIO_ReadPin(BUTT_FR2_GPIO_Port, BUTT_FR2_Pin);
            break;

        case BUTT_FR1_Pin:
            button_id = INPUTS_SHARED_BUTTON_ID_TOP_RIGHT_1;
            button_state = (bool)HAL_GPIO_ReadPin(BUTT_FR1_GPIO_Port, BUTT_FR1_Pin);
            break;

        case BUTT_FL3_Pin:
            button_id = INPUTS_SHARED_BUTTON_ID_BOTTOM_LEFT;
            button_state = (bool)HAL_GPIO_ReadPin(BUTT_FL3_GPIO_Port, BUTT_FL3_Pin);
            break;

        case BUTT_FL2_Pin:
            button_id = INPUTS_SHARED_BUTTON_ID_TOP_LEFT_2;
            button_state = (bool)HAL_GPIO_ReadPin(BUTT_FL2_GPIO_Port, BUTT_FL2_Pin);
            break;

        case BUTT_FL1_Pin:
            button_id = INPUTS_SHARED_BUTTON_ID_TOP_LEFT_1;
            button_state = (bool)HAL_GPIO_ReadPin(BUTT_FL1_GPIO_Port, BUTT_FL1_Pin);
            break;

        case ENC_FR_SW_Pin:
            button_id = INPUTS_SHARED_BUTTON_ID_KNOB_PUSH_FRONT_RIGHT;
            button_state = (bool)HAL_GPIO_ReadPin(ENC_FR_SW_GPIO_Port, ENC_FR_SW_Pin);
            break;

        case ENC_SR_SW_Pin:
            button_id = INPUTS_SHARED_BUTTON_ID_KNOB_PUSH_SIDE_RIGHT;
            button_state = (bool)HAL_GPIO_ReadPin(ENC_SR_SW_GPIO_Port, ENC_SR_SW_Pin);
            break;
    }

    inputs_api_update_button(button_id, button_state, timestamp);
}

/* USER CODE END 2 */
