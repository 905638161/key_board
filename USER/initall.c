#include "main.h"

void init_all()
{
	delay_init();
	USART1_Init(9600);//AA
	USART2_Init(4800);//EFLOW 
	USART3_Init(4800);//spO2 
	UART4_Init(19200);//touch 
	UART5_Init(115200);//gui
	WT588D_Init();
	LED_Init();
	Touch_Busy_Led_Init();
	KEY_Init();
	Gpio_Init();
	Alarm_LED_Init();
	//TSC2046_Init();
	Rotary_Encoder_Init();
	adc1_interrupt_init();
	BackLight_Init();
	IndicatorLedInit();
	Bat_LED_Init();
	TIM2_Int_Init(99, 7199); //10KHZ£¬10ms
	TIM3_PWM_Init(LCD_PWM_TIM3_PERIOD-1,0);//²»·ÖÆµ¡£PWMÆµÂÊ=72000/(719+1)=100Khz 
	
	NVIC_Configuration();
		
	paraValueInit();
}
