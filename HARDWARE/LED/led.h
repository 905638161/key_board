#ifndef __LED_H
#define __LED_H
#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////

//All rights reserved
//////////////////////////////////////////////////////////////////////////////////
#define LED1 PDout(15)	// PB0



/**
  * @brief  枚举报警状态
  */

#define  	Alarm_None_Flag   									0x00  //没有报警
#define  	Alarm_High_Flag   									0x04  //高级报警
#define  	Alarm_Medium_Flag 									0x02  //中级报警
#define  	Alarm_Low_Flag    									0x01  //低级报警
#define   Alarm_High_and_Medium_and_low_Flag 	0x07  //高级、中级和低级报警
#define   Alarm_High_and_Medium_Flag 					0x06  //高级报警和中级报警同时发生
#define   Alarm_High_and_Low_Flag   					0x05	//高级报警和低级报警同时发生
#define		Medium_Flag_and_Low_Flag    				0x03	//中级报警和低级报警


#define		ALARM_YELLOW_H_LED 		PEout(2)	
#define		ALARM_YELLOW_L_LED 		PEout(5)	
#define		ALARM_RED_H_LED 			PEout(4)	
#define		ALARM_RED_L_LED 			PEout(3)		


#define		STANDBY_INDICATOR_LED		0x0001
#define		SILENCE_INDICATOR_LED		0x0002


void LED_Init(void);  //初始化
void Touch_Busy_Led_Init(void);
void Bat_LED_Init(void);
void LED_Flash (void);//指示LED灯的闪烁
void setLED_Flash(u8 on);
void Alarm_LED_Init(void);
void Alarm_LED_Flash(u8 module);
void setBatLedState(u8 state);
void BackLight_Init(void);

void IndicatorLedInit(void);
void setIndicatorLed(u16 ledState);
void setTouchBusyLed(u8 state);
#endif
