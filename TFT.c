#include "TFT.h"
#include "GPIO.h"
#include "SPI.h"
#include "font5x7.h"
#include "string.h"
#include <stdlib.h>
void delay_ms(uint32_t ms) {
    for (volatile uint32_t i = 0; i < ms * 8000; i++);
}

void TFT_WriteCommand(uint8_t cmd) {
    GPIO_ResetBits(TFT_GPIO, 1 << TFT_DC_PIN);
    GPIO_ResetBits(TFT_GPIO, 1 << TFT_CS_PIN);
    SPI1_Transmit(cmd);
    GPIO_SetBits(TFT_GPIO, 1 << TFT_CS_PIN);
}

void TFT_WriteData(uint8_t data) {
    GPIO_SetBits(TFT_GPIO, 1 << TFT_DC_PIN);
    GPIO_ResetBits(TFT_GPIO, 1 << TFT_CS_PIN);
    SPI1_Transmit(data);
    GPIO_SetBits(TFT_GPIO, 1 << TFT_CS_PIN);
}

void TFT_Reset(void) {
    GPIO_SetBits(TFT_GPIO, 1 << TFT_RST_PIN);
    delay_ms(100);
    GPIO_ResetBits(TFT_GPIO, 1 << TFT_RST_PIN);
    delay_ms(100);
    GPIO_SetBits(TFT_GPIO, 1 << TFT_RST_PIN);
    delay_ms(120);
}

void TFT_SetAddressWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1) {
    TFT_WriteCommand(0x2A);
    TFT_WriteData(x0 >> 8);
    TFT_WriteData(x0 & 0xFF);
    TFT_WriteData(x1 >> 8);
    TFT_WriteData(x1 & 0xFF);

    TFT_WriteCommand(0x2B);
    TFT_WriteData(y0 >> 8);
    TFT_WriteData(y0 & 0xFF);
    TFT_WriteData(y1 >> 8);
    TFT_WriteData(y1 & 0xFF);

    TFT_WriteCommand(0x2C);
}

void TFT_Init(void) {
    // Kh?i t?o SPI và GPIO di?u khi?n
    SPI1_Init();

    GPIO_SetMode(TFT_GPIO, TFT_CS_PIN, GPIO_OUTPUT_MODE_2M_PP);
    GPIO_SetMode(TFT_GPIO, TFT_DC_PIN, GPIO_OUTPUT_MODE_2M_PP);
    GPIO_SetMode(TFT_GPIO, TFT_RST_PIN, GPIO_OUTPUT_MODE_2M_PP);

    TFT_Reset();

    TFT_WriteCommand(0x36);  // Command d? thi?t l?p hu?ng màn hình
    TFT_WriteData(0x00);     // Thay d?i giá tr? này n?u mu?n thay d?i hu?ng màn hình

    TFT_WriteCommand(0x3A);  // Command d? thi?t l?p màu
    TFT_WriteData(0x55);     // Cài d?t ch? d? màu 16-bit, có th? thay d?i tu? vào c?u hình c?a b?n

    TFT_WriteCommand(0xB2);  // Command thi?t l?p gamma
    TFT_WriteData(0x0C);     // Giá tr? gamma có th? thay d?i tùy theo yêu c?u
    TFT_WriteData(0x0C);
    TFT_WriteData(0x00);
    TFT_WriteData(0x33);
    TFT_WriteData(0x33);

    TFT_WriteCommand(0xB7);  // Command di?u ch?nh t?n s? quét
    TFT_WriteData(0x35);

    TFT_WriteCommand(0xBB);  // Command thi?t l?p di?n áp
    TFT_WriteData(0x19);     // Giá tr? di?n áp có th? thay d?i

    TFT_WriteCommand(0xC0);  // Command thi?t l?p d? sáng
    TFT_WriteData(0x2C);     // Thay d?i d? di?u ch?nh d? sáng

    TFT_WriteCommand(0xC2);  // Command thi?t l?p di?u ki?n ngu?n di?n
    TFT_WriteData(0x01);

    TFT_WriteCommand(0xC3);  // Command d? thi?t l?p hi?u su?t màu
    TFT_WriteData(0x12);
    TFT_WriteCommand(0xC4);  // Command d? thi?t l?p thông s? hi?u su?t màu
    TFT_WriteData(0x20);

    TFT_WriteCommand(0xC6);  // Command thi?t l?p d? sáng
    TFT_WriteData(0x0F);

    TFT_WriteCommand(0xD0);  // Command thi?t l?p d? tuong ph?n
    TFT_WriteData(0xA4);     // Có th? thay d?i giá tr? này tùy theo yêu c?u
    TFT_WriteData(0xA1);

    TFT_WriteCommand(0x20);  // Inversion ON
    TFT_WriteCommand(0x11);  // Sleep out
    delay_ms(120);

    TFT_WriteCommand(0x29);  // Display ON
    delay_ms(20);
}


