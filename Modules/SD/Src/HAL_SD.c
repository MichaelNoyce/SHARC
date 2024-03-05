/*
 * HAL_SD.c
 *
 *  Created on: Apr 20, 2022
 *      Author: Michael Noyce
 */


//======================== 1. Includes ==============================================================

#include "HAL_SD.h"
#include "string.h"
#include "main.h"

//======================== 6. SD Configuration Functions =========================================

sd_status_t SD_Mount(FATFS SDFatFs, char diskPath[])
{

	if(f_mount(&SDFatFS, (TCHAR const*)diskPath, 0) != FR_OK)
	{
	  return SD_MOUNT_ERROR;
	}

	return SD_OK;
}


sd_status_t SD_Unmount(FATFS SDFatFs)
{
	char diskPath[10];

	if(f_mount(0, (TCHAR const*)diskPath, 0) != FR_OK)
	{
	  printmsg("SD unmount error \r\n");
	  return SD_MOUNT_ERROR;
	}
	else
	{
		//printmsg("Dismount successful!");
	}

	return SD_OK;
}


sd_status_t SD_File_Open(FIL *myFile, uint32_t fileNumber)
{

	  char diskPath[10]; /* User logical drive path */
	  sd_status_t errorCode;
	  FRESULT res;
	  uint32_t byteswritten = 0;

	  if(SD_Mount(SDFatFs, diskPath) == SD_OK)
	  {
		  sprintf(diskPath, "Log%ld.txt", fileNumber);
		   if(f_open(myFile, (const TCHAR*)diskPath, FA_CREATE_ALWAYS | FA_WRITE) == FR_OK)
		    {
			  //printmsg("Opened file!");
		    }
		  else
		  {
			return SD_OPEN_ERROR;
			printmsg("Open failed!");
		  }
	  }
	  else
	  {
		  printmsg("Mount failed");
		  return SD_MOUNT_ERROR;
	  }

	  return SD_OK;

}

sd_status_t SD_File_Write(FIL *myFile, uint8_t *dataBuffer)
{

	uint32_t byteswritten = 0 ;  /* File write counts */

	if(f_write(myFile, dataBuffer, strlen((char *)dataBuffer), (void *)&byteswritten)!= FR_OK)
	{
		printmsg("Write failed!");
		return SD_WRITE_ERROR;
	}

	if(byteswritten == 0)
    {
	  printmsg("Write failed!");
  	  return SD_WRITE_ERROR;
    }



	return SD_OK;
}

sd_status_t SD_File_Close(FIL *myFile)
{
	if(f_close(myFile) != FR_OK)
	{
		printmsg("Close Failed!");
		return SD_CLOSE_ERROR;
	}


	return SD_OK;
}

/**
 * @brief Closes Current Directory
 *
 * @param myDir
 * @return
 */
sd_status_t SD_Dir_Close(DIR *myDir)
{
	if(f_closedir(myDir) != FR_OK)
	{
		printmsg("CloseDir Failed!");
		return SD_CLOSE_ERROR;
	}

	return SD_OK;
}

/**
 * @brief Function to open a wave log file for writing local data buffers to.
 *
 * Description: The function opens a wavLog file in a waveDir directory to be written to
 *
 *
 * @param myFile
 * @param waveBufferSegment
 * @return
 */
sd_status_t SD_Wave_Open(FIL *myFile, DIR *myDir, FILINFO* fno, uint32_t waveDirNo, uint32_t waveLogNo)
{

     char diskPath[14]; /* User logical drive path */
     char dirPath[14]; //max path length is 14 without long path names enabled

	  if(SD_Mount(SDFatFs, diskPath) == SD_OK)
	  {

		  sprintf(dirPath, "WD");

		  if(f_stat((const TCHAR*)dirPath, fno) == FR_NO_FILE)
		  {
		  	 if(f_mkdir((const TCHAR*)dirPath) != FR_OK)
		  	 {
		  		printmsg("Main Wave Directory not created \r\n");
		  	 }
		  }


		  sprintf(dirPath, "WD/WS%d", waveDirNo);

		  if(f_stat((const TCHAR*)dirPath, fno) == FR_NO_FILE)
		  {

			  FRESULT status = f_mkdir((const TCHAR*)dirPath);

			  if(status != FR_OK)
			  {
			  	printmsg("Directory not created, error %d\r\n", status);
			  }
		  }

			  sprintf(diskPath, "WD/WS%d/W%d.txt", waveDirNo, waveLogNo);

			  if(f_open(myFile, (const TCHAR*)diskPath, FA_OPEN_APPEND | FA_WRITE) == FR_OK)
			  {
				  //printmsg("Opened file! \r\n");
			  }
			  else
			  {
				  printmsg("Open failed! \r\n");
				  return SD_OPEN_ERROR;
			  }
	  }
	  else
	  {
		   printmsg("Mount failed");
		   return SD_MOUNT_ERROR;
	  }


	  return SD_OK;


}




