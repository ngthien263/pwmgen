#include "stm32f10x.h"
#include "TFT.h"
#include "control.h"
#include <stdio.h>
#include "button.h"
#include "Timer.h"
#include "RCC.h"
#include <math.h>
#include <string.h>

#define M_PI 3.14
//void SystemInit(){
//}
void drawSineWaveform(uint16_t startX, uint16_t startY, uint16_t width, uint16_t height, uint8_t cycles);
void avgVoltage(int duty);
void clearCString(char* str, int maxLength) {
    for (int i = 0; i < maxLength; ++i) {
        str[i] = '\0';
    }
}

void EXTI0_Init(void) {
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;    // Clock GPIOA
    RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;    // Clock AFIO

    GPIOA->CRL &= ~GPIO_CRL_MODE0;         // Input mode
    GPIOA->CRL &= ~GPIO_CRL_CNF0;
    GPIOA->CRL |=  GPIO_CRL_CNF0_1;        // Input pull-up/down
    GPIOA->ODR |=  GPIO_ODR_ODR0;          // Pull-up

    AFIO->EXTICR[0] &= ~AFIO_EXTICR1_EXTI0; // EXTI0 dùng PA0

    EXTI->IMR  |= EXTI_IMR_MR0;            // Cho phép ng?t EXTI0
    EXTI->FTSR |= EXTI_FTSR_TR0;           // Su?n xu?ng
    EXTI->RTSR |= EXTI_RTSR_TR0;           // Su?n lên

    NVIC_EnableIRQ(EXTI0_IRQn);            // Cho phép ng?t EXTI0 trong NVIC
}

void TIM1_PinConfig() {
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
    GPIO_SetMode(GPIOA, 8, GPIO_ALT_MODE_50M_PP);
}

void TIM2_PinConfig() {
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
    GPIO_SetMode(GPIOA, 8, GPIO_ALT_MODE_50M_PP);
}

// Initialize PWM output for Timer 1
void TIM1_PWMOC(uint32_t Frequency, uint32_t Duty_Cycle) {
    TIM1_PinConfig();
    TIM_OC1_PWMM1_Init(TIM1, Frequency, Duty_Cycle);
}

void TIM2_PWMOC(uint32_t Frequency, uint32_t Duty_Cycle) {
    TIM1_PinConfig();
    TIM_OC1_PWMM1_Init(TIM1, Frequency, Duty_Cycle);
}

uint8_t sine_table[42] = {0, 5, 10, 15, 20, 25, 30, 35, 40, 45, 50, 55, 60, 65, 70, 75, 80, 85, 90, 95, 100, 95, 90, 85, 80, 75, 70, 64, 60, 55, 50, 45, 40, 35, 30, 25, 20, 15, 10, 5, 0};

volatile uint8_t sine_index = 0;
void TIM2_Interrupt_Init(uint32_t frequency) {
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;

    uint32_t f_clk = RCC_GetAbp1Clk();   // ví d?: 36MHz
    uint32_t f_update = 42 * frequency;  // c?n ng?t 42 l?n cho m?i chu k? sin

    // Gi? s? ta ch?n tru?c PSC = 71 d? chia xung xu?ng (36MHz / 72 = 500kHz)
    TIM2->PSC = 71;  // f_timer = 500 kHz
    TIM2->ARR = (f_clk / (TIM2->PSC + 1)) / f_update - 1;

    TIM2->CNT = 0;
    TIM2->DIER |= TIM_DIER_UIE;
    TIM2->CR1  |= TIM_CR1_CEN;

    NVIC_EnableIRQ(TIM2_IRQn);
}


void TIM2_IRQHandler(void) {
    if (TIM2->SR & TIM_SR_UIF) {
        TIM2->SR &= ~TIM_SR_UIF;

        sine_index = (sine_index + 1) % 42;
        uint8_t duty = sine_table[sine_index];

        TIM1->CCR1 = (TIM1->ARR + 1) * duty / 100;  // Set duty PWM tuong ?ng
    }
}


int main(void) {
    TFT_Init();       
    Systick_Init();
    Button_Init();
    //TFT_FillScreen(COLOR_BLUE);      
    displayInfo(freq, duty);
		//updatePWM(freq, duty);
		TIM1_PinConfig();
		TIM1_PWMOC(freq, duty);
		//TIM2_Interrupt_Init(1);
		//GPIO_SetMode(GPIOA, 9, GPIO_OUTPUT_MODE_50M_OD);
		
    while (1){
//			GPIOA->ODR |=  GPIO_ODR_ODR9;   // Set PA9 = 1
//for (int i = 0; i < 50; i++);
//GPIOA->ODR &= ~GPIO_ODR_ODR9;   // Set PA9 = 0
//for (int i = 0; i < 50; i++);

		}
}


