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
#include "SHARC_Frame.h"
#ifdef __cplusplus 
}
#endif

//======================== Function Prototypes ============================================================

//**
//  * @brief Place code for IMU Event Detection here
//  * @param None
//  * @retval None
//  */
//static void Routine_ASYNC_IMU_EVENT(void);

/*
 * @brief Code for Reset State Routine
 * @param None
 * @retval None
 */
static void Routine_STATE_RESET(void);

/*
 * @brief Code for Sleep State Routine
 * @param None
 * @retval None
 */
static void Routine_STATE_SLEEP(void);

/*
 * @brief Code for Sample State Routine
 * @param None
 * @retval None
 */
static void Routine_STATE_SAMPLE(void);

/*
 * @brief Code for Transmit State Routine
 * @param None
 * @retval None
 */
static void Routine_STATE_TRANSMIT(void);
/*
 * @brief Code for Init State Routine
 * @param None
 * @retval None
 */
static void Routine_Init_STATE(void);

//======================== Peripheral Handles ============================================================
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
//======================== 0. Variable Declaration ==============================//
	GPS_Data_t Gdata;
	RTC_TimeTypeDef gTime;
	RTC_DateTypeDef gDate;
	Buoy_State_typedef Current_State;

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
/* Infinite loop START */
	while (1) {

		//=========================== 3. SYSTEM STATE CHECK =============================//
		/*
		 * Main Loop state detection: use this area to implement state
		 * check.States are positive integers written to Back up Register
		 * 0. State functions include Reading/ Writing to Back Up register
		 * determing previous state before sleep and writing state to back up register
		 * states are defined in the enum Buoy_State_typedef. The state check block performs the following routine
		 *
		 */

		//enable access to back up registers
		switch (__GET_PREV_STATE()) {
		case STATE_RESET:
			//system encountered a power on reset, put peripherals here
			Current_State = STATE_SAMPLE;
			break;

		case STATE_SAMPLE:
			//check how many samples have been recorded by the Buoy
			sample_count = __GET_SAMPLE_COUNT();
			if (sample_count >= 1) {
				//set next Buoy State to Transmit
				Current_State = STATE_TRANSMIT;
				IMU_On = 1;
			}
			// Set Buoy Next state to Sleep
			else {
				IMU_On = 0;
				Current_State = STATE_SLEEP;
				__HAL_RCC_PWR_CLK_ENABLE();
				__SET_CURRENT_STATE(Current_State);
				__HAL_RCC_PWR_CLK_DISABLE();
			}
			break;

		case STATE_SLEEP:
			hrtc.Instance = RTC;				//attach RTC instance to handler
			HAL_PWREx_DisableInternalWakeUpLine();//clear wake up pending interrupt from internal wake up
			__HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU);//clear pending interrupt from ext wake up pins
			__HAL_PWR_CLEAR_FLAG(PWR_FLAG_SB);
			set_WUP_Pin(IMU_EVENT_WAKE_PIN, MODE_EXTI); //reconfigure wake up pins
			Current_State = STATE_SAMPLE;
			break;

		case STATE_TRANSMIT:
			Current_State = STATE_SLEEP;
			__SET_CURRENT_STATE(Current_State);
			break;

			//default case: reset state
		default:
			Current_State = STATE_INIT;
		}
		__HAL_RCC_PWR_CLK_DISABLE();

	//=================================== 3. END ====================================//

	//============================== 4. STATE FUNCTIONS =============================//

		/*
		 * Place Routine code Here
		 */
		switch (Current_State) {
		case STATE_INIT: {
			Routine_Init_STATE();
			Current_State = STATE_RESET;
			break;
		}
		case STATE_RESET: {
			Routine_STATE_RESET();
			break;
		}
			// SLEEP STATE
		case STATE_SLEEP: {
			Routine_STATE_SLEEP();
			break;
		}
			//RESET STATE
		case STATE_ASYNCINT: {
			break;
		}
			//SAMPLE STATE
		case STATE_SAMPLE: {
			Routine_STATE_SAMPLE();
			break;
		}
			//TRANSMIT STATE
		case STATE_TRANSMIT: {
			Routine_STATE_TRANSMIT();
			break;
		}
		default:
			break;
		}
		// ADDITONAL STATE FUNCTIONS HERE:

		//=================================== 4. END ====================================//
		//========================= 5. END OF ROUTINE FUNCTION ==========================//
		/*
		 * After each routine has run, save state to the back up registers
		 */
		__HAL_RCC_PWR_CLK_ENABLE();
		if (Current_State == STATE_SAMPLE)//increment sample counter after each sampe
				{
			sample_count = __GET_SAMPLE_COUNT();
			__SET_SAMPLE_COUNT(++sample_count);
		}

		__SET_CURRENT_STATE(Current_State);	   //write value to back up register
		__HAL_RCC_PWR_CLK_DISABLE();

		//=================================== 5. END ====================================//
	}
}

