

#include "sys.h"
#include "datahandle.h"
#include "usart.h"
#include "iram.h"
#include "spo2.h"
#include "adc.h"


u8 UART5_TX_BUSY_FLAG = FALSE ;
u8 UART2_TX_BUSY_FLAG = FALSE ;
u8 UART3_TX_BUSY_FLAG = FALSE ;
//u8 UART2_TX_BUSY_FLAG = FALSE ;
u32 USART5_TXD_REGISTER = 0; //发送寄存器
u32 USART1_TXD_REGISTER = 0; //发送寄存器
u32 USART3_TXD_REGISTER = 0; //发送寄存器
u32 USART2_TXD_REGISTER = 0;

u32 System_Task_Register = 0; //系统事件处理

ALARM_SET		Alarm_Set;
KEY_LED_SET		KEY_LED_Set;
EFLOW_METER_SET		EFlow_Meter_Set;
IRAM_CALI		IRAM_CaliData;
OXYGEN_SET		Oxygen_Set;
EFLOWMETER_SET	EflowMeter_Set;
CALI_STATUS			Cali_Status;
GET_ID			Get_Id;

/**发送的数据数组**/
u16  KM_Ver 				= 2 << 12 | 1 << 7 | 0;
u16  EFlow_Ver 			= 0 << 12 | 0 << 7 | 0;
u16  DM_Board_Ver 	= 0 << 12 | 0 << 7 | 0;

POWER_INFO powerInfo;

u16		kmSelfStatus;
u16  	eflowSelfStatus;

extern u8 Uart5TxBuffer[14];
extern u8 Uart2TxBuffer[14];

extern IRMA_SENSOR_TXD_FRAME_DATA	IRMA_Txd_SensorFrameData;
extern struct SPO2_TxdFrame SPO2_TxdData;

extern u8 UART1_TX_BUSY_FLAG;

u16 keyValue;

BatteryState batteryState;
EFlow_Monitor_Data eflowMonitorData;
//SourcePressureData sourcePressureData;
u8 LcdPwmValue;

u16 currentKeyLed;

u8 GetBatteryVolume(u16 BatteryVoltage)
{
	u8	VolumeRank;
	static  u8 LastVolumeRank;
	
	BatteryVoltage = BatteryVoltage/10;
	
	if (BatteryVoltage >= 260)  
	{
		VolumeRank = 4;
	}
	else if (BatteryVoltage>= 226)
	{
		if (LastVolumeRank <= 3)
		{
			VolumeRank = 3;
		}
		else
		{
			VolumeRank = 4;
		}
	}
	else if (BatteryVoltage>= 225)
	{
		VolumeRank = 3;
	}
	else if (BatteryVoltage >= 209)
	{
		if (LastVolumeRank <= 2)
		{
			VolumeRank = 2;
		}
		else
		{
			VolumeRank = 3;
		}
	}
	else if (BatteryVoltage >= 208) 
	{
		VolumeRank = 2;
	}
	else if (BatteryVoltage>= 191)
	{
		if (LastVolumeRank <= 1)
		{
			VolumeRank = 1;
		}
		else
		{
			VolumeRank = 2;
		}
	}
	else //if (BatteryVoltage>= 190)
	{
		VolumeRank = 1;
	}
	
	LastVolumeRank = VolumeRank;
	
	return VolumeRank;
}


BatteryState getBatteryStatus(void)
{
  BatteryState ret = FINISHED;
  static BatteryState last_ret = FINISHED;
  static unsigned char count;
	
  if(BatteryHighLevel == 100)
  {
			ret = CHARGEED;
			count = 0;
  }
  else if(BatteryLowLevel == 100)
  {
			ret = FINISHED;
			count = 0;
  }
  else
  {
			if(last_ret != NO_INSTALLED)
			{
					count ++;
					if(count >= 4)
					{
						ret = NO_INSTALLED;
						count = 0;
					}
			}
			else
			{
					ret = last_ret;
			}
  }
  
  last_ret = ret;
  return ret;
}

/**
 * @brief 电源信息的处理
 * @param capacity：电池的电量，数值选择（0-6）
		@arg：
				0：电池未连接
				1：电池耗尽
				2：电池电量低
				3：电池放电一半
				4：电池满电
				5：电池充电中
				6：电池充电完成
 * @param status：电源状态
 * @return None
**/

void getPowerInfo(void)
{
	static u8 count;
	static u8 count1;
	static PowerState lastPowerState;
	PowerState powerState;
	
	if(CheckAcStatus())
	{
			powerState = AC_WORK;
	}
	else
	{
			powerState = DC_WORK;
	}
	
	if(powerState == AC_WORK)
	{
		  if(lastPowerState == AC_WORK)
			{
				  powerInfo.powerState = AC_WORK;
					batteryState = getBatteryStatus();
					if(batteryState == FINISHED)
					{
							powerInfo.batteryState = BAT_STATE_CHARGE_COM;
							count1 = 0;
					}
					else if(batteryState == CHARGEED)
					{		
							powerInfo.batteryState = BAT_STATE_CHARGE_ING;
							count1 = 0;
					}
					else
					{								
							if(count1 ++ > 10)
							{
									powerInfo.batteryState = BAT_STATE_DISCONNECT;
									count1 = 0;
							}
					}
					count = 0;
			}
			else
			{
				  if(count ++ >= 8)
					{
						lastPowerState = powerState;
						count = 0;
					}
					
					count1 = 0;
			}
	}
	else
	{
		   if(lastPowerState == DC_WORK)
			 {
				    powerInfo.powerState = DC_WORK;
						powerInfo.batteryState = GetBatteryVolume(getBatteryValue());
						count = 0;
			 }
			 else
			 {
				  if(count ++ >= 8)
					{
							lastPowerState = powerState;
							count = 0;
					}
			 }
			 count1 = 0;
	}
}


