#include "control.h"
#include <stdio.h>
#include "TFT.h"
#include "Timer.h"
volatile uint16_t duty = 50;
volatile uint16_t freq = 1000;
volatile double volt = 3.3;
int mode = 2;

void increaseDuty(void) {
		duty += 10;
		if (duty > 100) duty = 100;
		TIM_OC1_PWMM1_Init(TIM1, freq, duty);
}

void decreaseDuty(void) {
    duty -= 10;
		if (duty > 100 && duty <= 65535  ) duty = 0;
		TIM_OC1_PWMM1_Init(TIM1, freq, duty);
}

void increaseFreq(void) {
    if (freq < 20000) freq += 100;
}

void decreaseFreq(void) {
    if (freq > 100) freq -= 100;
}
