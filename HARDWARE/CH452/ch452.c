
#include "ch452.h"
#include "usart.h"
#include "led.h"
#include "sys.h"
/****************************
// CH452������2�߽ӿڣ����͵�ƽ���尴���ж����ڣ�ֻ��Ҫ2��I/O���ţ�����I2C/IICʱ��
// �����Ʒ�ʽSCL/SDA�������ж���SDA�������ӵ�Ƭ��������/�����ж�����
// ����Ƶ�ʵ���24MHz��MCS51��Ϊ�˽�Լ����ʱ�䣬�����ʵ�����SCL/SDA֮�����ʱ
****************************************************************************/
/**
* @brief CH452����ĳ�ʼ����
* @param None
* @return None
* @details CH452�ĳ�ʼ����������ʹ����PB4��JNTRST��������Ҫ�ر�JTAGģ�飬PB4������Ϊ��ͨ��IO�����
*/
void CH452_Init()             // CH452�ĳ�ʼ��
{
	GPIO_InitTypeDef GPIO_InitStructure; //RCC->APB2ENR|=1<<4;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE); //��������ʱ��

    GPIO_PinRemapConfig(GPIO_Remap_SWJ_NoJTRST, ENABLE);//��ȫSWJ(JTAG-DP + SW-DP)����û��JNTRST,��PB4����ʹ��
	   
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4|GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP ;   //�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
     
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;         //PB7�ĳ�ʼ     
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU ;   //��������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	CH452_SCL=1;
	CH452_SDA=1;
	CH452_Write(CH452_SYSON2);	//����ɨ��ʹ��,������2�ߴ��нӿڵ��ж������
								//�����ж�������͵�ƽ��Ч
									
	/*
	�趨ϵͳ������01000[GPOE][INTM][SSPD][DPLR]0[KEYB][DISP]B
	
    λ						  ��д				    ��0                      ��1
	0 	��ʾ�������ܵ�ʹ��    DISP 				�ر���ʾ���� 			������ʾ����
    1 	����ɨ�蹦�ܵ�ʹ��    KEYB 				�رռ���ɨ�� 			���ü���ɨ��
	2 	���Ź����ܵ�ʹ��      WDOG 				�رտ��Ź� 				���ÿ��Ź�
	3 	������DIG �������    DPLR 				�͵�ƽ��Ч 				�ߵ�ƽ��Ч
	4 	��˸�ٶ�/Ƶ��         SSPD				���٣�Լ1.5Hz�� 		���٣�Լ3Hz��
		
	5 	�����ж������ʽ      INTM	    	  	�͵�ƽ��Ч				�͵�ƽ���������жϣ�
	                                           ����ƽ������жϣ�
											   
    6 	ͨ���������ʹ��  	  GPOE				������ʾ�����������    ��ɨ�輫��Ϊ1��6 ʱ
		����DIG6��DIG7���ţ�                    ��ֹͨ�����            �����DIG6��DIG7����
																		����ͨ��������ֱ���
																		��˸λD6S ��D7S ����
	*/
}


/*�����ƣ����������ź�*/
void  CH452_I2c_Start_2(void)
{
	CH452_SDA_OUT();
	CH452_SDA=1;     //sda�����
	CH452_SCL=1;
	delay_us(4);
 	CH452_SDA=0; //START:when CLK is high,DATA change form high to low 
	delay_us(4);
	CH452_SCL=0;//ǯסI2C���ߣ�׼�����ͻ�������� 
}	  



void CH452_I2c_Stop_2(void)
{
    CH452_SDA_OUT();   //sda�����
	
	CH452_SCL=0;
	CH452_SDA=0;
 	delay_us(4);
	
	CH452_SCL=1;
	CH452_SDA=1;//����I2C���߽����ź�
	delay_us(4);				
    	
}


void CH452_I2c_WrByte(unsigned char dat)	//дһ���ֽ�����
{
	unsigned char i;
	CH452_SDA_OUT();   /* ����SDAΪ������� */
	CH452_SCL=0;
	
	for(i=0;i<8;i++)  // ���8λ����
	{
		if(dat&0x80) 
			{CH452_SDA=1;}
		else 
			{CH452_SDA=0;}
		dat<<=1;
		delay_us(2);
			
		CH452_SCL=1;
        delay_us(2);
			
		CH452_SCL=0;
		delay_us(2);
	}
	CH452_SDA=1;
	delay_us(2);
	
	CH452_SCL=1;  // ����Ӧ��
	delay_us(2);
	
	CH452_SCL=0;
	delay_us(2);
}

unsigned char  CH452_I2c_RdByte(void)		//��һ���ֽ�����
{
	unsigned char dat,i;
	CH452_SDA_IN();   /* ����SDAΪ���뷽�� */
	dat=0;
	for(i=0;i<8;i++)  // ����8λ����
	{
		CH452_SCL=0;
		delay_us(2);
		CH452_SCL=1;
		dat<<=1;
		if(READ_CH452_SDA) dat++;  // ����1λ
		CH452_SCL=0;
		delay_us(1);

	}
	return	dat;
	
}

void CH452_Write(unsigned short cmd)	//д����
{
	
	CH452_I2c_Start_2();                  /*��������*/
   	CH452_I2c_WrByte(((unsigned char)(cmd>>7)&CH452_I2C_MASK)|CH452_I2C_ADDR1);  // CH452��ADDR=1ʱ(Ĭ��)
	
   	CH452_I2c_WrByte((unsigned char)cmd); /*��������*/
	
  	CH452_I2c_Stop_2();                   /*��������*/ 

}

unsigned char CH452_Read(void)		//��ȡ����
{
	unsigned char keycode;
   	CH452_I2c_Start_2();                /*��������*/
	
   	CH452_I2c_WrByte(((unsigned char)(CH452_GET_KEY>>7)&CH452_I2C_MASK)|0x01|CH452_I2C_ADDR1); 
	
   	keycode=CH452_I2c_RdByte();         /*��ȡ����*/
	CH452_I2c_Stop_2();                 /*��������*/ 
	
	return(keycode);
	
}

/**
 * @brief ��������ɨ��
 * @param	None
 * @return 	KeyValue
 * @details	ʹ�ò�ѯ�ķ�ʽ��ͨ����ѯIO�ڵĵ�λ�ߵ㣬
			ִ�а����Ķ�ȡ����
**/
void KEY_Scan(void)
{
	if (CH452_INT==0)
	{
	}

}
extern u16 keyValue;
void Key_Scan_CH452(void)
{
		u8 key;
		static u8 KeyStatus;
	
    key=CH452_Read();
	
    switch (KeyStatus)
    {
				case 0: 
				{				
						if ((key>=CH452_KEY_MIN)&&(key<=CH452_KEY_MAX))
						{
									keyValue = key;
							
									System_Task_Register |= TASK_UPDATE_KEY_VALUE;
									KeyStatus=1;
						}
						else
						{
									KeyStatus = 0;
						}
				}
				break;
				case 1:  
				{				
						if((key&0x40)==0)
						{
								KeyStatus = 0;
								keyValue &= ~0x40;
						}
				} 
				break;
  } 
}