/*---------------------------------------------------------------------------------------------------*/
/*******************************以下为接收的数据预处理************************************************/
/*---------------------------------------------------------------------------------------------------*/

/**
 * @brief 从上位机接收到的信息处理
 * @param None
 * @return None
 * @details 对收到的信息进行处理
**/

void System_Task_Handler(void)
{
		u32 i;
    if(System_Task_Register)
    {
        for(i = 0; i < 32; i++)
        {
            if(System_Task_Register & (1 << i))
            {
                break;
            }
        }
				
				switch(1<<i)
				{
					case TASK_UPDATE_20MS :
					{
								System_Task_Register &= ~TASK_UPDATE_20MS;
								if(IRMA_Txd_SensorFrameData.IRMA_ID != IRMA_NA)
								{
										USART5_TXD_REGISTER  |= UART5_SEND_UPDATE_AA_DATA2;		
									
										if(UART5_TX_BUSY_FLAG==FALSE)
										{
												Inquire_TXD_Handler(UART5);
										}
								}
								
								if(tsData.pendown)
								{
										USART5_TXD_REGISTER  |= UART5_SEND_TOUCH_DATA_STATUS;
										if(tsData.yData == 0)
										{
												tsData.pendown = 0;
										}
										
										if(tsData.pendown)
										{
												setTouchBusyLed(1);
										}
										else
										{
												setTouchBusyLed(0);
										}
										
										if(UART5_TX_BUSY_FLAG==FALSE)
										{
												Inquire_TXD_Handler(UART5);
										}
								}
					}
					break;
					case TASK_UPDATE_250MS:
					{
								System_Task_Register &= ~TASK_UPDATE_250MS;
								Alarm_LED_Flash(Alarm_Set.alarm_led);
						    getPowerInfo();
						    Check_IRAM_ModuleExist();
								Check_SPO2_ModuleExist();
								USART5_TXD_REGISTER  |= UART5_SEND_UPDATE_POWER_STATUS|UART5_SEND_UPDATE_EFLOW_VALUE;
						
								if(IRMA_Txd_SensorFrameData.IRMA_ID != IRMA_NA)
								{
										USART5_TXD_REGISTER  |= UART5_SEND_UPDATE_AA_DATA0|UART5_SEND_UPDATE_AA_DATA1|UART5_SEND_UPDATE_AA_DATA3;
								}
								
								if(SPO2_TxdData.Status)
								{
										USART5_TXD_REGISTER  |= UART5_SEND_UPDATE_SPO2_VALUE;
								}
								
								setBatLedState(powerInfo.powerState == DC_WORK);
								
								if(UART5_TX_BUSY_FLAG==FALSE)
								{
										Inquire_TXD_Handler(UART5);
								}
					}
					break;
					case TASK_UPDATE_ALARM:
					{
								Alarm_voice_Select(Alarm_Set.alarm_priority,Alarm_Set.volume,Alarm_Set.silence);
								System_Task_Register &= ~TASK_UPDATE_ALARM;
					}
					break;
					case TASK_UPDATE_VER:
					{
								USART5_TXD_REGISTER  |= UART5_SEND_UPDATE_VER;
								System_Task_Register &= ~TASK_UPDATE_VER;
								
								if(UART5_TX_BUSY_FLAG==FALSE)
								{
										Inquire_TXD_Handler(UART5);
								}
					}
					break;
					case TASK_UPDATE_KEY_VALUE:
					{
								USART5_TXD_REGISTER  |= UART5_SEND_UPDATE_KEY_VALUE;
								System_Task_Register &= ~TASK_UPDATE_KEY_VALUE;
								
								if(UART5_TX_BUSY_FLAG==FALSE)
								{
										Inquire_TXD_Handler(UART5);
								}
					}
					break;
					case TASK_UPDATE_CALI_STATUS :
					{
							USART5_TXD_REGISTER  |= UART5_SEND_UPDATE_CALI_STATUS;
							System_Task_Register &= ~TASK_UPDATE_CALI_STATUS;
								
							if(UART5_TX_BUSY_FLAG==FALSE)
							{
										Inquire_TXD_Handler(UART5);
							}
					}
					break;
					case TASK_UPDATE_IRAM_CALI :
					{
							USART1_TXD_REGISTER  |= UART1_IRAM_CALI_ID;
							System_Task_Register &= ~TASK_UPDATE_IRAM_CALI;
								
							if(UART1_TX_BUSY_FLAG==FALSE)
							{
										Inquire_TXD_Handler(USART1);
							}
					}
					break;
					case TASK_EFLOW_SET_VALUE :
					{
							USART2_TXD_REGISTER  |= UART2_SEND_EFLOW_BAL_FLAG|UART2_SEND_EFLOW_OXY_FLOW_FLAG|UART2_SEND_EFLOW_BAL_FLOW_FLAG;
						
							System_Task_Register &= ~TASK_EFLOW_SET_VALUE;
								
							if(UART2_TX_BUSY_FLAG==FALSE)
							{
										Inquire_TXD_Handler(USART2);
							}
					}
					break;
					case TASK_EFLOW_SEND_ERROR:
					{
								USART5_TXD_REGISTER  |= UART5_SEND_EFLOW_ERROR_STATUS;
								
								if(UART5_TX_BUSY_FLAG==FALSE)
								{
										Inquire_TXD_Handler(UART5);
								}
					}
					break;
					case TASK_EFLOW_CALI_ID :
					{
							USART2_TXD_REGISTER  |= UART2_SEND_EFLOW_CALI_FLAG;
						
							System_Task_Register &= ~TASK_EFLOW_CALI_ID;
								
							if(UART2_TX_BUSY_FLAG==FALSE)
							{
										Inquire_TXD_Handler(USART2);
							}
					}
					break;
					case TASK_UART3_INIT_ID:
					{
							System_Task_Register &= ~TASK_UART3_INIT_ID;
					}
					break;
					case TASK_LCD_PWM_SET_ID:
					{
							setLCD_PWM_Compare(LcdPwmValue);
							System_Task_Register &= ~TASK_LCD_PWM_SET_ID;
				
					}
					break;
					case TASK_UPDATE_KEY_LED:
					{
							setIndicatorLed(currentKeyLed);
							System_Task_Register &= ~TASK_UPDATE_KEY_LED;
					}
					break;
//					case TASK_SEND_SOURCE_PRESSURE:
//					{
//								USART5_TXD_REGISTER  |= UART5_SEND_SOURCE_PRESSURE_STATUS|UART5_SEND_YOKE_PRESSURE_STATUS;
//								
//								if(UART5_TX_BUSY_FLAG==FALSE)
//								{
//										Inquire_TXD_Handler(UART5);
//								}
//					}
					case TASK_SEND_SELF_RESULT:
					{
							System_Task_Register &= ~TASK_SEND_SELF_RESULT;
							USART5_TXD_REGISTER  |= UART5_SEND_SELF_TEST_STATUS;
								
							if(UART5_TX_BUSY_FLAG==FALSE)
							{
										Inquire_TXD_Handler(UART5);
							}
					}
					break;
					case TASK_SEND_SELF_TEST:
					{
							System_Task_Register &= ~TASK_SEND_SELF_TEST;
							USART5_TXD_REGISTER  |=  UART5_SEND_SELF_TEST_RESULT;
								
							if(UART5_TX_BUSY_FLAG==FALSE)
							{
										Inquire_TXD_Handler(UART5);
							}
					}
					break;
					case TASK_SELF_TEST_HANDLER:
					{
							eflowSelfStatus = 0;
							kmSelfStatus = SERIAL_CONNECT_FLAG;
						
							//USART3_TXD_REGISTER  |= UART3_SEND_SELF_TEST;
							USART2_TXD_REGISTER  |= UART2_SEND_EFLOW_SELF_TEST_FLAG;
							System_Task_Register &= ~TASK_SELF_TEST_HANDLER;
								
							
							if(UART2_TX_BUSY_FLAG==FALSE)
							{
									Inquire_TXD_Handler(USART2);
							}
							
							kmSelfTest();
							
							System_Task_Register |= TASK_SEND_SELF_TEST;
					}
					break;
				}
    }
}


