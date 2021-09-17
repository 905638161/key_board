#ifndef __TSC2046_SPI_H
#define __TSC2046_SPI_H

#include "sys.h"

#define TSC2046_MAX_SAMPLE	10		/* ÿ�βɼ��Ĵ�����ȡ��ƽ��ֵ */


struct TouchscreenData
{
	u16  xData;
	u16  yData;
	u8	 pendown;
};

void	tsc2046ReadData(void);

void TSC2046_Init(void);



extern struct TouchscreenData tsData;
#endif





