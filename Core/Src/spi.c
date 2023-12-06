#include "stm32f4xx_hal.h"
#include "FreeRTOSConfig.h"
SPI_HandleTypeDef 		hspi;

/* SPI2 pins, stm32 dioscovery board  */
#define SPI2_SCK_Pin   GPIO_PIN_13 /* SPI Serial Clock */
#define SPI2_SCK_Port  GPIOB       /* SPI2 clock GPIO port */
#define SPI2_MOSI_Pin  GPIO_PIN_15 /* SPI Master Out Slave In */
#define SPI2_MOSI_Port GPIOB       /* SPI2 MOSI GPIO port */
#define SPI2_MISO_Pin  GPIO_PIN_2  /* SPI Master In Slave Out */
#define SPI2_MISO_Port GPIOC       /* SPI2 MISO GPIO port */
#define SPI_PERIPH     SPI2        /* SPI2 Peripheral number */

/**
 * @brief Initialization of GPIO
 * @param  
 */
static void spi_gpio_init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  __HAL_RCC_GPIOA_CLK_ENABLE();

  /* Commoon configuration */
  GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull      = GPIO_NOPULL;
  GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF5_SPI2;

  /* Init SCK */
  GPIO_InitStruct.Pin = SPI2_SCK_Pin;
  HAL_GPIO_Init(SPI2_SCK_Port, &GPIO_InitStruct);

  /* Init MOSI */
  GPIO_InitStruct.Pin = SPI2_MOSI_Pin;
  HAL_GPIO_Init(SPI2_MOSI_Port, &GPIO_InitStruct);

  /* Init MISO */
  GPIO_InitStruct.Pin = SPI2_MISO_Pin;
  HAL_GPIO_Init(SPI2_MISO_Port, &GPIO_InitStruct);

  /* SPI1 interrupt Init */
  /* Configure priority to be lower than configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY, so as to allow interrupt safe calls to rtos API */
  HAL_NVIC_SetPriority(SPI1_IRQn, configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY + 1, 0);
}

/**
 * @brief Initialization of SPI peropheral
 * @param hspi 
 */
static void spi_periph_init(SPI_HandleTypeDef *hspi)
{
	/* Enabl clock */
	__HAL_RCC_SPI2_CLK_ENABLE();

  hspi->Instance               = SPI_PERIPH;
  hspi->Init.Mode              = SPI_MODE_MASTER;
  hspi->Init.Direction         = SPI_DIRECTION_2LINES;
  hspi->Init.DataSize          = SPI_DATASIZE_8BIT;
  hspi->Init.CLKPolarity       = SPI_POLARITY_LOW;
  hspi->Init.CLKPhase          = SPI_PHASE_1EDGE;
  hspi->Init.NSS               = SPI_NSS_SOFT;
  hspi->Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
  hspi->Init.FirstBit          = SPI_FIRSTBIT_MSB;
  hspi->Init.TIMode            = SPI_TIMODE_DISABLE;
  hspi->Init.CRCCalculation    = SPI_CRCCALCULATION_DISABLE;
  hspi->Init.CRCPolynomial     = 7;
  HAL_SPI_Init(hspi);
}

/**
 * @brief Initialization of SPI peropheral and  GPIO
 * @param  
 */
void spi_init(void)
{
  spi_gpio_init();
  spi_periph_init(&hspi);
}

/**
 * @brief Transmit data over SPI
 * @param data data to transmit
 * @param len length of data to transmit
 * @return true if success
 * @return false if failure
 */
uint8_t spi_transmit(uint8_t* pdata, uint16_t len)
{
  HAL_StatusTypeDef retval = HAL_SPI_Transmit(&hspi, pdata, len, HAL_MAX_DELAY);
  return retval == HAL_OK;
}

/**
 * @brief Receive data over SPI
 * @param pbuff data buffer to receive data to
 * @param len length of data to receive
 * @note is blocking 
 * @return true if success

 */
uint8_t spi_receive(uint8_t* pbuff, uint16_t len)
{
  HAL_StatusTypeDef retval = HAL_SPI_Receive(&hspi, pbuff, len, HAL_MAX_DELAY);
  return retval == HAL_OK;
}