u8	Inquire_TXD_Handler(USART_TypeDef* USARTx)
{	
	int   i;
	u8  len = 0;
	u8  retVal=0;

	if(USARTx == UART5)
	{
			if(USART5_TXD_REGISTER == 0)
			{
					return 1;
			}

			for(i=0;i<32;i++)
			{
					if(USART5_TXD_REGISTER&(1<<i))
					{
							break;
					}
			}
			
			switch(1<<i)
			{
					case UART5_SEND_UPDATE_VER:
					{
								len = KM_HEAD_LEN;
								Uart5TxBuffer[len ++] = SEND_VER_ID;
								Uart5TxBuffer[len ++] = KM_Ver>>8;
								Uart5TxBuffer[len ++] = KM_Ver;
								Uart5TxBuffer[len ++] = EFlow_Ver>>8;
								Uart5TxBuffer[len ++] = EFlow_Ver;
								Uart5TxBuffer[len ++] = DM_Board_Ver>>8;
								Uart5TxBuffer[len ++] = DM_Board_Ver;
								Uart5TxBuffer[len]		= CalculateCheckSum(len+1,Uart5TxBuffer+KM_HEAD_LEN);
								len ++;
						
								UART5_Data_Transmit(len,Uart5TxBuffer);
								UART5_TX_BUSY_FLAG = TRUE;
								USART5_TXD_REGISTER &= ~UART5_SEND_UPDATE_VER;
					}
					break;
					case UART5_SEND_UPDATE_KEY_VALUE:
					{
								len = KM_HEAD_LEN;
								Uart5TxBuffer[len ++] = SEND_KEYVALUE_ID;
								Uart5TxBuffer[len ++] = keyValue>>8;
								Uart5TxBuffer[len ++] = keyValue;
								Uart5TxBuffer[len]		= CalculateCheckSum(len+1,Uart5TxBuffer+KM_HEAD_LEN);
								len ++;
						
								UART5_Data_Transmit(len,Uart5TxBuffer);
								UART5_TX_BUSY_FLAG = TRUE;
								USART5_TXD_REGISTER &= ~UART5_SEND_UPDATE_KEY_VALUE;
					}
					break;
					case UART5_SEND_UPDATE_POWER_STATUS:
					{
								len = KM_HEAD_LEN;
								Uart5TxBuffer[len ++] = SEND_POWERSTATUS_ID;
								Uart5TxBuffer[len ++] = powerInfo.batteryState;
								Uart5TxBuffer[len ++] = powerInfo.powerState;
								Uart5TxBuffer[len]		= CalculateCheckSum(len+1,Uart5TxBuffer+KM_HEAD_LEN);
								len ++;
						
								UART5_Data_Transmit(len,Uart5TxBuffer);
								UART5_TX_BUSY_FLAG = TRUE;
								USART5_TXD_REGISTER &= ~UART5_SEND_UPDATE_POWER_STATUS;
					}
					break;
					case UART5_SEND_UPDATE_EFLOW_VALUE:
					{
								len = KM_HEAD_LEN;
								Uart5TxBuffer[len ++] = SEND_EFLOWVALUE_ID;
								Uart5TxBuffer[len ++] = eflowMonitorData.oxyValue>>8;
								Uart5TxBuffer[len ++] = eflowMonitorData.oxyValue;
								Uart5TxBuffer[len ++] = eflowMonitorData.balValue>>8;
								Uart5TxBuffer[len ++] = eflowMonitorData.balValue;
								Uart5TxBuffer[len ++] = eflowMonitorData.airValue>>8;
								Uart5TxBuffer[len ++] = eflowMonitorData.airValue;
								Uart5TxBuffer[len ++] = eflowMonitorData.altFlag;
								Uart5TxBuffer[len]		= CalculateCheckSum(len+1,Uart5TxBuffer+KM_HEAD_LEN);
								len ++;
						
								UART5_Data_Transmit(len,Uart5TxBuffer);
								UART5_TX_BUSY_FLAG = TRUE;
								USART5_TXD_REGISTER &= ~UART5_SEND_UPDATE_EFLOW_VALUE;
					}
					break;
					case UART5_SEND_UPDATE_SPO2_VALUE :
					{
								len = KM_HEAD_LEN;
								Uart5TxBuffer[len ++] = SEND_SPO2VALUE_ID;
								Uart5TxBuffer[len ++] = SPO2_TxdData.SpO2;
								Uart5TxBuffer[len ++] = SPO2_TxdData.Pulse;
								Uart5TxBuffer[len ++] = SPO2_TxdData.Status;
								Uart5TxBuffer[len ++] = SPO2_TxdData.SensorError;
								Uart5TxBuffer[len]		= CalculateCheckSum(len+1,Uart5TxBuffer+KM_HEAD_LEN);
								len ++;
						
								UART5_Data_Transmit(len,Uart5TxBuffer);
								UART5_TX_BUSY_FLAG = TRUE;
								USART5_TXD_REGISTER &= ~UART5_SEND_UPDATE_SPO2_VALUE;
					}
					break;
					case UART5_SEND_UPDATE_FIO2_VALUE :
					{
								len = KM_HEAD_LEN;
								Uart5TxBuffer[len ++] = SEND_FIO2VALUE_ID;
								Uart5TxBuffer[len ++] = 0;
								Uart5TxBuffer[len ++] = 0;
								Uart5TxBuffer[len ++] = 0;
								Uart5TxBuffer[len]		= CalculateCheckSum(len+1,Uart5TxBuffer+KM_HEAD_LEN);
								len ++;
						
								UART5_Data_Transmit(len,Uart5TxBuffer);
								UART5_TX_BUSY_FLAG = TRUE;
								USART5_TXD_REGISTER &= ~UART5_SEND_UPDATE_FIO2_VALUE;
					}
					break;
					case UART5_SEND_UPDATE_AA_DATA0 :
					{
								len = KM_HEAD_LEN;
								Uart5TxBuffer[len ++] = SEND_AA_DATA0_ID;
								Uart5TxBuffer[len ++] = IRMA_Txd_SensorFrameData.EtCO2;
								Uart5TxBuffer[len ++] = IRMA_Txd_SensorFrameData.FiCO2;
								Uart5TxBuffer[len ++] = IRMA_Txd_SensorFrameData.IRMA_ID;
								Uart5TxBuffer[len ++] = IRMA_Txd_SensorFrameData.AtmPressure>>8;
								Uart5TxBuffer[len ++] = IRMA_Txd_SensorFrameData.AtmPressure;
								Uart5TxBuffer[len]		= CalculateCheckSum(len+1,Uart5TxBuffer+KM_HEAD_LEN);
								len ++;
						
								UART5_Data_Transmit(len,Uart5TxBuffer);
								UART5_TX_BUSY_FLAG = TRUE;
								USART5_TXD_REGISTER &= ~UART5_SEND_UPDATE_AA_DATA0;
					}
					break;
					case UART5_SEND_UPDATE_AA_DATA1 :
					{
								len = KM_HEAD_LEN;
								Uart5TxBuffer[len ++] = SEND_AA_DATA1_ID;
								Uart5TxBuffer[len ++] = IRMA_Txd_SensorFrameData.EtAA1;
								Uart5TxBuffer[len ++] = IRMA_Txd_SensorFrameData.FiAA1;
								Uart5TxBuffer[len ++] = IRMA_Txd_SensorFrameData.EtAA2;
								Uart5TxBuffer[len ++] = IRMA_Txd_SensorFrameData.FiAA2;
								Uart5TxBuffer[len ++] = IRMA_Txd_SensorFrameData.EtN2O;
								Uart5TxBuffer[len ++] = IRMA_Txd_SensorFrameData.FiN2O;
								Uart5TxBuffer[len ++] = IRMA_Txd_SensorFrameData.AA1Id;
								Uart5TxBuffer[len ++] = IRMA_Txd_SensorFrameData.AA2Id;
								Uart5TxBuffer[len]		= CalculateCheckSum(len+1,Uart5TxBuffer+KM_HEAD_LEN);
								len ++;
						
								UART5_Data_Transmit(len,Uart5TxBuffer);
								UART5_TX_BUSY_FLAG = TRUE;
								USART5_TXD_REGISTER &= ~UART5_SEND_UPDATE_AA_DATA1;
					}
					break;
					case UART5_SEND_UPDATE_AA_DATA2 :
					{
								len = KM_HEAD_LEN;
								Uart5TxBuffer[len ++] = SEND_AA_DATA2_ID;
								Uart5TxBuffer[len ++] = IRMA_Txd_SensorFrameData.CO2>>8;
								Uart5TxBuffer[len ++] = IRMA_Txd_SensorFrameData.CO2;
								Uart5TxBuffer[len ++] = IRMA_Txd_SensorFrameData.N2O>>8;
								Uart5TxBuffer[len ++] = IRMA_Txd_SensorFrameData.N2O;
								Uart5TxBuffer[len ++] = IRMA_Txd_SensorFrameData.AA1>>8;
								Uart5TxBuffer[len ++] = IRMA_Txd_SensorFrameData.AA1;
								Uart5TxBuffer[len ++] = IRMA_Txd_SensorFrameData.AA2>>8;
								Uart5TxBuffer[len ++] = IRMA_Txd_SensorFrameData.AA2;
								Uart5TxBuffer[len]		= CalculateCheckSum(len+1,Uart5TxBuffer+KM_HEAD_LEN);
								len ++;
						
								UART5_Data_Transmit(len,Uart5TxBuffer);
								UART5_TX_BUSY_FLAG = TRUE;
								USART5_TXD_REGISTER &= ~UART5_SEND_UPDATE_AA_DATA2;
					}
					break;
					case UART5_SEND_UPDATE_AA_DATA3 :
					{
								len = KM_HEAD_LEN;
								Uart5TxBuffer[len ++] = SEND_AA_DATA3_ID;
								Uart5TxBuffer[len ++] = IRMA_Txd_SensorFrameData.Sensor_Err;
								Uart5TxBuffer[len ++] = IRMA_Txd_SensorFrameData.Adapter_status;
								Uart5TxBuffer[len ++] = IRMA_Txd_SensorFrameData.Data_Valid;
								Uart5TxBuffer[len ++] = IRMA_Txd_SensorFrameData.Zero;
								Uart5TxBuffer[len ++] = IRMA_Txd_SensorFrameData.Config;
								Uart5TxBuffer[len]		= CalculateCheckSum(len+1,Uart5TxBuffer+KM_HEAD_LEN);
								len ++;
						
								UART5_Data_Transmit(len,Uart5TxBuffer);
								UART5_TX_BUSY_FLAG = TRUE;
								USART5_TXD_REGISTER &= ~UART5_SEND_UPDATE_AA_DATA3;
					}
					break;
					case UART5_SEND_UPDATE_CALI_STATUS :
					{
								len = KM_HEAD_LEN;
								Uart5TxBuffer[len ++] = SEND_CALISTATUS_ID;
								Uart5TxBuffer[len ++] = Cali_Status.id;
								Uart5TxBuffer[len ++] = Cali_Status.status;
								Uart5TxBuffer[len]		= CalculateCheckSum(len+1,Uart5TxBuffer+KM_HEAD_LEN);
								len ++;
						
								UART5_Data_Transmit(len,Uart5TxBuffer);
								UART5_TX_BUSY_FLAG = TRUE;
								USART5_TXD_REGISTER &= ~UART5_SEND_UPDATE_CALI_STATUS;
					}
					break;
					case UART5_SEND_TOUCH_DATA_STATUS:
					{
										len = KM_HEAD_LEN;
										Uart5TxBuffer[len ++] = SEND_TOUCH_DATA_ID;
										Uart5TxBuffer[len ++] = tsData.xData>>8;
										Uart5TxBuffer[len ++] = tsData.xData;
										Uart5TxBuffer[len ++] = tsData.yData>>8;
										Uart5TxBuffer[len ++] = tsData.yData;
										Uart5TxBuffer[len]		= CalculateCheckSum(len+1,Uart5TxBuffer+KM_HEAD_LEN);
										len ++;
								
										UART5_Data_Transmit(len,Uart5TxBuffer);
										UART5_TX_BUSY_FLAG = TRUE;
										USART5_TXD_REGISTER &= ~UART5_SEND_TOUCH_DATA_STATUS;
					}
					break;
					case UART5_SEND_SELF_TEST_RESULT:
					{
										len = KM_HEAD_LEN;
										Uart5TxBuffer[len ++] = SEND_SELF_TEST_RESULT_ID;
										Uart5TxBuffer[len ++] = eflowMonitorData.check_result>>8;
										Uart5TxBuffer[len ++] = eflowMonitorData.check_result;
										Uart5TxBuffer[len]		= CalculateCheckSum(len+1,Uart5TxBuffer+KM_HEAD_LEN);
										len ++;
								
										UART5_Data_Transmit(len,Uart5TxBuffer);
										UART5_TX_BUSY_FLAG = TRUE;
										USART5_TXD_REGISTER &= ~UART5_SEND_SELF_TEST_RESULT;
					}
					break;
					case UART5_SEND_SELF_TEST_STATUS:
					{
										len = KM_HEAD_LEN;
										Uart5TxBuffer[len ++] = SEND_SELF_TEST_ID;
										Uart5TxBuffer[len ++] = kmSelfStatus>>8;
										Uart5TxBuffer[len ++] = kmSelfStatus;
										Uart5TxBuffer[len ++] = eflowSelfStatus>>8;
										Uart5TxBuffer[len ++] = eflowSelfStatus;
										Uart5TxBuffer[len]		= CalculateCheckSum(len+1,Uart5TxBuffer+KM_HEAD_LEN);
										len ++;
								
										UART5_Data_Transmit(len,Uart5TxBuffer);
										UART5_TX_BUSY_FLAG = TRUE;
										USART5_TXD_REGISTER &= ~UART5_SEND_SELF_TEST_STATUS;
					}
					break;
//					case UART5_SEND_SOURCE_PRESSURE_STATUS :
//					{
//								len = KM_HEAD_LEN;
//								Uart5TxBuffer[len ++] = SEND_SOURCE_PRESSURE_ID;
//								Uart5TxBuffer[len ++] = sourcePressureData.sourceOxyValue>>8;
//								Uart5TxBuffer[len ++] = sourcePressureData.sourceOxyValue;
//								Uart5TxBuffer[len ++] = sourcePressureData.sourceN2OValue>>8;
//								Uart5TxBuffer[len ++] = sourcePressureData.sourceN2OValue;
//								Uart5TxBuffer[len ++] = sourcePressureData.sourceAirValue>>8;
//								Uart5TxBuffer[len ++] = sourcePressureData.sourceAirValue;
//								Uart5TxBuffer[len]		= CalculateCheckSum(len+1,Uart5TxBuffer+KM_HEAD_LEN);
//								len ++;
//						
//								UART5_Data_Transmit(len,Uart5TxBuffer);
//								UART5_TX_BUSY_FLAG = TRUE;
//								USART5_TXD_REGISTER &= ~UART5_SEND_SOURCE_PRESSURE_STATUS;
//					}
//					break;
//					case UART5_SEND_YOKE_PRESSURE_STATUS :
//					{
//								len = KM_HEAD_LEN;
//								Uart5TxBuffer[len ++] = SEND_SOURCE_YOKE_ID;
//								Uart5TxBuffer[len ++] = sourcePressureData.yokeOxyValue>>8;
//								Uart5TxBuffer[len ++] = sourcePressureData.yokeOxyValue;
//								Uart5TxBuffer[len ++] = sourcePressureData.yokeN2OValue>>8;
//								Uart5TxBuffer[len ++] = sourcePressureData.yokeN2OValue;
//								Uart5TxBuffer[len ++] = sourcePressureData.yokeAirValue>>8;
//								Uart5TxBuffer[len ++] = sourcePressureData.yokeAirValue;
//								Uart5TxBuffer[len]		= CalculateCheckSum(len+1,Uart5TxBuffer+KM_HEAD_LEN);
//								len ++;
//						
//								UART5_Data_Transmit(len,Uart5TxBuffer);
//								UART5_TX_BUSY_FLAG = TRUE;
//								USART5_TXD_REGISTER &= ~UART5_SEND_YOKE_PRESSURE_STATUS;
//					}
//					break;
					case UART5_SEND_EFLOW_ERROR_STATUS:
					{
								len = KM_HEAD_LEN;
								Uart5TxBuffer[len ++] = SEND_EFLOW_ERROR_ID;
								Uart5TxBuffer[len ++] = eflowMonitorData.errorFlag>>8;
								Uart5TxBuffer[len ++] = eflowMonitorData.errorFlag;
								Uart5TxBuffer[len]		= CalculateCheckSum(len+1,Uart5TxBuffer+KM_HEAD_LEN);
								len ++;
						
								UART5_Data_Transmit(len,Uart5TxBuffer);
								UART5_TX_BUSY_FLAG = TRUE;
								USART5_TXD_REGISTER &= ~UART5_SEND_UPDATE_EFLOW_VALUE;
					}
					break;
					default:
					{
							retVal = 1;
					}
			}
		}
	else if(USARTx == USART1)
	{
		if(USART1_TXD_REGISTER==0)
			{
					return 1;
			}

			for(i=0;i<2;i++)
			{
					if(USART1_TXD_REGISTER&(1<<i))
					{
							break;
					}
			}
			
			switch(1<<i)
			{
					case UART1_IRAM_CALI_ID:
					{					
								IRAM_Send_Data_Transmit();
								UART1_TX_BUSY_FLAG = TRUE;
								USART1_TXD_REGISTER &= ~UART1_IRAM_CALI_ID;
					}
					break;
					default:
					{
						retVal = 1;
					}
			}
	}
	else if(USARTx == USART2)
	{
		if(USART2_TXD_REGISTER==0)
			{
					return 1;
			}

			for(i=0;i<32;i++)
			{
					if(USART2_TXD_REGISTER&(1<<i))
					{
							break;
					}
			}
			
			switch(1<<i)
			{
					case UART2_SEND_EFLOW_CALI_FLAG:
					{					
								Uart2TxBuffer[0] = '$';
								Uart2TxBuffer[1] = '@';
								Uart2TxBuffer[2] = UART2_SEND_EFLOW_SET_CALI;
								Uart2TxBuffer[3] = 0;
								Uart2TxBuffer[4] = 0;
								USART2_Data_Transmit();
								UART2_TX_BUSY_FLAG = TRUE;
								USART2_TXD_REGISTER &= ~UART2_SEND_EFLOW_CALI_FLAG;
					}
					break;
					case UART2_SEND_EFLOW_BAL_FLAG:
					{		
								Uart2TxBuffer[0] = '$';
								Uart2TxBuffer[1] = '@';
								Uart2TxBuffer[2] = UART2_SEND_EFLOW_SET_BALANCE;
								Uart2TxBuffer[3] = EFlow_Meter_Set.balance_gas;
								Uart2TxBuffer[4] = EFlow_Meter_Set.eflowCotrolMode;
								USART2_Data_Transmit();
								UART2_TX_BUSY_FLAG = TRUE;
								USART2_TXD_REGISTER &= ~UART2_SEND_EFLOW_BAL_FLAG;
					}
					break;
					case UART2_SEND_EFLOW_OXY_FLOW_FLAG:
					{			
								Uart2TxBuffer[0] = '$';
								Uart2TxBuffer[1] = '@';
								Uart2TxBuffer[2] = UART2_SEND_EFLOW_SET_OXY_FLOWVALUE;
								Uart2TxBuffer[3] = EFlow_Meter_Set.oxyFlowValue;
								Uart2TxBuffer[4] = EFlow_Meter_Set.oxyFlowValue>>8;
						
								USART2_Data_Transmit();
								UART2_TX_BUSY_FLAG = TRUE;
								USART2_TXD_REGISTER &= ~UART2_SEND_EFLOW_OXY_FLOW_FLAG;
					}
					break;
					case UART2_SEND_EFLOW_BAL_FLOW_FLAG:
					{			
								Uart2TxBuffer[0] = '$';
								Uart2TxBuffer[1] = '@';
								Uart2TxBuffer[2] = UART2_SEND_EFLOW_SET_BAL_FLOWVALUE;
								Uart2TxBuffer[3] = EFlow_Meter_Set.balFlowValue;
								Uart2TxBuffer[4] = EFlow_Meter_Set.balFlowValue>>8;
						
								USART2_Data_Transmit();
								UART2_TX_BUSY_FLAG = TRUE;
								USART2_TXD_REGISTER &= ~UART2_SEND_EFLOW_BAL_FLOW_FLAG;
					}
					break;
					case UART2_SEND_EFLOW_SELF_TEST_FLAG:
					{			
								Uart2TxBuffer[0] = '$';
								Uart2TxBuffer[1] = '@';
								Uart2TxBuffer[2] = UART2_SEND_EFLOW_SELF_TEST;
								Uart2TxBuffer[3] = 0;
								Uart2TxBuffer[4] = 0;
						
								USART2_Data_Transmit();
								UART2_TX_BUSY_FLAG = TRUE;
								USART2_TXD_REGISTER &= ~UART2_SEND_EFLOW_SELF_TEST_FLAG;
					}
					break;
					default:
					{
						retVal = 1;
					}
			}
			
	}

	return retVal;
}

