 #include "adc.h"
 #include "delay.h"
//////////////////////////////////////////////////////////////////////////////////	 
//使用PA0 作为采样口，采样电池电量		  
//////////////////////////////////////////////////////////////////////////////////


u16 ADCresults[3][AD_Count];//AD采集值
u32 AD_Data[3];      //AD采集值
__IO uint16_t ADC_ConvertedValue[AD_Count][3];
	   
extern u8 flag_ADC;
u32 adc_value=0;
u32 adc_count;


static u16 BatteryVoltageSample[FILTER_NUM]={0};

/**************************adc1_polling***********************************/
u16 readAdcValue(u8 ch)
{
	ADC_RegularChannelConfig(ADC1, ch, 1, ADC_SampleTime_41Cycles5 );	
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);	 
	while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC ));
	return ADC_GetConversionValue(ADC1);	
}

 
void adc1_polling_init() 
{
		GPIO_InitTypeDef GPIO_InitStructure;
		ADC_InitTypeDef ADC_InitStructure;
		
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1 | RCC_APB2Periph_GPIOA, ENABLE);
			
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
    ADC_InitStructure.ADC_ScanConvMode = DISABLE;
    ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
    ADC_InitStructure.ADC_NbrOfChannel = 1;
    ADC_Init(ADC1, &ADC_InitStructure);
    ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_41Cycles5);    
    
    ADC_Cmd(ADC1, ENABLE);
		while(ADC_GetResetCalibrationStatus(ADC1));	//等待复位校准结束
	
		ADC_StartCalibration(ADC1);	 //开启AD校准
		while(ADC_GetCalibrationStatus(ADC1));//等待校准结束
}

/**************************end***********************************/

/**************************adc1_dma_interrupt***********************************/

void DMA1_Channel1_IRQHandler(void){
  if(DMA_GetITStatus(DMA1_IT_TC1))    
  {
				DMA_ClearITPendingBit(DMA1_IT_TC1);
        adc_value = ADC_GetConversionValue(ADC1);
        adc_count++;
  }
}

void adc1_dma_interrupt_init(void) 
{
		GPIO_InitTypeDef GPIO_InitStructure;
		DMA_InitTypeDef DMA_InitStructure;
	  NVIC_InitTypeDef NVIC_InitStructure;
	  ADC_InitTypeDef ADC_InitStructure;
	
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1 | RCC_APB2Periph_GPIOA, ENABLE);
		RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
     
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    DMA_DeInit(DMA1_Channel1);
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&ADC1->DR;
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&adc_value;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
    DMA_InitStructure.DMA_BufferSize = 1;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Disable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular; //DMA_Mode_Normal;
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
    DMA_Init(DMA1_Channel1, &DMA_InitStructure);
		DMA_ITConfig(DMA1_Channel1, DMA_IT_TC, ENABLE);
    DMA_Cmd(DMA1_Channel1, ENABLE);
 

		NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel1_IRQn;
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
		NVIC_Init(&NVIC_InitStructure);
    
    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
    ADC_InitStructure.ADC_ScanConvMode = DISABLE;
    ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
    ADC_InitStructure.ADC_NbrOfChannel = 1;
    ADC_Init(ADC1, &ADC_InitStructure);
    ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_41Cycles5);    
    
		ADC_DMACmd(ADC1, ENABLE);
    ADC_Cmd(ADC1, ENABLE);
    while(ADC_GetResetCalibrationStatus(ADC1));	//等待复位校准结束
	
		ADC_StartCalibration(ADC1);	 //开启AD校准
		while(ADC_GetCalibrationStatus(ADC1));//等待校准结束
 
    ADC_SoftwareStartConvCmd(ADC1,ENABLE);
}
/**************************end***********************************/

/**************************adc1_interrupt***********************************/
void ADC1_2_IRQHandler(void)
{
		BatteryVoltageSample[adc_count] = ADC_GetConversionValue(ADC1);
    adc_count++;
	
		if(adc_count >= FILTER_NUM)
		{
				adc_count = 0;
		}
}
 
void adc1_interrupt_init() 
{
		NVIC_InitTypeDef NVIC_InitStructure; 
		GPIO_InitTypeDef GPIO_InitStructure;
		ADC_InitTypeDef ADC_InitStructure;
	
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1 | RCC_APB2Periph_GPIOA, ENABLE);
    
		NVIC_InitStructure.NVIC_IRQChannel = ADC1_2_IRQn;
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
		NVIC_Init(&NVIC_InitStructure);
     
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
     
    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
    ADC_InitStructure.ADC_ScanConvMode = DISABLE;
    ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
    ADC_InitStructure.ADC_NbrOfChannel = 1;
    ADC_Init(ADC1, &ADC_InitStructure);
    ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_41Cycles5);    
    
    ADC_Cmd(ADC1, ENABLE);
    while(ADC_GetResetCalibrationStatus(ADC1));	//等待复位校准结束
	
		ADC_StartCalibration(ADC1);	 //开启AD校准
		while(ADC_GetCalibrationStatus(ADC1));//等待校准结束
    ADC_ITConfig(ADC1, ADC_IT_EOC, ENABLE);
    ADC_SoftwareStartConvCmd(ADC1,ENABLE);
}

/**************************end***********************************/
u16 getBatteryValue(void)
{
	  return DataFilterHandle(BatteryVoltageSample,FILTER_NUM);
}


u16	DataFilterHandle(u16 *Pdata,u16 Len)
{
	u16	i;
	u32	temp;
	u16	min,max;
	min = *Pdata;
  max = *Pdata;
	
	for(i=1; i < Len; i++)
	{
		if(max < *(Pdata + i))
		{
			max = *(Pdata + i);
		}
		if(min > *(Pdata + i))
		{
			min = *(Pdata + i);
		}
	}
	for(i=0,temp = 0; i < Len; i++)
	{
			temp  += (u32)(*(Pdata + i));		
	}
	temp  -=  max;
	temp  -=  min;
	temp  =  temp / (Len-2);	
	
	return ((u16)temp);
}

