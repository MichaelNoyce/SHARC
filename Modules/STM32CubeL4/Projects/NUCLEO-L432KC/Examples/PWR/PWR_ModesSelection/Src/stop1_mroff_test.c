/**
  ******************************************************************************
  * @file    PWR/PWR_ModesSelection/Src/stop1_mroff_test.c
  * @author  MCD Application Team
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and
  *          peripherals interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2017 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include <stdio.h>

/** @addtogroup STM32L4xx_HAL_Examples
  * @{
  */

/** @addtogroup PWR_ModesSelection
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

void test_stop1_mroff(void)
{
  printf("\n\r Executing test (STOP1 + MR OFF) \n\r");
  printf(" Please measure current then use Reset button to select another test \n\r");

  /* Set all GPIO in analog state to reduce power consumption */
  GPIO_AnalogState_Config();
  
  /* Enable Power Clock */
  __HAL_RCC_PWR_CLK_ENABLE();
  
  /* Enter STOP 1 mode (Main Regulator OFF and Low Power Regulator On) */
  HAL_PWREx_EnterSTOP1Mode(PWR_STOPENTRY_WFI);
  
}


/**
  * @}
  */

/**
  * @}
  */