void Uart2_RecvDataProcess(u8 *array)
{
		switch(array[0])
		{
//					case UART2_RECV_EFLOW_MAN_OXY:
//					{
//							eflowMonitorData.oxyValue = array[1]<<8 | array[2];
//					}
//					break;
//					case UART2_RECV_EFLOW_MAN_N2O:
//					{
//							eflowMonitorData.balValue = array[1]<<8 | array[2];
//					}
//					break;
//					case UART2_RECV_EFLOW_MAN_AIR:
//					{
//							eflowMonitorData.airValue = array[1]<<8 | array[2];
//					}
//					break;
//					case UART2_RECV_EFLOW_MAN_TOTAL:
//					{
//							eflowMonitorData.totalValue = array[1]<<8 | array[2];
//					}
//					break;
					case UART2_RECV_EFLOW_AUTO_OXY:
					{
							eflowMonitorData.oxyValue = array[1]<<8 | array[2];
					}
					break;
					case UART2_RECV_EFLOW_AUTO_BAL:
					{
							eflowMonitorData.balValue = array[1]<<8 | array[2];
					}
					break;
					case UART2_RECV_EFLOW_AUTO_ALT:
					{
							eflowMonitorData.altFlag = array[1]<<8 | array[2];
					}
					break;
					case UART2_RECV_EFLOW_AUTO_CALI:
					{
							Cali_Status.id			= SEND_CALISTATUS_ID;
							Cali_Status.status	=  array[1];
						
							System_Task_Register |= TASK_UPDATE_CALI_STATUS;
					}
					break;
					case UART2_RECV_EFLOW_AUTO_VER:
					{
							EFlow_Ver			= array[1]<<8|array[2];
							System_Task_Register |= TASK_UPDATE_VER;
					}
					break;
					case UART2_RECV_EFLOW_AUTO_ERROR:
					{
							eflowMonitorData.errorFlag = array[1]<<8 | array[2];
							System_Task_Register |= TASK_EFLOW_SEND_ERROR;
					}
					break;
					case UART2_RECV_EFLOW_SELF_STATUS:
					{
							eflowSelfStatus = array[1]<<8|array[2];
							System_Task_Register |= TASK_SEND_SELF_TEST;
					}
					break;
					case UART2_RECV_EFLOW_SELF_RESULT:
					{
							eflowMonitorData.check_result = array[1]<<8|array[2];
							System_Task_Register |= TASK_SEND_SELF_RESULT;
					}
					break;
//					case UART2_RECV_EFLOW_SOURCE_OXY_ID:
//					{
//							sourcePressureData.sourceOxyValue = array[1]<<8|array[2];
//					}
//					break;
//					case UART2_RECV_EFLOW_SOURCE_N2O_ID:
//					{
//							sourcePressureData.sourceN2OValue = array[1]<<8|array[2];
//					}
//					break;
//					case UART2_RECV_EFLOW_SOURCE_AIR_ID:
//					{
//							sourcePressureData.sourceAirValue = array[1]<<8|array[2];
//					}
//					break;
//					case UART2_RECV_EFLOW_YOKE_OXY_ID:
//					{
//							sourcePressureData.yokeOxyValue = array[1]<<8|array[2];
//					}
//					break;
//					case UART2_RECV_EFLOW_YOKE_N2O_ID:
//					{
//							sourcePressureData.yokeN2OValue = array[1]<<8|array[2];
//						
//					}
//					break;
//					case UART2_RECV_EFLOW_YOKE_AIR_ID:
//					{
//							sourcePressureData.yokeAirValue = array[1]<<8|array[2];
//							System_Task_Register |= TASK_SEND_SOURCE_PRESSURE;
//					}
//					break;
		}
}


