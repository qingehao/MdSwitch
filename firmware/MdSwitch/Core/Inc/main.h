/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
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
#include "stm32g0xx_hal.h"

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
#define SWD_SW_A0_Pin GPIO_PIN_14
#define SWD_SW_A0_GPIO_Port GPIOC
#define SWD_SW_A1_Pin GPIO_PIN_15
#define SWD_SW_A1_GPIO_Port GPIOC
#define UART_SW_A0_Pin GPIO_PIN_0
#define UART_SW_A0_GPIO_Port GPIOA
#define UART_SW_A1_Pin GPIO_PIN_1
#define UART_SW_A1_GPIO_Port GPIOA
#define LED_SWD1_Pin GPIO_PIN_2
#define LED_SWD1_GPIO_Port GPIOA
#define LED_SWD2_Pin GPIO_PIN_3
#define LED_SWD2_GPIO_Port GPIOA
#define LED_SWD3_Pin GPIO_PIN_4
#define LED_SWD3_GPIO_Port GPIOA
#define SWD_SW_Pin GPIO_PIN_5
#define SWD_SW_GPIO_Port GPIOA
#define LED_UART3_Pin GPIO_PIN_6
#define LED_UART3_GPIO_Port GPIOA
#define LED_UART2_Pin GPIO_PIN_7
#define LED_UART2_GPIO_Port GPIOA
#define LED_UART1_Pin GPIO_PIN_0
#define LED_UART1_GPIO_Port GPIOB
#define UART_SW_Pin GPIO_PIN_1
#define UART_SW_GPIO_Port GPIOB
#define JVREF_EN_Pin GPIO_PIN_8
#define JVREF_EN_GPIO_Port GPIOA
#define JVREF_SEL_Pin GPIO_PIN_6
#define JVREF_SEL_GPIO_Port GPIOC
#define LED_JVREF_1V8_Pin GPIO_PIN_15
#define LED_JVREF_1V8_GPIO_Port GPIOA
#define LED_JVREF_3V3_Pin GPIO_PIN_3
#define LED_JVREF_3V3_GPIO_Port GPIOB
#define JVREF_SW_Pin GPIO_PIN_5
#define JVREF_SW_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
