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
#include "stm32h7xx_hal.h"

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
#define LED_Pin GPIO_PIN_3
#define LED_GPIO_Port GPIOE
#define W5300_BRDY1_Pin GPIO_PIN_13
#define W5300_BRDY1_GPIO_Port GPIOC
#define W5300_BRDY2_Pin GPIO_PIN_6
#define W5300_BRDY2_GPIO_Port GPIOF
#define W5300_BRDY3_Pin GPIO_PIN_7
#define W5300_BRDY3_GPIO_Port GPIOF
#define PA0_LED_Pin GPIO_PIN_0
#define PA0_LED_GPIO_Port GPIOA
#define SD_CD_Pin GPIO_PIN_6
#define SD_CD_GPIO_Port GPIOA
#define PC4_IO_Pin GPIO_PIN_4
#define PC4_IO_GPIO_Port GPIOC
#define PC5_IO_Pin GPIO_PIN_5
#define PC5_IO_GPIO_Port GPIOC
#define PB0_IO_Pin GPIO_PIN_0
#define PB0_IO_GPIO_Port GPIOB
#define PB1_IO_Pin GPIO_PIN_1
#define PB1_IO_GPIO_Port GPIOB
#define PB2_IO_Pin GPIO_PIN_2
#define PB2_IO_GPIO_Port GPIOB
#define PF11_IO_Pin GPIO_PIN_11
#define PF11_IO_GPIO_Port GPIOF
#define PG0_IO_Pin GPIO_PIN_0
#define PG0_IO_GPIO_Port GPIOG
#define PG1_IO_Pin GPIO_PIN_1
#define PG1_IO_GPIO_Port GPIOG
#define I2S1_SD_MODE_Pin GPIO_PIN_11
#define I2S1_SD_MODE_GPIO_Port GPIOD
#define PG2_BTN2_Pin GPIO_PIN_2
#define PG2_BTN2_GPIO_Port GPIOG
#define PG3_BTN1_Pin GPIO_PIN_3
#define PG3_BTN1_GPIO_Port GPIOG
#define PG4_BTN0_Pin GPIO_PIN_4
#define PG4_BTN0_GPIO_Port GPIOG
#define W5300_BRDY0_Pin GPIO_PIN_9
#define W5300_BRDY0_GPIO_Port GPIOB
#define W530_NINT_Pin GPIO_PIN_0
#define W530_NINT_GPIO_Port GPIOE
#define W530_NRES_Pin GPIO_PIN_1
#define W530_NRES_GPIO_Port GPIOE

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