//==================== 6. Configuration, Initialization and Debugging Functions ====================//
	void printmsg(const char *format,...) {
		char str[80];

		/*Extract the the argument list using VA apis */
		va_list args;
		va_start(args, format);
		vsprintf(str, format,args);
		HAL_UART_Transmit(&hlpuart1,(uint8_t *)str, strlen(str),HAL_MAX_DELAY);
		va_end(args);
	}


//=============================== 7. Routines =======================================//

static void Routine_STATE_RESET(void) {
	//initialise RTC
	//Enable Interrupt pins as EXTI Outputs
	//set_WUP_Pin(IMU_EVENT_WAKE_PIN,MODE_EXTI);
	printmsg("Device Online!\r\n");
	printmsg("Current State: RESET \t Next State: SAMPLE\r\n");
//	 HAL_GPIO_WritePin(LD2_GPIO_Port,LD2_Pin,SET);

}


static void Routine_STATE_SLEEP(void) {
	printmsg("Current State: SLEEP \t Next State: SAMPLE\r\n");
	printmsg("Good Night! \r\n");
	//Go_To_Sleep(STDBY,T_SLEEP);
}

static void Routine_STATE_SAMPLE(void) {
	//Initialize RTC struct to get current time
	RTC_TimeTypeDef gTime;
	RTC_DateTypeDef gDate;

	//Log initial time
	Sample_Time_Start = HAL_GetTick();

	//Get Previous Directory and Log Numbers for SD Cards
	HAL_PWR_EnableBkUpAccess();
	waveDirNo = HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR2);
	gpsDirNo = HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR3);
	envDirNo = HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR4);
	waveLogNo = HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR5);
	gpsLogNo = HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR6);
	envLogNo = HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR7);
	HAL_PWR_DisableBkUpAccess();

	//Revert all logs to zero - except wave logs

	//SD Init Routine
	if (SD_Init() == SD_OK) {
		printmsg("SD Card online \r\n");
	} else {
		printmsg("SD Card offline \r\n");
	}

	//GPS  Init Routine
	GPS_Data_t Gdata;
	sample_count = __GET_SAMPLE_COUNT();
	if (sample_count <= 1) {
		printmsg("Current State: SAMPLE \t Next State: SLEEP\r\n");
	} else {
		printmsg("Current State: SAMPLE \t Next State: TRANS\r\n");
		IMU_On = 1;
	}

	//initialise gps peripherals and begin comms on sensor instanc
// 	if (GPS_Init() == GPS_Init_OK) {

// 		// Sample Routine
// 		GPS_InterruptsOn();

// 		HAL_Delay(5000);

// 		gpsDataLive = { 0 };
// 		GPS_Update();

