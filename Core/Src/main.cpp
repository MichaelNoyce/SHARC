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
#include "BMP280.h"
#include "GPS_Struct.h"
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
SPI_HandleTypeDef hspi1;
BMP_Handle_Typedef bmp;

int main(void) {

     //======================== 0. Initialisation ==============================================================
HAL_Init();
SystemClock_Config();
//GPS  Init Routine
GPS_Data_t Gdata;
RTC_TimeTypeDef gTime;
RTC_DateTypeDef gDate;


//======================== 1. SYSTEM INIT & CLOCK CONFIG ========================//
	HAL_Init();		//Init Flash prefetch, systick timer, NVIC and LL functions
	SystemClock_Config();	//configure clock
	MX_GPIO_Init();
	MX_LPUART1_UART_Init(); 
	MX_SDMMC1_SD_Init();	//Test single segment
	MX_FATFS_Init();
	MX_I2C1_Init(); 
	MX_RTC_Init();
	printmsg("SHARC BUOY STARTING! \r\n");
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_SET);
	//MX_USART3_UART_Init();
//=================================== 1. END ====================================//

//======================= 2. POWER AND RESET STATE CHECK ========================//

/*
	* When system powers on, check for any asynchronous resets that
	* may have occured. Use this area to add in any reset handling
	*/
if (__HAL_RCC_GET_FLAG(RCC_FLAG_BORRST) == SET) {
	BOR_Handler();
}
uint8_t flag = __HAL_RCC_GET_PORRST_FLAG();
if (flag == SET) {
	printmsg("Software Reset Detected. Initializing main program...\r\n");
	POR_Handler();
}
//=================================== 2. END ====================================//
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
