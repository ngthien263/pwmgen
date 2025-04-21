#ifndef __CONTROL_H__
#define __CONTROL_H__

#include <stdint.h>

void increaseDuty(void);
void decreaseDuty(void);
void increaseFreq(void);
void decreaseFreq(void);
void display();

extern volatile uint16_t duty;
extern volatile uint16_t freq;

#endif
