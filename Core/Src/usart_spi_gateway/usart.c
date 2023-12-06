#include "stm32f4xx_hal.h"
#include "FreeRTOSConfig.h"
#include "usart.h"
#define USART_TX_PIN       GPIO_PIN_2
#define USART_TX_GPIO_PORT GPIOA

#define USART_RX_PIN       GPIO_PIN_3
#define USART_RX_GPIO_PORT GPIOA

#define USART_CK_PIN       GPIO_PIN_7
#define USART_CK_GPIO_PORT GPIOD

#define GPIO_USART_AF        GPIO_AF7_USART2
#define USART_BAUDRATE     115200
#define USART_NUM          USART2
#define USART_IRQn         USART2_IRQn
#define USART_IRQ_PRIORITY configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY + 1 /* lower priority than the kernel */

USART_HandleTypeDef husart;
uart_rx_callback_t usart_rx_cb_fn = NULL;

static inline void usart_io_cfg(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* Enable GPIO clocks */
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();

  /* Com,mon configuration of all used GPIO pins */
  GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull      = GPIO_NOPULL;
  GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_USART_AF;

  /* TX pin configuration */
  GPIO_InitStruct.Pin = USART_TX_PIN;
  HAL_GPIO_Init(USART_TX_GPIO_PORT, &GPIO_InitStruct);

  /* RX pin configuration */
  GPIO_InitStruct.Pin = USART_RX_PIN;
  HAL_GPIO_Init(USART_RX_GPIO_PORT, &GPIO_InitStruct);

  /* CK pin configuration */
  GPIO_InitStruct.Pin = USART_CK_PIN;
  HAL_GPIO_Init(USART_CK_GPIO_PORT, &GPIO_InitStruct);
}

static inline void usart_periph_cfg(void)
{
  __HAL_RCC_USART2_CLK_ENABLE();
  husart.Instance        = USART_NUM;
  husart.Init.BaudRate   = 115200;
  husart.Init.WordLength = USART_WORDLENGTH_8B;
  husart.Init.StopBits   = USART_STOPBITS_1;
  husart.Init.Parity     = USART_PARITY_NONE;
  husart.Init.Mode       = USART_MODE_TX_RX;
  HAL_USART_Init(&husart);

  // Enable USART interrupt
  HAL_NVIC_SetPriority(USART_IRQn, USART_IRQ_PRIORITY, 0);
  __HAL_USART_ENABLE_IT(&husart, USART_IT_RXNE);
  HAL_NVIC_EnableIRQ(USART_IRQn);
}

void usart_init(uart_rx_callback_t cb)
{
  usart_io_cfg();
  usart_periph_cfg();
  usart_rx_cb_fn = cb;
}

/**
 * @brief Set the callback function to be called when a character is received.
 * @param cb callback function \ref uart_rx_callback_t
 */
void usart_set_rx_callback(uart_rx_callback_t cb)
{
  usart_rx_cb_fn = cb;
}

/**
 * @brief USART interrupt handler.
 * This function handles the USART interrupt. When a character is received, it is sent to the SPI task via a queue.
 */
void USART2_IRQHandler(void) {
  char received_char = 0;
  if (__HAL_USART_GET_FLAG(&husart, USART_FLAG_RXNE) != RESET)
  {
    USART_NUM->SR &= ~USART_FLAG_RXNE;
    received_char = (char)(USART_NUM->DR & (uint8_t)0xFF); /* Reading from DR clears flag */
    if (usart_rx_cb_fn)
    {
      usart_rx_cb_fn(received_char);
    }
  }
  // End the interrupt in the NVIC
  HAL_NVIC_ClearPendingIRQ(USART_IRQn);
}


/**
 * @brief Transmit data to the UART peripheral.
 * @param pdata pointer to the data buffer
 * @param len length of the data
 */
void usart_transmit(uint8_t *pdata, uint16_t len)
{
  HAL_USART_Transmit(&husart, pdata, len, HAL_MAX_DELAY);
}


