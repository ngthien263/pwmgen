#include "stm32f10x.h"
#include "TFT.h"
#include "control.h"
#include <stdio.h>
#include "button.h"
#include "Timer.h"

//void SystemInit(){
//}
void clearCString(char* str, int maxLength) {
    for (int i = 0; i < maxLength; ++i) {
        str[i] = '\0';
    }
}


void TIM1_PinConfig() {
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
    GPIO_SetMode(GPIOA, 8, GPIO_ALT_MODE_50M_PP);
}

// Initialize PWM output for Timer 1
void TIM1_PWMOC(uint32_t Frequency, uint32_t Duty_Cycle) {
    TIM1_PinConfig();
    TIM_OC1_PWMM1_Init(TIM1, Frequency, Duty_Cycle);
}

uint8_t sine_table[21] = {0,10, 20, 30, 40, 50, 60, 70, 80, 90, 100, 90, 80, 70, 60, 50, 40, 30, 20, 10, 0};

int main(void) {
    TFT_Init();       // Kh?i t?o màn hình
    Systick_Init();
    Button_Init();
    TFT_FillScreen(COLOR_BLUE);      // Màu d? (F800 là RED, không ph?i xanh)
    display();
		//TIM1_PinConfig();
		
    while (1);
}


void display() {
		TFT_DrawString(20, 50, "Freq: ", 0xFFFF, 4); // V? ch? tr?ng
		TFT_DrawString(20, 100, "Duty: ", 0xFFFF, 4); // V? ch? tr?ng
    char str[20];
    sprintf(str, "%d", freq); 
		clearString(str, 150, 50, 10);
    TFT_DrawString(150, 50, str, 0xFFFF, 4);
		clearCString(str, 20);
    sprintf(str, "%d", duty); 
    clearString(str, 150, 100, 10);
    TFT_DrawString(150, 100, str, 0xFFFF, 4);
}