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
MX_GPIO_Init();
MX_LPUART1_UART_Init(); 
MX_SDMMC1_SD_Init();	//Test single segment
MX_FATFS_Init();
MX_I2C1_Init(); 
MX_RTC_Init();

	//GPS  Init Routine
	GPS_Data_t Gdata;
	RTC_TimeTypeDef gTime;
	RTC_DateTypeDef gDate;

// Environmental Sensor Init Routine
	BMP_Init_Typedef BMP_InitStruct = { 0 };
	//configure device for environmental sensing
	BMP_InitStruct.BMP_Pressure_OverSample = BMP280_CTRLMEAS_OSRSP_OS_1;
	BMP_InitStruct.BMP_Temperature_OverSample = BMP280_CTRLMEAS_OSRST_OS_1;
	BMP_InitStruct.BMP_IIR_FILTER_COEFFICIENTS = BMP280_CONFIG_FILTER_COEFF_OFF;
	BMP_InitStruct.BMP_Power_Mode = BMP280_CTRLMEAS_MODE_FORCED;
	if (BMP280_Init(&BMP_InitStruct) == BMP_OK) {
		printmsg("Environmental Sensor Online!\r\n");
		//create variables
		uint32_t temp, press;
		int32_t t_fine;
		for (int i = 0; i < 2; ++i) //60 second compensation period for averaging
				{
			BMP280_Force_Measure(&temp, &press);		//trigger conversion
			Gdata.env_Temp = BMP280_Compensate_Temp(temp, &t_fine,
					bmp.Factory_Trim);			//compensate temperature
			Gdata.atm_Press = BMP280_Compensate_Pressure(press, t_fine,
					bmp.Factory_Trim) / 256;	//compensate pressure
			HAL_Delay(1000);

		}
		printmsg("Temp = %ld�C \t Pressure = %lu Pa \r\n", Gdata.env_Temp,
				Gdata.atm_Press);

		//Store environmental samples
		uint8_t tempENVBuf[200];
		SD_ENV_Open(&File, &Dir, &fno, envDirNo, envLogNo);
		HAL_RTC_GetTime(&hrtc, &gTime, RTC_FORMAT_BIN);
		sprintf((char*) tempENVBuf, "%d:%d:%d, %ld, %lu \r\n", gTime.Hours,
				gTime.Minutes, gTime.Seconds, Gdata.env_Temp, Gdata.atm_Press);
		SD_File_Write(&File, tempENVBuf);
		SD_File_Close(&File);
		SD_Unmount(SDFatFs);
		envLogNo++;

		if(envLogNo == 48)
		{
			envDirNo++;
			envLogNo = 1;
		}

	} else {
		printmsg("BMP280 offline! \r\n");
	}

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
