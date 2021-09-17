/**
* @file 	spo2.h
* @brief	Ѫ��ģ��ͷ�ļ�
* @details	����Ѫ��ģ��Ľ��պͷ������ݽṹ
* @author	��
* @date		date
* @version	A001
* @par Copyright (c): 
* 		���������㼼���������ι�˾
* @par History:         
*	  version: author, date, desc

*/
#ifndef __SPO2_H
#define __SPO2_H

#include "sys.h"

/**����Ѫ��ģ�鷢�͵Ľṹ������**/
struct SPO2_TxdFrame{
	u8 SpO2;
	u8 Pulse;
	u8 Status;
	u8 SensorError;
};   

/**����Ѫ��ģ����յ������ݽṹ**/
struct SPO2_PARAMATERS
{
	u8 WaveData;//����
	u8 SensorError;//û��⻼��
	u8 PulseRate;//����
	u8 SpO2;//Ѫ��Ũ��
};
#endif
