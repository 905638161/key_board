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
	
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1,ENABLE);  //时钟使能
	DMA_DeInit(DMA1_Channel1);//将dma的通道1寄存器重设为缺省值
	DMA_InitStructure.DMA_PeripheralBaseAddr=(u32)&ADC1->DR;//DMA 外设ADC基地址
	DMA_InitStructure.DMA_MemoryBaseAddr=(u32)&ADC_ConvertedValue;//DMA 内存基地址
	DMA_InitStructure.DMA_DIR=DMA_DIR_PeripheralSRC;//内存作为数据传输的目的地(单向的)
	DMA_InitStructure.DMA_BufferSize=3*AD_Count;//通道的DMA缓存数目大小
	DMA_InitStructure.DMA_PeripheralInc=DMA_PeripheralInc_Disable;//
	DMA_InitStructure.DMA_MemoryInc=DMA_MemoryInc_Enable;//
	DMA_InitStructure.DMA_PeripheralDataSize=DMA_PeripheralDataSize_HalfWord;//
	DMA_InitStructure.DMA_MemoryDataSize=DMA_MemoryDataSize_HalfWord;//
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular; // 循环模式 
	DMA_InitStructure.DMA_Priority = DMA_Priority_High; // 
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable; // 
	DMA_Init(DMA1_Channel1, &DMA_InitStructure); //
	
	DMA_ClearITPendingBit(DMA1_IT_TC1); /*清除一次中断完成标志*/
	DMA_ITConfig (DMA1_Channel1,DMA_IT_TC,ENABLE);//Enale trasfer complete interrupt mask
	DMA_ITConfig(DMA1_Channel1, DMA1_IT_TC1, ENABLE); /*使能DMA传输完成中断*/
	

	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel1_IRQn;  /*选择DMA1通道中断*/
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;   	/*中断使能*/ 
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;  //先占优先级0级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  //从优先级3级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道被使能
	NVIC_Init(&NVIC_InitStructure);  
 
}

/**
  * @brief  DMA1_Channel1中断服务函数
  * @param  无
  * @retval 无
  */
void DMA1_Channel1_IRQHandler(void)  
{
	/*判断DMA传输完成中断*/ 
	if(DMA_GetITStatus(DMA1_IT_TC1) != RESET)                        
	{ 
		
		flag_ADC = 1;
	}	
	/*清除DMA中断标志位*/	
	DMA_ClearITPendingBit(DMA1_IT_TC1);                      
}

