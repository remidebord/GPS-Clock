#ifndef __POWER_H
#define __POWER_H

/* includes ---------------------------------------------------------------- */
#include "stm32f0xx_pwr.h"

/* class ------------------------------------------------------------------- */
class Power
{
	private:
	
	public:
		
		Power(void);
		void stop(void);
		void standby(char pin);
};

#endif
