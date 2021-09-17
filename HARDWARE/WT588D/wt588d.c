
#include "sys.h"
#include "wt588d.h"
/*********************************************************************
**ע�⣺��WT588D���������һ��ָ�����ɣ���һ������ʱҪ���30ms
**
**********************************************************************/

/**
  * @brief  IO�ڳ�ʼ��
  * @param  None
  * @retval None
  */
void WT588D_Init(void )
{
    GPIO_InitTypeDef  GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; 
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
    GPIO_Init(GPIOB, &GPIO_InitStructure);					 //�����趨������ʼ��GPIOB.0


    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;//����������10MHz
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
    GPIO_Init(GPIOB, &GPIO_InitStructure);//��ʼ������GPIOx�Ĵ���
	
	
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0; 
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
    GPIO_Init(GPIOE, &GPIO_InitStructure);					 //�����趨������ʼ��GPIOB.0

}

/**
 * @brief  һ��������оƬͨ����������
 * @param  addr: send data to the WT588D.
 * @retval None
 */
void WTW_Send(u8 addr)
{
    u8 i;
    RST = 0;
    delay_ms(5);
    RST = 1;
    delay_ms(20);

    SDA = 0;
    delay_ms(5);
    for(i = 0; i < 8; i++)
    {
        SDA = 1;

        if(addr & 0x01)
        {
            delay_us(600);
            SDA = 0;
            delay_us(200);
        }
        else
        {
            delay_us(200);
            SDA = 0;
            delay_us(600);
        }
        addr >>= 1;
    }
    SDA = 1;
}
/**
 * @brief  �߼�����
 * @param  None
 * @return None
**/
void High_Alarm(void)
{
    WTW_Send(alarm_H_addr);

    delay_ms(30);

    WTW_Send(S_loop);
}
/**
 * @brief �м�����
 * @param None
 * @return None
**/
void Medium_Alarm()
{
    WTW_Send(alarm_M_addr);

    delay_ms(30);

    WTW_Send(S_loop);
}

/**
 * @brief �ͼ�����
 * @param None
 * @return None
**/
void Low_Alarm(void)
{
    WTW_Send(alarm_L_addr);

    delay_ms(30);

    WTW_Send(S_loop);
}

/**
 * @brief ����ģʽ���ޱ���
 * @param the first input value
 * @return the return value
 * @details a full description of what the function does
**/
void Silence_Alarm(void)
{

    WTW_Send(E_loop);
    delay_ms(30);
}

/**
 * @brief ��������
 * @param data
 * @return None
 * @details a full description of what the function does
**/
void Volume_Change(u8 volume)
{
    delay_ms(90); 	//�����ĵ��ڣ����֮ǰ��������������ַ��
    //������90ms��,�ٷ���������������
    WTW_Send(volume);
    delay_ms(30);
}
/**
 * @brief ������������Ӧ
 * @param
	priority : �������ȼ�
    volume:    ������������ 0XE0~0XE7
 * @return the return value
 * @details a full description of what the function does
**/
void Alarm_voice_Select(u8 priority, u8 volume, u8 silence)
{
    static u8 LastPriority = 0xFF;
    static u8 LastVolume = 0xE0;
	  
		if(SELF_ALARM != priority)
		{
				if(silence)
				{
						Silence_Alarm();
						LastPriority=0;
				}
				else 
				{
						if(LastPriority != priority)
						{

								if(priority == HIGH_ALARM)
								{
										High_Alarm();
								}
								else if(priority == MEDIUM_ALARM)
								{
										Medium_Alarm();
								}
								else if(priority == LOW_ALARM)
								{
										Low_Alarm();
								}
								else
								{
										Silence_Alarm();
								}
								
								LastPriority = priority;
						}
				}

				volume |= 0xE0;
				if(LastVolume != volume)
				{
						/**�����ĵ���**/
						if((volume & 0xE0) == 0XE0)
						{
								Volume_Change(volume);
								LastVolume = volume;
						}
				}
		}
		else
		{
				Medium_Alarm();
				LastPriority = 0xFF;
		}
	
}



