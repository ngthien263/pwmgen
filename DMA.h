#ifndef DMA_H
#define DMA_H

#include "stm32f10x.h"

#define UART_DMA_BUFFER_SIZE  64  


void DMA_UART_Config(void);
void UART_DMA_Send(uint8_t *data, uint16_t size);
volatile char* get_DMA_Buffer();
void eraseBuffer();
#endif 