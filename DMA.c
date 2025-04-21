#include "DMA.h"
#include "FreeRTOS.h"
#include "semphr.h"
#include <string.h>
#include "global.h"
#include "USART.h"

#define UART_TX_DMA_CHANNEL  DMA1_Channel4
#define UART_RX_DMA_CHANNEL  DMA1_Channel5


volatile char uart_rx_buffer[UART_DMA_BUFFER_SIZE];  // B? nh? nh?n d? li?u

volatile char* get_DMA_Buffer() {
    return uart_rx_buffer;
}



void DMA_UART_Config(void) {
    // 1. B?t Clock cho USART1 và DMA1
    //RCC->APB2ENR |= RCC_APB2ENR_USART1EN;  // B?t clock cho UART1
    RCC->AHBENR  |= RCC_AHBENR_DMA1EN;     // B?t clock cho DMA1

    // 3. C?u hình DMA1 Channel5 (USART1_RX)
    UART_RX_DMA_CHANNEL->CCR &= ~DMA_CCR1_EN;  // T?t DMA tru?c khi c?u hình

    UART_RX_DMA_CHANNEL->CCR = DMA_CCR1_MINC    // T? d?ng tang d?a ch? b? nh?
                             | DMA_CCR1_CIRC    // Ch? d? Circular (nh?n liên t?c)
                             | DMA_CCR1_TCIE;   // B?t ng?t khi nh?n xong

    // 5. Gán d?a ch? ngu?n & dích cho RX
    UART_RX_DMA_CHANNEL->CPAR = (uint32_t)&USART1->DR;  // Ngu?n: Thanh ghi USART_DR
    UART_RX_DMA_CHANNEL->CMAR = (uint32_t)uart_rx_buffer; // Ðích: B? nh? RAM
    UART_RX_DMA_CHANNEL->CNDTR = UART_DMA_BUFFER_SIZE;   // S? byte c?n nh?n

    // 6. B?t DMA UART RX
    UART_RX_DMA_CHANNEL->CCR |= DMA_CCR1_EN;

    // 7. Kích ho?t ch? d? DMA cho USART1
    USART1->CR3 |= USART_CR3_DMAT | USART_CR3_DMAR;  // B?t DMA TX và RX

    // 8. C?u hình NVIC cho DMA ng?t
    NVIC_SetPriority(DMA1_Channel5_IRQn, 5);
    NVIC_EnableIRQ(DMA1_Channel5_IRQn);  // Ng?t khi nh?n xong
}


void DMA1_Channel5_IRQHandler(void) {  // Ng?t khi nh?n xong
    if (DMA1->ISR & DMA_ISR_TCIF5) {
        DMA1->IFCR |= DMA_IFCR_CTCIF5;  // Xóa c? ng?t
        USART_str(USART1, (unsigned char*)"Da ngat\n");
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        xSemaphoreGiveFromISR(dmaUartSemaphore, &xHigherPriorityTaskWoken);
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);      
    }
}

void eraseBuffer() {
    memset((void*)uart_rx_buffer, 0, sizeof(uart_rx_buffer));
}

/*void UART_DMA_Send(uint8_t *data, uint16_t size) {
    UART_TX_DMA_CHANNEL->CCR &= ~DMA_CCR1_EN;  // T?t DMA tru?c khi c?u hình l?i

    UART_TX_DMA_CHANNEL->CMAR = (uint32_t)data; // Ð?a ch? b? nh? ngu?n
    UART_TX_DMA_CHANNEL->CNDTR = size;          // S? byte c?n truy?n

    UART_TX_DMA_CHANNEL->CCR |= DMA_CCR1_EN;  // B?t DMA d? b?t d?u truy?n
}*/



