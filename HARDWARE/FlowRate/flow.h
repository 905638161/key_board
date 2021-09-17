#ifndef __FLOW_H
#define __FLOW_H
#include "sys.h"





//extern u8 TimerFlagOf3s;       //
//extern u8 KeyTimerOf3s;        //

//extern u16 TimerOfSetHoldDisplay;
//extern u8  HoldDisplayFlag;

u16 Calculate_Flow(u16 Flowdata,u8 num);
void Unit_change(void);

#endif
