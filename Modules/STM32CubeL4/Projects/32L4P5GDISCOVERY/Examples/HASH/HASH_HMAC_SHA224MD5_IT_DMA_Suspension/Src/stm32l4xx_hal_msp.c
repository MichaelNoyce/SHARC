/**
  ******************************************************************************
  * @file    HASH/HASH_HMAC_SHA224MD5_IT_DMA_Suspension/Src/stm32l4xx_hal_msp.c
  * @author  MCD Application Team
  * @brief   HAL MSP module.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2019 STMicroelectronics.
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

/** @addtogroup STM32L4xx_HAL_Examples
  * @{
  */

/** @defgroup HAL_MSP
  * @brief HAL MSP module.
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/** @defgroup HAL_MSP_Private_Functions
  * @{
  */

/**
  * @brief  Initializes the HASH MSP.
  *        This function configures the hardware resources used in this example:
  *           - HASH clock enable
  *           - DMA2 clock enable
  *           - USART1 clock enable    
  * @param  hhash: HASH handle pointer
  * @retval None
  */
void HAL_HASH_MspInit(HASH_HandleTypeDef *hhash)
{
  static DMA_HandleTypeDef  hdma_hash;

  /* Enable HASH clock */
  __HAL_RCC_HASH_CLK_ENABLE();
  
  
  /* Enable and set HASH Interrupt to the highest priority */
  HAL_NVIC_SetPriority(HASH_IRQn, NVIC_PRIORITYGROUP_4, 0);
  HAL_NVIC_EnableIRQ(HASH_IRQn);  
  
  /* Enable DMA clock */
  DMA_CLK_ENABLE();
  DMAMUX_CLK_ENABLE();
  
  /***************** Configure common DMA In parameters ***********************/
  hdma_hash.Init.Request             = DMA_REQUEST;
  hdma_hash.Init.Direction           = DMA_MEMORY_TO_PERIPH;
  hdma_hash.Init.PeriphInc           = DMA_PINC_DISABLE;
  hdma_hash.Init.MemInc              = DMA_MINC_ENABLE;
  hdma_hash.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
  hdma_hash.Init.MemDataAlignment    = DMA_MDATAALIGN_WORD;
  hdma_hash.Init.Mode                = DMA_NORMAL;
  hdma_hash.Init.Priority            = DMA_PRIORITY_HIGH;
  hdma_hash.Instance = DMA_CHANNEL;
  
  /* Associate the DMA handle */
  __HAL_LINKDMA(hhash, hdmain, hdma_hash);
  
  /* Deinitialize the Stream for new transfer */
  HAL_DMA_DeInit(hhash->hdmain);
  
  /* Configure the DMA Stream */
  HAL_DMA_Init(hhash->hdmain);    
  
  HAL_NVIC_SetPriority(HASH_DMA_IRQn, 0x0F, 0x0);
  HAL_NVIC_EnableIRQ(HASH_DMA_IRQn);    
  
  
  /* Enable USART clock */
  __USART1_CLK_ENABLE();
  
  /* Set USART1 priority higher than that of HASH_DMA_IRQn */
  HAL_NVIC_SetPriority(USART1_IRQn, 0x0E, 0x0);
  HAL_NVIC_EnableIRQ(USART1_IRQn);  
}

/**
  * @brief  De-Initializes the HASH MSP.
  * @param  hhash: HASH handle pointer
  * @retval None
  */
void HAL_HASH_MspDeInit(HASH_HandleTypeDef *hhash)
{
  /* Force the HASH Periheral Clock Reset */  
  __HAL_RCC_HASH_FORCE_RESET(); 
  
  /* Release the HASH Periheral Clock Reset */  
  __HAL_RCC_HASH_RELEASE_RESET();
  
  /* Disable HASH clock */
  __HAL_RCC_HASH_CLK_DISABLE();
}


/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

