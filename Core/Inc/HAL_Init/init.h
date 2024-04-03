#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"
 
# define SD_Detect_Pin GPIO_PIN_5
# define SD_Detect_GPIO_Port GPIOF

void SystemClock_Config(void);
void MX_GPIO_Init(void);
void MX_LPUART1_UART_Init(void);
void MX_SDMMC1_SD_Init(void);
void MX_I2C1_Init(void);



#ifdef __cplusplus
}
#endif
