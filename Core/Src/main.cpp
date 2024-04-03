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
     MX_I2C1_Init(); 
     MX_RTC_Init();

	uint8_t buf[12];
	int16_t val;
	imu_status_t flag;
	float acc;

	//======================== 1. Testing Initialisation ==============================================================

  printmsg("IMU Test started!");

  imu_status_t initStatus = ICM20649_Init_IMU(GYRO_CONFIG_FSSEL_500DPS, ACC_CONFIG_AFSSEL_4G, ACCEL_DPLFCFG_0, GYRO_DPLFCFG_0);

  if(initStatus != IMU_OK)
  {
	  printmsg("Error in i2c init, init error %d \r\n", initStatus);
  }
  else
  {
	  printmsg("Init successful! \r\n");
  }

  uint8_t int_status_1;

  if((HAL_I2C_Mem_Read(&hi2c1,IMU_Device_Address, INT_STATUS_1, 1, &int_status_1, 1, 100) != HAL_OK))
  {
  	 return IMU_I2C_ERROR;
  }
  else
  {
	  printmsg("INT_STATUS_1 %d \r\n", int_status_1);
  }

     uint8_t int_pin_config = 0;
    //Print out int_config register

    if(HAL_I2C_Mem_Read(&hi2c1,IMU_Device_Address, INT_PIN_CFG, I2C_MEMADD_SIZE_8BIT, &int_pin_config, 1, 100) != HAL_OK)
     {
   	 printmsg("i2c read error");
          return IMU_I2C_ERROR;
     }
    else
    {
   	  printmsg("INT_PIN_CONFIG: %d \r\n", int_pin_config);
    }

    uint8_t user_ctrl = 0;
    uint8_t fifo_rst = 0;
    uint8_t fifo_en_1 = 0;
    uint8_t fifo_en_2 = 0;


    if(HAL_I2C_Mem_Read(&hi2c1,IMU_Device_Address, USER_CTRL, 1, &user_ctrl, 1, 100) != HAL_OK)
     {
          return IMU_I2C_ERROR;
          printmsg("i2c read error");
     }
    else
    {
   	  printmsg("User control:  %d \r\n", user_ctrl);
    }

    if(HAL_I2C_Mem_Read(&hi2c1,IMU_Device_Address, FIFO_RST, 1, &fifo_rst, 1, 100) != HAL_OK)
     {
          return IMU_I2C_ERROR;
          printmsg("i2c read error");
     }
    else
    {
   	  printmsg("FIFO Reset:  %d \r\n", fifo_rst);
    }

    if(HAL_I2C_Mem_Read(&hi2c1,IMU_Device_Address, FIFO_EN_1, 1, &fifo_en_1, 1, 100) != HAL_OK)
     {
          return IMU_I2C_ERROR;
          printmsg("i2c read error");
     }
    else
    {
   	  printmsg("FIFO_EN_1:  %d \r\n", fifo_en_1);
    }

    if(HAL_I2C_Mem_Read(&hi2c1,IMU_Device_Address, FIFO_EN_2, 1, &fifo_en_2, 1, 100) != HAL_OK)
     {
          return IMU_I2C_ERROR;
          printmsg("i2c read error");
     }
    else
    {
   	  printmsg("FIFO_EN_2: %d \r\n", fifo_en_2);
    }


     int32_t accelTemp[3];
	int32_t gyroTemp[3];
	uint8_t tempFIFOBuf[500];
	uint8_t imu[12] = { 0 };
	uint8_t data_status;
     bool IMU_On; //IMU status flag
     RTC_TimeTypeDef gTime;
	RTC_DateTypeDef gDate;

	IMU_On = 1;

	SD_Wave_Open(&File, &Dir, &fno, waveDirNo, waveLogNo);

	if ((ICM20649_Init_IMU(GYRO_CONFIG_FSSEL_500DPS, ACC_CONFIG_AFSSEL_4G,
	ACCEL_DPLFCFG_1, GYRO_DPLFCFG_0) == IMU_OK)) {

		while (IMU_On) {

			if (imu_sample_count == WAVELOGBUFNO)
					{
				SD_File_Close(&File); //NB Remember to close file
				waveLogNo++;
				imu_sample_count = 0;
				SD_Wave_Open(&File, &Dir, &fno, waveDirNo, waveLogNo);
				printmsg("New Wave log! \r\n");
			}

			ICM20649_Is_Data_Ready(&hi2c1, &data_status);

			if (data_status) {

				ICM20649_Get_IMU_RawData(&hi2c1, imu);

				HAL_RTC_GetTime(&hrtc, &gTime, RTC_FORMAT_BIN);

				HAL_RTC_GetDate(&hrtc, &gDate, RTC_FORMAT_BIN); //unlocks time stamp

                    printmsg("Time: %d:%d:%d \r\n", gTime.Hours, gTime.Minutes, gTime.Seconds);

				accelTemp[0] = ((int16_t) (imu[0] << 8) | imu[1]);
				accelTemp[1] = ((int16_t) (imu[2] << 8) | imu[3]);
				accelTemp[2] = ((int16_t) (imu[4] << 8) | imu[5]);
				gyroTemp[0] = ((int16_t) (imu[6] << 8) | imu[7]);
				gyroTemp[1] = ((int16_t) (imu[8] << 8) | imu[9]);
				gyroTemp[2] = ((int16_t) (imu[10] << 8) | imu[11]);

				printmsg("Ax      Ay      Az      Gx      Gy      Gz \r\n");
				printmsg("%d  %d  %d  %d  %d  %d \r\n", accelTemp[0], accelTemp[1], accelTemp[2], gyroTemp[0], gyroTemp[1],gyroTemp[2]);

				sprintf((char*) tempFIFOBuf,
						"%d:%d:%d, %ld, %ld, %ld, %ld, %ld, %ld \r\n",
						gTime.Hours, gTime.Minutes, gTime.Seconds, accelTemp[0],
						accelTemp[1], accelTemp[2], gyroTemp[0], gyroTemp[1],
						gyroTemp[2]);
				SD_File_Write(&File, tempFIFOBuf);

				imu_sample_count++;

			}

			if (waveLogNo == WAVELOGNO + 1) {
				SD_File_Close(&File); //NB Remember to close file
				waveDirNo++;
				waveLogNo = 0;
				break;
			}

		}

	}

	else {
		printmsg("IMU Offline!\r\n");
	}

	ICM20649_Deinit_IMU();
	SD_Unmount(SDFatFs);

	IMU_On = 0;

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

  /* USER CODE END 3 */
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
