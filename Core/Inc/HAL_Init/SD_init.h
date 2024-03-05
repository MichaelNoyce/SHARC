/**
 * @file SD_init.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2023-12-01
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"
#include "fatfs.h"

//============================= 1. Defines ===============================================

#define SD_Detect_Pin GPIO_PIN_3
#define SD_Detect_GPIO_Port GPIOF


 //======================== 2. Static Functions Prototypes ===========================================
HAL_StatusTypeDef SD_Init(void);
void MX_GPIO_Init_SD(void);
void MX_SDMMC1_SD_Init(void);
void MX_CRC_Init(void);
void MX_RTC_Init(void);
void MX_FATFS_Init();

#ifdef __cplusplus
}
#endif
