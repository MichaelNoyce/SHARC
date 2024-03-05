/**
  ******************************************************************************
  * @file    Examples_LL/DAC/DAC_GenerateConstantSignal_TriggerSW_LP/Src/main.c
  * @author  MCD Application Team
  * @brief   This example describes how to use the DAC peripheral to generate
  *          a constant voltage signal with DAC low power feature
  *          sample-and-hold. To be effective, this mode requires to connect
  *          a capacitor on DAC channel output and sample-and-hold timings
  *          must be tuned in function of capacitor value.
  *          This example is based on the STM32L4xx DAC LL API;
  *          Peripheral initialization done using LL unitary services functions.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2017 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/** @addtogroup STM32L4xx_LL_Examples
  * @{
  */

/** @addtogroup DAC_GenerateConstantSignal_TriggerSW_LP
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/

/* Definitions of environment analog values */
  /* Value of analog reference voltage (Vref+), connected to analog voltage   */
  /* supply Vdda (unit: mV).                                                  */
  #define VDDA_APPLI                       ((uint32_t)3300)
  
/* Definitions of data related to this example */
  /* Full-scale digital value with a resolution of 12 bits (voltage range     */
  /* determined by analog voltage references Vref+ and Vref-,                 */
  /* refer to reference manual).                                              */
  #define DIGITAL_SCALE_12BITS             (__LL_DAC_DIGITAL_SCALE(LL_DAC_RESOLUTION_12B))

/* Definitions of data related to DAC low power mode sample-and-hold */
  /* LSI frequency (unit: Hertz) */
  #define LSI_FREQUENCY_HZ                   ((uint32_t)32000)

  /* Definitions of DAC mode sample-and-hold timings (unit: micro-seconds)    */
  /* Note: Timings are set to an arbitrary value, in a real application       */
  /*       they should be tuned in function of capacitor value connected      */
  /*       to DAC output (refer to reference manual).                         */
  /*       If no capacitor is connected on DAC channel output or if           */
  /*       inappropriate timings are set, the voltage appears as              */
  /*       discontinuous.                                                     */
  #define SAMPLE_AND_HOLD_TIMING_SAMPLE_US   ((uint32_t)6000)
  #define SAMPLE_AND_HOLD_TIMING_HOLD_US     ((uint32_t)2000)
  #define SAMPLE_AND_HOLD_TIMING_REFRESH_US  ((uint32_t)5000)

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
__IO uint8_t ubButtonPress = 0;
__IO uint8_t ubButtonPressCount = 0;

/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/
void     SystemClock_Config(void);
void     Configure_DAC(void);
void     Activate_DAC(void);
void     LED_Init(void);
void     LED_On(void);
void     LED_Off(void);
void     LED_Toggle(void);
void     LED_Blinking(uint32_t Period);
void     UserButton_Init(void);
void     WaitForUserButtonPress(void);

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
int main(void)
{
  /* Configure the system clock to 80 MHz */
  SystemClock_Config();
  
  /* Initialize LED1 */
  LED_Init();
  
  /* Initialize button in EXTI mode */
  UserButton_Init();
  
  /* Wait for User push-button press */
  WaitForUserButtonPress();
  
  /* Turn-off LED1 */
  LED_Off();
  
  /* Enable LSI (required for DAC mode sample-and-hold) */
  LL_RCC_LSI_Enable();
  
  /* Configure DAC channel */
  Configure_DAC();
  
  /* Activate DAC channel */
  Activate_DAC();
  
  /* Turn-on LED1 */
  LED_On();
  
  /* Infinite loop */
  while (1)
  {
    /* Note: LED state depending and DAC channel output are set into          */
    /*       function "UserButton_Callback()".                                */
  }
}

/**
  * @brief  Configure DAC (DAC instance: DAC1, DAC instance channel: channel1 )
  *         and GPIO used by DAC channel.
  * @note   This function configures the DAC channel but does not enable it,
  *         in order to optimize power consumption (DAC channel enabled only
  *         when needed).
  * @note   Peripheral configuration is minimal configuration from reset values.
  *         Thus, some useless LL unitary functions calls below are provided as
  *         commented examples - setting is default configuration from reset.
  * @param  None
  * @retval None
  */
