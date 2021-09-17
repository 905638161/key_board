#ifndef __DISPLAY_H
#define __DISPLAY_H


#include "sys.h"


#define LED_A    0x77  	//×ÖÄ¸A
#define LED_O    0x3f  	//×ÖÄ¸O
#define LED_N    0x37   //×ÖÄ¸N
#define LED_0    0x3f	//×ÖÄ¸0
#define LED_1    0x06	//×ÖÄ¸1
#define LED_2    0x5b	//×ÖÄ¸2
#define LED_3    0x4f	//×ÖÄ¸3
#define LED_4    0x66 	//×ÖÄ¸4
#define LED_5    0x6d	//×ÖÄ¸5
#define LED_6    0x7d	//×ÖÄ¸6
#define LED_7    0x07	//×ÖÄ¸7
#define LED_8    0x7f	//×ÖÄ¸8
#define LED_9    0x6f	//×ÖÄ¸9
#define LED_NULL 0x00	//¿ÕÏÔÊ¾


void Display_Value_Clear(void);

void Display_Value_Normal(s8 CaliPoint);
void Display_Value_Flash(s8 CaliPoint);

void Display_Mode_Clear(void);

void Display_ModeA_Normal(void); 
void Display_ModeA_Flash(void);

void Display_ModeN2O_Normal(void);
void Display_ModeN2O_Flash(void);

void Display_ModeO2_Normal(void);
void Display_ModeO2_Flash(void);

void Set_No_Flash(void);


void Display_init(void);
void Display_FlowValue(void);   

extern u8 DisplayBuffer[6];
extern u8 SendBuffer[6];

extern s8  DisplayDotAirFlag;   //20170525
extern s8  DisplayDotN2OFlag;	//20170525
extern s8  DisplayDotO2Flag;    //20170525




#endif 

