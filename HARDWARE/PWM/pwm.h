#ifndef __PWM_H__
#define __PWM_H__
#include "sys.h"

#define LCD_PWM_TIM3_PERIOD  720


void TIM3_PWM_Init(u16 arr,u16 psc);

void setLCD_PWM_Compare(u8 value);

#endif
