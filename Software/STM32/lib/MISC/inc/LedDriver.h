#ifndef __LEDDRIVER_H
#define __LEDDRIVER_H

/* includes ---------------------------------------------------------------- */
#include "I2C.h"

/* defines ----------------------------------------------------------------- */
#define PWM_OUTPUT 16
#define RGB_OUTPUT (PWM_OUTPUT/3)

#define ALLCALL_ADRESS 0xE0

/* class ------------------------------------------------------------------- */
class LedDriver
{
	private:
		
		I2C* m_i2c;
		char m_address;
		char m_led[RGB_OUTPUT][3];	
	
	public:
		
		LedDriver(I2C* i2cx, char address);
		void init(void);
		void setColor(char red, char green, char blue);
		void setColorAll(char red, char green, char blue);
		void setLedColor(int led, char red, char green, char blue);
		void setLedColor2(int led, char red, char green, char blue);
		void update(void);
		void updateAll(void);
		void setOutput(char out, char value);
};

#endif
