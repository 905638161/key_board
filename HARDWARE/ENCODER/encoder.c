
/**
* @file 	encoder.c
* @brief	������ť����������
* @details	This is the detail description.
* @author	ETERNITY
* @date		2018-06-26
* @version	001
* @par Copyright (c):
* 		�����㹫˾
* @par History:
*	  version: author, date, desc

*/
#include "encoder.h"
#include "delay.h"
#include "led.h"
#include "usart.h"


/**
* @brief ������ť��IO�ڳ�ʼ��
* @param  None
* @return None
* @details Ϊ�˵�Ƭ������5V��IO�ڣ��ʶ�IO�ĳ�ʼ��Ϊ�������룬�������ó��������������롣
*/
void Rotary_Encoder_Init(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE); //ʹ�ܻ���ʧ��APB2����ʱ��

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9|GPIO_Pin_11;       //   TS-A TS-B
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;//����������2MHz
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
    GPIO_Init(GPIOE, &GPIO_InitStructure);//��ʼ������GPIOx�Ĵ���
}

u8 DLStatus1[4]                     = {0,1,3,2};	
u8 DLStatus;
u8 Get_DL_Status1(void)
{
		u8 i;
    i=0;
    if (TS_A==0)
		{
			i = 1;
		}
		
    if (TS_B==0)
		{
    	i^= 2;
		}
		
    return (i);
}
                                 //����ֵΪ״̬�ֽڣ�0��3��
u8 Get_DL_Status(void)             //���������������ʲô��������
{
		u8 i,j;
    static u8 k=0;
    u8 temp[20];
	
    i=Get_DL_Status1();
	
    if(k==0)
    {
    	k=1;
    	temp[0]=i;
    }
    else
    {
    	if (i!=temp[k-1])
      		temp[k++]=i;
    	if (k==20)
      		k=0;
    }
		
    for (j=0;j<4;j++)
		{
    	if (DLStatus1[j]==i) break;
		}
		
    return (j);
}


void Check_DL(void)
{
		u8 i,j;
    static u8 k1=0;
    static u8 k2=0;
    i=Get_DL_Status1();
    j=DLStatus;
    if (DLStatus1[j]!=i)
    {
    	if (j==3)
        	j=0;
        else
        	j++;
    	if (DLStatus1[j]==i)
    	{
      		k1++;
      		k2=0;
      		if (k1>=4)
      		{
							k1=0;
							keyValue = 0x55;
							System_Task_Register |= TASK_UPDATE_KEY_VALUE;
      		}
    	}
    	else
    	{
        	j=DLStatus;
      		if (j==0)
        		j=3;
      		else
        		j--;
      		if (DLStatus1[j]==i)
      		{
        		k1=0;
        		k2++;
        		if (k2>=4)
        		{
									k2=0;
									keyValue = 0xDD;
									System_Task_Register |= TASK_UPDATE_KEY_VALUE;
        		}
      		}
      		else
        		j=Get_DL_Status();
    	}
  		DLStatus=j;
	}
}

