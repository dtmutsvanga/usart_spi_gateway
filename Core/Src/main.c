
#include "main.h"
#include "cmsis_os.h"
#include "sys.h"
#include "app_usart_spi_gateway.h"

/* Dummy task to initialize LED1 on stm32f4discovery and toggle it every 1s */
void led_task(void *argument)
{
  /* Initialize LED  as output */
  while (1)
  {
    toggle_led();
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}

/******************************************************************************************************/
/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  sys_init();
  /* Start led task */
  xTaskCreate(led_task, "LED task", configMINIMAL_STACK_SIZE, NULL, 1, NULL);
  usart_spi_gateway_init();
  /* Start schedular  */
  vTaskStartScheduler();
  while (1)
  {
    /* Life sucks if we ever get here */
  }
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
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
