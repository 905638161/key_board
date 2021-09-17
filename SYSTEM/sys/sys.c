#include "sys.h"
#include "datahandle.h"
u16 BatteryHighLevel = 0;
u16 BatteryLowLevel = 1000;


void Gpio_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13|GPIO_Pin_12; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;// ;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz ;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
}

//********************************************************************************  
void NVIC_Configuration(void)
{

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	//设置NVIC中断分组2:2位抢占优先级，2位响应优先级

}


u8 CheckAcStatus(void)
{
  u8 state = FALSE;
  if(PBin(12))
  {
     state = TRUE;
  }
  return(state);
}


u8 CheckChargeStatus(void)
{
  u8 state = 0;
	
  if(PBin(13))
  {
     state = 0;
  }
  else
  {
     state = 1;
  }
  
  return(state);
}

void getPowerStatus(void)
{
  static unsigned int count ;
  static unsigned int high_level;
  static unsigned int low_level;
  if(!CheckChargeStatus())
    high_level ++;
  else
    low_level ++;
	
  count++;
	
  if(count >= 100)//1s
  {
    BatteryHighLevel = high_level;
    BatteryLowLevel = low_level;
    high_level = 0;
    low_level = 0;
    count = 0;
  }
}