void Configure_DAC(void)
{
  /*## Configuration of GPIO used by DAC channels ############################*/
  
  /* Enable GPIO Clock */
  LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOA); 
  
  /* Configure GPIO in analog mode to be used as DAC output */
  LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_4, LL_GPIO_MODE_ANALOG);
  
  /*## Configuration of NVIC #################################################*/
  /* Configure NVIC to enable DAC1 interruptions */
  NVIC_SetPriority(TIM6_DAC_IRQn, 0);
  NVIC_EnableIRQ(TIM6_DAC_IRQn);
  
  /*## Configuration of DAC ##################################################*/
  
  /* Enable DAC clock */
  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_DAC1);
  
  /* Set the mode for the selected DAC channel */
  // LL_DAC_SetMode(DAC1, LL_DAC_CHANNEL_1, LL_DAC_MODE_NORMAL_OPERATION);
  
  /* Select trigger source */
  LL_DAC_SetTriggerSource(DAC1, LL_DAC_CHANNEL_1, LL_DAC_TRIG_SOFTWARE);
  
  /* Set the output for the selected DAC channel */
  LL_DAC_ConfigOutput(DAC1, LL_DAC_CHANNEL_1, LL_DAC_OUTPUT_MODE_SAMPLE_AND_HOLD, LL_DAC_OUTPUT_BUFFER_ENABLE, LL_DAC_OUTPUT_CONNECT_GPIO);
  
  /* Note: DAC channel output configuration can also be done using            */
  /*       LL unitary functions:                                              */
  // LL_DAC_SetOutputMode(DAC1, LL_DAC_CHANNEL_1, LL_DAC_OUTPUT_MODE_SAMPLE_AND_HOLD);
  // LL_DAC_SetOutputBuffer(DAC1, LL_DAC_CHANNEL_1, LL_DAC_OUTPUT_BUFFER_ENABLE);
  // LL_DAC_SetOutputConnection(DAC1, LL_DAC_CHANNEL_1, LL_DAC_OUTPUT_CONNECT_GPIO);
  
  /* Set DAC mode sample-and-hold timings */
  LL_DAC_SetSampleAndHoldSampleTime (DAC1, LL_DAC_CHANNEL_1, (SAMPLE_AND_HOLD_TIMING_SAMPLE_US  * LSI_FREQUENCY_HZ / 1000000));
  LL_DAC_SetSampleAndHoldHoldTime   (DAC1, LL_DAC_CHANNEL_1, (SAMPLE_AND_HOLD_TIMING_HOLD_US    * LSI_FREQUENCY_HZ / 1000000));
  LL_DAC_SetSampleAndHoldRefreshTime(DAC1, LL_DAC_CHANNEL_1, (SAMPLE_AND_HOLD_TIMING_REFRESH_US * LSI_FREQUENCY_HZ / 1000000));
  
  /* Disable DAC channel DMA request */
  // LL_DAC_DisableDMAReq(DAC1, LL_DAC_CHANNEL_1);
  
  /* Set the data to be loaded in the data holding register */
  // LL_DAC_ConvertData12RightAligned(DAC1, LL_DAC_CHANNEL_1, 0x000);
  
  /* Enable interruption DAC channel1 underrun */
  LL_DAC_EnableIT_DMAUDR1(DAC1);
}

/**
  * @brief  Perform DAC activation procedure to make it ready to generate
  *         a voltage (DAC instance: DAC1).
  * @note   Operations:
  *         - Enable DAC instance channel
  *         - Wait for DAC instance channel startup time
  * @param  None
  * @retval None
  */
void Activate_DAC(void)
{
  __IO uint32_t wait_loop_index = 0;
  
  /* Enable DAC channel */
  LL_DAC_Enable(DAC1, LL_DAC_CHANNEL_1);
  
  /* Delay for DAC channel voltage settling time from DAC channel startup.    */
  /* Compute number of CPU cycles to wait for, from delay in us.              */
  /* Note: Variable divided by 2 to compensate partially                      */
  /*       CPU processing cycles (depends on compilation optimization).       */
  /* Note: If system core clock frequency is below 200kHz, wait time          */
  /*       is only a few CPU processing cycles.                               */
  wait_loop_index = ((LL_DAC_DELAY_STARTUP_VOLTAGE_SETTLING_US * (SystemCoreClock / (100000 * 2))) / 10);
  while(wait_loop_index != 0)
  {
    wait_loop_index--;
  }
  
  /* Enable DAC channel trigger */
  /* Note: DAC channel conversion can start from trigger enable:              */
  /*       - if DAC channel trigger source is set to SW:                      */
  /*         DAC channel conversion will start after trig order               */
  /*         using function "LL_DAC_TrigSWConversion()".                      */
  /*       - if DAC channel trigger source is set to external trigger         */
  /*         (timer, ...):                                                    */
  /*         DAC channel conversion can start immediately                     */
  /*         (after next trig order from external trigger)                    */
  LL_DAC_EnableTrigger(DAC1, LL_DAC_CHANNEL_1);
}

