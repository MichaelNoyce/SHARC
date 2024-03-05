/**
 * @file ICM20649_init.h
 * @author Michael Noyce
 * @brief 
 * @version 0.1
 * @date 2023-12-07
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"

//============================= 0. Handlers ===============================================
extern I2C_HandleTypeDef hi2c1;

extern DMA_HandleTypeDef hdma_i2c1_rx;

extern DMA_HandleTypeDef hdma_i2c1_tx;

//======================== 1. Function Prototypes ===========================================

void MX_DMA_Init(void);

void MX_I2C1_Init(void);

void MX_GPIO_Init_IMU(void);

void IMU_Interface_Init(void);


#ifdef __cplusplus
}
#endif