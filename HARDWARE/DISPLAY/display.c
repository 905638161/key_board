#include "display.h"
#include "ch452.h"

/*****************************
Copyright (C), Eternity.Co.,Ltd.
FileName: display.c
Author:   GUO
Version : ver0.0
date:     2017-09-14
Description: 对采样到的流量数据进行加工，显示
History:           // 历史修改记录
**Ver0.0
******************************/

void Display_init(void)
{ 	
	
	CH452_Init();                          //CH452相关IO口的初始化
	
    CH452_Write(CH452_DIG0 | LED_0);       //参数监测的百位，最后一个数码管
	CH452_Write(CH452_DIG1 | 0x80 | LED_0);//参数监测的百位
	CH452_Write(CH452_DIG2 | LED_0);       //参数监测的百位
	CH452_Write(CH452_DIG3 | 0x80 | LED_0);//参数监测的百位
	CH452_Write(CH452_DIG4 | LED_0);       //参数监测的百位
	CH452_Write(CH452_DIG5 | 0x80 | LED_0);//参数监测的百位
	
}

 
//==========在DIG5 DIG4位置闪烁显示A 用于A N2O O2的选择
void Display_ModeA_Flash(void)  
{
  CH452_Write(CH452_TWINKLE | 0x10);
     CH452_Write(CH452_DIG4 | LED_A);    
}

//==========在DIG5 DIG4位置常规显示A 用于A N2O O2的选择
void Display_ModeA_Normal(void)     
{
   CH452_Write(CH452_TWINKLE);
   CH452_Write(CH452_DIG4 | LED_A);  
}
//==========在DIG5 DIG4位置闪烁显示N2O 用于A N2O O2的选择
void Display_ModeN2O_Flash(void)
{
 
  CH452_Write(CH452_TWINKLE | 0x30);
     CH452_Write(CH452_DIG5 | LED_N); 
     CH452_Write(CH452_DIG4 | LED_2);    
   
}
//==========在DIG5 DIG4位置常规显示N2O 用于A N2O O2的选择
void Display_ModeN2O_Normal(void)
{
     CH452_Write(CH452_TWINKLE);
     CH452_Write(CH452_DIG5 | LED_N); 
     CH452_Write(CH452_DIG4 | LED_2);  
}
//==========在DIG5 DIG4位置闪烁显示O2 用于A N2O O2的选择
void Display_ModeO2_Flash(void)
{
 
   CH452_Write(CH452_TWINKLE | 0x30);
     CH452_Write(CH452_DIG5 | LED_O); 
     CH452_Write(CH452_DIG4 | LED_2);    
   
}
//==========在DIG5 DIG4位置常规显示O2 用于A N2O O2的选择
void Display_ModeO2_Normal(void)
{
   CH452_Write(CH452_TWINKLE);
     CH452_Write(CH452_DIG5 | LED_O); 
     CH452_Write(CH452_DIG4 | LED_2);   
}
/*显示点值到LED段码的转换 dot--带小数点=1;不带小数点=0*/
s8 Exchange_Value_To_LED(s8 value)
{
    s8 temp;
    switch(value)
    {
      case 0:
         temp=LED_0;
         break;
      case 1:
         temp=LED_1;
         break;
      case 2:
         temp=LED_2;
         break;
      case 3:
         temp=LED_3;
         break;
      case 4:
         temp=LED_4;
         break;
      case 5:
         temp=LED_5;
         break;
      case 6:
         temp=LED_6;
         break;
      case 7:
         temp=LED_7;
         break;
      case 8:
         temp=LED_8;
         break;
      case 9:
         temp=LED_9;
         break;
	  case 10:
         temp=LED_1;
         break;
    }
    return temp;
}
//==========利用数组SendBuffer[6]对显示数据进行拆分,这对显示很好
//==========正常显示流量数据  以后可根据define常量进行选择亮两个还是一个
void Display_FlowValue(void)        
{
	s8 i;
	u8 dotValue;
	for (i=0;i<6;i++)
	DisplayBuffer[i]=Exchange_Value_To_LED(SendBuffer[i]);
	

	if(DisplayDotAirFlag)	//20170525
	{
		dotValue = 0x80;
	}
	else
	{
		dotValue = 0;
	}
	CH452_Write(CH452_DIG0 | DisplayBuffer[0]);//参数监测的百位最后一个数码管
	CH452_Write(CH452_DIG1 | DisplayBuffer[1]+dotValue);//参数监测的百位
	if(DisplayDotN2OFlag)	  //20170525
	{
		dotValue = 0x80;
	}
	else
	{
		dotValue = 0;
	}
	CH452_Write(CH452_DIG2 | DisplayBuffer[2]);//参数监测的百位
	CH452_Write(CH452_DIG3 | DisplayBuffer[3]+dotValue);//参数监测的百位
	if(DisplayDotO2Flag)	  //20170525
	{
		dotValue = 0x80;
	}
	else
	{
		dotValue = 0;
	}
	CH452_Write(CH452_DIG4 | DisplayBuffer[4]);//参数监测的百位
	CH452_Write(CH452_DIG5 | DisplayBuffer[5]+dotValue);//参数监测的百位     
} 
//==========在DIG2 DIG1 DIG0位置闪烁显示标定点值SetStatus&0x1f
void Display_Value_Flash(s8 CaliPoint)
{
  s8 temp;
  s8 temp1;
  s8 LEDvalue; 
  temp=CaliPoint;
  
  if ( temp <= 9 )
  {
    LEDvalue=Exchange_Value_To_LED(temp); 
    CH452_Write(CH452_TWINKLE | 0x03);  //后两位设置闪烁
    CH452_Write(CH452_DIG1 | LEDvalue);
    CH452_Write(CH452_DIG0 | LED_0);                 
  }
  else if( temp < 17 )
  {
    temp1=temp-10;
    LEDvalue=Exchange_Value_To_LED(temp1); 
    CH452_Write(CH452_TWINKLE | 0x07);   
    CH452_Write(CH452_DIG2 | LED_1);
    CH452_Write(CH452_DIG1 | LEDvalue);
    CH452_Write(CH452_DIG0 | LED_0);  
  }
  else if( temp == 17 )
  {
    CH452_Write(CH452_TWINKLE | 0x07);   
    CH452_Write(CH452_DIG2 | LED_1);
    CH452_Write(CH452_DIG1 | LED_8);
    CH452_Write(CH452_DIG0 | LED_0);    
  }  
  else
  {
    CH452_Write(CH452_TWINKLE | 0x07);   
    CH452_Write(CH452_DIG2 | LED_2);
    CH452_Write(CH452_DIG1 | LED_0);
    CH452_Write(CH452_DIG0 | LED_0);    
  }  
}