sd_status_t SD_GPS_Open(FIL *myFile, DIR *myDir, FILINFO* fno, uint32_t GPSDirNo, uint8_t GPSLogNo)
{

	 //printmsg("SDMMC Interface Starting! \r\n");
     char diskPath[16]; /* User logical drive path */
     char dirPath[16]; //max path length is 14 without long path names enabled

     if(SD_Mount(SDFatFs, diskPath) == SD_OK)
     {

     	sprintf(dirPath, "GD");

		 if(f_stat((const TCHAR*)dirPath, fno) == FR_NO_FILE)
		 {
			if(f_mkdir((const TCHAR*)dirPath) != FR_OK)
			{
				printmsg("Directory not created");
			}
		 }


		 	 sprintf(dirPath, "GD/GS%d", GPSDirNo);

			 if(f_stat((const TCHAR*)dirPath, fno) == FR_NO_FILE)
			 {

				 if(f_mkdir((const TCHAR*)dirPath) != FR_OK)
				 {
					 printmsg("Directory not created \r\n");
				 }
			 }

			 	 sprintf(diskPath, "GD/GS%d/G%d.txt", GPSDirNo, GPSLogNo);

				  if(f_open(myFile, (const TCHAR*)diskPath, FA_CREATE_ALWAYS | FA_WRITE) == FR_OK)
				  {
					  //printmsg("Opened file! \r\n");
				  }
				  else
				  {
					  printmsg("Open failed! \r\n");
					  return SD_OPEN_ERROR;
				  }
	  }
	  else
	  {
		   printmsg("Mount failed \r\n");
		   return SD_MOUNT_ERROR;
	  }


	  return SD_OK;


}

sd_status_t SD_Wave_Read(FIL *myFile, int32_t *IMUArray,  uint32_t WaveDirNo, uint32_t WaveLogNo, IMU_Data_SD_t inertialDataType, uint32_t *fpointer)
{

	uint8_t dataBuf[60];
	int maxLen = 60;
	char diskPath[16]; /* User logical drive path */
	FSIZE_t fp = *fpointer;

	char *p; //pointer for strtok functions
	const char delim[] = ", ";
	char tempString[60];


	sprintf(diskPath, "WD/WS%d/W%d.txt", waveDirNo, waveLogNo);


	  if(SD_Mount(SDFatFs, diskPath) == SD_OK)
	     {

			 sprintf(diskPath, "WD/WS%d/W%d.txt", WaveDirNo, WaveLogNo);

			  if(f_open(myFile, (const TCHAR*)diskPath, FA_READ) == FR_OK)
			  {

				  //Open file to correct location
				  fp = *fpointer;
				  printmsg("Initial file pointer location %ld \r\n", *fpointer);
				  f_lseek(myFile, fp);

				  for(int i = 0; i<=1024; i++)
				  {
					  f_gets(dataBuf, maxLen, myFile);

					  p = strtok(dataBuf, delim);

					  //Value in string is determined by IMU_data_t enum
					  for(int i=0; i<inertialDataType; i++)
					  {
						  p = strtok(NULL, delim);
					  }

					  IMUArray[i] = atoi(p);

				  }

				  //store current file pointer location
				  *fpointer = f_tell(myFile);

			  }
			  else
			  {
				  printmsg("Open failed! \r\n");
				  return SD_OPEN_ERROR;
			  }


		  }
		  else
		  {
			   printmsg("Mount failed \r\n");
			   return SD_MOUNT_ERROR;
		  }

	  return SD_OK;

}


sd_status_t SD_GPS_Read(FIL *myFile, int32_t *GPSArray,  uint32_t GPSDirNo, uint32_t GPSLogNo, GPS_Data_SD_t GPSDataType, uint32_t *fpointer)
{

	uint8_t dataBuf[60];
	int maxLen = 60;
	char diskPath[16]; /* User logical drive path */
	FSIZE_t fp = *fpointer;

	char *p; //pointer for strtok functions
	const char delim[] = ", ";
	char tempString[60];


	sprintf(diskPath, "WD/WS%d/W%d.txt", waveDirNo, waveLogNo);


	  if(SD_Mount(SDFatFs, diskPath) == SD_OK)
	     {

			 sprintf(diskPath, "GD/GS%d/G%d.txt", GPSDirNo, GPSLogNo);

			  if(f_open(myFile, (const TCHAR*)diskPath, FA_READ) == FR_OK)
			  {

				  //Open file to correct location
				  fp = *fpointer;
				  printmsg("Initial file pointer location %ld \r\n", *fpointer);
				  f_lseek(myFile, fp);

				  for(int i = 0; i<=1024; i++)
				  {
					  f_gets(dataBuf, maxLen, myFile);

					  p = strtok(dataBuf, delim);

					  //Value in string is determined by IMU_data_t enum
					  for(int i=0; i<GPSDataType; i++)
					  {
						  p = strtok(NULL, delim);
					  }

					  GPSArray[i] = atoi(p);

				  }

				  //store current file pointer location
				  *fpointer = f_tell(myFile);

			  }
			  else
			  {
				  printmsg("Open failed! \r\n");
				  return SD_OPEN_ERROR;
			  }


		  }
		  else
		  {
			   printmsg("Mount failed \r\n");
			   return SD_MOUNT_ERROR;
		  }

	  return SD_OK;

}
