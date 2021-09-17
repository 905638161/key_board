#ifndef __ADC_H
#define __ADC_H	
#include "sys.h"
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK战舰STM32开发板
//ADC 代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2012/9/7
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 
#define FILTER_NUM  20
#define AD_Count   255			//AD滤波采样次数	

void Adc_Init(void);
u16  Get_Adc(u8 ch); 
u16 Get_Adc_Average(u8 ch,u8 times); 

void ADC_Filter(void);
u16 getBatteryValue(void);
void adc1_dma_interrupt_init(void);
void adc1_interrupt_init(void);
void adc1_polling_init(void);
u16	DataFilterHandle(u16 *Pdata,u16 Len);

#endif 
