/**
* @file 	spo2.c
* @brief	Ѫ��ģ��ͨ�Ŵ��ڵĳ�ʼ��,�������ݵĴ���
* @details
* @author	��
* @date		date
* @version	A001
* @par Copyright (c):
* 		���������㼼���������ι�˾��˾
* @par History:
*	  version: author, date, desc

*/

#include "spo2.h"
#include "datahandle.h"


u16 SPO2_Counter = 0;
struct SPO2_TxdFrame SPO2_TxdData;

/*---------------------------- USART3 Mode Configuration -----------------------*/
/**
  * @brief  USART3��ʼ����PB10��PB11Ĭ�Ͽڲ���Ҫӳ����USART2
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

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; //USART3 TX��
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; //�������������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure); //�˿�B��PB10

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11; //USART3 RX��
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; //�������룻
    GPIO_Init(GPIOB, &GPIO_InitStructure); //�˿�B��PB11

    USART_InitStructure.USART_BaudRate = baud; //�����ʣ�
    USART_InitStructure.USART_WordLength = USART_WordLength_8b; //����λ8λ��
    USART_InitStructure.USART_StopBits = USART_StopBits_1; //ֹͣλ1λ��
    USART_InitStructure.USART_Parity = USART_Parity_Odd;//��У��
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ�����أ�
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;//�շ�ģʽ��
    USART_Init(USART3, &USART_InitStructure);//���ô��ڲ�����

//    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //�����ж���2��0��3��ռ���ȼ���0��3��Ӧ���ȼ���

    NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn; //�жϺţ�
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3; //��ռ���ȼ���
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3; //��Ӧ���ȼ���
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);
    USART_Cmd(USART3, ENABLE); //ʹ�ܴ��ڣ�
}
/**
  * @brief  Transmits single data through the USART3 peripheral.
  * @param  Data��The data to transmit.
  * @retval None
  */
void USART3_Send_Byte(u8 Data) //����һ���ֽڣ�
{
    USART_SendData(USART3, Data);
    while( USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET );
}
/**
  * @brief  Transmits string data through the USART3 peripheral.
  * @param  Data��The data to transmit.
  * @retval None
  */
void USART3_Send_String(u8 *Data) //�����ַ�����
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
  * @brief  ������յ�Ѫ������
  * @param  *pointer
  * @details �Խ��յ������ݽ��д����ӹ���ת��������λ��
			 ͨ�ŵĸ�ʽ
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
void USART3_IRQHandler(void) //�жϴ�������
{
    static u8 	Usart3RxBuffer[6] = {0};
    static u16	Wait_Syn = TRUE;
    static u8		Rx_num = 0;
    u8 					data = 0;
		
    if(USART_GetITStatus(USART3, USART_IT_RXNE) == SET) //�ж��Ƿ����жϣ�
    {
        if(Wait_Syn == TRUE)
        {
            data = USART_ReceiveData(USART3); //�������ݣ�
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
							SPO2_DataHandle(Usart3RxBuffer); //���յ������ݽ��д���ת��
						}
        }
		}
}

