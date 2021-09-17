#ifndef __IRAM_H
#define __IRAM_H

#include "sys.h"
#include "datahandle.h"

#define IRMA_NA  0
#define IRMA_CO2 1
#define IRMA_AA  2

typedef struct {
	
	u8  Id;
	u8  Sts;
	u16 CO2;
	u16 N2O;
	u16 AA1;
	u16 AA2;
	u16 O2;
	u8  SlowData[6];
	
} IRMA_SENSOR_RXD_PARAMETERS; /**AA/CO2麻醉气体模块接收数据结构**/

typedef struct {
    u8  IRMA_ID;
    u8 	EtCO2;
    u8 	FiCO2;
    u8 	EtAA1;
    u8 	FiAA1;
    u8 	EtAA2;
    u8 	FiAA2;
    u8 	EtN2O;
    u8 	FiN2O;
    u8 	AA1Id;
    u8	AA2Id;
    u8	Sensor_Err;
    u8 	Adapter_status;
    u8  Data_Valid;
		u8	Zero;
		u8 	Config;
    u16 CO2;
    u16 N2O;
		u16 AA1;
		u16 AA2;
    u16 AtmPressure;
} IRMA_SENSOR_TXD_FRAME_DATA;  /**IRMA传感发送数据结构**/


void USART1_Init(u32 baud);

void IRAM_Send_Data_Transmit(void);

#endif 