void TFT_DrawPixel(uint16_t x, uint16_t y, uint16_t color) {
    // Chuy?n t?a d? t? portrait sang landscape n?u c?n
    // Xoay 90 d? ngu?c chi?u kim d?ng h?
    uint16_t newX = TFT_WIDTH - y;
    uint16_t newY = x;

    if (newX >= TFT_WIDTH || newY >= TFT_HEIGHT) return;
    TFT_SetAddressWindow(newX, newY, newX, newY);
    TFT_WriteData(color >> 8);
    TFT_WriteData(color & 0xFF);
}


void TFT_FillScreen(uint16_t color) {
    TFT_SetAddressWindow(0, 0, TFT_WIDTH - 1, TFT_HEIGHT - 1);
    for (uint32_t i = 0; i < (uint32_t)TFT_WIDTH * TFT_HEIGHT; i++) {
        TFT_WriteData(color >> 8);
        TFT_WriteData(color & 0xFF);
    }
}

void TFT_DrawChar(uint16_t x, uint16_t y, char c, uint16_t color, uint8_t scale) {
    if (c < 32 || c > 126) return;

    for (uint8_t i = 0; i < 5; i++) {
        uint8_t line = font5x7[c - 32][i];
        for (uint8_t j = 0; j < 8; j++) {
            if (line & (1 << j)) {
                // V? 1 di?m thành 1 kh?i scale x scale pixel
                for (uint8_t dx = 0; dx < scale; dx++) {
                    for (uint8_t dy = 0; dy < scale; dy++) {
                        TFT_DrawPixel(x + i * scale + dx, y + j * scale + dy, color);
                    }
                }
            }
        }
    }
}

void TFT_DrawString(uint16_t x, uint16_t y, const char* str, uint16_t color, uint8_t scale) {
    while (*str) {
        TFT_DrawChar(x, y, *str++, color, scale);
        x += 6 * scale;
        if (x + 6 * scale > TFT_HEIGHT) {
            x = 0;
            y += 8 * scale;
        }
    }
}

void TFT_FillRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color) {
    for (uint16_t i = 0; i < w; i++) {
        for (uint16_t j = 0; j < h; j++) {
            uint16_t px = x + i;
            uint16_t py = y + j;

            // Chuy?n to? d? nhu trong TFT_DrawPixel
            uint16_t newX = TFT_WIDTH - py;
            uint16_t newY = px;

            if (newX < TFT_WIDTH && newY < TFT_HEIGHT) {
                TFT_SetAddressWindow(newX, newY, newX, newY);
                TFT_WriteData(color >> 8);
                TFT_WriteData(color & 0xFF);
            }
        }
    }
}

void TFT_DrawLine(int x0, int y0, int x1, int y1, uint16_t color) {
    int dx = abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
    int dy = -abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
    int err = dx + dy, e2; // error value

    while (1) {
        TFT_DrawPixel(x0, y0, color);  // V? di?m hi?n t?i

        if (x0 == x1 && y0 == y1) break;
        e2 = 2 * err;
        if (e2 >= dy) { err += dy; x0 += sx; }
        if (e2 <= dx) { err += dx; y0 += sy; }
    }
}


void clearString(char *str, uint16_t x, uint16_t y, uint8_t scale) {
    uint16_t len = strlen(str);
    uint16_t charW = 6 * scale;
    uint16_t charH = 8 * scale;
    TFT_FillRect(x, y, (len + 1) * charW, charH, COLOR_BLACK);
}

