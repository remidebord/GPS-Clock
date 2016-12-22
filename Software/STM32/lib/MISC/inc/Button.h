#ifndef __BUTTON_H
#define __BUTTON_H

/* includes ---------------------------------------------------------------- */
#include "Digital.h"

/* defines ----------------------------------------------------------------- */
#define SHORT_PRESS 			0x01
#define LONG_PRESS				0x02
#define CONTINUOUS_PRESS	0x03

#define WAIT_PRESS 				0x01
#define WAIT_RELEASE 			0x02
#define WAIT_CONTINUOUS 	0x03

#define DEBOUNCE_TIME					50		// 50 ms
#define LONG_PRESS_TIME 			900		// 900 ms
#define CONTINUOUS_PRESS_TIME 1000	// 1000 ms

/* class ------------------------------------------------------------------- */
class Button
{
	private:
		
		DigitalIn m_button;
		char m_value;
		char m_state;
		int m_timer;
	
	public:
		
		Button(GPIO_common pin);
		void update(void);
		char get(void);
		void timer(void);
};

#endif