void Uart3_ReceiveCommandProcess(u8 len, u8 *array)
{
	u8 sum=len;
	u8 i;
	for(i=0; i<len-KM_HEAD_LEN; i++)
	{
		sum += array[i];
	}
	
	if(!sum)
	{
		switch(array[0])
		{
				case UART3_RECV_VER_ID:
				{
						System_Task_Register |= TASK_UPDATE_VER|TASK_UART3_INIT_ID;
				}
				break;
				case UART3_RECV_KEY_VALUE_ID:
				{
						System_Task_Register |= TASK_UPDATE_KEY_VALUE;
				}
				break;
				case UART3_RECV_SELF_TEST_ID:
				{
						System_Task_Register |= TASK_SEND_SELF_TEST;
				}
				break;
		}
	}
}

void Uart5_ReceiveCommandProcess(u8 len, u8 *array)
{
	u8 sum=len;
	u8 i;
	for(i=0; i<len-KM_HEAD_LEN; i++)
	{
		sum += array[i];
	}
	
	if(!sum)
	{
		switch(array[0])
		{
				case UART5_RECV_ALARM_SET:
				{
						Alarm_Set.volume				=array[1];
						Alarm_Set.alarm_priority=array[2];
						Alarm_Set.silence				=array[3];
						Alarm_Set.alarm_led			=array[4];
					  System_Task_Register |= TASK_UPDATE_ALARM;
				}
				break;
				case UART5_RECV_KEY_LED_SET:
				{
					  currentKeyLed = array[1]<<8|array[2];
					
					  System_Task_Register |= TASK_UPDATE_KEY_LED;
				}
				break;
				case UART5_RECV_EFLOW_SET:
				{
						EFlow_Meter_Set.eflowCotrolMode = array[1];
						EFlow_Meter_Set.balance_gas			= array[2];
						EFlow_Meter_Set.oxyFlowValue 		= array[3]<<8|array[4];
						EFlow_Meter_Set.balFlowValue 		= array[5]<<8|array[6];
					  
						System_Task_Register |= TASK_EFLOW_SET_VALUE;
				}
				break;
				case UART5_RECV_IRMA_CALI:
				{
					IRAM_CaliData.Item = array[1];
					IRAM_CaliData.value = array[2];
					System_Task_Register |= TASK_UPDATE_IRAM_CALI;
				}
				break;
				case UART5_RECV_LCD_PWM_SET:
				{
					LcdPwmValue = array[1];
					System_Task_Register |= TASK_LCD_PWM_SET_ID;
				}
				break;
				case UART5_RECV_EFLOW_CALI:
				{
						System_Task_Register |= TASK_EFLOW_CALI_ID;
				}
				break;
				case UART5_RECV_SELF_TEST:
				{
						System_Task_Register |= TASK_SELF_TEST_HANDLER;
				}
				break;
		}
	}
}


