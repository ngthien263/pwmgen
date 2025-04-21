#ifndef __BUTTON_H__
#define __BUTTON_H__

#include <stdint.h>

extern volatile uint16_t duty;
extern volatile uint16_t freq;

void Button_Init(void);
void Systick_Init(void);
#endif // __BUTTON_H__
