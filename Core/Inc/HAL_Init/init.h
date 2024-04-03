#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"

# define SD_Detect_Pin GPIO_PIN_5
# define SD_Detect_GPIO_Port GPIOF

/**
 * @brief Configure system clock
 * 
 */
void SystemClock_Config(void);
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



#ifdef __cplusplus
}
#endif
