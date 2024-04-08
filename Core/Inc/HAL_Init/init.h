#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"

//=============================0. Definitions=====================================

//SD definitions
# define SD_Detect_Pin GPIO_PIN_5
# define SD_Detect_GPIO_Port GPIOF

//SPI Definitions
#define SPI_CS_Pin GPIO_PIN_4
#define SPI_CS_GPIO_Port GPIOA
#define BMP_SPI_SCLK_PIN GPIO_PIN_5
#define BMP_SPI_MISO_PIN GPIO_PIN_6
#define BMP_SPI_MOSI_PIN GPIO_PIN_7
#define BMP_SPI_GPIO_PORT GPIOA
#define BMP_SPI_PORT SPI1

// Custom flag set detection for Event Power on Reset (PWR_BORST, PWR_SWRST, PWR_NRST Set )
#define RCC_FLAG_PORRST (0b101<<26)
#define __HAL_RCC_GET_PORRST_FLAG() ((READ_REG(RCC->CSR)&(RCC_FLAG_PORRST))>>26)&&0b111

//define GPIO Port for LED2
#define LD2_GPIO_Port GPIOB

//define GPIO Pin for LED2
#define LD2_Pin GPIO_PIN_7

//IMU definitions
#define IMU_EVENT_WAKE_FLAG PWR_FLAG_WUF5	//flag definition
#define IMU_EVENT_WAKE_PIN PWR_WAKEUP_PIN5  //Pin Definition


//===================== 1. Macros===================================================
/**
 * @brief: state machine Macros
 */
//Returns the state the buoy was in prior to state check
#define __GET_PREV_STATE() (RTC->BKP0R)&0xFF

#define __SET_CURRENT_STATE(state) WRITE_REG(RTC->BKP0R,(RTC->BKP0R &0xFF00)|(state))

#define __GET_SAMPLE_COUNT()  (((RTC->BKP0R)&0xFF00)>>8)

#define __SET_SAMPLE_COUNT(count)  WRITE_REG(RTC->BKP0R, (RTC->BKP0R &0xFF)|((count)<<8))

//===================== 2. Type definitions ====================================

/*
 * 							PWR_MODE_t
 *
 * @brief: enumeration of possibled deep sleep modes. To be used as input for function
 * 		   static void Go_To_Sleep(PWR_MODE_t mode, uint32_t seconds)
 * @param SHUTDOWN - enter shutdown mode (0kb Ram etention, Power circuitry disabled)
 * @param STDBY	   - enter Standby mode (32KB RAM retention, power circuitry enabled)
 */

typedef enum
{
	SHUTDOWN,
	STDBY
}PWR_MODE_t;

/**
 * 							Buoy_State_typedef
 *
 * @brief: enumeration of states. Here, each abstract state is enumerated with a unique
 * 		   value. States are set in and read from byte 0 of RTC Back Up register 0. When
 * 		   the device enter shutdown/ standby mode, the back up domain is still supplied
 * 		   allowing for 1Kb of data retention in powerdown mode. More information is given
 * 		   in the document  State Machine Design.pdf
 *
 *@param  STATE_INIT		- Initialisation Mode:		Calibration and initialisation routines for first time start up
 * @param STATE_RESET		- Reset Mode:				system initailisad
 * @param STATE_SAMPLE  	- Data acquisition Mode:	Sensors initialised and sampled
 * @param STATE_SLEEP		- SLEEP MODE:				Buoy is in power saving mode with wake up pins active
 * @param STATE_TRANSMIT	- Transmit Mode:			Iridium Modem  activated and transmitted/recieved
 * @param STATE_ASYNCINT	- Asynchronous Interrupt:	Device recieved rising edge on one of 2 interrupt pins resulting in the activation of asynchronous routines
 */
typedef enum
{
	STATE_INIT			= 0b00,
	STATE_RESET 		= 0b01,
	STATE_SAMPLE 		= 0b10,
	STATE_SLEEP 		= 0b11,
	STATE_TRANSMIT  	= 0b110,
	STATE_ASYNCINT		= 0b111
}Buoy_State_typedef;

/**
 * @brief: enum structure contains values to be used as input to the function  void set_WUP_Pin(uint32_t Pin,PinMode_typedef mode),
 * 	 	   Used to set the type interrupt mode for the desired pin. Note: Functionality changes depending on the power mdoe
 * 	 	   of the buoy. Make sure the wake up pins are reconfigured after each power mode transition
 *
 * @param:  MODE_EXTI - Pin will be configured as an external interrupt pin on a rising trigger.
 *
 * @param:  MODE_WUP  - Pin will be configured as an external wake up pin
 */

typedef enum
{
   MODE_WUP,
   MODE_EXTI
} PinMode_typedef;

//============================ 3. Function Prototypes ============================

/**
 * @brief Configure system clock
 * 
 */
void SystemClock_Config(void);

/**
 * @brief Error handler
 * 
 */
void Error_Handler(void);

/**
 * @brief GPIO Initialization Function
 * @param None
 * @retval None
 */
void MX_GPIO_Init(void);

/**
 * @brief LPUART1 Initialization Function
 * @param None
 * @retval None
 */
void MX_LPUART1_UART_Init(void);

/**
 * @brief SDMMC1 Initialization Function
 * @param None
 * @retval None
 */

void MX_SDMMC1_SD_Init(void);
/**
 * @brief I2C1 Initialization Function
 * @param None
 * @retval None
 
 */

void MX_I2C1_Init(void);
/**
 * @brief RTC Initialization Function
 * @param None
 * @retval None
 */
void MX_RTC_Init(void);

/**
 * @brief SPI Initialization function
 * @param None 
 * @retval None
 * 
 */
void MX_SPI1_Init(void);

/**
 * @brief BROWN OUT RESET HANDLER:
 *
 * NB: Device must have Vbrownout threshold set in option bytes
 *
 *  Routines that runs when device recovers from a brown out Vbat < Vbrountout
 */
void BOR_Handler(void);

/**
 * @brief POWER ON RESET HANDLER:
 *
 * Routines that runs when device is powered on
 */
void POR_Handler(void);

/*
 * Function Name: void set_WUP_Pin(uint32_t Pin,PinMode_typedef mode);
 * @brief: This function initializes a pin to be used as an interrupt source to wake up the device from sleep mode
 * 			There are 5 wake up pins on the device. The following table shows the wake up pin mapping to GPIO pin port and
 * 			the input argument to the function to enable it
 * 			+------------------------------------------+
 * 			| Wake Up Pin | GPIO Pin | Argument        |
 * 			|-------------|----------|-----------------|
 * 			| WUP PIN 1   | PA0      | PWR_WAKEUP_PIN1 |
 * 			|-------------|----------|-----------------|
 * 			| WUP PIN 2   | PC13     | PWR_WAKEUP_PIN2 |
 * 			|-------------|----------|-----------------|
 * 			| WUP PIN 3   | PE6      | PWR_WAKEUP_PIN3 |
 * 			|-------------|----------|-----------------|
 * 			| WUP PIN 4   | PA2      | PWR_WAKEUP_PIN4 |
 * 			|-------------|----------|-----------------|
 * 			| WUP PIN 5   | PC5      | PWR_WAKEUP_PIN5 |
 * 			+------------------------------------------+
 *
 * @param: uint32_t Pin: Pin to be configured (Must be 1 value from the arguement column in the table above)
 *
 * @return: void
 */

void set_WUP_Pin(uint32_t Pin,PinMode_typedef mode);

#ifdef __cplusplus
}
#endif
