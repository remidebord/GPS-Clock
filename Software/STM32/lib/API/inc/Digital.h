#ifndef __DIGITAL_H
#define __DIGITAL_H

/* includes ---------------------------------------------------------------- */
#include "stm32f0xx_gpio.h"
#include "Common.h"

/* class ------------------------------------------------------------------- */
class DigitalIn
{
	protected:
		
		const GPIO_common* m_gpio;
	
	public:
		
		DigitalIn(const GPIO_common* gpio);
		void mode(GPIOPuPd_TypeDef pull);				// Set input pin mode (GPIO_PuPd_UP, GPIO_PuPd_DOWN, GPIO_PuPd_NOPULL)
		uint32_t read();												// Read output state
		operator uint32_t();										// Read output state (shorthand)
};

class DigitalOut
{
	protected:
		
		const GPIO_common* m_gpio;
	
	public:
		
		DigitalOut(const GPIO_common* gpio);
		DigitalOut(const GPIO_common* gpio, GPIOOType_TypeDef type);
		void write(uint32_t value);							// Write on the output
		uint32_t read();												// Read output state
		DigitalOut& operator= (uint32_t value);	// Write on the output (shorthand)
		operator uint32_t();										// Read output state (shorthand)
};

class InterruptIn : public DigitalIn
{
	protected:
	
	public:
		
		InterruptIn(const GPIO_common* gpio);
		void rise(void(*f)(void));
		void fall(void(*f)(void));
		void risefall(void(*f)(void));
};

#endif
