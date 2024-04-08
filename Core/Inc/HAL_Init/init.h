#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"

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

/**
 * @brief SPI Initialization function
 * @param None 
 * @retval None
 * 
 */
void MX_SPI1_Init(void);

#ifdef __cplusplus
}
#endif
