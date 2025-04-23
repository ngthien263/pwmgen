#include "stm32f10x.h"
#include "TFT.h"
#include "control.h"
#include <stdio.h>
#include "button.h"
#include "Timer.h"
#include "RCC.h"
//void SystemInit(){
//}
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
    TIM2->PSC =  8640;            
    TIM2->ARR = RCC_GetAbp2Clk()/TIM2->PSC/frequency;             
    TIM2->DIER |= TIM_DIER_UIE;      
    TIM2->CR1  |= TIM_CR1_CEN;      

    NVIC_EnableIRQ(TIM2_IRQn);
}

void TIM2_IRQHandler(void) {
    if (TIM2->SR & TIM_SR_UIF) {
        TIM2->SR &= ~TIM_SR_UIF;     // Clear interrupt flag

        TIM1->CCR1 = TIM1->ARR * sine_table[sine_index] / 100;  // C?p nh?t duty cycle
        sine_index++;
        if (sine_index >= sizeof(sine_table)) {
            sine_index = 0;
        }
    }
}

int main(void) {
    TFT_Init();       
    Systick_Init();
    Button_Init();
    TFT_FillScreen(COLOR_BLUE);      
    display();
		TIM1_PinConfig();
		TIM1_PWMOC(freq, duty);
		TIM2_Interrupt_Init(20);
    while (1);
}


void display() {
		TFT_DrawString(20, 50, "Freq: ", 0xFFFF, 4); 
		TFT_DrawString(20, 100, "Duty: ", 0xFFFF, 4); 
    char str[20];
    sprintf(str, "%d", freq); 
		clearString(str, 150, 50, 10);
    TFT_DrawString(150, 50, str, 0xFFFF, 4);
		clearCString(str, 20);
    sprintf(str, "%d", duty); 
    clearString(str, 150, 100, 10);
    TFT_DrawString(150, 100, str, 0xFFFF, 4);
}