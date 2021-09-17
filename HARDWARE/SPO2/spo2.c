/**
* @file 	spo2.c
* @brief	血氧模块通信串口的初始化,串口数据的处理
* @details
* @author	亚
* @date		date
* @version	A001
* @par Copyright (c):
* 		北京易世恒技术有限责任公司公司
* @par History:
*	  version: author, date, desc

*/

#include "spo2.h"
#include "datahandle.h"


u16 SPO2_Counter = 0;
struct SPO2_TxdFrame SPO2_TxdData;

/*---------------------------- USART3 Mode Configuration -----------------------*/
/**
  * @brief  USART3初始化，PB10、PB11默认口不需要映射向USART2
  * @param  baud: Set the communication baud rate
  * @retval None
  */

void USART3_Init(u32 baud)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE );
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE );

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; //USART3 TX；
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; //复用推挽输出；
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure); //端口B；PB10

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11; //USART3 RX；
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; //浮空输入；
    GPIO_Init(GPIOB, &GPIO_InitStructure); //端口B；PB11

    USART_InitStructure.USART_BaudRate = baud; //波特率；
    USART_InitStructure.USART_WordLength = USART_WordLength_8b; //数据位8位；
    USART_InitStructure.USART_StopBits = USART_StopBits_1; //停止位1位；
    USART_InitStructure.USART_Parity = USART_Parity_Odd;//奇校验
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件流控；
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;//收发模式；
    USART_Init(USART3, &USART_InitStructure);//配置串口参数；

//    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //设置中断组2，0—3抢占优先级，0—3响应优先级；

    NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn; //中断号；
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3; //抢占优先级；
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3; //响应优先级；
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);
    USART_Cmd(USART3, ENABLE); //使能串口；
}
/**
  * @brief  Transmits single data through the USART3 peripheral.
  * @param  Data：The data to transmit.
  * @retval None
  */
void USART3_Send_Byte(u8 Data) //发送一个字节；
{
    USART_SendData(USART3, Data);
    while( USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET );
}
/**
  * @brief  Transmits string data through the USART3 peripheral.
  * @param  Data：The data to transmit.
  * @retval None
  */
void USART3_Send_String(u8 *Data) //发送字符串；
{
    while(*Data)
        USART3_Send_Byte(*Data++);
}

void USART3_Data_Transmit(u8 len, u8 *data)
{
    data[0] = 0xCB;
    data[1] = 0xBC;
    data[2] = len;

		USART_ITConfig(USART3, USART_IT_TXE,ENABLE); 
}


/**
  * @brief  处理接收的血氧数据
  * @param  *pointer
  * @details 对接收到的数据进行处理，加工，转换成与上位机
			 通信的格式
  * @retval None
  */

void SPO2_DataHandle(u8 *array)
{
	  struct SPO2_PARAMATERS SpO2_Parameters;
	
	  if(((array[1]&0x80) == 0) && ((array[2]&0x80)==0) && ((array[3]&0x80)==0) && ((array[4]&0x80)==0))
		{
				SpO2_Parameters.WaveData 		= array[1] & 0x7F;
				SpO2_Parameters.SensorError = array[2] & 0x10;
				SpO2_Parameters.PulseRate 	= (((array[2] & 0x40) << 1) | (array[3] & 0x7F));
				SpO2_Parameters.SpO2 				= array[4] & 0x7F;
			
				SPO2_TxdData.SpO2					=	SpO2_Parameters.SpO2;
				SPO2_TxdData.Pulse				=	SpO2_Parameters.PulseRate;
				SPO2_TxdData.SensorError 	= SpO2_Parameters.SensorError;
			
				if(SpO2_Parameters.SensorError)
				{
						SPO2_TxdData.SpO2		=	0x7F;
						SPO2_TxdData.Pulse	=	0xFF;
				}	
		}	
		else
		{
				SPO2_TxdData.SpO2						= 0x7F;
				SPO2_TxdData.Pulse					= 0xFF;
				SpO2_Parameters.SensorError = 0x10;
		}
		
		SPO2_TxdData.Status = 1;
}

/**
  * @brief  USART3 interrupt handing function.
  * @param  None
  * @retval None
  */
void USART3_IRQHandler(void) //中断处理函数；
{
    static u8 	Usart3RxBuffer[6] = {0};
    static u16	Wait_Syn = TRUE;
    static u8		Rx_num = 0;
    u8 					data = 0;
		
    if(USART_GetITStatus(USART3, USART_IT_RXNE) == SET) //判断是否发生中断；
    {
        if(Wait_Syn == TRUE)
        {
            data = USART_ReceiveData(USART3); //接收数据；
            if(data & 0x80) 
						{
								Wait_Syn = FALSE;
								Rx_num = 0;
								Usart3RxBuffer[Rx_num] = data;
						}
        }
        else
        {
						Rx_num++;
					  SPO2_Counter ++;
            Usart3RxBuffer[Rx_num] =  USART_ReceiveData(USART3);
            if(Rx_num >= 4) 
						{
							Wait_Syn = TRUE;
							Rx_num = 0;
							SPO2_DataHandle(Usart3RxBuffer); //对收到的数据进行处理转换
						}
        }
		}
}

