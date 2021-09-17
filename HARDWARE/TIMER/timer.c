#include "timer.h"
#include "key.h"
#include "led.h"
#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////


//ͨ�ö�ʱ���жϳ�ʼ��
//����ʱ��ѡ��ΪAPB1��2������APB1Ϊ36M
//arr���Զ���װֵ��
//psc��ʱ��Ԥ��Ƶ��
//����ʹ�õ��Ƕ�ʱ��2!
void TIM2_Int_Init(u16 arr, u16 psc)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE); //ʱ��ʹ��

    TIM_TimeBaseStructure.TIM_Period = arr; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ	 ������5000Ϊ500ms
    TIM_TimeBaseStructure.TIM_Prescaler = psc; //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ  10Khz�ļ���Ƶ��
    TIM_TimeBaseStructure.TIM_ClockDivision = 0; //����ʱ�ӷָ�:TDTS = Tck_tim
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure); //����TIM_TimeBaseInitStruct��ָ���Ĳ�����ʼ��TIMx��ʱ�������λ

    TIM_ITConfig(  //ʹ�ܻ���ʧ��ָ����TIM�ж�
        TIM2, //TIM2
        TIM_IT_Update ,
        ENABLE  //ʹ��
    );
    NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;  //TIM2�ж�
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;  //��ռ���ȼ�0��3,��ռ���ȼ�2
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  //�����ȼ�0-3�������ȼ�Ϊ3
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQͨ����ʹ��
    NVIC_Init(&NVIC_InitStructure);  //����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ���

    TIM_Cmd(TIM2, ENABLE);  //ʹ��TIMx����

}


/**
* @brief ��ʱ�������жϴ�����
* @param None
* @return None

* @details a full description of what the function does

*/
u8   Flag_250ms;
u8	 Flag_20ms;
u8   Flag_1s;

void TIM2_IRQHandler(void)   //TIM2�ж�
{
    static u8 LED_Flash_Count;

    if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET) //���ָ����TIM�жϷ������:TIM �ж�Դ
    {
        if(LED_Flash_Count%25==0)    //��ʱ250ms
        {
            Flag_250ms = !Flag_250ms;
					  System_Task_Register|=TASK_UPDATE_250MS;
        }
				setLED_Flash(Flag_250ms);
				
				if(LED_Flash_Count%2==0)
				{
						Flag_20ms=1;
					  System_Task_Register|=TASK_UPDATE_20MS;
				}
				
        if(LED_Flash_Count%100==0)  //��ʱ1s
        {

            Flag_1s = !Flag_1s;
            LED_Flash_Count = 0;    //��������
        }

        LED_Flash_Count++;
				
				getPowerStatus();
        TIM_ClearITPendingBit(TIM2, TIM_IT_Update);  //���TIMx���жϴ�����λ:TIM �ж�Դ
    }
}












