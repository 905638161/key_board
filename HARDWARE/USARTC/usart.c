
#include "usart.h"
#include "datahandle.h"

u8  Uart5TxBuffer[14] = {0};
u8 	Uart5RxBuffer[30] = {0};


u8 	Uart4RxBuffer[30] = {0};

u8  Uart2TxBuffer[14] = {0};
u8 	Uart2RxBuffer[14] = {0};

/*---------------------------- USART2 Mode Configuration -----------------------*/
/**
  * @brief  USART2 ��ʼ����USART2��������ӳ�䣬�����Ҫ��ʼ��USART1���в�ͬ
  * @param  baud: Set the communication baud rate 4800
  * @details Ѫ��ģ��ͨ�Ŵ���,������Ϊ4800,��У��
  * @retval None
  */
void USART2_Init(u32 baud)
{
    //GPIO�˿�����
    GPIO_InitTypeDef  GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef  NVIC_InitStructure;


    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);	//ʹ��USART2
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);	//ʹ��GPIOD,��������ʱ��
    USART_DeInit(USART2);  //��λ����2

    /*����USART2����ӳ������*/
   // GPIO_PinRemapConfig  (GPIO_Remap_USART2, ENABLE);

    /*USART2_TX   PA.2*/
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2; 
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /*USART2_RX   PA.3*/
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
    GPIO_Init(GPIOA, &GPIO_InitStructure);  //��ʼ��PD6

//    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //�����ж���2��0-3��ռ���ȼ���0-3��Ӧ���ȼ���


    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3 ; //��ռ���ȼ�3
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//�����ȼ�3
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
    NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���

    //USART��ʼ������

    USART_InitStructure.USART_BaudRate = baud;//�����ʵ�����
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
    USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
    USART_InitStructure.USART_Parity = USART_Parity_No ; //��У��λ�
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ

    USART_Init(USART2, &USART_InitStructure); //��ʼ������
    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//�����ж�
    USART_Cmd(USART2, ENABLE);                    //ʹ�ܴ���
}

/**
  * @brief  Transmits single data through the USART2 peripheral.
  * @param  Data��The data to transmit.
  * @retval None
  */
void USART2_Send_Byte(u8 Data) //����һ���ֽڣ�
{
    USART_SendData(USART2, Data);
    while( USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET );
}
/**
  * @brief  Transmits  string data through the USART2 peripheral.
  * @param  Data: the data to transmit.
  * @retval None
  */
void USART2_Send_String(u8 *Data) //�����ַ�����
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
  * @details ���յ������ݽ��г����Ĵ��������ӹ�
			 Ѫ��ģ�鷢�͵�������5���ֽ�
  * @retval None
  */