void Uart4_ReceiveCommandProcess(u8 *array)
{
		int tmpValue;
		if(array[0] == 0xFF)
		{
				if(((array[1]|array[2]|array[3]|array[4]) & 0x80) == 0x00)
				{
					 tsData.pendown = 1;
					 tmpValue =  (array[3]&0xFF)<<7;
           tsData.xData = tmpValue + (array[4]&0xFF);
           tmpValue = (array[1]&0xFF)<<7;
           tsData.yData = tmpValue + (array[2]&0xff);
				}
				else
				{
						tsData.yData = 0;
						tsData.xData = 0;
				}
		}
		else
		{
				tsData.yData = 0;
				tsData.xData = 0;	
		}
}
	


void Check_SPO2_ModuleExist(void)
{
	static u16 lastSPO2_Counter;
	
	if(SPO2_TxdData.Status)
	{
			if(lastSPO2_Counter == SPO2_Counter)
			{
					SPO2_TxdData.Pulse 	= 0x7F;
					SPO2_TxdData.SpO2		= 0xFF;
					SPO2_TxdData.Status	= 0;
					SPO2_TxdData.SensorError = 0x10;
					USART5_TXD_REGISTER  |= UART5_SEND_UPDATE_SPO2_VALUE;
			}
	}
	
	lastSPO2_Counter = SPO2_Counter;
}


