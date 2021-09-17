#include "key.h"


void KEY_Init(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE); //使能或者失能APB2外设时钟

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9; 
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;//最高输出速率2MHz
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
    GPIO_Init(GPIOC, &GPIO_InitStructure);//初始化外设GPIOx寄存器

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE); //使能或者失能APB2外设时钟
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;       //PE11	TS-B
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;//最高输出速率2MHz
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
    GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化外设GPIOx寄存器
	
	
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE); //使能或者失能APB2外设时钟
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;       //
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;//最高输出速率2MHz
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
    GPIO_Init(GPIOE, &GPIO_InitStructure);//初始化外设GPIOx寄存器
}

void Check_Key_Enter(void)
{
	static u8 key_up=1;//按键按松开标志  
	
	if(KEY_ENTER)
	{
			if(key_up)
			{
				delay_ms(2);//去抖动 
				if(KEY_ENTER)
				{
						key_up=0;
						keyValue = 0x7A;
						System_Task_Register |= TASK_UPDATE_KEY_VALUE;	
				}
		}
	}
	else
	{
			key_up=1;  
	}
}


void Check_Key_Slience(void)
{
	static u8 key_up=1;//按键按松开标志  
	
	if(!KEY_SLIENCE)
	{
			if(key_up)
			{
					delay_ms(2);//去抖动 
					if(!KEY_SLIENCE)
					{
							key_up=0;
							keyValue = 0x7D;
							System_Task_Register |= TASK_UPDATE_KEY_VALUE;	
					}
			}
	}
	else
	{
			key_up=1;  
	}
}

void Check_Key_Home(void)
{
	static u8 key_up=1;//按键按松开标志  
	
	if(!KEY_HOME)
	{
			if(key_up)
			{
					delay_ms(2);//去抖动 
					if(!KEY_HOME)
					{
							key_up=0;
							keyValue = 0x7B;
							System_Task_Register |= TASK_UPDATE_KEY_VALUE;	
					}
			}
	}
	else
	{
			key_up=1;  
	}
}

void Check_Key_Alarm_Reset(void)
{
	static u8 key_up=1;//按键按松开标志  
	
	if(!KEY_ALARM_RESET)
	{
			if(key_up)
			{
					delay_ms(2);//去抖动 
					if(!KEY_ALARM_RESET)
					{
							key_up=0;
							keyValue = 0x7C;
							System_Task_Register |= TASK_UPDATE_KEY_VALUE;	
					}
			}
	}
	else
	{
			key_up=1;  
	}
}

void Check_Key_Alarm_Standby(void)
{
	static u8 key_up=1;//按键按松开标志  
	
	if(!KEY_STANDBY)
	{
			if(key_up)
			{
					delay_ms(2);//去抖动 
					if(!KEY_STANDBY)
					{
							key_up=0;
							keyValue = 0x7E;
							System_Task_Register |= TASK_UPDATE_KEY_VALUE;	
					}
			}
	}
	else
	{
			key_up=1;  
	}
}

void KEY_Value_Scan(void)
{
		Check_Key_Enter();
		Check_Key_Slience();
		Check_Key_Home();
		Check_Key_Alarm_Reset();
		Check_Key_Alarm_Standby();
}