// 		printmsg("Logging Data...\r\n");
// //				  HAL_GPIO_WritePin(LD2_GPIO_Port,LD2_Pin,SET);
// 		Gdata.coordinates = gpsDataLive.coordinates;
// 		Gdata.diag = gpsDataLive.diag;
// 		gpsDataLive = { 0 };
// 		printmsg("local time: %lu, position: %f Lat, %f long\r\n",
// 				Gdata.coordinates.time, Gdata.coordinates.lat,
// 				Gdata.coordinates.longi);
// 		printmsg("HDOP = %d.%d, \t PDOP = %d.%d, VDOP = %d.%d\r\n",
// 				Gdata.diag.HDOP.digit, Gdata.diag.HDOP.precision,
// 				Gdata.diag.PDOP.digit, Gdata.diag.PDOP.precision,
// 				Gdata.diag.VDOP.digit, Gdata.diag.VDOP.precision,
// 				Gdata.diag.num_sats);
// 		printmsg("Number of Satellites %d, Fix Type = %d\r\n",
// 				Gdata.diag.num_sats, Gdata.diag.fix_type);

// 		GPS_InterruptsOff();

// 		// Record all GPS data onto SD cards
// 		uint8_t tempGPSBuf[500];
// 		SD_GPS_Open(&File, &Dir, &fno, gpsDirNo, gpsLogNo);
// 		HAL_RTC_GetTime(&hrtc, &gTime, RTC_FORMAT_BIN);
// 		snprintf((char*) tempGPSBuf, 500,
// 				"%d:%d:%d, %f, %f, %lu, %d, %d, %d, %d, %d, %d, %d, %d, %d \r\n",
// 				gTime.Hours, gTime.Minutes, gTime.Seconds,
// 				Gdata.coordinates.lat, Gdata.coordinates.longi,
// 				Gdata.coordinates.time, Gdata.diag.HDOP.digit,
// 				Gdata.diag.HDOP.precision, Gdata.diag.PDOP.digit,
// 				Gdata.diag.PDOP.precision, Gdata.diag.VDOP.digit,
// 				Gdata.diag.VDOP.precision, Gdata.diag.num_sats,
// 				Gdata.diag.fix_type);
// 		SD_File_Write(&File, tempGPSBuf);
// 		SD_File_Close(&File);
// 		SD_Unmount(SDFatFs);

// 		gpsLogNo++;

// 		if(gpsLogNo == 48)
// 		{
// 			gpsDirNo++;
// 			gpsLogNo = 1;
// 		}


// 	}

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

	/* High Frequency IMU sampling */

	//Temporary variables related to IMU sampling
	int32_t accelTemp[3];
	int32_t gyroTemp[3];
	uint8_t tempFIFOBuf[500];
	uint8_t imu[12] = { 0 };
	uint8_t data_status;

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

				accelTemp[0] = ((int16_t) (imu[0] << 8) | imu[1]);
				accelTemp[1] = ((int16_t) (imu[2] << 8) | imu[3]);
				accelTemp[2] = ((int16_t) (imu[4] << 8) | imu[5]);
				gyroTemp[0] = ((int16_t) (imu[6] << 8) | imu[7]);
				gyroTemp[1] = ((int16_t) (imu[8] << 8) | imu[9]);
				gyroTemp[2] = ((int16_t) (imu[10] << 8) | imu[11]);

				//printmsg("Ax      Ay      Az      Gx      Gy      Gz \r\n");
				//printmsg("%d  %d  %d  %d  %d  %d \r\n", accelTemp[0], accelTemp[1], accelTemp[2], gyroTemp[0], gyroTemp[1],gyroTemp[2]);

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

	//configure device for environmental sensing
	//initialise gps peripherals and begin comms on sensor instanc
	// if (GPS_Init() == GPS_Init_OK) {

		// Sample Routine
		// GPS_InterruptsOn();

		// HAL_Delay(5000);

		//gpsDataLive = { 0 };
		//GPS_Update();

