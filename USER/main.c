#include "main.h"

extern u8 USART2_Sta;

int  main(void)
{
	  kmSelfTest();
		System_Task_Register |= TASK_UPDATE_VER|TASK_UPDATE_ALARM;
    while(1)
    {
				System_Task_Handler();
				KEY_Value_Scan();
				Check_DL();
			  //tsc2046ReadData();
    }
}
















