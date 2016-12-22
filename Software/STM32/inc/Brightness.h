#ifndef __BRIGHTNESS_H
#define __BRIGHTNESS_H

/* includes ---------------------------------------------------------------- */
#include "Timer.h"
#include "cie1931.h"

/* defines ----------------------------------------------------------------- */
#define BRIGHTNESS_MIN 0 // 0%
#define BRIGHTNESS_MAX 10 // 100%

/* structure --------------------------------------------------------------- */

/* class ------------------------------------------------------------------- */
class Brightness
{
	private:
		
		PwmOut m_output;
		int m_value;
	
	public:
		
		Brightness(GPIO_common GPIO_c, int freq, TIM_common PWM_c);
		void set(void);
		void set(int value);
		int* value(void);
};

#endif