// 		printmsg("Logging Data...\r\n");
// //				  HAL_GPIO_WritePin(LD2_GPIO_Port,LD2_Pin,SET);
// 		Gdata.coordinates = gpsDataLive.coordinates;
// 		Gdata.diag = gpsDataLive.diag;
// 		gpsDataLive = { 0 };
// 		printmsg("local time: %lu, position: %f Lat, %f long\r\n",
// 				Gdata.coordinates.time, Gdata.coordinates.lat,
// 				Gdata.coordinates.longi);
// 		printmsg("HDOP = %d.%d, \t PDOP = %d.%d, VDOP = %d.%d\r\n",
// 				Gdata.diag.HDOP.digit, Gdata.diag.HDOP.precision,
// 				Gdata.diag.PDOP.digit, Gdata.diag.PDOP.precision,
// 				Gdata.diag.VDOP.digit, Gdata.diag.VDOP.precision,
// 				Gdata.diag.num_sats);
// 		printmsg("Number of Satellites %d, Fix Type = %d\r\n",
// 				Gdata.diag.num_sats, Gdata.diag.fix_type);

		//GPS_InterruptsOff();

		// Record all GPS data onto SD cards
		// uint8_t tempGPSBuf[500];
		// SD_GPS_Open(&File, &Dir, &fno, gpsDirNo, gpsLogNo);
		// HAL_RTC_GetTime(&hrtc, &gTime, RTC_FORMAT_BIN);
		// snprintf((char*) tempGPSBuf, 500,
		// 		"%d:%d:%d, %f, %f, %lu, %d, %d, %d, %d, %d, %d, %d, %d, %d \r\n",
		// 		gTime.Hours, gTime.Minutes, gTime.Seconds,
		// 		Gdata.coordinates.lat, Gdata.coordinates.longi,
		// 		Gdata.coordinates.time, Gdata.diag.HDOP.digit,
		// 		Gdata.diag.HDOP.precision, Gdata.diag.PDOP.digit,
		// 		Gdata.diag.PDOP.precision, Gdata.diag.VDOP.digit,
		// 		Gdata.diag.VDOP.precision, Gdata.diag.num_sats,
		// 		Gdata.diag.fix_type);
		// SD_File_Write(&File, tempGPSBuf);
		// SD_File_Close(&File);
		// SD_Unmount(SDFatFs);

		// gpsLogNo++;

		// if(gpsLogNo == 48)
		// {
		// 	gpsDirNo++;
		// 	gpsLogNo = 1;
		// }
	//}

	// Environmental Sensor Init Routine
	BMP_Init_Typedef BMP_InitStruct2 = { 0 };
	//configure device for environmental sensing
	BMP_InitStruct2.BMP_Pressure_OverSample = BMP280_CTRLMEAS_OSRSP_OS_1;
	BMP_InitStruct2.BMP_Temperature_OverSample = BMP280_CTRLMEAS_OSRST_OS_1;
	BMP_InitStruct2.BMP_IIR_FILTER_COEFFICIENTS = BMP280_CONFIG_FILTER_COEFF_OFF;
	BMP_InitStruct2.BMP_Power_Mode = BMP280_CTRLMEAS_MODE_FORCED;
	if (BMP280_Init(&BMP_InitStruct) == BMP_OK) {
		printmsg("Environmental Sensor Online!\r\n");
		//create variables
		uint32_t temp, press;
		int32_t t_fine;
		for (int i = 0; i < 30; ++i) //60 second compensation period for averaging
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

	//Store updated directory and log numbers
	HAL_PWR_EnableBkUpAccess();
	HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR2, waveDirNo);
	HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR3, gpsDirNo);
	HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR4, envDirNo);
	HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR5, waveLogNo);
	HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR6, gpsLogNo);
	HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR7, envLogNo);
	HAL_PWR_DisableBkUpAccess();


}

