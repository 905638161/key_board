
#include "usart.h"
#include "datahandle.h"

u8  Uart5TxBuffer[14] = {0};
u8 	Uart5RxBuffer[30] = {0};


u8 	Uart4RxBuffer[30] = {0};

u8  Uart2TxBuffer[14] = {0};
u8 	Uart2RxBuffer[14] = {0};

/*---------------------------- USART2 Mode Configuration -----------------------*/
/**
  * @brief  USART2 ³õÊ¼»¯£¬USART2½øÐÐÁËÖØÓ³Éä£¬Òò´ËÐèÒª³õÊ¼»¯USART1ÂÔÓÐ²»Í¬
  * @param  baud: Set the communication baud rate 4800
  * @details ÑªÑõÄ£¿éÍ¨ÐÅ´®¿Ú,²¨ÌØÂÊÎª4800,ÆæÐ£Ñé
  * @retval None
  */
void USART2_Init(u32 baud)
{
    //GPIO¶Ë¿ÚÉèÖÃ
    GPIO_InitTypeDef  GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef  NVIC_InitStructure;


    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);	//Ê¹ÄÜUSART2
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);	//Ê¹ÄÜGPIOD,¿ªÆô¸´ÓÃÊ±ÖÓ
    USART_DeInit(USART2);  //¸´Î»´®¿Ú2

    /*½øÐÐUSART2µÄÖØÓ³ÉäÅäÖÃ*/
   // GPIO_PinRemapConfig  (GPIO_Remap_USART2, ENABLE);

    /*USART2_TX   PA.2*/
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2; 
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//¸´ÓÃÍÆÍìÊä³ö
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /*USART2_RX   PA.3*/
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//¸¡¿ÕÊäÈë
    GPIO_Init(GPIOA, &GPIO_InitStructure);  //³õÊ¼»¯PD6

//    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //ÉèÖÃÖÐ¶Ï×é2£¬0-3ÇÀÕ¼ÓÅÏÈ¼¶£¬0-3ÏìÓ¦ÓÅÏÈ¼¶£»


    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3 ; //ÇÀÕ¼ÓÅÏÈ¼¶3
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//×ÓÓÅÏÈ¼¶3
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQÍ¨µÀÊ¹ÄÜ
    NVIC_Init(&NVIC_InitStructure);	//¸ù¾ÝÖ¸¶¨µÄ²ÎÊý³õÊ¼»¯VIC¼Ä´æÆ÷

    //USART³õÊ¼»¯ÉèÖÃ

    USART_InitStructure.USART_BaudRate = baud;//²¨ÌØÂÊµÄÉèÖÃ
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;//×Ö³¤Îª8Î»Êý¾Ý¸ñÊ½
    USART_InitStructure.USART_StopBits = USART_StopBits_1;//Ò»¸öÍ£Ö¹Î»
    USART_InitStructure.USART_Parity = USART_Parity_No ; //ÎÞÐ£ÑéÎ»£
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//ÎÞÓ²¼þÊý¾ÝÁ÷¿ØÖÆ
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//ÊÕ·¢Ä£Ê½

    USART_Init(USART2, &USART_InitStructure); //³õÊ¼»¯´®¿Ú
    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//¿ªÆôÖÐ¶Ï
    USART_Cmd(USART2, ENABLE);                    //Ê¹ÄÜ´®¿Ú
}

/**
  * @brief  Transmits single data through the USART2 peripheral.
  * @param  Data£ºThe data to transmit.
  * @retval None
  */
void USART2_Send_Byte(u8 Data) //·¢ËÍÒ»¸ö×Ö½Ú£»
{
    USART_SendData(USART2, Data);
    while( USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET );
}
/**
  * @brief  Transmits  string data through the USART2 peripheral.
  * @param  Data: the data to transmit.
  * @retval None
  */
void USART2_Send_String(u8 *Data) //·¢ËÍ×Ö·û´®£»
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
  * @details ¶ÔÊÕµ½µÄÊý¾Ý½øÐÐ³õ²½µÄ´¦Àí£¬³õ¼¶¼Ó¹¤
			 ÑªÑõÄ£¿é·¢ËÍµÄÊý¾ÝÓÐ5¸ö×Ö½Ú
  * @retval None
  */
