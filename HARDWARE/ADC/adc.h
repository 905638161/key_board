#ifndef __ADC_H
#define __ADC_H	
#include "sys.h"
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEKս��STM32������
//ADC ����	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2012/9/7
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 
#define FILTER_NUM  20
#define AD_Count   255			//AD�˲���������	

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
