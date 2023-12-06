#include <stdint.h>
#include <string.h>
#include "FreeRTOS.h"
#include "task.h"
#include "app_usart_spi_gateway.h"
#include "usart.h"
#include "spi.h"
#include "queue.h"
#include "stream_buffer.h"

#define LEN_QUEUE_USART_to_SPI     1023                       /**< Maximum size for dynamic frame (16 bits) */
#define LEN_MSGBUFFER_SPI_to_USART 1023                       /**< Maximum size for dynamic frame (16 bits) */
#define STRING_MAX_SIZE            255                        /**< Maximum length of a received string (16 bits) */

static QueueHandle_t        xQueue_USART_to_SPI  = NULL; /**< Queue for sending data from USART to SPI */
static StreamBufferHandle_t xBuffer_SPI_to_USART = NULL; /**< Message buffer for sending data from SPI to USART */

/**
 * @brief Get the lenght of the received data foir further transmission.
 * @param str received string
 * @param maxlen max length == size of the string buffer
 * @details 
 * @return 
 */
static inline uint32_t get_rx_len(char* str, uint32_t maxlen)
{
  char*    term_char = memchr(str, '\0', maxlen); /* Use memchr instead of strlen to avoid memory leaks */
  uint16_t rx_strlen = term_char != NULL ? (uint16_t)(term_char - str) + 1 : maxlen;
  return rx_strlen;
}

/**
 * @brief Callback function for USART.
 * @param rx_char received character
 */
static void usart_rx_callback(char rx_char)
{
  BaseType_t xHigherPriorityTaskWoken = pdFALSE;
  if (xQueue_USART_to_SPI)
  {
    xQueueSendFromISR(xQueue_USART_to_SPI, &rx_char, &xHigherPriorityTaskWoken);
  }
  // portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

/**
 * @brief Task for handling USART communication.
 * @param pvParameters: NULL. Not used 
 * This task checks if there is a string to send from the SPI task. If a string is received, it is sent to the USART.
 */
static void vTask_USART(void* pvParameters)
{

  static char uart_send_string[STRING_MAX_SIZE] = {'\0'};
  static char spi_send_string[STRING_MAX_SIZE]  = {'\0'};
  char        uart_rx_char                      = '\0';
  uint32_t    rx_len_uart_spi                   = 0;
  uint32_t    rx_len_spi_uart                   = 0;

  while (1)
  {
    /* Check if there is a string to send from SPI task */
    rx_len_spi_uart = xStreamBufferReceive(xBuffer_SPI_to_USART, uart_send_string, STRING_MAX_SIZE, (TickType_t)0);
    if (rx_len_spi_uart > 0)
    {
      /* Send received string to USART */
      usart_transmit((uint8_t*)uart_send_string, rx_len_spi_uart);
    }

    /* Check if there is a char to send from USART task */
    if (xQueueReceive(xQueue_USART_to_SPI, &uart_rx_char, (TickType_t)0))
    {
      /*  Add received char to send_string */
      strncat(spi_send_string, &uart_rx_char, 1);
      if (uart_rx_char)
      {
        rx_len_uart_spi++; /*  in case of receiving multiple \0 frames */
      }

       /* If received char is NULL, send the string to SPI slave device */
      if ((uart_rx_char == '\0' || rx_len_uart_spi + 1 >= STRING_MAX_SIZE)  /* in case buffer full, +1 for NULL */
          && rx_len_uart_spi > 0)
      {
        spi_transmit((uint8_t*)spi_send_string, rx_len_uart_spi);
        spi_send_string[0] = '\0';  /* Reset send_string */
        uart_rx_char       = 0;
      }
    }
  }
}

/**
 * @brief Task for handling SPI communication.
 * 
 * This task checks if there is a character to send from the USART task. If a character is received, it is added to a string. If the received character is NULL, the string is sent to the SPI slave device.
 */
static void vTask_SPI(void *pvParameters) {
  uint32_t    rx_strlen                    = 0;
  static char send_string[STRING_MAX_SIZE] = "\0"; 
  while (1)
  {
    /* Receive string from SPI slave device */
    spi_receive((uint8_t*)send_string, STRING_MAX_SIZE);
     /* If received string is not NULL, send it to USART task via queue */
    if (send_string[0] != '\0')
    {
      rx_strlen = get_rx_len(send_string, STRING_MAX_SIZE);
       /* TODO: Handle errors from streambuffer */
      xStreamBufferSend(xBuffer_SPI_to_USART, send_string, rx_strlen, portMAX_DELAY);
      rx_strlen      = 0;
      send_string[0] = '\0'; /*  Reset send_string */
    }
  }
}

/**
 * @brief Create rtos tasks and queues for transfering data
 * @param  
 */
static inline void create_tasks(void)
{
  xQueue_USART_to_SPI  = xQueueCreate(LEN_QUEUE_USART_to_SPI, sizeof(char));
  xBuffer_SPI_to_USART = xStreamBufferCreate(LEN_MSGBUFFER_SPI_to_USART, sizeof(char));
  xTaskCreate(vTask_USART, "USART Task", configMINIMAL_STACK_SIZE * 2, NULL, tskIDLE_PRIORITY + 1, NULL);
  xTaskCreate(vTask_SPI, "SPI Task", configMINIMAL_STACK_SIZE * 2, NULL, tskIDLE_PRIORITY + 1, NULL);
}

/**
 * @brief Initalize the gateway 
 * @param  
 */
void usart_spi_gateway_init(void)
{
    create_tasks();
    usart_init(usart_rx_callback);
    spi_init();
}