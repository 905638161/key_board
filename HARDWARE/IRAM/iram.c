/**
* @file 	iram.c
* @brief	IRAM��������ģ�����ݴ������������ã����ݴ���
* @details	�����˴������������յ������ݽ����˴���ӹ���������������ݷ����и����ذ�
			ʹ���˴���1
* @author	guo
* @date		date
* @version	A001
* @par Copyright (c): 
* 		���������㹫˾
* @par History:         
*	  version: ������, 20180710, Ver0.0
*/

/**INCLUDES-------------------------------------------------------------------------**/
#include "iram.h"
#include "led.h"

IRMA_SENSOR_RXD_PARAMETERS 	IramPara;                 /*����IRAMģ����յ����ݽṹ����*/
IRMA_SENSOR_TXD_FRAME_DATA	IRMA_Txd_SensorFrameData; /*����IRAMģ�鷢�͵����ݽṹ����*/

u8 Uart1TxBuffer[6];
u8 UART1_TX_BUSY_FLAG = FALSE;
extern IRAM_CALI		IRAM_CaliData;
u16 IRMA_Counter = 0;
/**
  * @brief  USART1 ��ʼ����USART1��������ӳ�䣬�����Ҫ��ʼ��USART1���в�ͬ
  * @param  baud: Set the communication baud rate
  * @retval None
  */
void USART1_Init(u32 baud)
{
    //GPIO�˿�����
    GPIO_InitTypeDef  GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef  NVIC_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA, ENABLE);	//ʹ��USART1��GPIOAʱ��
    USART_DeInit(USART1);  //��λ����1
    //USART1_TX   PA.9n
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //PA.9
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������,�˿ڸ���
    GPIO_Init(GPIOA, &GPIO_InitStructure); //��ʼ��PA9

    //USART1_RX	  PA.10
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
    GPIO_Init(GPIOA, &GPIO_InitStructure);  //��ʼ��PA10

//    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //�����ж���2��0-3��ռ���ȼ���0-3��Ӧ���ȼ���

    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3 ; //��ռ���ȼ�3
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//�����ȼ�3
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
    NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���

    //USART ��ʼ������

    USART_InitStructure.USART_BaudRate = baud;//һ������Ϊ9600;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
    USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
    USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ

    USART_Init(USART1, &USART_InitStructure); //��ʼ������
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//�����ж�
    USART_Cmd(USART1, ENABLE);                    //ʹ�ܴ���
}

/**
  * @brief  Transmits single data through the USART1 peripheral.
  * @param  Data: the data to transmit.
  * @retval None
  */
void USART1_Send_Byte(u8 Data) //����һ���ֽڣ�
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
void USART1_Send_String(u8 *Data) //�����ַ�����
{
    while(*Data)
        USART1_Send_Byte(*Data++);
}


/**
 * @brief ��������ģ��Ĺ���
 * @param pointer���ͣ����պ������׵�ַ
 * @return IramPara ����������ģ�����ͽṹ�ķ���ֵ
 * @details ��������������ģ�����͵����ݽṹ���������

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
  * @detail CO2/AA��������ģ��ͨ�Žӿ�
  */
void USART1_IRQHandler(void) //�жϴ�������
{
    static u8 	Usart1RxBuffer[30] = {0};
    static u8 	LastData = 0;
    static u16	Wait_Syn = TRUE;
    static u8	Rx_num = 0;
    static u8   Tx_num = 0;
	
    u8 data = 0;
    u8 sum = 0;
    u8 i = 0;
    if(USART_GetITStatus(USART1, USART_IT_RXNE) == SET) //�ж��Ƿ����жϣ�
    {
				USART_ClearFlag(USART1, USART_IT_RXNE); //�����־λ��
        if (Wait_Syn == TRUE)  				  //�ж���ʼ��־0xaa,0x55,�Ƿ����
        {
            data = USART_ReceiveData(USART1); //�������ݣ�
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
                if(!sum)   //��У����ֵΪ��
                {
                    IRAM_DataDecode(Usart1RxBuffer);
					
										IRMA_Txd_SensorFrameData.CO2=IramPara.CO2;
										IRMA_Txd_SensorFrameData.N2O=IramPara.N2O;
										IRMA_Txd_SensorFrameData.AA1=IramPara.AA1;
										IRMA_Txd_SensorFrameData.AA2=IramPara.AA2;
									
                    switch(IramPara.Id)
                    {
                    case 0:  //����InspVals
										{
                        IRMA_Txd_SensorFrameData.FiCO2 = IramPara.SlowData[0];
                        IRMA_Txd_SensorFrameData.FiN2O = IramPara.SlowData[1];
                        IRMA_Txd_SensorFrameData.FiAA1 = IramPara.SlowData[2];
                        IRMA_Txd_SensorFrameData.FiAA2 = IramPara.SlowData[3];
//                        IRMA_Txd_SensorFrameData.O2  = IramPara.SlowData[4];
										}
                    break;
                    case 1:  //����ExpVals
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

