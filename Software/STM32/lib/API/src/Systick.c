#include "Systick.h"

void Systick_Init(void)
{
	/*!< At this stage the microcontroller clock setting is already configured, 
			 this is done through SystemInit() function which is called from startup
			 file (startup_stm32f0xx.s) before to branch to application main.
			 To reconfigure the default setting of SystemInit() function, refer to
			 system_stm32f0xx.c file
	*/
		
	/* SysTick end of count event each 1ms */
	SysTick_Config(SystemCoreClock / 1000);	
}
