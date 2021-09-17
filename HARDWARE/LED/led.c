
#include "sys.h"

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

//初始化PB5和PE5为输出口.并使能这两个口的时钟
//LED IO初始化
void LED_Init(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);	 //使能PE端口时钟

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;				 //LED0-->PB.0端口配置
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
    GPIO_Init(GPIOD, &GPIO_InitStructure);					 //根据设定参数初始化GPIOB.0
    GPIO_SetBits(GPIOD, GPIO_Pin_15);				 //PB.0输出高
}


void Touch_Busy_Led_Init(void)
{
		GPIO_InitTypeDef  GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	 //使能PE端口时钟

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;				 //LED0-->PB.0端口配置
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
    GPIO_Init(GPIOA, &GPIO_InitStructure);					 //根据设定参数初始化GPIOB.0
    GPIO_SetBits(GPIOA, GPIO_Pin_11);						 //PB.0输出高
}

void IndicatorLedInit(void)
{
		GPIO_InitTypeDef  GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	 

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14|GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
    GPIO_Init(GPIOB, &GPIO_InitStructure);					 //根据设定参数初始化GPIOB.0
	
		GPIO_SetBits(GPIOB, GPIO_Pin_14);
		GPIO_SetBits(GPIOB, GPIO_Pin_15);
}

void Bat_LED_Init(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD|RCC_APB2Periph_AFIO, ENABLE);	 //使能PE端口时钟

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;				
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
    GPIO_Init(GPIOD, &GPIO_InitStructure);					 //根据设定参数初始化GPIOB.0
    GPIO_SetBits(GPIOD, GPIO_Pin_8);						 //PB.0输出高
}



void BackLight_Init(void)
{
		GPIO_InitTypeDef  GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD|RCC_APB2Periph_AFIO, ENABLE);	 //使能PE端口时钟

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4|GPIO_Pin_6;				 //LED0-->PB.0端口配置
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
    GPIO_Init(GPIOD, &GPIO_InitStructure);					 //根据设定参数初始化GPIOB.0
    GPIO_SetBits(GPIOD, GPIO_Pin_6);						 //PB.0输出高
		GPIO_SetBits(GPIOD, GPIO_Pin_4);
}

//LED灯的闪烁程序
void LED_Flash (void)
{
    static u32 temp;
    temp++;
    if(temp < 500000)
        LED1 = 1;      //FLAG输出高
    else if(temp < 1000000)
        LED1 = 0;      //FLAG输出低
    else temp = 0;

}

void setLED_Flash(u8 on)
{
	 LED1 = on;
}


void setBatLedState(u8 state)
{
		if(state)
		{
				GPIO_SetBits(GPIOD, GPIO_Pin_8);
		}
		else
		{
				GPIO_ResetBits(GPIOD, GPIO_Pin_8);
		}
}



void setTouchBusyLed(u8 state)
{
		if(state)
		{
				GPIO_ResetBits(GPIOA, GPIO_Pin_11);
		}
		else
		{
				GPIO_SetBits(GPIOA, GPIO_Pin_11);
		}
}



void setIndicatorLed(u16 ledState)
{
		if(ledState & SILENCE_INDICATOR_LED)
		{
				GPIO_SetBits(GPIOB, GPIO_Pin_14);
		}
		else
		{
				GPIO_ResetBits(GPIOB, GPIO_Pin_14);
		}		
		
		if(ledState & STANDBY_INDICATOR_LED)
		{
				GPIO_SetBits(GPIOB, GPIO_Pin_15);
		}
		else
		{
				GPIO_ResetBits(GPIOB, GPIO_Pin_15);
		}
}

/**
  * @brief  初始化报警灯控制的IO口
  * @param  None
  * @retval None
  */
void Alarm_LED_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);	 //使能PD端口时钟

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;//最高输出速率2MHz
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;//推挽输出
    GPIO_Init(GPIOE, &GPIO_InitStructure);//初始化外设GPIOx寄存器

}

/**
 * @brief 报警灯的闪烁
 * @param module：
					报警灯闪烁模式
					Alarm_Set.alarm_led
 * @return the return value
 * @details a full description of what the function does
**/
void Alarm_LED_Flash(u8 module)
{
    switch(module)
    {
    case Alarm_High_Flag:     //红灯闪烁
        if(Flag_250ms)
        {
            ALARM_RED_H_LED = 1;
            ALARM_RED_L_LED = 1;
					  ALARM_YELLOW_L_LED = 0;
            ALARM_YELLOW_H_LED = 0;
        }
        else
        {
            ALARM_RED_H_LED = 0;
            ALARM_RED_L_LED = 0;
					  ALARM_YELLOW_L_LED = 0;
            ALARM_YELLOW_H_LED = 0;
        }
        break;
    case Alarm_Medium_Flag:  //黄灯闪烁
	  case Medium_Flag_and_Low_Flag:
        if(Flag_1s)
        {
            ALARM_YELLOW_L_LED = 1; 
					  ALARM_RED_H_LED = 0;
					  ALARM_RED_L_LED = 0;
            ALARM_YELLOW_H_LED = 1;
        }
        else
        {
            ALARM_YELLOW_L_LED = 0;
            ALARM_YELLOW_H_LED = 0;
					  ALARM_RED_H_LED = 0;
            ALARM_RED_L_LED = 0;
        }
        break;
    case Alarm_Low_Flag:    //黄灯常亮
        ALARM_YELLOW_L_LED = 1;
        ALARM_RED_H_LED = 0;
        ALARM_RED_L_LED = 0;
        ALARM_YELLOW_H_LED = 1;
        break;
    case Alarm_High_and_Medium_Flag:
		case Alarm_High_and_Medium_and_low_Flag:
        /**左半边高级报警灯闪烁**/
        if(Flag_250ms)
        {
            ALARM_RED_H_LED = 1;
            ALARM_RED_L_LED = 0;
        }
        else
        {
            ALARM_RED_H_LED = 0;
            ALARM_RED_L_LED = 0;
        }
        /**右半边中级报警灯闪烁**/
        if(Flag_1s)
        {
            ALARM_YELLOW_L_LED = 1;
            ALARM_YELLOW_H_LED = 0;
        }
        else
        {
            ALARM_YELLOW_L_LED = 0;
            ALARM_YELLOW_H_LED = 0;
        }
        break;
    case Alarm_High_and_Low_Flag:
        /**左半边高级报警灯闪烁**/
        if(Flag_250ms)
        {
            ALARM_RED_H_LED = 1;
            ALARM_RED_L_LED = 0;
        }
        else
        {
            ALARM_RED_H_LED = 0;
            ALARM_RED_L_LED = 0;
        }
        /**右半边低级报警灯常亮**/
        ALARM_YELLOW_L_LED = 1;
        ALARM_YELLOW_H_LED = 0;
        break;
    case Alarm_None_Flag:

        ALARM_YELLOW_L_LED = 0;
        ALARM_RED_H_LED = 0;
        ALARM_RED_L_LED = 0;
        ALARM_YELLOW_H_LED = 0;
        break;
    default:
        ALARM_YELLOW_L_LED = 0;
        ALARM_RED_H_LED = 0;
        ALARM_RED_L_LED = 0;
        ALARM_YELLOW_H_LED = 0;
        break;
    }

}
