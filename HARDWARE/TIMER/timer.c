#include "timer.h"
#include "key.h"
#include "led.h"
#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////


//通用定时器中断初始化
//这里时钟选择为APB1的2倍，而APB1为36M
//arr：自动重装值。
//psc：时钟预分频数
//这里使用的是定时器2!
void TIM2_Int_Init(u16 arr, u16 psc)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE); //时钟使能

    TIM_TimeBaseStructure.TIM_Period = arr; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值	 计数到5000为500ms
    TIM_TimeBaseStructure.TIM_Prescaler = psc; //设置用来作为TIMx时钟频率除数的预分频值  10Khz的计数频率
    TIM_TimeBaseStructure.TIM_ClockDivision = 0; //设置时钟分割:TDTS = Tck_tim
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure); //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位

    TIM_ITConfig(  //使能或者失能指定的TIM中断
        TIM2, //TIM2
        TIM_IT_Update ,
        ENABLE  //使能
    );
    NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;  //TIM2中断
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;  //先占优先级0—3,先占优先级2
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  //从优先级0-3，从优先级为3
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道被使能
    NVIC_Init(&NVIC_InitStructure);  //根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器

    TIM_Cmd(TIM2, ENABLE);  //使能TIMx外设

}


/**
* @brief 定时器三的中断处理函数
* @param None
* @return None

* @details a full description of what the function does

*/
u8   Flag_250ms;
u8	 Flag_20ms;
u8   Flag_1s;

void TIM2_IRQHandler(void)   //TIM2中断
{
    static u8 LED_Flash_Count;

    if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET) //检查指定的TIM中断发生与否:TIM 中断源
    {
        if(LED_Flash_Count%25==0)    //计时250ms
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
				
        if(LED_Flash_Count%100==0)  //计时1s
        {

            Flag_1s = !Flag_1s;
            LED_Flash_Count = 0;    //计数清零
        }

        LED_Flash_Count++;
				
				getPowerStatus();
        TIM_ClearITPendingBit(TIM2, TIM_IT_Update);  //清除TIMx的中断待处理位:TIM 中断源
    }
}