void drawPWMWaveform(uint16_t startX, uint16_t startY, uint16_t width, uint16_t height, uint8_t duty, uint8_t cycles) {
    uint16_t period = width / cycles;
    uint16_t high = height / 2;
    uint16_t low = height;
    TFT_FillRect(startX, startY, width, height, COLOR_BLACK);

    // Tru?ng h?p d?c bi?t: duty = 0 (luôn LOW)
    if (duty == 0) {
        for (uint16_t i = 0; i < width; i++) {
            TFT_DrawPixel(startX + i, startY + high, COLOR_GREEN);
        }
        return;
    }

    // Tru?ng h?p d?c bi?t: duty = 100 (luôn HIGH)
    if (duty == 100) {
        for (uint16_t i = 0; i < width; i++) {
            TFT_DrawPixel(startX + i, startY, COLOR_GREEN);
        }
        return;
    }

    // Các tru?ng h?p còn l?i
    for (uint8_t i = 0; i < cycles; i++) {
        uint16_t x = startX + i * period;
        uint16_t highWidth = (period * duty) / 100;
        uint16_t lowWidth = period - highWidth;

        // C?nh lên
        for (uint16_t j = 0; j < high; j++)
            TFT_DrawPixel(x, startY + j, COLOR_GREEN);

        // M?c HIGH
        for (uint16_t j = 0; j < highWidth; j++)
            TFT_DrawPixel(x + j, startY, COLOR_GREEN);

        // C?nh xu?ng
        for (uint16_t j = 0; j < high; j++)
            TFT_DrawPixel(x + highWidth, startY + j, COLOR_GREEN);

        // M?c LOW
        for (uint16_t j = 0; j < lowWidth; j++)
            TFT_DrawPixel(x + highWidth + j, startY + high, COLOR_GREEN);
    }
}

void drawSineWaveform(uint16_t startX, uint16_t startY, uint16_t width, uint16_t height, uint8_t cycles) {
    TFT_FillRect(startX, startY, width, height, COLOR_BLACK); // Clear vùng v?

    float centerY = startY + height / 2.0f;   
    float amplitude = height / 2.5f;
    float step = (2.0f * M_PI * cycles) / (width * 2);  // tang d? phân gi?i g?p dôi

    uint16_t prevX = startX;
    uint16_t prevY = (uint16_t)(centerY - sinf(0) * amplitude);

    for (uint16_t i = 1; i <= width * 2; i++) {
        float angle = i * step;
        float y = sinf(angle);
        uint16_t x = startX + (i / 2);  // mapping l?i v? width g?c
        uint16_t pixelY = (uint16_t)(centerY - y * amplitude);

        // N?i do?n th?ng gi?a 2 di?m g?n nhau
        TFT_DrawLine(prevX, prevY, x, pixelY, COLOR_MAGENTA);

        prevX = x;
        prevY = pixelY;
    }
}


char freqStr[10];
char dutyStr[10];
void displayInfo(int freq, int duty) {
    TFT_FillScreen(COLOR_BLACK);

    TFT_DrawString(90, 10, "PWM GENERATOR", COLOR_YELLOW, 2);
    TFT_FillRect(10, 35, 300, 2, COLOR_WHITE);
		
    // FREQ
    
    TFT_DrawString(15, 70, "Freq(Hz):", COLOR_WHITE, 2);
    sprintf(freqStr, "%d", freq);
    clearString(freqStr, 130, 70, 2);
    TFT_DrawString(130, 70, freqStr, COLOR_CYAN, 2);
		
    // DUTY
    
    TFT_DrawString(15, 140, "Duty(%):", COLOR_WHITE, 2);
    sprintf(dutyStr, "%d", duty);
    clearString(dutyStr, 140, 130, 2);
    TFT_DrawString(140, 140, dutyStr, COLOR_CYAN, 2);
		avgVoltage(duty);
		TFT_FillRect(210, 62, 1, 100, COLOR_WHITE);
    drawSineWaveform(20, 200, 300, 40, 6);
		//drawPWMWaveform(20, 200, 300, 50, duty, 10);
}

char voltStr[10];
void updatePWM(int freq, int duty) {
	sprintf(freqStr, "%d", freq);
  clearString(freqStr, 130, 70, 2);
  TFT_DrawString(130, 70, freqStr, COLOR_CYAN, 2);
	sprintf(dutyStr, "%d", duty);
  clearString(dutyStr, 140, 140, 2);
  TFT_DrawString(140, 140, dutyStr, COLOR_CYAN, 2);
	
	clearString(voltStr, 230, 130, 3);
	sprintf(voltStr, "%.2f", volt);
	TFT_DrawString(230, 130, voltStr, COLOR_CYAN, 3);
	drawPWMWaveform(20, 200, 300, 30, duty, 5);
}


void avgVoltage(int duty) {
	volt = volt * duty/100;
	//TFT_FillRect(230, 60, 70, 100, COLOR_BLUE);
	TFT_DrawString(240, 75, "AVG", COLOR_WHITE, 3);
	sprintf(voltStr, "%.2f", volt);
	TFT_DrawString(238, 130, voltStr, COLOR_CYAN, 3);
}