void USART2_IRQHandler(void) //�жϴ�����;
{
    static u8 	LastData = 0;
    static u16	Wait_Syn = TRUE;
    static u8		Rx_num = 0;
    static u8   Tx_num = 0;
    u8 data = 0;
		
    if(USART_GetITStatus(USART2, USART_IT_RXNE) == SET) //�ж��Ƿ����ж�;
    {
        USART_ClearFlag(USART2, USART_IT_RXNE); //�����־λ��
        if (Wait_Syn == TRUE)  				  //�ж���ʼ��־0xaa,0x55,�Ƿ����
        {
            data = USART_ReceiveData(USART2); //�������ݣ�
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
  * @brief  UART4��ʼ�� PC10,PC11
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

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; //UART4 TX��
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; //�������������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure); //�˿�C��

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11; //UART4 RX��
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; //�������룻
    GPIO_Init(GPIOC, &GPIO_InitStructure); //�˿�C��

    USART_InitStructure.USART_BaudRate = baud; //�����ʣ�
    USART_InitStructure.USART_WordLength = USART_WordLength_8b; //����λ8λ��
    USART_InitStructure.USART_StopBits = USART_StopBits_1; //ֹͣλ1λ��
    USART_InitStructure.USART_Parity = USART_Parity_No ; //��У��λ��
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ�����أ�
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;//�շ�ģʽ��
    USART_Init(UART4, &USART_InitStructure);//���ô��ڲ�����

//    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //�����ж���2��0-3��ռ���ȼ���0-3��Ӧ���ȼ���

    NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn; //�жϺţ�
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3; //��ռ���ȼ���
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3; //��Ӧ���ȼ���
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    USART_ITConfig(UART4, USART_IT_RXNE, ENABLE);
    USART_Cmd(UART4, ENABLE); //ʹ�ܴ��ڣ�
}
/**
  * @brief  Transmits single data through the UART4 peripheral.
  * @param  Data��The data to transmit.
  * @retval None
  */
void UART4_Send_Byte(u8 Data) //����һ���ֽڣ�
{
    USART_SendData(UART4, Data);
    while( USART_GetFlagStatus(UART4, USART_FLAG_TC) == RESET );
}
/**
  * @brief  Transmits string data through the UART4 peripheral.
  * @param  Data��The data to transmit.
  * @retval None
  */
void UART4_Send_String(u8 *Data) //�����ַ�����
{
    while(*Data)
        UART4_Send_Byte(*Data++);
}
/**
  * @brief  UART4 interrupt handing function.
  * @param  None
  * @retval None
  */
void UART4_IRQHandler(void) //�жϴ�������
{		
    static u16	Wait_Syn = TRUE;
    static u8		Rx_num = 0;
    u8 data = 0;
		
    if(USART_GetITStatus(UART4, USART_IT_RXNE) == SET) //�ж��Ƿ����жϣ�
    {
        if (Wait_Syn == TRUE)  				  //�ж���ʼ��־0xaa,0x55,�Ƿ����
        {
            data = USART_ReceiveData(UART4); //�������ݣ�
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
  * @brief  UART5��ʼ�� PC10,PC11
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

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12; //UART5 TX��
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; //�������������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure); //�˿�C��

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2; //UART5 RX��
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; //�������룻
    GPIO_Init(GPIOD, &GPIO_InitStructure); //�˿�D��

    USART_InitStructure.USART_BaudRate = baud; //�����ʣ�
    USART_InitStructure.USART_WordLength = USART_WordLength_8b; //����λ8λ��
    USART_InitStructure.USART_StopBits = USART_StopBits_1; //ֹͣλ1λ��
    USART_InitStructure.USART_Parity = USART_Parity_No ; //��У��λ��
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ�����أ�
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;//�շ�ģʽ��
    USART_Init(UART5, &USART_InitStructure);//���ô��ڲ�����


//    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //�����ж���2��0-3��ռ���ȼ���0-3��Ӧ���ȼ���

    NVIC_InitStructure.NVIC_IRQChannel = UART5_IRQn; //�жϺţ�
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3; //��ռ���ȼ���
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3; //��Ӧ���ȼ���
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);


    USART_ITConfig(UART5, USART_IT_RXNE, ENABLE);
    USART_Cmd(UART5, ENABLE); //ʹ�ܴ��ڣ�
}
/**
  * @brief  Transmits single data through the UART5 peripheral.
  * @param  Data��The data to transmit.
  * @retval None
  */
void UART5_Send_Byte(u8 Data) //����һ���ֽڣ�
{
    USART_SendData(UART5, Data);
    while( USART_GetFlagStatus(UART5, USART_FLAG_TC) == RESET );
}
/**
* @brief ����һ��������
* @param
**len: ���ݵĳ���
**data:�������ݵ��׵�ַ
* @return None
* @details ������USART5�ķ���һ���ֽڵĺ���
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
  * @param  Data��The data to transmit.
  * @retval None
  */
void UART5_Send_String(u8 *Data) //�����ַ������ַ�����ĩβΪ��
{
    while(*Data)
        UART5_Send_Byte(*Data++);
}
/**
  * @brief  UART5 interrupt handing function.
  * @param  None
  * @retval None
  */
void UART5_IRQHandler(void) //�жϴ�������
{
    static u8 	LastData = 0;
    static u16	Wait_Syn = TRUE;
    static u8		Rx_num = 0;
    static u8   Tx_num = 0;
    u8 data = 0;
		
    if(USART_GetITStatus(UART5, USART_IT_RXNE) == SET) //�ж��Ƿ����жϣ�
    {
        if (Wait_Syn == TRUE)  				  //�ж���ʼ��־0xaa,0x55,�Ƿ����
        {
            data = USART_ReceiveData(UART5); //�������ݣ�
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
* @brief ����ͨ�ų���
* @param
*len: ���ݳ���
*ID:  ���ݵ�����
*data[len]:��Ҫ���͵�����
*CHK:���ݵ�У��
* @return the return value
* @details a full description of what the function does
*/
void UART5_Data_Transmit(u8 len, u8 *data)
{

    data[0] = 0xFD;
    data[1] = 0xDF;
    data[2] = len;

		USART_ITConfig(UART5, USART_IT_TXE,ENABLE); 
   //UART5_Send_Data(len, data); /**ͨ������5��������**/

}
/**
 *@brief ���ݵĺ�У��:CHK
 *@param len:   ���ݵĳ���
 *@param array�����յ���������
 *@return sum�����ݺ�Ϊ��
 *@details ���͵����ݸ�ʽ��
  ===============================================================================================
    ��־1   |     ��־2     |   ���ݳ���  |  ��������  |     ���͵�����   |   ���ݵĺ�У��
  ===============================================================================================
    Flag1   |     Flag2     |     Len     |     ID     |      Data(0-9)   |        CHK
						|               |             |            |                  |
  -----------------------------------------------------------------------------------------------
    byte    |      byte     |      byte   |    byte    |     (1-10)bytes  |        byte
            |               |             |            |		        		  |
  -----------------------------------------------------------------------------------------------
 *@details У����㹫ʽ��
	CHK = ~(Len+Id + Data[0-9])+1.
**/
u8 CalculateCheckSum(u8 len, u8 *array)
{
    u8 sum = len;
    u8 i;
    for(i = 0; i < len - KM_HEAD_LEN - 1; i++) //���ݵĳ����Ƴ�flag1,flag2,ĩλCHK
    {
        sum += array[i];
    }
    sum = ~sum + 1;

    return sum;
}
