

#pragma once
#include <stdint.h>

typedef void (*uart_rx_callback_t)(char c);
void usart_init(uart_rx_callback_t cb);
void usart_transmit(uint8_t *data, uint16_t len);
char usart_receive(void);
void usart_set_rx_callback(uart_rx_callback_t cb);

