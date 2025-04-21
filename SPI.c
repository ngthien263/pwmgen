// SPI.c
#include "SPI.h"
#include "GPIO.h"

void SPI1_Init(void) {
    // B?t clock SPI1, GPIOA, AFIO
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN | RCC_APB2ENR_SPI1EN | RCC_APB2ENR_AFIOEN;

    // PA5 (SCK), PA7 (MOSI): Alternate function push-pull
    GPIO_SetMode(GPIOA, 5, GPIO_ALT_MODE_50M_PP); // SCK
    GPIO_SetMode(GPIOA, 7, GPIO_ALT_MODE_50M_PP); // MOSI

    // PA6 (MISO) n?u c?n
    // GPIO_Config(GPIOA, 6, GPIO_INPUT_FLOATING); // n?u dùng

    // C?u hình SPI1
    SPI1->CR1 = 0;
    SPI1->CR1 |= SPI_CR1_MSTR;      // Master mode
    SPI1->CR1 |= SPI_CR1_SSM;       // Software slave management
    SPI1->CR1 |= SPI_CR1_SSI;       // Internal slave select
    SPI1->CR1 &= ~SPI_CR1_BR;      // Baud rate = F_PCLK/8 ~ 9MHz (72MHz/8)
    SPI1->CR1 &= ~SPI_CR1_CPOL;     // CPOL = 0
    SPI1->CR1 &= ~SPI_CR1_CPHA;     // CPHA = 0
    SPI1->CR1 |= SPI_CR1_SPE;       // Enable SPI
}

void SPI1_Transmit(uint8_t data) {
    while (!(SPI1->SR & SPI_SR_TXE));    // Wait until TX buffer empty
    SPI1->DR = data;
    while (!(SPI1->SR & SPI_SR_TXE));    // Wait again
    while (SPI1->SR & SPI_SR_BSY);       // Wait until not busy
}

uint8_t SPI1_Receive(void) {
    while (!(SPI1->SR & SPI_SR_TXE));
    SPI1->DR = 0xFF;                     // Send dummy
    while (!(SPI1->SR & SPI_SR_RXNE));
    return SPI1->DR;
}

uint8_t SPI1_TransmitReceive(uint8_t data) {
    while (!(SPI1->SR & SPI_SR_TXE));
    SPI1->DR = data;
    while (!(SPI1->SR & SPI_SR_RXNE));
    return SPI1->DR;
}
