
#ifndef __DataHandle_H
#define	__DataHandle_H

#include "sys.h"

/**向IRAM发送数据**/
#define UART1_IRAM_CALI_ID											0x0001


#define		 UART3_RECV_VER_ID										0x01//版本信息
#define		 UART3_RECV_KEY_VALUE_ID							0x02//按键信息	
#define		 UART3_RECV_TOUCH_DATA_ID							0x03//触摸屏数据接收
#define		 UART3_RECV_SELF_TEST_ID							0x04//触摸屏数据接收

#define    UART3_SEND_ALARM_LED_ID  						0x01    //报警LED设置
#define	   UART3_SEND_KEY_LED_ID	    					0x02		//按键LED设置
#define    UART3_SEND_PWM_ID										0x03		//LCD PWM 设置
#define	   UART3_SEND_POWER_LED_ID	    				0x04		//按键LED设置
#define		 UART3_SEND_SELF_TEST_ID							0x05		//模块板自检

#define    UART3_SEND_ALARM_LED_SET  						0x0001    //报警LED设置
#define	   UART3_SEND_KEY_LED_SET	    					0x0002		//按键LED设置
#define    UART3_SEND_PWM_SET										0x0004		//LCD PWM 设置
#define	   UART3_SEND_POWER_LED_SET	    				0x0008		//按键LED设置
#define		 UART3_SEND_SELF_TEST									0x0010		//模块板初始化


/**接收到的信息标志位**/
#define    UART5_RECV_ALARM_SET  								0x01    //报警设置
#define	   UART5_RECV_KEY_LED_SET	    					0x02	//按键LED设置
#define    UART5_RECV_EFLOW_SET									0x03	//电子流量计设置
#define    UART5_RECV_IRMA_CALI									0x04	//麻醉气体模快校验
#define    UART5_RECV_OXYGEN_CALI    						0x05	//顺磁氧校验
#define    UART5_RECV_EFLOW_CALI  							0x06	//电子流量计校准
#define		 UART5_RECV_SELF_TEST									0x07	//模块板自检
#define		 UART5_RECV_LCD_PWM_SET								0x08	//LCD背光设置
//#define    GET_ID_FLAG			1<<6	//ID获取

/** 系统任务处理**/
#define		 	TASK_UPDATE_ALARM										0x00000001 //更新报警信息
#define 		TASK_UPDATE_VER											0x00000002 
#define			TASK_UPDATE_KEY_VALUE								0x00000004
#define			TASK_UPDATE_CALI_STATUS							0x00000008

#define		 	TASK_UPDATE_250MS										0x00000010//250ms
#define		 	TASK_UPDATE_20MS								  	0x00000020//20ms
#define		 	TASK_UPDATE_IRAM_CALI								0x00000040//校准信息
#define		 	TASK_EFLOW_SET_VALUE								0x00000080//校准信息

#define		 	TASK_EFLOW_CALI_ID									0x00000100//校准信息
#define		 	TASK_UART3_INIT_ID									0x00000200//校准信息
#define		 	TASK_LCD_PWM_SET_ID									0x00000400//校准信息

#define			TASK_SELF_TEST_HANDLER							0x00001000
#define			TASK_SEND_SELF_TEST									0x00002000
#define			TASK_UPDATE_KEY_LED									0x00004000
#define			TASK_EFLOW_SEND_ERROR								0x00008000

#define			TASK_SEND_SELF_RESULT								0x00010000
//#define			TASK_SEND_SOURCE_PRESSURE						0x00010000

/** 系统任务处理**/
#define		 UART5_SEND_UPDATE_VER								0x00000002//版本信息，从1开始增加
#define		 UART5_SEND_UPDATE_KEY_VALUE					0x00000004//按键信息	
#define		 UART5_SEND_UPDATE_POWER_STATUS				0x00000008//电源状态信息