/**
  * @brief  Initialize LED1.
  * @param  None
  * @retval None
  */
void LED_Init(void)
{
  /* Enable the LED1 Clock */
  LED1_GPIO_CLK_ENABLE();

  /* Configure IO in output push-pull mode to drive external LED1 */
  LL_GPIO_SetPinMode(LED1_GPIO_PORT, LED1_PIN, LL_GPIO_MODE_OUTPUT);
  /* Reset value is LL_GPIO_OUTPUT_PUSHPULL */
  //LL_GPIO_SetPinOutputType(LED1_GPIO_PORT, LED1_PIN, LL_GPIO_OUTPUT_PUSHPULL);
  /* Reset value is LL_GPIO_SPEED_FREQ_LOW */
  //LL_GPIO_SetPinSpeed(LED1_GPIO_PORT, LED1_PIN, LL_GPIO_SPEED_FREQ_LOW);
  /* Reset value is LL_GPIO_PULL_NO */
  //LL_GPIO_SetPinPull(LED1_GPIO_PORT, LED1_PIN, LL_GPIO_PULL_NO);
}

/**
  * @brief  Turn-on LED1.
  * @param  None
  * @retval None
  */
void LED_On(void)
{
  /* Turn LED1 on */
  LL_GPIO_SetOutputPin(LED1_GPIO_PORT, LED1_PIN);
}

/**
  * @brief  Turn-off LED1.
  * @param  None
  * @retval None
  */
void LED_Off(void)
{
  /* Turn LED1 off */
  LL_GPIO_ResetOutputPin(LED1_GPIO_PORT, LED1_PIN);
}

/**
  * @brief  Toggle LED1.
  * @param  None
  * @retval None
  */
void LED_Toggle(void)
{
  /* Toggle LED1 */
  LL_GPIO_TogglePin(LED1_GPIO_PORT, LED1_PIN);
}

/**
  * @brief  Set LED1 to Blinking mode for an infinite loop (toggle period based on value provided as input parameter).
  * @param  Period : Period of time (in ms) between each toggling of LED
  *   This parameter can be user defined values. Pre-defined values used in that example are :
  *     @arg LED_BLINK_FAST : Fast Blinking
  *     @arg LED_BLINK_SLOW : Slow Blinking
  *     @arg LED_BLINK_ERROR : Error specific Blinking
  * @retval None
  */
void LED_Blinking(uint32_t Period)
{
  /* Turn LED1 on */
  LL_GPIO_SetOutputPin(LED1_GPIO_PORT, LED1_PIN);
  
  /* Toggle IO in an infinite loop */
  while (1)
  {
    LL_GPIO_TogglePin(LED1_GPIO_PORT, LED1_PIN);  
    LL_mDelay(Period);
  }
}

/**
  * @brief  Configures User push-button in EXTI Line Mode.
  * @param  None
  * @retval None
  */
void UserButton_Init(void)
{
  /* Enable the BUTTON Clock */
  USER_BUTTON_GPIO_CLK_ENABLE();
  
  /* Configure GPIO for BUTTON */
  LL_GPIO_SetPinMode(USER_BUTTON_GPIO_PORT, USER_BUTTON_PIN, LL_GPIO_MODE_INPUT);
  LL_GPIO_SetPinPull(USER_BUTTON_GPIO_PORT, USER_BUTTON_PIN, LL_GPIO_PULL_NO);
  
  /* Connect External Line to the GPIO */
  USER_BUTTON_SYSCFG_SET_EXTI();
  
  /* Enable a rising trigger EXTI line 13 Interrupt */
  USER_BUTTON_EXTI_LINE_ENABLE();
  USER_BUTTON_EXTI_FALLING_TRIG_ENABLE();
  
  /* Configure NVIC for USER_BUTTON_EXTI_IRQn */
  NVIC_EnableIRQ(USER_BUTTON_EXTI_IRQn); 
  NVIC_SetPriority(USER_BUTTON_EXTI_IRQn,0x03);  
  
}

/**
  * @brief  Wait for User push-button press to start transfer.
  * @param  None 
  * @retval None
  */
void WaitForUserButtonPress(void)
{
  while (ubButtonPress == 0)
  {
    LL_GPIO_TogglePin(LED1_GPIO_PORT, LED1_PIN);
    LL_mDelay(LED_BLINK_FAST);
  }
}

