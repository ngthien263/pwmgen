// SPI.h
#ifndef __SPI_H
#define __SPI_H

#include "stm32f10x.h"

void SPI1_Init(void);
void SPI1_Transmit(uint8_t data);
uint8_t SPI1_Receive(void);
uint8_t SPI1_TransmitReceive(uint8_t data);

#endif
