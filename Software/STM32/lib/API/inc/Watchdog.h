#ifndef __WATCHDOG_H
#define __WATCHDOG_H

/* includes ---------------------------------------------------------------- */
#include "stm32f0xx_iwdg.h"

/* class ------------------------------------------------------------------- */
class Watchdog
{
	private:
	
	public:
		
		Watchdog(int timeout);
		void reload(void);
};

#endif