#define		 UART5_SEND_UPDATE_EFLOW_VALUE				0x00000010//电子流量计数据信息	
#define		 UART5_SEND_UPDATE_SPO2_VALUE					0x00000020//血氧数据信息
#define		 UART5_SEND_UPDATE_FIO2_VALUE					0x00000040//顺磁氧数据信息	
#define		 UART5_SEND_UPDATE_AA_DATA0						0x00000080//AA/EtCO2 发送数据0

#define		 UART5_SEND_UPDATE_AA_DATA1						0x00000100//AA/EtCO2 发送数据1	
#define		 UART5_SEND_UPDATE_AA_DATA2						0x00000200//AA/EtCO2 发送数据2
#define		 UART5_SEND_UPDATE_AA_DATA3						0x00000400//AA/EtCO2 发送数据3	
#define		 UART5_SEND_UPDATE_CALI_STATUS				0x00000800//校准信息


#define		 UART5_SEND_TOUCH_DATA_STATUS					0x00001000
#define		 UART5_SEND_SELF_TEST_STATUS					0x00002000
#define		 UART5_SEND_EFLOW_ERROR_STATUS				0x00004000
#define		 UART5_SEND_SELF_TEST_RESULT  				0x00008000
//#define		 UART5_SEND_SOURCE_PRESSURE_STATUS		0x00008000

//#define		 UART5_SEND_YOKE_PRESSURE_STATUS			0x00010000




#define	KM_HEAD_LEN											3


/*******************EFlow recv data ID******************************/

//#define UART2_RECV_EFLOW_MAN_OXY                 	0xFA
//#define UART2_RECV_EFLOW_MAN_AIR                 	0xFC
//#define UART2_RECV_EFLOW_MAN_N2O                 	0xFB
//#define UART2_RECV_EFLOW_MAN_TOTAL               	0xFD


#define UART2_RECV_EFLOW_AUTO_OXY									0xE0
#define UART2_RECV_EFLOW_AUTO_BAL									0xE1
#define UART2_RECV_EFLOW_AUTO_CALI			  				0xE2
#define UART2_RECV_EFLOW_AUTO_ERROR		  					0xE3
#define UART2_RECV_EFLOW_AUTO_ALT				  				0xE4
#define UART2_RECV_EFLOW_AUTO_VER				  				0xE5
#define UART2_RECV_EFLOW_SELF_STATUS		  				0xE6
#define UART2_RECV_EFLOW_SELF_RESULT		  				0xE7

//#define UART2_RECV_EFLOW_SOURCE_OXY_ID						0xE8
//#define UART2_RECV_EFLOW_SOURCE_AIR_ID						0xE9
//#define UART2_RECV_EFLOW_SOURCE_N2O_ID						0xEA

//#define UART2_RECV_EFLOW_YOKE_OXY_ID							0xEB
//#define UART2_RECV_EFLOW_YOKE_AIR_ID							0xEC
//#define UART2_RECV_EFLOW_YOKE_N2O_ID							0xED


/***************************end*************************************/


/*******************EFlow send data ID******************************/

#define UART2_SEND_EFLOW_CALI_FLAG								0x0001
#define UART2_SEND_EFLOW_BAL_FLAG									0x0002
#define UART2_SEND_EFLOW_OXY_FLOW_FLAG						0x0004
#define UART2_SEND_EFLOW_BAL_FLOW_FLAG						0x0008

#define UART2_SEND_EFLOW_SELF_TEST_FLAG						0x0010


#define UART2_SEND_EFLOW_SET_CALI 								0xF0
#define UART2_SEND_EFLOW_SET_BALANCE 							0xF1
#define UART2_SEND_EFLOW_SET_OXY_FLOWVALUE 				0xF2
#define UART2_SEND_EFLOW_SELF_TEST			 					0xF3
#define UART2_SEND_EFLOW_SET_BAL_FLOWVALUE			 	0xF4

/***************************end*************************************/


#define BAT_STATE_DISCONNECT						0
#define BAT_STATE_DISCHARGE_FAIL				1
#define BAT_STATE_DISCHARGE_LOW					2
#define BAT_STATE_DISCHARGE_MED					3
#define BAT_STATE_DISCHARGE_FULL				4
#define BAT_STATE_CHARGE_ING						5
#define BAT_STATE_CHARGE_COM						6