static void Routine_Init_STATE(void) {
	//Initialize and configure the SD cards
	printmsg("Initialisation routine...\r\n");

	//Initialise RTC
	MX_RTC_Init();



	//Initial log and directory values for SD cards
	waveLogNo = 1;
	waveDirNo = 1;
	gpsLogNo = 1;
	gpsDirNo = 1;
	envLogNo = 1;
	envDirNo = 1;

	//Store current SD and log numbers variable in RTC->BCKUP//
	HAL_PWR_EnableBkUpAccess();
	HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR2, waveDirNo);
	HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR3, gpsDirNo);
	HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR4, envDirNo);
	HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR5, waveLogNo);
	HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR6, gpsLogNo);
	HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR7, envLogNo);
	HAL_PWR_DisableBkUpAccess();

	/* Test BMP280 Sensor*/
	BMP_Init_Typedef BMP_InitStruct = { 0 };
	BMP280_Init_Preset_Mode(Weather_Monitoring, &BMP_InitStruct);
	if (BMP280_Init(&BMP_InitStruct) == BMP_OK) {
		printmsg("Environmental Sensor Online!\r\n");
	} else {
		printmsg("Environmental Sensor Offline!\r\n");
	}

	// /* Test INA 219 Sensor */
	// if (INA219_Init_Sensor() == INA_OK) {
	// 	printmsg("Current Monitor Online!\r\n");
	// } else {
	// 	printmsg("Current Monitor Offline!\r\n");
	// }

	// /* Test INA 219 Sensor */
	// if (GPS_Init() == GPS_Init_OK) {
	// 	printmsg("GPS Online!");
	// 	GPS_InterruptsOff();
	// 	GPS_On = 1;
	// } else {
	// 	printmsg("GPS offline!\r\n");
	// }

	/* Test SD Cards */
	if (SD_Init() == SD_OK) {
		printmsg(" SD Cards Online! \r\n");
	} else {
		printmsg("SD Cards Offline! \r\n");
	}

	/* Test ICM20649 Sensor */

	if (ICM20649_Init_IMU(GYRO_CONFIG_FSSEL_500DPS, ACC_CONFIG_AFSSEL_4G,
	ACCEL_DPLFCFG_1, GYRO_DPLFCFG_0) == IMU_OK) {
		printmsg("IMU Online \r\n");
	} else {
		printmsg("IMU Offline \r\n");
	}
	ICM20649_Deinit_IMU();

	//MX_USART3_UART_Init();
}