void USART2_IRQHandler(void) //ÖÐ¶Ï´¦Àíº¯Êý;
{
    static u8 	LastData = 0;
    static u16	Wait_Syn = TRUE;
    static u8		Rx_num = 0;
    static u8   Tx_num = 0;
    u8 data = 0;
		
    if(USART_GetITStatus(USART2, USART_IT_RXNE) == SET) //ÅÐ¶ÏÊÇ·ñ·¢ÉúÖÐ¶Ï;
    {
        USART_ClearFlag(USART2, USART_IT_RXNE); //Çå³ý±êÖ¾Î»£»
        if (Wait_Syn == TRUE)  				  //ÅÐ¶ÏÆðÊ¼±êÖ¾0xaa,0x55,ÊÇ·ñÍê³É
        {
            data = USART_ReceiveData(USART2); //½ÓÊÕÊý¾Ý£»
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
  * @brief  UART4³õÊ¼»¯ PC10,PC11
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

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; //UART4 TX£»
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; //¸´ÓÃÍÆÍìÊä³ö£»
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure); //¶Ë¿ÚC£»

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11; //UART4 RX£»
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; //¸¡¿ÕÊäÈë£»
    GPIO_Init(GPIOC, &GPIO_InitStructure); //¶Ë¿ÚC£»

    USART_InitStructure.USART_BaudRate = baud; //²¨ÌØÂÊ£»
    USART_InitStructure.USART_WordLength = USART_WordLength_8b; //Êý¾ÝÎ»8Î»£»
    USART_InitStructure.USART_StopBits = USART_StopBits_1; //Í£Ö¹Î»1Î»£»
    USART_InitStructure.USART_Parity = USART_Parity_No ; //ÎÞÐ£ÑéÎ»£»
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//ÎÞÓ²¼þÁ÷¿Ø£»
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;//ÊÕ·¢Ä£Ê½£»
    USART_Init(UART4, &USART_InitStructure);//ÅäÖÃ´®¿Ú²ÎÊý£»

//    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //ÉèÖÃÖÐ¶Ï×é2£¬0-3ÇÀÕ¼ÓÅÏÈ¼¶£¬0-3ÏìÓ¦ÓÅÏÈ¼¶£»

    NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn; //ÖÐ¶ÏºÅ£»
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3; //ÇÀÕ¼ÓÅÏÈ¼¶£»
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3; //ÏìÓ¦ÓÅÏÈ¼¶£»
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    USART_ITConfig(UART4, USART_IT_RXNE, ENABLE);
    USART_Cmd(UART4, ENABLE); //Ê¹ÄÜ´®¿Ú£»
}
/**
  * @brief  Transmits single data through the UART4 peripheral.
  * @param  Data£ºThe data to transmit.
  * @retval None
  */
void UART4_Send_Byte(u8 Data) //·¢ËÍÒ»¸ö×Ö½Ú£»
{
    USART_SendData(UART4, Data);
    while( USART_GetFlagStatus(UART4, USART_FLAG_TC) == RESET );
}
/**
  * @brief  Transmits string data through the UART4 peripheral.
  * @param  Data£ºThe data to transmit.
  * @retval None
  */
void UART4_Send_String(u8 *Data) //·¢ËÍ×Ö·û´®£»
{
    while(*Data)
        UART4_Send_Byte(*Data++);
}
/**
  * @brief  UART4 interrupt handing function.
  * @param  None
  * @retval None
  */
void UART4_IRQHandler(void) //ÖÐ¶Ï´¦Àíº¯Êý£»
{		
    static u16	Wait_Syn = TRUE;
    static u8		Rx_num = 0;
    u8 data = 0;
		
    if(USART_GetITStatus(UART4, USART_IT_RXNE) == SET) //ÅÐ¶ÏÊÇ·ñ·¢ÉúÖÐ¶Ï£»
    {
        if (Wait_Syn == TRUE)  				  //ÅÐ¶ÏÆðÊ¼±êÖ¾0xaa,0x55,ÊÇ·ñÍê³É
        {
            data = USART_ReceiveData(UART4); //½ÓÊÕÊý¾Ý£»
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
  * @brief  UART5³õÊ¼»¯ PC10,PC11
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

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12; //UART5 TX£»
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; //¸´ÓÃÍÆÍìÊä³ö£»
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure); //¶Ë¿ÚC£»

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2; //UART5 RX£»
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; //¸¡¿ÕÊäÈë£»
    GPIO_Init(GPIOD, &GPIO_InitStructure); //¶Ë¿ÚD£»

    USART_InitStructure.USART_BaudRate = baud; //²¨ÌØÂÊ£»
    USART_InitStructure.USART_WordLength = USART_WordLength_8b; //Êý¾ÝÎ»8Î»£»
    USART_InitStructure.USART_StopBits = USART_StopBits_1; //Í£Ö¹Î»1Î»£»
    USART_InitStructure.USART_Parity = USART_Parity_No ; //ÎÞÐ£ÑéÎ»£»
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//ÎÞÓ²¼þÁ÷¿Ø£»
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;//ÊÕ·¢Ä£Ê½£»
    USART_Init(UART5, &USART_InitStructure);//ÅäÖÃ´®¿Ú²ÎÊý£»


//    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //ÉèÖÃÖÐ¶Ï×é2£¬0-3ÇÀÕ¼ÓÅÏÈ¼¶£¬0-3ÏìÓ¦ÓÅÏÈ¼¶£»

    NVIC_InitStructure.NVIC_IRQChannel = UART5_IRQn; //ÖÐ¶ÏºÅ£»
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3; //ÇÀÕ¼ÓÅÏÈ¼¶£»
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3; //ÏìÓ¦ÓÅÏÈ¼¶£»
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);


    USART_ITConfig(UART5, USART_IT_RXNE, ENABLE);
    USART_Cmd(UART5, ENABLE); //Ê¹ÄÜ´®¿Ú£»
}
/**
  * @brief  Transmits single data through the UART5 peripheral.
  * @param  Data£ºThe data to transmit.
  * @retval None
  */
void UART5_Send_Byte(u8 Data) //·¢ËÍÒ»¸ö×Ö½Ú£»
{
    USART_SendData(UART5, Data);
    while( USART_GetFlagStatus(UART5, USART_FLAG_TC) == RESET );
}
/**
* @brief ·¢ËÍÒ»´®µÄÊý¾Ý
* @param
**len: Êý¾ÝµÄ³¤¶È
**data:·¢ËÍÊý¾ÝµÄÊ×µØÖ·
* @return None
* @details Ì×ÓÃÁËUSART5µÄ·¢ËÍÒ»¸ö×Ö½ÚµÄº¯Êý
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
  * @param  Data£ºThe data to transmit.
  * @retval None
  */
void UART5_Send_String(u8 *Data) //·¢ËÍ×Ö·û´®£¬×Ö·û´®µÄÄ©Î²ÎªÁã
{
    while(*Data)
        UART5_Send_Byte(*Data++);
}
/**
  * @brief  UART5 interrupt handing function.
  * @param  None
  * @retval None
  */
void UART5_IRQHandler(void) //ÖÐ¶Ï´¦Àíº¯Êý£»
{
    static u8 	LastData = 0;
    static u16	Wait_Syn = TRUE;
    static u8		Rx_num = 0;
    static u8   Tx_num = 0;
    u8 data = 0;
		
    if(USART_GetITStatus(UART5, USART_IT_RXNE) == SET) //ÅÐ¶ÏÊÇ·ñ·¢ÉúÖÐ¶Ï£»
    {
        if (Wait_Syn == TRUE)  				  //ÅÐ¶ÏÆðÊ¼±êÖ¾0xaa,0x55,ÊÇ·ñÍê³É
        {
            data = USART_ReceiveData(UART5); //½ÓÊÕÊý¾Ý£»
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
* @brief ´®¿ÚÍ¨ÐÅ³ÌÐò
* @param
*len: Êý¾Ý³¤¶È
*ID:  Êý¾ÝµÄÀàÐÍ
*data[len]:½«Òª·¢ËÍµÄÊý¾Ý
*CHK:Êý¾ÝµÄÐ£Ñé
* @return the return value
* @details a full description of what the function does
*/
void UART5_Data_Transmit(u8 len, u8 *data)
{

    data[0] = 0xFD;
    data[1] = 0xDF;
    data[2] = len;

		USART_ITConfig(UART5, USART_IT_TXE,ENABLE); 
   //UART5_Send_Data(len, data); /**Í¨¹ý´®¿Ú5·¢ËÍÊý¾Ý**/

}
/**
 *@brief Êý¾ÝµÄºÍÐ£Ñé:CHK
 *@param len:   Êý¾ÝµÄ³¤¶È
 *@param array£º½ÓÊÕµ½µÄÊý¾Ý×é
 *@return sum£ºÊý¾ÝºÍÎªÁã
 *@details ·¢ËÍµÄÊý¾Ý¸ñÊ½£º
  ===============================================================================================
    ±êÖ¾1   |     ±êÖ¾2     |   Êý¾Ý³¤¶È  |  ½âÎö·½·¨  |     ·¢ËÍµÄÊý¾Ý   |   Êý¾ÝµÄºÍÐ£Ñé
  ===============================================================================================
    Flag1   |     Flag2     |     Len     |     ID     |      Data(0-9)   |        CHK
						|               |             |            |                  |
  -----------------------------------------------------------------------------------------------
    byte    |      byte     |      byte   |    byte    |     (1-10)bytes  |        byte
            |               |             |            |		        		  |
  -----------------------------------------------------------------------------------------------
 *@details Ð£Ñé¼ÆËã¹«Ê½£º
	CHK = ~(Len+Id + Data[0-9])+1.
**/
u8 CalculateCheckSum(u8 len, u8 *array)
{
    u8 sum = len;
    u8 i;
    for(i = 0; i < len - KM_HEAD_LEN - 1; i++) //Êý¾ÝµÄ³¤¶ÈÒÆ³ýflag1,flag2,Ä©Î»CHK
    {
        sum += array[i];
    }
    sum = ~sum + 1;

    return sum;
}
