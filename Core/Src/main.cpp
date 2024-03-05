#include "main.hpp"
#include "init.h"
#include "SD_init.h"
#include "ICM20649_init.h"
#include "string.h"
#include <stdarg.h>
#include "stdio.h"


void SystemClock_Config(void);
void Error_Handler(void);

UART_HandleTypeDef hlpuart1;
UART_HandleTypeDef huart4;
DMA_HandleTypeDef hdma_uart4_rx;
DMA_HandleTypeDef hdma_uart4_tx;
SD_HandleTypeDef hsd1;
TIM_HandleTypeDef htim1;
DMA_HandleTypeDef hdma_memtomem_dma1_channel1;
UART_HandleTypeDef huart2;
RTC_HandleTypeDef hrtc;
bool watchMe = false;
HAL_StatusTypeDef  UARTStatus;
bool bullshit_variable = false;



int main(void) {
	HAL_Init();
	SystemClock_Config();
	MX_GPIO_Init();
	MX_GPIO_Init_SD(); 
	MX_SDMMC1_SD_Init();
	MX_LPUART1_UART_Init(); 
	IMU_Interface_Init(); 
	UARTStatus = HAL_ERROR; 

	HAL_Delay(5000);

	bullshit_variable = true; 
	uint8_t Test[] = "Hello World !!!\r\n"; //Data to send
	UARTStatus = HAL_UART_Transmit(&hlpuart1,Test,sizeof(Test),100);// Sending in normal mode

	HAL_Delay(5000);

	SD_Init();
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
