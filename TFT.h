#ifndef __TFT_H__
#define __TFT_H__

#include "stm32f10x.h"
#include "GPIO.h"

// TFT commands
#define TFT_NOP     0x00
#define TFT_SWRESET 0x01
#define TFT_SLPIN   0x10
#define TFT_SLPOUT  0x11
#define TFT_DISPOFF 0x28
#define TFT_DISPON  0x29
#define TFT_CASET   0x2A
#define TFT_RASET   0x2B
#define TFT_RAMWR   0x2C
#define TFT_MADCTL  0x36
#define TFT_COLMOD  0x3A

#define COLOR_BLACK   0x0000
#define COLOR_WHITE   0xFFFF
#define COLOR_RED     0xF800
#define COLOR_GREEN   0x07E0
#define COLOR_BLUE    0x001F
#define COLOR_YELLOW  0xFFE0
#define COLOR_CYAN    0x07FF
#define COLOR_MAGENTA 0xF81F
#define COLOR_ORANGE  0xFD20


// Pin definitions
#define TFT_CS_PORT   GPIOA
#define TFT_CS_PIN    PIN4
#define TFT_DC_PORT   GPIOA
#define TFT_DC_PIN    PIN3
#define TFT_RST_PORT  GPIOA
#define TFT_RST_PIN   PIN2

// SPI configuration
#define TFT_SPI       SPI1
#define TFT_SPI_PORT  GPIOA
#define TFT_SCK_PORT  GPIOA
#define TFT_MOSI_PORT GPIOA
#define TFT_SCK_PIN   PIN5
#define TFT_MOSI_PIN  PIN7

// Screen dimensions
#define TFT_WIDTH  240
#define TFT_HEIGHT 320

// Ð?nh nghia chân k?t n?i (tu? ch?nh tu? b?n n?i)
#define TFT_GPIO     GPIOA

void TFT_Init(void);
void TFT_FillScreen(uint16_t color);
void TFT_DrawPixel(uint16_t x, uint16_t y, uint16_t color);
void TFT_DrawChar(uint16_t x, uint16_t y, char c, uint16_t color, uint8_t scale);
void TFT_DrawString(uint16_t x, uint16_t y, const char* str, uint16_t color, uint8_t scale);
void TFT_FillRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color);
void clearString(char *str, uint16_t x, uint16_t y, uint8_t scale);
void TFT_DrawLine(int x0, int y0, int x1, int y1, uint16_t color);


#endif /* __TFT_H__ */