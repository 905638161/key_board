
#include "usart.h"
#include "datahandle.h"

u8  Uart5TxBuffer[14] = {0};
u8 	Uart5RxBuffer[30] = {0};


u8 	Uart4RxBuffer[30] = {0};

u8  Uart2TxBuffer[14] = {0};
u8 	Uart2RxBuffer[14] = {0};

/*---------------------------- USART2 Mode Configuration -----------------------*/
/**
  * @brief  USART2 初始化，USART2进行了重映射，因此需要初始化USART1略有不同
  * @param  baud: Set the communication baud rate 4800
  * @details 血氧模块通信串口,波特率为4800,奇校验
  * @retval None
  */
void USART2_Init(u32 baud)
{
    //GPIO端口设置
    GPIO_InitTypeDef  GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef  NVIC_InitStructure;


    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);	//使能USART2
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);	//使能GPIOD,开启复用时钟
    USART_DeInit(USART2);  //复位串口2

    /*进行USART2的重映射配置*/
   // GPIO_PinRemapConfig  (GPIO_Remap_USART2, ENABLE);

    /*USART2_TX   PA.2*/
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2; 
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /*USART2_RX   PA.3*/
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
    GPIO_Init(GPIOA, &GPIO_InitStructure);  //初始化PD6

//    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //设置中断组2，0-3抢占优先级，0-3响应优先级；


    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3 ; //抢占优先级3
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//子优先级3
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
    NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器

    //USART初始化设置

    USART_InitStructure.USART_BaudRate = baud;//波特率的设置
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
    USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
    USART_InitStructure.USART_Parity = USART_Parity_No ; //无校验位?
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式

    USART_Init(USART2, &USART_InitStructure); //初始化串口
    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//开启中断
    USART_Cmd(USART2, ENABLE);                    //使能串口
}

/**
  * @brief  Transmits single data through the USART2 peripheral.
  * @param  Data：The data to transmit.
  * @retval None
  */
void USART2_Send_Byte(u8 Data) //发送一个字节；
{
    USART_SendData(USART2, Data);
    while( USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET );
}
/**
  * @brief  Transmits  string data through the USART2 peripheral.
  * @param  Data: the data to transmit.
  * @retval None
  */
void USART2_Send_String(u8 *Data) //发送字符串；
{
    while(*Data)
        USART2_Send_Byte(*Data++);
}

void USART2_Data_Transmit(void)
{
		USART_ITConfig(USART2, USART_IT_TXE,ENABLE); 
}

/**
  * @brief  USART2 interrupt handing function.
  * @param  None
  * @details 对收到的数据进行初步的处理，初级加工
			 血氧模块发送的数据有5个字节
  * @retval None
  */
void USART2_IRQHandler(void) //中断处理函数;
{
    static u8 	LastData = 0;
    static u16	Wait_Syn = TRUE;
    static u8		Rx_num = 0;
    static u8   Tx_num = 0;
    u8 data = 0;
		
    if(USART_GetITStatus(USART2, USART_IT_RXNE) == SET) //判断是否发生中断;
    {
        USART_ClearFlag(USART2, USART_IT_RXNE); //清除标志位；
        if (Wait_Syn == TRUE)  				  //判断起始标志0xaa,0x55,是否完成
        {
            data = USART_ReceiveData(USART2); //接收数据；
            if((LastData == '$') && (data == '@'))
            {
                Wait_Syn = FALSE;
            }
            LastData = data;
						Rx_num = 0;
        }
        else
        {
            Uart2RxBuffer[Rx_num++] = USART_ReceiveData(USART2);
            if(Rx_num >= 3)
            {
								Uart2_RecvDataProcess(Uart2RxBuffer);
								Rx_num = 0;
								LastData = 0;
								Wait_Syn = TRUE;
            }
        }
    }
		else if(USART_GetITStatus(USART2,USART_IT_TXE) != RESET)	//send data interupt
		{
				if(Tx_num < 5)
				{	
						USART_SendData(USART2,Uart2TxBuffer[Tx_num]);
						Tx_num++;	
				}
				else
				{	
						Tx_num=0;
						USART_ITConfig(USART2, USART_IT_TXE,DISABLE);
						if(Inquire_TXD_Handler(USART2))
						{
								UART2_TX_BUSY_FLAG = FALSE;
						}
				}
		}
}

