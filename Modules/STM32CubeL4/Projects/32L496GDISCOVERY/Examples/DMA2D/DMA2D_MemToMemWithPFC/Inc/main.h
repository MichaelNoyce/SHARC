/**
  ******************************************************************************
  * @file    DMA2D/DMA2D_MemToMemWithPFC/Inc/main.h 
  * @author  MCD Application Team
  * @brief   Header for main.c module
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
  
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

/* Includes ------------------------------------------------------------------*/
#include "stm32l496g_discovery.h"
#include "stm32l496g_discovery_lcd.h"

#define LAYER_SIZE_X 			240
#define LAYER_SIZE_Y			160
#define ORIGINAL_LAYER_BYTE_PER_PIXEL	4 /* Corresponds to format ARGB8888 */
#define FINAL_LAYER_BYTE_PER_PIXEL	  2 /* Corresponds to format RGB565   */


/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

#endif /* __MAIN_H */

