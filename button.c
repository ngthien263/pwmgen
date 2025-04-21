#include "stm32f10x.h"
#include "GPIO.h"
#include "RCC.h"
#include "button.h"
#include "control.h"
#include "TFT.h"
#include <stdio.h>

volatile uint32_t tick_ms = 0;
 

static volatile uint32_t last_press_pb0 = 0;
static volatile uint32_t last_press_pb1 = 0;
static volatile uint32_t last_press_pb10 = 0;
static volatile uint32_t last_press_pb11 = 0;

void SysTick_Handler(void) {
    tick_ms++;
}

void Systick_Init(void) {
    uint32_t clk = RCC_GetSysClk();
    SysTick_Config(clk / 1000); // 1ms tick
}

void Button_Init(void) {
    RCC->APB2ENR |= RCC_APB2ENR_IOPBEN | RCC_APB2ENR_AFIOEN;

    GPIO_SetMode(GPIOB, PIN0, GPIO_INPUT_MODE_PuPd);
    GPIO_SetMode(GPIOB, PIN1, GPIO_INPUT_MODE_PuPd);
    GPIO_SetMode(GPIOB, PIN10, GPIO_INPUT_MODE_PuPd);
    GPIO_SetMode(GPIOB, PIN11, GPIO_INPUT_MODE_PuPd);
    GPIOB->ODR |= (1 << PIN0) | (1 << PIN1) | (1 << PIN10) | (1 << PIN11);

    AFIO->EXTICR[0] &= ~(0xF << 0);        // EXTI0 -> PB0
    AFIO->EXTICR[0] |=  (0x1 << 0);
    AFIO->EXTICR[0] &= ~(0xF << 4);        // EXTI1 -> PB1
    AFIO->EXTICR[0] |=  (0x1 << 4);
    AFIO->EXTICR[2] &= ~(0xF << 8);        // EXTI10 -> PB10
    AFIO->EXTICR[2] |=  (0x1 << 8);
    AFIO->EXTICR[2] &= ~(0xF << 12);       // EXTI11 -> PB11
    AFIO->EXTICR[2] |=  (0x1 << 12);

    EXTI->IMR  |= (1 << 0) | (1 << 1) | (1 << 10) | (1 << 11);
    //EXTI->RTSR |= (1 << 0) | (1 << 1) | (1 << 10) | (1 << 11);
    EXTI->RTSR &= ~((1 << 0) | (1 << 1) | (1 << 10) | (1 << 11)); // Xóa c?nh lên
    EXTI->FTSR |= (1 << 0) | (1 << 1) | (1 << 10) | (1 << 11);    // B?t c?nh xu?ng

    NVIC_EnableIRQ(EXTI0_IRQn);
    NVIC_EnableIRQ(EXTI1_IRQn);
    NVIC_EnableIRQ(EXTI15_10_IRQn);
}

void EXTI0_IRQHandler(void) {
    if (EXTI->PR & (1 << 0)) {
        if ((tick_ms - last_press_pb0) > 200) {
            increaseDuty();
            display();
            last_press_pb0 = tick_ms;
        }
        EXTI->PR |= (1 << 0);
    }
}

void EXTI1_IRQHandler(void) {
    if (EXTI->PR & (1 << 1)) {
        if ((tick_ms - last_press_pb1) > 200) {
            decreaseDuty(); 
            display();
            last_press_pb1 = tick_ms;
        }
        EXTI->PR |= (1 << 1);
    }
}

void EXTI15_10_IRQHandler(void) {
    if (EXTI->PR & (1 << 10)) {
        if ((tick_ms - last_press_pb10) > 200) {
            increaseDuty();
            last_press_pb10 = tick_ms;
        }
        EXTI->PR |= (1 << 10);
    }
    if (EXTI->PR & (1 << 11)) {
        if ((tick_ms - last_press_pb11) > 200) {
            decreaseFreq();
            last_press_pb11 = tick_ms;
        }
        EXTI->PR |= (1 << 11);
    }
}