/*---------------------------- UART4 Mode Configuration -----------------------*/
/**
  * @brief  UART4初始化 PC10,PC11
  * @param  baud: Set the communication baud rate
  * @retval None
  */
void UART4_Init(u32 baud)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE );
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4, ENABLE );

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; //UART4 TX；
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; //复用推挽输出；
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure); //端口C；

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11; //UART4 RX；
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; //浮空输入；
    GPIO_Init(GPIOC, &GPIO_InitStructure); //端口C；

    USART_InitStructure.USART_BaudRate = baud; //波特率；
    USART_InitStructure.USART_WordLength = USART_WordLength_8b; //数据位8位；
    USART_InitStructure.USART_StopBits = USART_StopBits_1; //停止位1位；
    USART_InitStructure.USART_Parity = USART_Parity_No ; //无校验位；
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件流控；
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;//收发模式；
    USART_Init(UART4, &USART_InitStructure);//配置串口参数；

//    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //设置中断组2，0-3抢占优先级，0-3响应优先级；

    NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn; //中断号；
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3; //抢占优先级；
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3; //响应优先级；
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    USART_ITConfig(UART4, USART_IT_RXNE, ENABLE);
    USART_Cmd(UART4, ENABLE); //使能串口；
}
/**
  * @brief  Transmits single data through the UART4 peripheral.
  * @param  Data：The data to transmit.
  * @retval None
  */
void UART4_Send_Byte(u8 Data) //发送一个字节；
{
    USART_SendData(UART4, Data);
    while( USART_GetFlagStatus(UART4, USART_FLAG_TC) == RESET );
}
/**
  * @brief  Transmits string data through the UART4 peripheral.
  * @param  Data：The data to transmit.
  * @retval None
  */
void UART4_Send_String(u8 *Data) //发送字符串；
{
    while(*Data)
        UART4_Send_Byte(*Data++);
}
/**
  * @brief  UART4 interrupt handing function.
  * @param  None
  * @retval None
  */
void UART4_IRQHandler(void) //中断处理函数；
{		
    static u16	Wait_Syn = TRUE;
    static u8		Rx_num = 0;
    u8 data = 0;
		
    if(USART_GetITStatus(UART4, USART_IT_RXNE) == SET) //判断是否发生中断；
    {
        if (Wait_Syn == TRUE)  				  //判断起始标志0xaa,0x55,是否完成
        {
            data = USART_ReceiveData(UART4); //接收数据；
            if((data == 0xFF) || (data == 0xBF))
            {
                Wait_Syn = FALSE;
								Rx_num = 0;
								Uart4RxBuffer[Rx_num ++] = data;
            }
        }
        else
        {
            Uart4RxBuffer[Rx_num++] = USART_ReceiveData(UART4);
					
            if(Rx_num >= 5)
            {
								Uart4_ReceiveCommandProcess(Uart4RxBuffer);
								Rx_num = 0;
								Wait_Syn = TRUE;
            }
        }
    }
}

void UART4_Data_Transmit(void)
{
		USART_ITConfig(UART4, USART_IT_TXE,ENABLE); 
}

/*---------------------------- UART5 Mode Configuration -----------------------*/
/**
  * @brief  UART5初始化 PC10,PC11
  * @param  baud: Set the communication baud rate
  * @retval None
  */
void UART5_Init(u32 baud)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD, ENABLE );
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART5, ENABLE );

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12; //UART5 TX；
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; //复用推挽输出；
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure); //端口C；

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2; //UART5 RX；
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; //浮空输入；
    GPIO_Init(GPIOD, &GPIO_InitStructure); //端口D；

    USART_InitStructure.USART_BaudRate = baud; //波特率；
    USART_InitStructure.USART_WordLength = USART_WordLength_8b; //数据位8位；
    USART_InitStructure.USART_StopBits = USART_StopBits_1; //停止位1位；
    USART_InitStructure.USART_Parity = USART_Parity_No ; //无校验位；
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件流控；
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;//收发模式；
    USART_Init(UART5, &USART_InitStructure);//配置串口参数；


//    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //设置中断组2，0-3抢占优先级，0-3响应优先级；

    NVIC_InitStructure.NVIC_IRQChannel = UART5_IRQn; //中断号；
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3; //抢占优先级；
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3; //响应优先级；
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);


    USART_ITConfig(UART5, USART_IT_RXNE, ENABLE);
    USART_Cmd(UART5, ENABLE); //使能串口；
}
/**
  * @brief  Transmits single data through the UART5 peripheral.
  * @param  Data：The data to transmit.
  * @retval None
  */
