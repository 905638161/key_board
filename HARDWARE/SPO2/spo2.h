/**
* @file 	spo2.h
* @brief	血氧模块头文件
* @details	定义血氧模块的接收和发送数据结构
* @author	亚
* @date		date
* @version	A001
* @par Copyright (c): 
* 		北京易世恒技术有限责任公司
* @par History:         
*	  version: author, date, desc

*/
#ifndef __SPO2_H
#define __SPO2_H

#include "sys.h"

/**定义血氧模块发送的结构体数据**/
struct SPO2_TxdFrame{
	u8 SpO2;
	u8 Pulse;
	u8 Status;
	u8 SensorError;
};   

/**定义血氧模块接收到的数据结构**/
struct SPO2_PARAMATERS
{
	u8 WaveData;//波形
	u8 SensorError;//没检测患者
	u8 PulseRate;//脉率
	u8 SpO2;//血氧浓度
};
#endif
