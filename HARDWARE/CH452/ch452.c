
#include "ch452.h"
#include "usart.h"
#include "led.h"
#include "sys.h"
/****************************
// CH452的真正2线接口，含低电平脉冲按键中断在内，只需要2个I/O引脚，兼容I2C/IIC时序
// 两线制方式SCL/SDA，按键中断由SDA产生，接单片机的脉冲/边沿中断引脚
// 对于频率低于24MHz的MCS51，为了节约传输时间，可以适当减少SCL/SDA之间的延时
****************************************************************************/
/**
* @brief CH452程序的初始化，
* @param None
* @return None
* @details CH452的初始化程序，由于使用了PB4（JNTRST），所以要关闭JTAG模块，PB4才能作为普通的IO口输出
*/
void CH452_Init()             // CH452的初始化
{
	GPIO_InitTypeDef GPIO_InitStructure; //RCC->APB2ENR|=1<<4;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE); //开启复用时钟

    GPIO_PinRemapConfig(GPIO_Remap_SWJ_NoJTRST, ENABLE);//完全SWJ(JTAG-DP + SW-DP)，但没有JNTRST,故PB4正常使用
	   
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4|GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP ;   //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
     
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;         //PB7的初始     
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU ;   //上拉输入
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	CH452_SCL=1;
	CH452_SDA=1;
	CH452_Write(CH452_SYSON2);	//键盘扫描使能,设置有2线串行接口的中断输出，
								//键盘中断输出，低电平有效
									
	/*
	设定系统参数：01000[GPOE][INTM][SSPD][DPLR]0[KEYB][DISP]B
	
    位						  简写				    置0                      置1
	0 	显示驱动功能的使能    DISP 				关闭显示驱动 			允许显示驱动
    1 	键盘扫描功能的使能    KEYB 				关闭键盘扫描 			启用键盘扫描
	2 	看门狗功能的使能      WDOG 				关闭看门狗 				启用看门狗
	3 	字驱动DIG 输出极性    DPLR 				低电平有效 				高电平有效
	4 	闪烁速度/频率         SSPD				低速（约1.5Hz） 		快速（约3Hz）
		
	5 	按键中断输出方式      INTM	    	  	低电平有效				低电平脉（边沿中断）
	                                           （电平或边沿中断）
											   
    6 	通用输出引脚使能  	  GPOE				用于显示驱动字输出，    当扫描极限为1～6 时
		（仅DIG6和DIG7引脚）                    禁止通用输出            多余的DIG6和DIG7引脚
																		用于通用输出，分别由
																		闪烁位D6S 和D7S 控制
	*/
}


/*两线制，启动总线信号*/
void  CH452_I2c_Start_2(void)
{
	CH452_SDA_OUT();
	CH452_SDA=1;     //sda线输出
	CH452_SCL=1;
	delay_us(4);
 	CH452_SDA=0; //START:when CLK is high,DATA change form high to low 
	delay_us(4);
	CH452_SCL=0;//钳住I2C总线，准备发送或接收数据 
}	  



void CH452_I2c_Stop_2(void)
{
    CH452_SDA_OUT();   //sda线输出
	
	CH452_SCL=0;
	CH452_SDA=0;
 	delay_us(4);
	
	CH452_SCL=1;
	CH452_SDA=1;//发送I2C总线结束信号
	delay_us(4);				
    	
}


void CH452_I2c_WrByte(unsigned char dat)	//写一个字节数据
{
	unsigned char i;
	CH452_SDA_OUT();   /* 设置SDA为输出方向 */
	CH452_SCL=0;
	
	for(i=0;i<8;i++)  // 输出8位数据
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
	
	CH452_SCL=1;  // 接收应答
	delay_us(2);
	
	CH452_SCL=0;
	delay_us(2);
}

unsigned char  CH452_I2c_RdByte(void)		//读一个字节数据
{
	unsigned char dat,i;
	CH452_SDA_IN();   /* 设置SDA为输入方向 */
	dat=0;
	for(i=0;i<8;i++)  // 输入8位数据
	{
		CH452_SCL=0;
		delay_us(2);
		CH452_SCL=1;
		dat<<=1;
		if(READ_CH452_SDA) dat++;  // 输入1位
		CH452_SCL=0;
		delay_us(1);

	}
	return	dat;
	
}

void CH452_Write(unsigned short cmd)	//写命令
{
	
	CH452_I2c_Start_2();                  /*启动总线*/
   	CH452_I2c_WrByte(((unsigned char)(cmd>>7)&CH452_I2C_MASK)|CH452_I2C_ADDR1);  // CH452的ADDR=1时(默认)
	
   	CH452_I2c_WrByte((unsigned char)cmd); /*发送数据*/
	
  	CH452_I2c_Stop_2();                   /*结束总线*/ 

}

unsigned char CH452_Read(void)		//读取按键
{
	unsigned char keycode;
   	CH452_I2c_Start_2();                /*启动总线*/
	
   	CH452_I2c_WrByte(((unsigned char)(CH452_GET_KEY>>7)&CH452_I2C_MASK)|0x01|CH452_I2C_ADDR1); 
	
   	keycode=CH452_I2c_RdByte();         /*读取数据*/
	CH452_I2c_Stop_2();                 /*结束总线*/ 
	
	return(keycode);
	
}

/**
 * @brief 按键功能扫描
 * @param	None
 * @return 	KeyValue
 * @details	使用查询的方式，通过查询IO口的电位高点，
			执行按键的读取工作
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

