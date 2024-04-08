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

//======================== Function Prototypes ============================================================

//**
//  * @brief Place code for IMU Event Detection here
//  * @param None
//  * @retval None
//  */
static void Routine_ASYNC_IMU_EVENT(void);

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

//======================== 0. Initialisation ==============================================================
	GPS_Data_t Gdata;
	RTC_TimeTypeDef gTime;
	RTC_DateTypeDef gDate;
	Buoy_State_typedef Current_State;
	uint8_t Sample_On,sample_count,IMU_On;
	uint32_t Sample_Time_Start; //used to measure length of Routine_STATE_SAMPLE
	uint32_t Sample_Time; //used to measure length of Routine_STATE_SAMPLE


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
