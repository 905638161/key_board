/**
* @file 	iram.c
* @brief	IRAM麻醉气体模块数据处理串口驱动配置，数据处理。
* @details	配置了串口驱动，对收到的数据进行了处理加工，方便后续将数据发送行给主控板
			使用了串口1
* @author	guo
* @date		date
* @version	A001
* @par Copyright (c): 
* 		北京易世恒公司
* @par History:         
*	  version: 技术部, 20180710, Ver0.0
*/

/**INCLUDES-------------------------------------------------------------------------**/
#include "iram.h"
#include "led.h"

IRMA_SENSOR_RXD_PARAMETERS 	IramPara;                 /*定义IRAM模块接收的数据结构变量*/
IRMA_SENSOR_TXD_FRAME_DATA	IRMA_Txd_SensorFrameData; /*定义IRAM模块发送的数据结构变量*/

u8 Uart1TxBuffer[6];
u8 UART1_TX_BUSY_FLAG = FALSE;
extern IRAM_CALI		IRAM_CaliData;
u16 IRMA_Counter = 0;
/**
  * @brief  USART1 初始化，USART1进行了重映射，因此需要初始化USART1略有不同
  * @param  baud: Set the communication baud rate
  * @retval None
  */
void USART1_Init(u32 baud)
{
    //GPIO端口设置
    GPIO_InitTypeDef  GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef  NVIC_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA, ENABLE);	//使能USART1，GPIOA时钟
    USART_DeInit(USART1);  //复位串口1
    //USART1_TX   PA.9n
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //PA.9
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出,端口复用
    GPIO_Init(GPIOA, &GPIO_InitStructure); //初始化PA9

    //USART1_RX	  PA.10
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
    GPIO_Init(GPIOA, &GPIO_InitStructure);  //初始化PA10

//    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //设置中断组2，0-3抢占优先级，0-3响应优先级；

    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3 ; //抢占优先级3
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//子优先级3
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
    NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器

    //USART 初始化设置

    USART_InitStructure.USART_BaudRate = baud;//一般设置为9600;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
    USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
    USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式

    USART_Init(USART1, &USART_InitStructure); //初始化串口
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//开启中断
    USART_Cmd(USART1, ENABLE);                    //使能串口
}

/**
  * @brief  Transmits single data through the USART1 peripheral.
  * @param  Data: the data to transmit.
  * @retval None
  */
void USART1_Send_Byte(u8 Data) //发送一个字节；
{

    USART_SendData(USART1, Data);
    while( USART_GetFlagStatus(USART1, USART_FLAG_TC) != SET );
    LED1 = !LED1;
}

/**
  * @brief  Transmits string data through the USART1 peripheral.
  * @param  Data: the data to transmit.
  * @retval None
  */
void USART1_Send_String(u8 *Data) //发送字符串；
{
    while(*Data)
        USART1_Send_Byte(*Data++);
}


/**
 * @brief 麻醉气体模块的工作
 * @param pointer类型，接收函数的首地址
 * @return IramPara ：麻醉气体模块类型结构的返回值
 * @details 定义了麻醉气体模块类型的数据结构，方便调用

**/
void  IRAM_DataDecode(u8* pointer)
{
    u8 i;

    IramPara.Id = *pointer;
    IramPara.Sts = *(pointer + 1);
    IramPara.CO2 = (*(pointer + 2) << 8) | (*(pointer + 3));
    IramPara.N2O = (*(pointer + 4) << 8) | (*(pointer + 5));
    IramPara.AA1 = (*(pointer + 6) << 8) | (*(pointer + 7));
    IramPara.AA2 = (*(pointer + 8) << 8) | (*(pointer + 9));
    IramPara.O2 = (*(pointer + 10) << 8) | (*(pointer + 11));
    for(i = 0; i < 6; i++)
    {
        IramPara.SlowData[i] = *(pointer + 12 + i);
    }

    //IramPara.Chk = *(pointer+18);
}

/**
  * @brief  Transmits single data through the USART1 peripheral.
  * @param  Data: the data to transmit.
  * @retval None
  * @detail CO2/AA麻醉气体模块通信接口
  */