/**
  * @brief  System Clock Configuration
  *         The system Clock is configured as follows :
  *            System Clock source            = PLL (MSI)
  *            SYSCLK(Hz)                     = 80000000
  *            HCLK(Hz)                       = 80000000
  *            AHB Prescaler                  = 1
  *            APB1 Prescaler                 = 1
  *            APB2 Prescaler                 = 1
  *            MSI Frequency(Hz)              = 4000000
  *            PLL_M                          = 1
  *            PLL_N                          = 40
  *            PLL_R                          = 2
  *            Flash Latency(WS)              = 4
  * @param  None
  * @retval None
  */
void SystemClock_Config(void)
{
  /* MSI configuration and activation */
  LL_FLASH_SetLatency(LL_FLASH_LATENCY_4);
  LL_RCC_MSI_Enable();
  while(LL_RCC_MSI_IsReady() != 1) 
  {
  };
  
  /* Main PLL configuration and activation */
  LL_RCC_PLL_ConfigDomain_SYS(LL_RCC_PLLSOURCE_MSI, LL_RCC_PLLM_DIV_1, 40, LL_RCC_PLLR_DIV_2);
  LL_RCC_PLL_Enable();
  LL_RCC_PLL_EnableDomain_SYS();
  while(LL_RCC_PLL_IsReady() != 1) 
  {
  };
  
  /* Sysclk activation on the main PLL */
  LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);
  LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL);
  while(LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_PLL) 
  {
  };
  
  /* Set APB1 & APB2 prescaler*/
  LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_1);
  LL_RCC_SetAPB2Prescaler(LL_RCC_APB2_DIV_1);

  /* Set systick to 1ms in using frequency set to 80MHz */
  /* This frequency can be calculated through LL RCC macro */
  /* ex: __LL_RCC_CALC_PLLCLK_FREQ(__LL_RCC_CALC_MSI_FREQ(LL_RCC_MSIRANGESEL_RUN, LL_RCC_MSIRANGE_6), 
                                  LL_RCC_PLLM_DIV_1, 40, LL_RCC_PLLR_DIV_2)*/
  LL_Init1msTick(80000000);
  
  /* Update CMSIS variable (which can be updated also through SystemCoreClockUpdate function) */
  LL_SetSystemCoreClock(80000000);
}

/******************************************************************************/
/*   USER IRQ HANDLER TREATMENT                                               */
/******************************************************************************/
/**
  * @brief  Function to manage IRQ Handler
  * @param  None
  * @retval None
  */
void UserButton_Callback(void)
{
  uint32_t tmp_dac_value = 0;
  
  /* On the first press on user button, update only user button variable      */
  /* to manage waiting function.                                              */
  /* Then, on next presses on user button, update DAC settings.               */
  if(ubButtonPress == 0)
  {
    /* Update User push-button variable : to be checked in waiting loop in main program */
    ubButtonPress = 1;
  }
  else
  {
    ubButtonPressCount++;
    
    /* Set value for DAC output */
    if(ubButtonPressCount < 4)
    {
      /* Increase amplitude by a quarter each time button is pushed */
      tmp_dac_value = DIGITAL_SCALE_12BITS * ubButtonPressCount/4;
      
      /* Toggle LED2 */
      LED_Toggle();
    }
    else
    {
      /* Final amplitude: end of example */
      tmp_dac_value = DIGITAL_SCALE_12BITS;

      /* Turn-on LED2 */
      LED_On(); 
    }
    
    /* Set the data to be loaded in the data holding register */
    LL_DAC_ConvertData12RightAligned(DAC1, LL_DAC_CHANNEL_1, tmp_dac_value);
    
    /* Trig DAC conversion by software */
    LL_DAC_TrigSWConversion(DAC1, LL_DAC_CHANNEL_1);
  }
}

/**
  * @brief  DAC underrun interruption callback
  * @note   This function is executed when DAC channel underrun error occurs.
  * @retval None
  */
void DacUnderrunError_Callback(void)
{
  /* Note: Disable DAC interruption that caused this error before entering in */
  /*       infinite loop below.                                               */
  
  /* Disable interruption DAC channel1 underrun */
  LL_DAC_DisableIT_DMAUDR1(DAC1);
  
  /* Error from ADC */
  LED_Blinking(LED_BLINK_ERROR);
}

#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

/**
  * @}
  */

/**
  * @}
  */

