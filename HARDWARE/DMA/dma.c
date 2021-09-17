#include "dma.h"
#include "adc.h"

/********************************************************
*
*
*
********************************************************/
void DMA_Configuration(void)
{
	DMA_InitTypeDef DMA_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1,ENABLE);  //ʱ��ʹ��
	DMA_DeInit(DMA1_Channel1);//��dma��ͨ��1�Ĵ�������Ϊȱʡֵ
	DMA_InitStructure.DMA_PeripheralBaseAddr=(u32)&ADC1->DR;//DMA ����ADC����ַ
	DMA_InitStructure.DMA_MemoryBaseAddr=(u32)&ADC_ConvertedValue;//DMA �ڴ����ַ
	DMA_InitStructure.DMA_DIR=DMA_DIR_PeripheralSRC;//�ڴ���Ϊ���ݴ����Ŀ�ĵ�(�����)
	DMA_InitStructure.DMA_BufferSize=3*AD_Count;//ͨ����DMA������Ŀ��С
	DMA_InitStructure.DMA_PeripheralInc=DMA_PeripheralInc_Disable;//
	DMA_InitStructure.DMA_MemoryInc=DMA_MemoryInc_Enable;//
	DMA_InitStructure.DMA_PeripheralDataSize=DMA_PeripheralDataSize_HalfWord;//
	DMA_InitStructure.DMA_MemoryDataSize=DMA_MemoryDataSize_HalfWord;//
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular; // ѭ��ģʽ 
	DMA_InitStructure.DMA_Priority = DMA_Priority_High; // 
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable; // 
	DMA_Init(DMA1_Channel1, &DMA_InitStructure); //
	
	DMA_ClearITPendingBit(DMA1_IT_TC1); /*���һ���ж���ɱ�־*/
	DMA_ITConfig (DMA1_Channel1,DMA_IT_TC,ENABLE);//Enale trasfer complete interrupt mask
	DMA_ITConfig(DMA1_Channel1, DMA1_IT_TC1, ENABLE); /*ʹ��DMA��������ж�*/
	

	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel1_IRQn;  /*ѡ��DMA1ͨ���ж�*/
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;   	/*�ж�ʹ��*/ 
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;  //��ռ���ȼ�0��
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  //�����ȼ�3��
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQͨ����ʹ��
	NVIC_Init(&NVIC_InitStructure);  
 
}

/**
  * @brief  DMA1_Channel1�жϷ�����
  * @param  ��
  * @retval ��
  */
void DMA1_Channel1_IRQHandler(void)  
{
	/*�ж�DMA��������ж�*/ 
	if(DMA_GetITStatus(DMA1_IT_TC1) != RESET)                        
	{ 
		
		flag_ADC = 1;
	}	
	/*���DMA�жϱ�־λ*/	
	DMA_ClearITPendingBit(DMA1_IT_TC1);                      
}