void Check_IRAM_ModuleExist(void)
{
		static u16 lastIRMA_Counter;
		if(IRMA_Txd_SensorFrameData.IRMA_ID != IRMA_NA)
		{
				if(lastIRMA_Counter ==  IRMA_Counter)
				{
						IRMA_Txd_SensorFrameData.AA1 = 0;
						IRMA_Txd_SensorFrameData.AA2 = 0;
						IRMA_Txd_SensorFrameData.N2O = 0;
						IRMA_Txd_SensorFrameData.CO2 = 0;
						IRMA_Txd_SensorFrameData.AA1Id = 0;
						IRMA_Txd_SensorFrameData.AA2Id = 0;
						IRMA_Txd_SensorFrameData.EtAA1 = 0xFF;
						IRMA_Txd_SensorFrameData.EtAA2 = 0xFF;
						IRMA_Txd_SensorFrameData.FiAA1 = 0xFF;
						IRMA_Txd_SensorFrameData.FiAA2 = 0xFF;
						IRMA_Txd_SensorFrameData.EtCO2 = 0xFF;
						IRMA_Txd_SensorFrameData.FiCO2 = 0xFF;
						IRMA_Txd_SensorFrameData.EtN2O = 0xFF;
						IRMA_Txd_SensorFrameData.FiN2O = 0xFF;
						IRMA_Txd_SensorFrameData.IRMA_ID = IRMA_NA;
						IRMA_Txd_SensorFrameData.Adapter_status = 0;
						IRMA_Txd_SensorFrameData.Data_Valid = 0;
						IRMA_Txd_SensorFrameData.Sensor_Err = 0;
						IRMA_Txd_SensorFrameData.Zero = 0;
						IRMA_Txd_SensorFrameData.Config = 0;
						USART5_TXD_REGISTER  |= UART5_SEND_UPDATE_AA_DATA0|UART5_SEND_UPDATE_AA_DATA1|UART5_SEND_UPDATE_AA_DATA3;
				}
				lastIRMA_Counter = IRMA_Counter;
	}
}


