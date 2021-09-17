#include "key.h"


void KEY_Init(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE); //ʹ�ܻ���ʧ��APB2����ʱ��

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9; 
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;//����������2MHz
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
    GPIO_Init(GPIOC, &GPIO_InitStructure);//��ʼ������GPIOx�Ĵ���

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE); //ʹ�ܻ���ʧ��APB2����ʱ��
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;       //PE11	TS-B
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;//����������2MHz
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
    GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ������GPIOx�Ĵ���
	
	
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE); //ʹ�ܻ���ʧ��APB2����ʱ��
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;       //
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;//����������2MHz
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
    GPIO_Init(GPIOE, &GPIO_InitStructure);//��ʼ������GPIOx�Ĵ���
}

void Check_Key_Enter(void)
{
	static u8 key_up=1;//�������ɿ���־  
	
	if(KEY_ENTER)
	{
			if(key_up)
			{
				delay_ms(2);//ȥ���� 
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
	static u8 key_up=1;//�������ɿ���־  
	
	if(!KEY_SLIENCE)
	{
			if(key_up)
			{
					delay_ms(2);//ȥ���� 
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
	static u8 key_up=1;//�������ɿ���־  
	
	if(!KEY_HOME)
	{
			if(key_up)
			{
					delay_ms(2);//ȥ���� 
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
	static u8 key_up=1;//�������ɿ���־  
	
	if(!KEY_ALARM_RESET)
	{
			if(key_up)
			{
					delay_ms(2);//ȥ���� 
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
	static u8 key_up=1;//�������ɿ���־  
	
	if(!KEY_STANDBY)
	{
			if(key_up)
			{
					delay_ms(2);//ȥ���� 
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
