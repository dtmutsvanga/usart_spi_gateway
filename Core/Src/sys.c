#include "stm32f4xx_hal.h"
#define LD2_GPIO_Port GPIOD
#define LD2_Pin GPIO_PIN_12

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */ 
 static void error_handler(void)
{
  __disable_irq();
  while (1)
  {
  }
}
/**
 * @brief Initialize LED GPIO_PIN_12 GPIOD
 * @param  
 */
static void intialize_led(void)
{
  /* Initialize GPIO D clock */
  __HAL_RCC_GPIOD_CLK_ENABLE();
  /* Initialize LED  as output */
  GPIO_InitTypeDef GPIO_InitStruct;            // create a GPIO_InitTypeDef structure
  GPIO_InitStruct.Pin = LD2_Pin;           // select pin 12
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;  // set mode as output push-pull
  GPIO_InitStruct.Pull = GPIO_NOPULL;          // disable pull-up or pull-down resistor
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW; // set speed as low frequency
  HAL_GPIO_Init(LD2_GPIO_Port, &GPIO_InitStruct);      // initialize GPIO port D with the configuration
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
static void sys_clk_cfg(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 50;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
  RCC_OscInitStruct.PLL.PLLQ = 7;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    error_handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    error_handler();
  }
}

/**
 * @brief Toggle led
 * @param  
 */
void toggle_led(void)
{
  HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
}

/**
 * @brief Initialize hal, clokcs and led
 * @param  
 */
void sys_init(void)
{
  HAL_Init();
  sys_clk_cfg();
  intialize_led();
}

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM1 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  if (htim->Instance == TIM1) {
    HAL_IncTick();
  }
}