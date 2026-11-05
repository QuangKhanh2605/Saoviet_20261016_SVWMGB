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
#include "stm32l4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "user_util.h"
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
#define PULSE_ON_OFF_Pin GPIO_PIN_0
#define PULSE_ON_OFF_GPIO_Port GPIOA
#define PULSE_DET_Pin GPIO_PIN_1
#define PULSE_DET_GPIO_Port GPIOA
#define PULSE_DET_EXTI_IRQn EXTI1_IRQn
#define DEBUG_TX_Pin GPIO_PIN_2
#define DEBUG_TX_GPIO_Port GPIOA
#define DEBUG_RX_Pin GPIO_PIN_3
#define DEBUG_RX_GPIO_Port GPIOA
#define SIM_PWR_KEY_Pin GPIO_PIN_4
#define SIM_PWR_KEY_GPIO_Port GPIOA
#define SIM_RESET_Pin GPIO_PIN_5
#define SIM_RESET_GPIO_Port GPIOA
#define V_PIN_Pin GPIO_PIN_6
#define V_PIN_GPIO_Port GPIOA
#define PRESSURE_IN1_Pin GPIO_PIN_7
#define PRESSURE_IN1_GPIO_Port GPIOA
#define SIM_TX_Pin GPIO_PIN_4
#define SIM_TX_GPIO_Port GPIOC
#define SIM_RX_Pin GPIO_PIN_5
#define SIM_RX_GPIO_Port GPIOC
#define SIM_DTR_Pin GPIO_PIN_0
#define SIM_DTR_GPIO_Port GPIOB
#define SIM_RI_Pin GPIO_PIN_1
#define SIM_RI_GPIO_Port GPIOB
#define SIM_ON_OFF_Pin GPIO_PIN_11
#define SIM_ON_OFF_GPIO_Port GPIOB
#define GPS_ON_OFF_Pin GPIO_PIN_12
#define GPS_ON_OFF_GPIO_Port GPIOB
#define EFLASH_CS_Pin GPIO_PIN_6
#define EFLASH_CS_GPIO_Port GPIOC
#define PULSE_LED_Pin GPIO_PIN_7
#define PULSE_LED_GPIO_Port GPIOC
#define EFLASH_ON_OFF_Pin GPIO_PIN_9
#define EFLASH_ON_OFF_GPIO_Port GPIOC
#define RS485_ON_OFF_Pin GPIO_PIN_8
#define RS485_ON_OFF_GPIO_Port GPIOA
#define RS485_TX_Pin GPIO_PIN_9
#define RS485_TX_GPIO_Port GPIOA
#define RS485_RX_Pin GPIO_PIN_10
#define RS485_RX_GPIO_Port GPIOA
#define EFLASH_HOLD_Pin GPIO_PIN_11
#define EFLASH_HOLD_GPIO_Port GPIOA
#define LED_Pin GPIO_PIN_12
#define LED_GPIO_Port GPIOA
#define RS485_TXDE_Pin GPIO_PIN_15
#define RS485_TXDE_GPIO_Port GPIOA
#define BOOST_ON_OFF_Pin GPIO_PIN_10
#define BOOST_ON_OFF_GPIO_Port GPIOC
#define Magnetic_switch_Pin GPIO_PIN_2
#define Magnetic_switch_GPIO_Port GPIOD
#define Magnetic_switch_EXTI_IRQn EXTI2_IRQn
#define PULSE_DIR_Pin GPIO_PIN_4
#define PULSE_DIR_GPIO_Port GPIOB
#define PULSE_DIR_EXTI_IRQn EXTI4_IRQn
#define PULSE_DTC_Pin GPIO_PIN_6
#define PULSE_DTC_GPIO_Port GPIOB
#define PIN_ON_OFF_Pin GPIO_PIN_9
#define PIN_ON_OFF_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