//==========在DIG2 DIG1 DIG0位置常规显示欲标定点 在延时期此值应为当前值减1 
void Display_Value_Normal(s8 CaliPoint)
{
  s8 temp;
  s8 temp1;
  s8 LEDvalue;

  temp=CaliPoint;
  CH452_Write(CH452_TWINKLE);  //后三位不设置闪烁
  if ( temp <= 9 )
  {
    LEDvalue=Exchange_Value_To_LED(temp); 
    CH452_Write(CH452_DIG1 | LEDvalue);
    CH452_Write(CH452_DIG0 | LED_0);                 
  }
  else if( temp < 17 )
  {
    temp1=temp-10;
    LEDvalue=Exchange_Value_To_LED(temp1); 
    CH452_Write(CH452_DIG2 | LED_1);
    CH452_Write(CH452_DIG1 | LEDvalue);
    CH452_Write(CH452_DIG0 | LED_0);  
  }
  else if( temp == 17 )
  {
    CH452_Write(CH452_DIG2 | LED_1);
    CH452_Write(CH452_DIG1 | LED_8);
    CH452_Write(CH452_DIG0 | LED_0);    
  }
  else
  {
    CH452_Write(CH452_DIG2 | LED_2);
    CH452_Write(CH452_DIG1 | LED_0);
    CH452_Write(CH452_DIG0 | LED_0);    
  }   
}
//========显示完成要刷掉DIG5 DIG4屏幕 取消闪烁 
void Display_Mode_Clear(void)
{ 
     CH452_Write(CH452_DIG4 | LED_NULL);   
     CH452_Write(CH452_DIG5 | LED_NULL); 
}

//========显示完成要刷掉DIG3 DIG2 DIG1 DIG0屏幕 取消闪烁 
void Display_Value_Clear(void)
{
   
     CH452_Write(CH452_DIG0 | LED_NULL);   
     CH452_Write(CH452_DIG1 | LED_NULL); 
     CH452_Write(CH452_DIG2 | LED_NULL); 
     CH452_Write(CH452_DIG3 | LED_NULL); 
}

/*
//========只有DIG5 DIG4 闪烁 
Set_Mode_Flash()
{
     CH452_Write(CH452_TWINKLE & 0xff00 | 0x0030);  //取消DIG3 DIG2 DIG1 DIG0的闪烁
}

//========只有DIG3 DIG2 DIG1 DIG0闪烁 
Set_Value_Flash()
{
     CH452_Write(CH452_TWINKLE & 0xff00 | 0x000f);   
}
*/
//========DIG5 DIG4 DIG3 DIG2 DIG1 DIG0不闪烁 
void Set_No_Flash(void)
{
     CH452_Write(CH452_TWINKLE & 0xff00);
}  

//================显示会受到延时期的影响，在延时期显示的应是上一状态的数值