void UART5_Send_Byte(u8 Data) //发送一个字节；
{
    USART_SendData(UART5, Data);
    while( USART_GetFlagStatus(UART5, USART_FLAG_TC) == RESET );
}
/**
* @brief 发送一串的数据
* @param
**len: 数据的长度
**data:发送数据的首地址
* @return None
* @details 套用了USART5的发送一个字节的函数
*/
void UART5_Send_Data(u8 len, u8 *data)
{
    u8 i = 0;
    for(i = 0; i < len; i++)
    {
        UART5_Send_Byte(*data++);
    }
}

/**
  * @brief  Transmits string data through the UART5 peripheral.
  * @param  Data：The data to transmit.
  * @retval None
  */
void UART5_Send_String(u8 *Data) //发送字符串，字符串的末尾为零
{
    while(*Data)
        UART5_Send_Byte(*Data++);
}
/**
  * @brief  UART5 interrupt handing function.
  * @param  None
  * @retval None
  */
void UART5_IRQHandler(void) //中断处理函数；
{
    static u8 	LastData = 0;
    static u16	Wait_Syn = TRUE;
    static u8		Rx_num = 0;
    static u8   Tx_num = 0;
    u8 data = 0;
		
    if(USART_GetITStatus(UART5, USART_IT_RXNE) == SET) //判断是否发生中断；
    {
        if (Wait_Syn == TRUE)  				  //判断起始标志0xaa,0x55,是否完成
        {
            data = USART_ReceiveData(UART5); //接收数据；
            if((LastData == 0xFD) && (data == 0xDF))
            {
                Wait_Syn = FALSE;
            }
            LastData = data;
        }
        else
        {
            Uart5RxBuffer[Rx_num++] = USART_ReceiveData(UART5);
            if(Rx_num >= Uart5RxBuffer[0] - 2)
            {
								Uart5_ReceiveCommandProcess(Uart5RxBuffer[0],Uart5RxBuffer+1);
								Rx_num = 0;
								LastData = 0;
								Wait_Syn = TRUE;
            }
        }
    }
		else if(USART_GetITStatus(UART5,USART_IT_TXE) != RESET)	//send data interupt
		{
				if(Tx_num < Uart5TxBuffer[2])
				{	
						USART_SendData(UART5,Uart5TxBuffer[Tx_num]);
						Tx_num++;	
				}
				else
				{	
						Tx_num=0;
						USART_ITConfig(UART5, USART_IT_TXE,DISABLE);
						if(Inquire_TXD_Handler(UART5))
						{
								UART5_TX_BUSY_FLAG = FALSE;
						}
				}
		}
}

/**
* @brief 串口通信程序
* @param
*len: 数据长度
*ID:  数据的类型
*data[len]:将要发送的数据
*CHK:数据的校验
* @return the return value
* @details a full description of what the function does
*/
void UART5_Data_Transmit(u8 len, u8 *data)
{

    data[0] = 0xFD;
    data[1] = 0xDF;
    data[2] = len;

		USART_ITConfig(UART5, USART_IT_TXE,ENABLE); 
   //UART5_Send_Data(len, data); /**通过串口5发送数据**/

}
/**
 *@brief 数据的和校验:CHK
 *@param len:   数据的长度
 *@param array：接收到的数据组
 *@return sum：数据和为零
 *@details 发送的数据格式：
  ===============================================================================================
    标志1   |     标志2     |   数据长度  |  解析方法  |     发送的数据   |   数据的和校验
  ===============================================================================================
    Flag1   |     Flag2     |     Len     |     ID     |      Data(0-9)   |        CHK
						|               |             |            |                  |
  -----------------------------------------------------------------------------------------------
    byte    |      byte     |      byte   |    byte    |     (1-10)bytes  |        byte
            |               |             |            |		        		  |
  -----------------------------------------------------------------------------------------------
 *@details 校验计算公式：
	CHK = ~(Len+Id + Data[0-9])+1.
**/
u8 CalculateCheckSum(u8 len, u8 *array)
{
    u8 sum = len;
    u8 i;
    for(i = 0; i < len - KM_HEAD_LEN - 1; i++) //数据的长度移除flag1,flag2,末位CHK
    {
        sum += array[i];
    }
    sum = ~sum + 1;

    return sum;
}
