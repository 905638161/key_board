#ifndef __USART_H
#define __USART_H
#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////


//********************************************************************************

enum data_id
{
    ALARM_SET_ID = 1,  //报警设置
    KEY_LED_SET_ID,	//按键LED设置
    EFLOW_SET_ID,		//电子流量计设置
    IRMA_CAL_ID,		//麻醉气体模快校验
    OXYGEN_CAL_ID,    	//顺磁氧校验
    EFLOWMETER_SET_ID,
    GET_ID_DATA,
};


extern ALARM_SET	Alarm_Set;
void USART2_Init(u32 baud);
void USART3_Init(u32 baud);

void UART4_Init(u32 baud);
void UART5_Init(u32 baud);

void UART5_Data_Transmit(u8 len, u8 *data);
void UART4_Data_Transmit(void);
void USART3_Data_Transmit(u8 len, u8 *data);
void USART2_Data_Transmit(void);

u8 CalculateCheckSum(u8 len, u8 *array);

#endif


