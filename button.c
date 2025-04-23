#include "stm32f10x.h"
#include "GPIO.h"
#include "RCC.h"
#include "button.h"
#include "control.h"
#include "TFT.h"
#include <stdio.h>

volatile uint32_t tick_ms = 0;

// Th?i di?m nh?n cu?i c?a t?ng nút (dùng ch?ng rung)
static volatile uint32_t last_press_pb0 = 0;
static volatile uint32_t last_press_pb1 = 0;
static volatile uint32_t last_press_pb10 = 0;
static volatile uint32_t last_press_pb11 = 0;

// ==========================
//        SysTick
// ==========================
void SysTick_Handler(void) {
    tick_ms++;
}

void Systick_Init(void) {
    uint32_t clk = RCC_GetSysClk();
    SysTick_Config(clk / 1000); // 1ms tick
}

// ==========================
//       Button Init
// ==========================
void Button_Init(void) {
    RCC->APB2ENR |= RCC_APB2ENR_IOPBEN | RCC_APB2ENR_AFIOEN;

    GPIO_SetMode(GPIOB, PIN0, GPIO_INPUT_MODE_PuPd);
    GPIO_SetMode(GPIOB, PIN1, GPIO_INPUT_MODE_PuPd);
    GPIO_SetMode(GPIOB, PIN10, GPIO_INPUT_MODE_PuPd);
    GPIO_SetMode(GPIOB, PIN11, GPIO_INPUT_MODE_PuPd);

    GPIOB->ODR |= (1 << PIN0) | (1 << PIN1) | (1 << PIN10) | (1 << PIN11); // Kéo lên

    // Gán EXTIx cho chân PBx
    AFIO->EXTICR[0] &= ~(0xF << 0); AFIO->EXTICR[0] |= (0x1 << 0);   // EXTI0 -> PB0
    AFIO->EXTICR[0] &= ~(0xF << 4); AFIO->EXTICR[0] |= (0x1 << 4);   // EXTI1 -> PB1
    AFIO->EXTICR[2] &= ~(0xF << 8); AFIO->EXTICR[2] |= (0x1 << 8);   // EXTI10 -> PB10
    AFIO->EXTICR[2] &= ~(0xF << 12); AFIO->EXTICR[2] |= (0x1 << 12); // EXTI11 -> PB11

    // B?t mask + su?n xu?ng
    EXTI->IMR  |= (1 << 0) | (1 << 1) | (1 << 10) | (1 << 11);
    EXTI->RTSR &= ~((1 << 0) | (1 << 1) | (1 << 10) | (1 << 11)); // T?t su?n lên
    EXTI->FTSR |= (1 << 0) | (1 << 1) | (1 << 10) | (1 << 11);    // B?t su?n xu?ng

    NVIC_EnableIRQ(EXTI0_IRQn);
    NVIC_EnableIRQ(EXTI1_IRQn);
    NVIC_EnableIRQ(EXTI15_10_IRQn);
}

// ==========================
//     Interrupt Handlers
// ==========================
volatile uint8_t tim2_enabled = 1;

void EXTI0_IRQHandler(void) {
    if (EXTI->PR & EXTI_PR_PR0) {
        EXTI->PR |= EXTI_PR_PR0; // Clear c? ng?t

        // Ki?m tra th?i di?m nút du?c nh?n xu?ng (không ki?m tra khi nh? ra)
        if ((tick_ms - last_press_pb0) > 50) {
            tim2_enabled = !tim2_enabled;  // Ð?o tr?ng thái

            if (tim2_enabled) {
                TIM2->DIER |= TIM_DIER_UIE;  // B?t ng?t TIM2
            } else {
                TIM2->DIER &= ~TIM_DIER_UIE; // T?t ng?t TIM2
            }

            last_press_pb0 = tick_ms;
        }
    }
}


void EXTI1_IRQHandler(void) {
    if (EXTI->PR & (1 << 1)) {
        if ((tick_ms - last_press_pb1) > 50) {
            decreaseDuty();
            display();
            last_press_pb1 = tick_ms;
        }
        EXTI->PR |= (1 << 1);
    }
}

void EXTI15_10_IRQHandler(void) {
    if (EXTI->PR & (1 << 10)) {
        if ((tick_ms - last_press_pb10) > 50) {
            increaseFreq();
            display();
            last_press_pb10 = tick_ms;
        }
        EXTI->PR |= (1 << 10);
    }
    if (EXTI->PR & (1 << 11)) {
        if ((tick_ms - last_press_pb11) > 50) {
            decreaseFreq();
            display();
            last_press_pb11 = tick_ms;
        }
        EXTI->PR |= (1 << 11);
    }
}
