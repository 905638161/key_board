#ifndef __KEY_H
#define __KEY_H	 
#include "sys.h"


#define KEY_ENTER  					PEin(13)
#define KEY_SLIENCE  				PCin(9)
#define KEY_HOME  					PCin(7)
#define KEY_STANDBY  				PAin(8)
#define KEY_ALARM_RESET  		PCin(8)

void KEY_Init(void);
void KEY_Value_Scan(void);

#endif
