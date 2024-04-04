#include "main.hpp"

#ifdef __cplusplus 
extern "C" {
#endif
#include "fatfs.h"
#include "HAL_SD.h" 
#include "init.h"
#include "string.h"
#include <stdarg.h>
#include "stdio.h"
#include "ICM20649.h"
#include "WAVE.h"
#ifdef __cplusplus 
}
#endif

void SystemClock_Config(void);
void Error_Handler(void);


// Peripheral Handles
UART_HandleTypeDef hlpuart1;
UART_HandleTypeDef huart4;
DMA_HandleTypeDef hdma_uart4_rx;
DMA_HandleTypeDef hdma_uart4_tx;
TIM_HandleTypeDef htim1;
DMA_HandleTypeDef hdma_memtomem_dma1_channel1;
UART_HandleTypeDef huart2;
RTC_HandleTypeDef hrtc;
HAL_StatusTypeDef  UARTStatus;
SD_HandleTypeDef hsd1;

int main(void) {

     //======================== 0. Initialisation ==============================================================
	HAL_Init();
	SystemClock_Config();
	MX_GPIO_Init();
	MX_LPUART1_UART_Init(); 
	MX_SDMMC1_SD_Init();
	MX_FATFS_Init();
     MX_I2C1_Init(); 
     MX_RTC_Init();



  uint8_t errorCode = SD_Init();
  printmsg("Error Code, %d \r\n", errorCode);


  uint32_t i = 0;

  float32_t PSD[FFT_LENGTH/2];
  int32_t rawData[FFT_LENGTH];
  int32_t timeSeries[FFT_LENGTH];

  Wave_Data_t waveData;
  uint32_t waveDirNo  = 1;
  uint32_t waveLogNo = 1;

  uint32_t fpointer = 0;

  //SD_Wave_Read(&File, rawData, waveDirNo, waveLogNo, Z_ACC, &fpointer);



    for(int i=0; i<1024; i++)
    {
    	 printmsg("%d \r\n", rawData[i]);
    }

  WelchMethod(PSD, waveDirNo);

  ///singleSegmentPipeline(timeSeries, 1, 3);

 //waveParamExtract(PSD, &waveData.Hm0, &waveData.TM01, &waveData.Hrms, &waveData.T0, &waveData.Tp, &waveData.M0, &waveData.M1, &waveData.M2);

//   fullPipeline(waveData, waveDirNo);

//  for(int i=0; i<30; i++)
//  {
// 	 printmsg("%f \r\n", waveData.PSD[i]);
//  }


//     printmsg("DSP Pipeline Finished!\r\n");

//   /* USER CODE END 3 */
}

void printmsg(const char *format,...) {
    char str[80];

    /*Extract the the argument list using VA apis */
    va_list args;
    va_start(args, format);
    vsprintf(str, format,args);
    HAL_UART_Transmit(&hlpuart1,(uint8_t *)str, strlen(str),HAL_MAX_DELAY);
    va_end(args);
}