static void Routine_STATE_TRANSMIT(void) {

	printmsg("Current State: TRANS \t Next State: SLEEP\r\n");

	//Get Previous Directory and Log Numbers for SD Cards
	HAL_PWR_EnableBkUpAccess();
	waveDirNo = HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR2);
	gpsDirNo = HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR3);
	envDirNo = HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR4);
	waveLogNo = HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR5);
	gpsLogNo = HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR6);
	envLogNo = HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR7);
	HAL_PWR_DisableBkUpAccess();

	//Transfer Buffer
	uint8_t GPS_trans_Buffer[94] = {0};
	//uint8_t WAVE_trans_Buffer[340];

	//File pointer
	uint32_t fpointer = 0;

	//Enable SD card interface
	SD_Init();

	//Create Temporary Arrays
	float32_t gpsData[12] = { 0 };
	float32_t envData[3] = { 0 };

	//GPS Buffer - Log GPS data here
	GPS_Data_t GdataT; //GPS struct used for transmission and binary conversion

	/* First GPS Buffer */

	//Get GPS data from SD card
	fpointer = 0;
	SD_GPS_Read(&File, gpsData, gpsDirNo, gpsLogNo-2, &fpointer);
	SD_File_Close(&File);

	//Note: gpsData[0] is a timestamp
	GdataT.coordinates.lat = gpsData[1];
	GdataT.coordinates.longi = gpsData[2];
	GdataT.coordinates.time = gpsData[3];
	GdataT.diag.HDOP.digit = (uint8_t) gpsData[4];
	GdataT.diag.HDOP.precision = (uint8_t) gpsData[5];
	GdataT.diag.VDOP.digit = (uint8_t) gpsData[6];
	GdataT.diag.VDOP.precision = (uint8_t) gpsData[7];
	GdataT.diag.PDOP.digit = (uint8_t) gpsData[8];
	GdataT.diag.PDOP.precision = (uint8_t) gpsData[9];
	GdataT.diag.num_sats = (uint8_t) gpsData[10];
	GdataT.diag.fix_type = (uint8_t) gpsData[11];

	//Get Environmental data from SD card
	fpointer = 0;
	SD_ENV_Read(&File, envData, envDirNo, envLogNo-2, &fpointer);
	SD_File_Close(&File);

	//ENV Data
	//Note: envData[0] is a timestamp
	GdataT.env_Temp = (uint32_t) envData[1];
	GdataT.atm_Press = (uint32_t) envData[2];

	//Convert GData Buffer to binary
	uint8_t *buffer = to_binary_format_GPS(GdataT, (__GET_SAMPLE_COUNT() - 1));

	//Populate first packet of GPS data into array
	memcpy(&GPS_trans_Buffer[0], buffer, DRIFTBUFFER_SIZE);

	//Wave Buffer
	//Calculate wave parameter extra action for previous two logs
	Wave_Data_t waveData1;
	fullPipeline(waveData1, waveDirNo - 1); //get wave data from previous record

	buffer = to_binary_format_Wave(waveData1, __GET_SAMPLE_COUNT() -1);

	memcpy(&GPS_trans_Buffer[DRIFTBUFFER_SIZE + 1], buffer, WAVEBUFFER_SIZE-1);

	/* Second GPS Buffer */

	//Get GPS data from SD card
	fpointer = 0;
	SD_GPS_Read(&File, gpsData, gpsDirNo, gpsLogNo-1, &fpointer);
	SD_File_Close(&File);

	//Note: gpsData[0] is a timestamp
	//Note: gpsData[0] is a timestamp
	GdataT.coordinates.lat = gpsData[1];
	GdataT.coordinates.longi = gpsData[2];
	GdataT.coordinates.time = gpsData[3];
	GdataT.diag.HDOP.digit = (uint8_t) gpsData[4];
	GdataT.diag.HDOP.precision = (uint8_t) gpsData[5];
	GdataT.diag.VDOP.digit = (uint8_t) gpsData[6];
	GdataT.diag.VDOP.precision = (uint8_t) gpsData[7];
	GdataT.diag.PDOP.digit = (uint8_t) gpsData[8];
	GdataT.diag.PDOP.precision = (uint8_t) gpsData[9];
	GdataT.diag.num_sats = (uint8_t) gpsData[10];
	GdataT.diag.fix_type = (uint8_t) gpsData[11];

	//Get Environmental data from SD card
	fpointer = 0;
	SD_ENV_Read(&File, envData, envDirNo, envLogNo-1, &fpointer);
	SD_File_Close(&File);

	//ENV Data
	//Note: envData[0] is a timestamp
	GdataT.env_Temp = (uint32_t) envData[0];
	GdataT.atm_Press = (uint32_t) envData[1];

	//Convert GData Buffer to binary
	buffer = to_binary_format_GPS(GdataT, __GET_SAMPLE_COUNT());

	//Populate Second packet of GPS data into array
	memcpy(&GPS_trans_Buffer[DRIFTBUFFER_SIZE + 1 + WAVEBUFFER_SIZE + 1], buffer, DRIFTBUFFER_SIZE );


	//Dismount SD cards! DO NOT WANT MOUNTED SD CARDS DURING IRIDIUM TRANSMISSION
	SD_Unmount(SDFatFs);
	// SBDX_Status_t sbd;
	// HAL_Delay(60000);
	// /* Test Iridium Modem */
	// if (IR_Init_Module() == IR_OK) {
	// 	if (IR_send_Bin_String(GPS_trans_Buffer, 94) == IR_OK) {
	// 		if (IR_start_SBD_Session(&sbd) == IR_OK) {
	// 				printmsg("Iridium transfer loop finished! \r\n");
	// 		}
	// 	}
	// }
	// IR_send_AT_CMD("AT+SBDD0\r");


	///Here to prevent interaction with SD Cards
	//HAL_Delay(1000);
	/* Reset Memory pointer*/
	//reset sample count
	__SET_SAMPLE_COUNT(0);
	//erase page and reset counter

}
