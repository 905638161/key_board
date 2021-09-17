#include "tsc2046_spi.h"

#define 	GET_TSC2046_DOUT()  		GPIO_ReadInputDataBit(GPIOD,GPIO_Pin_12) 
#define 	GET_TSC2046_PENING()    GPIO_ReadInputDataBit(GPIOD,GPIO_Pin_9) 


#define 	TSC2046_DIN_HIGH()		GPIO_SetBits(GPIOD, GPIO_Pin_13)	
#define 	TSC2046_DIN_LOW()			GPIO_ResetBits(GPIOD, GPIO_Pin_13)	

#define 	TSC2046_DCLK_HIGH()		GPIO_SetBits(GPIOD, GPIO_Pin_11)	
#define 	TSC2046_DCLK_LOW()		GPIO_ResetBits(GPIOD, GPIO_Pin_11)	

#define 	TSC2046_CS_HIGH()			GPIO_SetBits(GPIOD, GPIO_Pin_10)	
#define 	TSC2046_CS_LOW()			GPIO_ResetBits(GPIOD, GPIO_Pin_10)	

#define 	TSC2046_MEASURE_X			0xD0
#define 	TSC2046_MEASURE_Y			0x90
#define 	TSC2046_MEASURE_Z1  	0xB2
#define 	TSC2046_MEASURE_Z2  	0xC2

struct TouchscreenData tsData;

void tsc2046_Send_data(uint8_t data)
{
	uint8_t i = 0;
	TSC2046_DCLK_LOW();
	
	for(i = 0;i < 8;i ++)
	{
		if(data & 0x80)
		{
			TSC2046_DIN_HIGH();
		}
		else
		{
			TSC2046_DIN_LOW();
		}
		
		TSC2046_DCLK_HIGH();
		delay_us(10);
		TSC2046_DCLK_LOW();
		delay_us(10);
		
		data <<= 1;
	}
}

u16 tsc2046_Readdata()
{
	uint8_t i = 0;
	u32 rec_data = 0;

	for(i = 0;i < 12;i ++)
	{
		rec_data <<= 1;
		TSC2046_DCLK_HIGH();
		delay_us(10);
		
		TSC2046_DCLK_LOW();
		delay_us(10);
		
		if(GET_TSC2046_DOUT())
		{
			rec_data |= 0x01;
		}
	}
	//return rec_data>>3;
	return rec_data;
}



 
void TSC2046_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD|RCC_APB2Periph_AFIO, ENABLE);
    
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10|GPIO_Pin_11| GPIO_Pin_13 ;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9| GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOD, &GPIO_InitStructure);
}

 
uint16_t TSC2046_ReadOneData(uint8_t cmd)
{
	uint16_t retData;
	
	TSC2046_CS_LOW();	
	delay_us(5);
	
	tsc2046_Send_data(cmd);
	
	delay_us(30);

	retData = tsc2046_Readdata();	
	
	TSC2046_CS_HIGH();
	
	return retData;
}

void tsc2046Sort(u16 *buf)
{
	u16 temp, i, j;
	
	for (i = 0; i < TSC2046_MAX_SAMPLE - 1; i++)
	{
			for (j = i + 1; j < TSC2046_MAX_SAMPLE; j++)
			{
					if (buf[j] < buf[i])
					{
							temp = buf[i];
							buf[i] = buf[j];
							buf[j] = temp;
					}
			}
	}
}

u16		 touchXdata[TSC2046_MAX_SAMPLE];
u16		 touchYdata[TSC2046_MAX_SAMPLE];

void tsc2046ReadData(void)
{
	static u16 count = 0;
	u8 i;
	u32  tmpx = 0;
	u32  tmpy = 0;
	
	if(count < TSC2046_MAX_SAMPLE)
	{
			touchXdata[count] = TSC2046_ReadOneData(TSC2046_MEASURE_Y);
			touchYdata[count] = TSC2046_ReadOneData(TSC2046_MEASURE_X);
	}
	
	if(GET_TSC2046_PENING())
	{
			tsData.yData = 0;
			tsData.xData = 0;		
			count = 0;
	}
	else
	{
			if(count >= TSC2046_MAX_SAMPLE)
			{
					tsc2046Sort(touchXdata);
					tsc2046Sort(touchYdata);
				
					if((touchYdata[TSC2046_MAX_SAMPLE-1]-touchYdata[0] <= 25) && (touchXdata[TSC2046_MAX_SAMPLE-1]-touchXdata[0] <= 25))
					{
							for(i=0; i<TSC2046_MAX_SAMPLE; i++)
							{
								tmpx += touchXdata[i] ;
								tmpy += touchYdata[i];
							}
						
							tmpx /= TSC2046_MAX_SAMPLE;
							tmpy /= TSC2046_MAX_SAMPLE;
						
							if(tmpy < 4000 && tmpy < 4000 && tmpx > 200 && tmpx > 200)
							{
									tsData.pendown = 1;
									tsData.xData = tmpx;
									tsData.yData = tmpy;
							}
					}	
					count = 0;
			}
			else
			{
				if(touchXdata[count] < 4000 && touchYdata[count] < 4000 && touchXdata[count] > 200 && touchYdata[count] > 200)
				{
						count ++;
				}
			}
	}
}








