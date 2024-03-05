/**
  ******************************************************************************
  * @file    PWR/PWR_ModesSelection/Src/standby_rtc_sram2_test.c
  * @author  MCD Application Team
  * @brief   This sample code shows how to use STM32L4xx PWR HAL API to enter
  *          power modes.
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

  
  void test_standby_rtc_sram2(void)
 {
  printf("\n\r Executing test (STANDBY + RTC + SRAM2) \n\r");
  printf(" Please measure current then use Reset button to select another test \n\r");

  /* set RTC clock */
  /*     we do not set the interrupt on RTC clock since we do not want to wakeup from STANDBY*/
  /*     we want to stay in STANDBY mode with RTC clock activated for power conso */
  RTC_Config();
  
  /* Set all GPIO in analog state to reduce power consumption */
  GPIO_AnalogState_Config();
  
  /* Enable Power Clock */
  __HAL_RCC_PWR_CLK_ENABLE();
  
  /* Request to enter STANDBY mode with SRAM2 active */
  HAL_PWREx_EnableSRAM2ContentRetention();
  HAL_PWR_EnterSTANDBYMode();
  
}


/**
  * @}
  */

/**
  * @}
  */

