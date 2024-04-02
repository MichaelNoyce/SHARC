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
#ifdef __cplusplus 
}
#endif

void SystemClock_Config(void);
void Error_Handler(void);

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
	HAL_Init();
	SystemClock_Config();
	MX_GPIO_Init();
	MX_LPUART1_UART_Init(); 
	MX_SDMMC1_SD_Init();
	MX_FATFS_Init();
	UARTStatus = HAL_ERROR; 

  uint8_t errorCode = SD_Init();
  printmsg("Error Code, %d \r\n", errorCode);

  uint8_t waveBufferSegment[] = "-0.024, 0.038, 9.992, 3.603, 71.756, -8.321, -18.448 \r\n";
  waveLogNo = 0;
  waveDirNo = 0;

  //Single Write Test

  //Open wave Log
  SD_Wave_Open(&File, &Dir, &fno, waveDirNo, waveLogNo);

  printmsg("File opened!");
  //Write to wave log
  SD_File_Write(&File, waveBufferSegment);
  //Close Wave Log
  SD_File_Close(&File);

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
