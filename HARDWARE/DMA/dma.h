
#ifndef __DMA_H
#define __DMA_H
#include "sys.h"
#include "adc.h"


extern  __IO uint16_t ADC_ConvertedValue[AD_Count][3];
extern u8 flag_ADC;

void DMA_Configuration(void);
void DMA1_Channel1_IRQHandler(void) ; 

#endif