void USART1_IRQHandler(void) //中断处理函数；
{
    static u8 	Usart1RxBuffer[30] = {0};
    static u8 	LastData = 0;
    static u16	Wait_Syn = TRUE;
    static u8	Rx_num = 0;
    static u8   Tx_num = 0;
	
    u8 data = 0;
    u8 sum = 0;
    u8 i = 0;
    if(USART_GetITStatus(USART1, USART_IT_RXNE) == SET) //判断是否发生中断；
    {
				USART_ClearFlag(USART1, USART_IT_RXNE); //清除标志位；
        if (Wait_Syn == TRUE)  				  //判断起始标志0xaa,0x55,是否完成
        {
            data = USART_ReceiveData(USART1); //接收数据；
            if((LastData == 0xAA) && (data == 0x55))
            {
                Wait_Syn = FALSE;
            }
            LastData = data;
        }
        else
        {
            Usart1RxBuffer[Rx_num++] = USART_ReceiveData(USART1);
						IRMA_Counter ++;
            if(Rx_num >= 19)
            {
                for (i = 0; i <= 18; i++)
                {
                    sum += Usart1RxBuffer[i];
                }
                if(!sum)   //和校验数值为零
                {
                    IRAM_DataDecode(Usart1RxBuffer);
					
										IRMA_Txd_SensorFrameData.CO2=IramPara.CO2;
										IRMA_Txd_SensorFrameData.N2O=IramPara.N2O;
										IRMA_Txd_SensorFrameData.AA1=IramPara.AA1;
										IRMA_Txd_SensorFrameData.AA2=IramPara.AA2;
									
                    switch(IramPara.Id)
                    {
                    case 0:  //吸气InspVals
										{
                        IRMA_Txd_SensorFrameData.FiCO2 = IramPara.SlowData[0];
                        IRMA_Txd_SensorFrameData.FiN2O = IramPara.SlowData[1];
                        IRMA_Txd_SensorFrameData.FiAA1 = IramPara.SlowData[2];
                        IRMA_Txd_SensorFrameData.FiAA2 = IramPara.SlowData[3];
//                        IRMA_Txd_SensorFrameData.O2  = IramPara.SlowData[4];
										}
                    break;
                    case 1:  //呼气ExpVals
										{
                        IRMA_Txd_SensorFrameData.EtCO2 = IramPara.SlowData[0];
                        IRMA_Txd_SensorFrameData.EtN2O = IramPara.SlowData[1];
                        IRMA_Txd_SensorFrameData.EtAA1 = IramPara.SlowData[2];
                        IRMA_Txd_SensorFrameData.EtAA2 = IramPara.SlowData[3];
//                        IRMA_Txd_SensorFrameData.O2  = IramPara.SlowData[4];
										}
                    break;
                    case 3: //IrmaSlowParas_AtmPress
										{
                        IRMA_Txd_SensorFrameData.AA1Id = IramPara.SlowData[2];
                        IRMA_Txd_SensorFrameData.AA2Id = IramPara.SlowData[3];
                        IRMA_Txd_SensorFrameData.AtmPressure= IramPara.SlowData[4] * 256 + IramPara.SlowData[5];
										}
                    break;
                    case 4:
										{
                        IRMA_Txd_SensorFrameData.Sensor_Err = IramPara.SlowData[2];
                        IRMA_Txd_SensorFrameData.Adapter_status = IramPara.SlowData[3];
                        IRMA_Txd_SensorFrameData.Data_Valid = IramPara.SlowData[4];
										}
                    break;
                    case 5:
                    {
                        if(IramPara.SlowData[0] & (0x1 << 2))
                        {
                            IRMA_Txd_SensorFrameData.IRMA_ID = IRMA_AA;
                        }
                        else if (IramPara.SlowData[0] & (0x1 << 1))
                        {
                            IRMA_Txd_SensorFrameData.IRMA_ID = IRMA_CO2;
                        }
												else
												{
														IRMA_Txd_SensorFrameData.IRMA_ID = IRMA_NA;
												}
						
                        
                        IRMA_Txd_SensorFrameData.Config = IramPara.SlowData[0];
                       
                    }
                    break;
                    case 6:
										{
                        if((IramPara.SlowData[2] & 0x03) == 0)
												{
                            IRMA_Txd_SensorFrameData.Zero = 1;
												}
                        else
												{
                            IRMA_Txd_SensorFrameData.Zero = 0;
												}
										}
                    break;
                    }
                }
                Wait_Syn = TRUE;
                Rx_num = 0;
                LastData = 0;
            }
        }
    }
		else if(USART_GetITStatus(USART1,USART_IT_TXE) != RESET)	//send data interupt
		{
				if(Tx_num < 5)
				{	
						USART_SendData(USART1,Uart1TxBuffer[Tx_num]);
						Tx_num++;	
				}
				else
				{	
						Tx_num=0;
						USART_ITConfig(USART1, USART_IT_TXE,DISABLE);
						if(Inquire_TXD_Handler(USART1))
						{
								UART1_TX_BUSY_FLAG = FALSE;
						}
				}
		}
}


void IRAM_Send_Data_Transmit(void)
{
	Uart1TxBuffer[0] = 0xAA;
	Uart1TxBuffer[1] = 0x55;
	Uart1TxBuffer[2] = IRAM_CaliData.Item;
	Uart1TxBuffer[3] = IRAM_CaliData.value;
	Uart1TxBuffer[4] = ~(IRAM_CaliData.Item+IRAM_CaliData.value)+1;
	
	USART_ITConfig(USART1, USART_IT_TXE,ENABLE); 
}

