#include "display.h"
#include "ch452.h"

/*****************************
Copyright (C), Eternity.Co.,Ltd.
FileName: display.c
Author:   GUO
Version : ver0.0
date:     2017-09-14
Description: �Բ��������������ݽ��мӹ�����ʾ
History:           // ��ʷ�޸ļ�¼
**Ver0.0
******************************/

void Display_init(void)
{ 	
	
	CH452_Init();                          //CH452���IO�ڵĳ�ʼ��
	
    CH452_Write(CH452_DIG0 | LED_0);       //�������İ�λ�����һ�������
	CH452_Write(CH452_DIG1 | 0x80 | LED_0);//�������İ�λ
	CH452_Write(CH452_DIG2 | LED_0);       //�������İ�λ
	CH452_Write(CH452_DIG3 | 0x80 | LED_0);//�������İ�λ
	CH452_Write(CH452_DIG4 | LED_0);       //�������İ�λ
	CH452_Write(CH452_DIG5 | 0x80 | LED_0);//�������İ�λ
	
}

 
//==========��DIG5 DIG4λ����˸��ʾA ����A N2O O2��ѡ��
void Display_ModeA_Flash(void)  
{
  CH452_Write(CH452_TWINKLE | 0x10);
     CH452_Write(CH452_DIG4 | LED_A);    
}

//==========��DIG5 DIG4λ�ó�����ʾA ����A N2O O2��ѡ��
void Display_ModeA_Normal(void)     
{
   CH452_Write(CH452_TWINKLE);
   CH452_Write(CH452_DIG4 | LED_A);  
}
//==========��DIG5 DIG4λ����˸��ʾN2O ����A N2O O2��ѡ��
void Display_ModeN2O_Flash(void)
{
 
  CH452_Write(CH452_TWINKLE | 0x30);
     CH452_Write(CH452_DIG5 | LED_N); 
     CH452_Write(CH452_DIG4 | LED_2);    
   
}
//==========��DIG5 DIG4λ�ó�����ʾN2O ����A N2O O2��ѡ��
void Display_ModeN2O_Normal(void)
{
     CH452_Write(CH452_TWINKLE);
     CH452_Write(CH452_DIG5 | LED_N); 
     CH452_Write(CH452_DIG4 | LED_2);  
}
//==========��DIG5 DIG4λ����˸��ʾO2 ����A N2O O2��ѡ��
void Display_ModeO2_Flash(void)
{
 
   CH452_Write(CH452_TWINKLE | 0x30);
     CH452_Write(CH452_DIG5 | LED_O); 
     CH452_Write(CH452_DIG4 | LED_2);    
   
}
//==========��DIG5 DIG4λ�ó�����ʾO2 ����A N2O O2��ѡ��
void Display_ModeO2_Normal(void)
{
   CH452_Write(CH452_TWINKLE);
     CH452_Write(CH452_DIG5 | LED_O); 
     CH452_Write(CH452_DIG4 | LED_2);   
}
/*��ʾ��ֵ��LED�����ת�� dot--��С����=1;����С����=0*/
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
//==========��������SendBuffer[6]����ʾ���ݽ��в��,�����ʾ�ܺ�
//==========������ʾ��������  �Ժ�ɸ���define��������ѡ������������һ��
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
	CH452_Write(CH452_DIG0 | DisplayBuffer[0]);//�������İ�λ���һ�������
	CH452_Write(CH452_DIG1 | DisplayBuffer[1]+dotValue);//�������İ�λ
	if(DisplayDotN2OFlag)	  //20170525
	{
		dotValue = 0x80;
	}
	else
	{
		dotValue = 0;
	}
	CH452_Write(CH452_DIG2 | DisplayBuffer[2]);//�������İ�λ
	CH452_Write(CH452_DIG3 | DisplayBuffer[3]+dotValue);//�������İ�λ
	if(DisplayDotO2Flag)	  //20170525
	{
		dotValue = 0x80;
	}
	else
	{
		dotValue = 0;
	}
	CH452_Write(CH452_DIG4 | DisplayBuffer[4]);//�������İ�λ
	CH452_Write(CH452_DIG5 | DisplayBuffer[5]+dotValue);//�������İ�λ     
} 
//==========��DIG2 DIG1 DIG0λ����˸��ʾ�궨��ֵSetStatus&0x1f
void Display_Value_Flash(s8 CaliPoint)
{
  s8 temp;
  s8 temp1;
  s8 LEDvalue; 
  temp=CaliPoint;
  
  if ( temp <= 9 )
  {
    LEDvalue=Exchange_Value_To_LED(temp); 
    CH452_Write(CH452_TWINKLE | 0x03);  //����λ������˸
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

//==========��DIG2 DIG1 DIG0λ�ó�����ʾ���궨�� ����ʱ�ڴ�ֵӦΪ��ǰֵ��1 
void Display_Value_Normal(s8 CaliPoint)
{
  s8 temp;
  s8 temp1;
  s8 LEDvalue;

  temp=CaliPoint;
  CH452_Write(CH452_TWINKLE);  //����λ��������˸
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
//========��ʾ���Ҫˢ��DIG5 DIG4��Ļ ȡ����˸ 
void Display_Mode_Clear(void)
{ 
     CH452_Write(CH452_DIG4 | LED_NULL);   
     CH452_Write(CH452_DIG5 | LED_NULL); 
}

//========��ʾ���Ҫˢ��DIG3 DIG2 DIG1 DIG0��Ļ ȡ����˸ 
void Display_Value_Clear(void)
{
   
     CH452_Write(CH452_DIG0 | LED_NULL);   
     CH452_Write(CH452_DIG1 | LED_NULL); 
     CH452_Write(CH452_DIG2 | LED_NULL); 
     CH452_Write(CH452_DIG3 | LED_NULL); 
}

/*
//========ֻ��DIG5 DIG4 ��˸ 
Set_Mode_Flash()
{
     CH452_Write(CH452_TWINKLE & 0xff00 | 0x0030);  //ȡ��DIG3 DIG2 DIG1 DIG0����˸
}

//========ֻ��DIG3 DIG2 DIG1 DIG0��˸ 
Set_Value_Flash()
{
     CH452_Write(CH452_TWINKLE & 0xff00 | 0x000f);   
}
*/
//========DIG5 DIG4 DIG3 DIG2 DIG1 DIG0����˸ 
void Set_No_Flash(void)
{
     CH452_Write(CH452_TWINKLE & 0xff00);
}  

//================��ʾ���ܵ���ʱ�ڵ�Ӱ�죬����ʱ����ʾ��Ӧ����һ״̬����ֵ