void kmSelfTest(void)
{
		u8  selfTimer = 0;
	
		ALARM_RED_H_LED = 1;
    ALARM_RED_L_LED = 1;
		ALARM_YELLOW_L_LED = 0;
    ALARM_YELLOW_H_LED = 0;
	
	 setBatLedState(1);
	 setIndicatorLed(SILENCE_INDICATOR_LED|STANDBY_INDICATOR_LED);
	 Alarm_voice_Select(SELF_ALARM,0x07,0);
				
		while(selfTimer < 3)
		{
				delay_ms(1000);
				selfTimer ++;
		}
		
    ALARM_YELLOW_L_LED = 1; 
		ALARM_RED_H_LED = 0;
		ALARM_RED_L_LED = 0;
    ALARM_YELLOW_H_LED = 1;
		
		selfTimer = 0;
		while(selfTimer < 3)
		{
				delay_ms(1000);
				selfTimer ++;
		}
		
		ALARM_RED_H_LED = 0;
    ALARM_RED_L_LED = 0;
		ALARM_YELLOW_L_LED = 0;
    ALARM_YELLOW_H_LED = 0;
		//Alarm_voice_Select(NONE_ALARM,0x07,0);
		Alarm_voice_Select(Alarm_Set.alarm_priority,Alarm_Set.volume,Alarm_Set.silence);
		
		setIndicatorLed(currentKeyLed);
}

void  paraValueInit(void)
{
		powerInfo.batteryState = BAT_STATE_CHARGE_ING;
}

