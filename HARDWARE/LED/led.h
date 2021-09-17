#ifndef __LED_H
#define __LED_H
#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////

//All rights reserved
//////////////////////////////////////////////////////////////////////////////////
#define LED1 PDout(15)	// PB0



/**
  * @brief  ö�ٱ���״̬
  */

#define  	Alarm_None_Flag   									0x00  //û�б���
#define  	Alarm_High_Flag   									0x04  //�߼�����
#define  	Alarm_Medium_Flag 									0x02  //�м�����
#define  	Alarm_Low_Flag    									0x01  //�ͼ�����
#define   Alarm_High_and_Medium_and_low_Flag 	0x07  //�߼����м��͵ͼ�����
#define   Alarm_High_and_Medium_Flag 					0x06  //�߼��������м�����ͬʱ����
#define   Alarm_High_and_Low_Flag   					0x05	//�߼������͵ͼ�����ͬʱ����
#define		Medium_Flag_and_Low_Flag    				0x03	//�м������͵ͼ�����


#define		ALARM_YELLOW_H_LED 		PEout(2)	
#define		ALARM_YELLOW_L_LED 		PEout(5)	
#define		ALARM_RED_H_LED 			PEout(4)	
#define		ALARM_RED_L_LED 			PEout(3)		


#define		STANDBY_INDICATOR_LED		0x0001
#define		SILENCE_INDICATOR_LED		0x0002


void LED_Init(void);  //��ʼ��
void Touch_Busy_Led_Init(void);
void Bat_LED_Init(void);
void LED_Flash (void);//ָʾLED�Ƶ���˸
void setLED_Flash(u8 on);
void Alarm_LED_Init(void);
void Alarm_LED_Flash(u8 module);
void setBatLedState(u8 state);
void BackLight_Init(void);

void IndicatorLedInit(void);
void setIndicatorLed(u16 ledState);
void setTouchBusyLed(u8 state);
#endif
