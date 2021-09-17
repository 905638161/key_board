#ifndef __WT588D_H
#define __WT588D_H

#include "sys.h"

#define RST PEout(0)
#define SDA PBout(9) 
#define WTW_Flag PBin(8)



#define SELF_ALARM  		0xFF
#define HIGH_ALARM  		0x04
#define MEDIUM_ALARM 		0x03
#define LOW_ALARM				0x02
#define NOTE_ALARM			0x01
#define NONE_ALARM			0x00

#define alarm_L_addr   0    	//低级报警地址
#define alarm_M_addr   1    	//中级报警地址
#define alarm_H_addr   2    	//高级报警地址

#define S_loop 0xF2    			//开始循环播放
#define E_loop 0xFE    			//停止语音播放

void WT588D_Init(void);
void Alarm_voice_Select(u8 priority, u8 volume, u8 silence);
#endif
