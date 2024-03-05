/*
 * HAL_SD.h
 *
 *  Created on: Apr 20, 2022
 *      Author: Michael Noyce
 *      Student No: NYCMIC002
 *      For: The University of Cape Town
 *
 *  This Library is designed to be used with the STM32 HAL Driver files version 1.15.1
 *  Version 1.14.x is also supported
 *
 *  This library is designed to interface with SD card external memory using the
 *  onboard SDMMC driver on the STM32
 *
 *  This library contains all the functions and definitions to interface with and configure the sensor.
 *  This includes functions that
 *
 *  1. Configure the SD card and FATFS
 *  2. Open files
 *  3. Read files
 *  4. Create Files
 *
 * NB: get_fattime must be updated in the fatfs.c file to use f_utime and timestamp functions
 */

#ifdef __cplusplus
extern "C" {
#endif

#ifndef HAL_SD_H_
#define HAL_SD_H_

//============================= 1. Includes ==============================================

#include "stm32l4xx_hal.h"	//HAL library includes
#include "math.h"			//Math Functions
#include "stdint.h"			//integers
#include "string.h"			//Mem functions
#include "fatfs.h"
#include "stdio.h"
//========================== 2. Structs & Enums ===========================================

/*
 * sd_status_t
 *
 * @brief:	Used to represent numeric statuses returned as a result of
 * 		    running SDMMC interface
 */
typedef enum
{
	SD_SDMMC_ERROR,
	SD_INIT_ERROR,
	SD_MOUNT_ERROR,
	SD_CLOSE_ERROR,
	SD_OPEN_ERROR,
	SD_WRITE_ERROR,
	SD_CRC_ERROR,
	SD_DMA_ERROR,
	SD_OK,
	SD_TIME_ERROR,
}sd_status_t;



typedef enum
{
	TIMESTAMP_DATA,
	X_ACC,
	Y_ACC,
	Z_ACC,
	X_GYRO,
	Y_GYRO,
	Z_GYRO,
	TEMP,
}IMU_Data_SD_t;


typedef enum
{
	ETIME,		// UTC Epoch representation of time
	COORD,	//GPS coordinates
	DIAG,		//Diagnostic information
	ENV_TEMP,		//environmental temperature
	ATM_PRESS,		//atmospheric pressure
	SHUNT_V,		//shunt voltage (mv)
	BUS_V,			//bus voltage   (mV)
	CURRENT,		//battery current (mA)
	POWER,			//power consumption (mW)
}GPS_Data_SD_t;

//========================== 4. Global Variables ==========================================

FATFS SDFatFs; /* File system object for User logical drive */
FIL File; /* File object */
DIR Dir; /*Directory Object*/
FILINFO fno;
uint32_t waveLogNo;
uint32_t waveDirNo;
uint32_t gpsLogNo;
uint32_t gpsDirNo;

//======================== 7. SD Configuration Functions =========================================

/*Function Name sd_status_t SD_Mount(uint8_t mount);
 *
 * @brief: Mount SD Card
 *
 */
sd_status_t SD_Mount(FATFS SDFatFs, char myPath[]);

/*Function Name sd_status_t SD_File_Open(FIL myfile);
 *
 * @brief: Open file on SD Card
 *
 */

/**
 * @brief
 *
 * @param SDFatFs
 * @param diskPath
 * @return
 */
sd_status_t SD_Unmount(FATFS SDFatFs);

//======================== 6. SD Read/Write Functions ============================================

sd_status_t SD_File_Open(FIL *myFile, uint32_t fileNo);

/*Function Name sd_status_t SD_File_Write(FIL myfile, uint8_t dataBuffer[]);
 *
 * @brief: Write to SD Card
 *
 */
sd_status_t SD_File_Write(FIL *myFile, uint8_t *dataBuffer);

/*Function Name sd_status_t SD_File_Close(FIL myfile)
 *
 * @brief: Close File on SD Card
 *
 */
sd_status_t SD_File_Close(FIL *myFile);

/**
 * @brief Closes current file directory
 *
 * @param myFile
 * @return
 */
sd_status_t SD_Dir_Close(DIR *myDir);


/**
 * @brief Time-stamps myFile with current time from RTC
 *
 * @param myFile
 * @return
 */
sd_status_t SD_File_TimeStamp(FIL *myFile);



/**
 * @brief Opens file in wave file directory
 *
 * Description: The function takes a individual IMU sample segment from local memory and appends it to a numbered wave log file.
 * After a determined number of buffers have been added, close file and start new file.
 *
 * @param myFile
 * @param waveLogNo
 * @param waveLogNo
 * @return
 */
sd_status_t SD_Wave_Open(FIL *myFile, DIR *myDir, FILINFO* fno, uint32_t wavDirNo, uint32_t waveLogNo);


/**
 * @brief
 *
 * @param myFile
 * @param LineNo
 * @param dataBuf
 * @return
 */
sd_status_t SD_Wave_Read(FIL *myFile, int32_t *IMUArray,  uint32_t WaveDirNo, uint32_t WaveLogNo,  IMU_Data_SD_t inertialDataType, uint32_t *fpointer);




/**
 * @brief Opens file in GPS file directory
 *
 * @param myFile
 * @param myDir
 * @param fno
 * @param
 * @return
 */
sd_status_t SD_GPS_Open(FIL *myFile, DIR *myDir, FILINFO* fno, uint32_t GPSDirNo, uint8_t GPSLogNo);


/**
 * @brief
 *
 * @param myFile
 * @param GPSArray
 * @param GPSDirNo
 * @param GPSLogNo
 * @param GPSDataType
 * @param fpointer
 * @return
 */
sd_status_t SD_GPS_Read(FIL *myFile, int32_t *GPSArray,  uint32_t GPSDirNo, uint32_t GPSLogNo, GPS_Data_SD_t GPSDataType, uint32_t *fpointer);


#endif /* HAL_SD_H_ */

#ifdef __cplusplus
}
#endif