#define SERIAL_CONNECT_FLAG							1<<0


/*数据发送各种类型串口数据的ID*/
#define	SEND_VER_ID											0
#define	SEND_KEYVALUE_ID								1
#define	SEND_POWERSTATUS_ID							2
#define	SEND_EFLOWVALUE_ID							3
#define	SEND_SPO2VALUE_ID								4
#define	SEND_FIO2VALUE_ID								5
#define	SEND_AA_DATA0_ID								6
#define	SEND_AA_DATA1_ID								7
#define	SEND_AA_DATA2_ID								8
#define	SEND_AA_DATA3_ID								9
#define	SEND_CALISTATUS_ID							10
#define	SEND_TOUCH_DATA_ID							11
#define	SEND_SELF_TEST_ID								12
#define	SEND_EFLOW_ERROR_ID							13
#define	SEND_SELF_TEST_RESULT_ID			  14

//#define	SEND_SOURCE_PRESSURE_ID					14
//#define	SEND_SOURCE_YOKE_ID							15

typedef struct {
    u8 volume;
    u8 alarm_priority;
    u8 silence;
    u8 alarm_led;
} ALARM_SET;			//报警设置

typedef struct {
    u16 KEY_LED;
} KEY_LED_SET;  		//按键LED设置

typedef struct
{
	u16			balance_gas;
	u16			eflowCotrolMode;
	u16			oxyFlowValue;
	u16			balFlowValue;
}EFLOW_METER_SET;//电子流量计设置

typedef struct {
    u16 oxyValue;
    u16 balValue;
    u16 airValue;
		u16 totalValue;
    u16 altFlag;
	  u16 check_result;
	  u16 errorFlag;
} EFlow_Monitor_Data;			//电子流量计监测值


//typedef struct {
//	u16  sourceOxyValue;
//	u16  sourceAirValue;
//	u16  sourceN2OValue;
//	u16  yokeOxyValue;
//	u16  yokeAirValue;
//	u16  yokeN2OValue;
//}SourcePressureData;

typedef struct {
    u8 Item;
    u8 value;
} IRAM_CALI;        //麻醉气体模快校验

typedef struct{
    u8 Id;
} OXYGEN_SET; //顺磁氧校验

typedef struct {
    u8 Id;
} EFLOWMETER_SET; //电子流量计校验

typedef struct {
	u8 	id;
	u8	status;
}CALI_STATUS;

typedef struct {
    u8 Id;
} GET_ID;     //获取ID

typedef enum{
  FINISHED,
  CHARGEED,
  NO_INSTALLED
}BatteryState;

typedef enum{
  AC_WORK = 0x11,
  DC_WORK = 0x22,
}PowerState;

typedef struct {
    u8 batteryState;
		PowerState	 powerState;
} POWER_INFO;     //获取ID


extern u8 UART5_TX_BUSY_FLAG;
extern u8 UART2_TX_BUSY_FLAG;
extern u8 UART3_TX_BUSY_FLAG;

extern u32 System_Task_Register; 
extern u16 IRMA_Counter;
extern u16 SPO2_Counter;
extern u16 BatteryHighLevel;
extern u16 BatteryLowLevel;

extern u16 keyValue;

void 	Uart5_ReceiveCommandProcess(u8 len, u8 *array);
void 	Uart4_ReceiveCommandProcess(u8 *array);
void 	Uart3_ReceiveCommandProcess(u8 len, u8 *array);
void 	Uart2_RecvDataProcess(u8 *array);
void 	System_Task_Handler(void);
u8 		Inquire_TXD_Handler(USART_TypeDef* USARTx);
void 	Check_IRAM_ModuleExist(void);
void 	Check_SPO2_ModuleExist(void);
u8 		GetBatteryVolume(u16 BatteryVoltage);
void 	getPowerInfo(void);
void 	kmSelfTest(void);
void  paraValueInit(void);
#endif
