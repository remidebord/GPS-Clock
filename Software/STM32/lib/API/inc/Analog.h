#ifndef __ANALOG_H
#define __ANALOG_H

/* includes ---------------------------------------------------------------- */
#include "stm32f0xx_adc.h"
#include "Common.h"

/* defines ----------------------------------------------------------------- */
#define ADC_MAX_INPUT 19

/* private functions ------------------------------------------------------- */
int GetChannel(GPIO_TypeDef* port, int pin);
int GetIndex(int x);

/* class ------------------------------------------------------------------- */
class AnalogIn
{
	private:
		
		ADC_TypeDef* m_adc;
		GPIO_TypeDef* m_port;
		int m_pin;
		int m_channel;
		int m_value;
	
	public:
		
		AnalogIn(GPIO_common GPIO_c);
		int read();												// Read (conversion)
		operator int();										// Read (no conversion)
};

class AnalogIn_DMA
{
	private:
		
		GPIO_TypeDef* m_port;
		int m_pin;
		int m_index;
		int m_channel;
		__IO uint16_t m_value;
	
	public:
		
		static ADC_TypeDef* m_adc[ADC_MAX_INPUT];
		static int m_channels[ADC_MAX_INPUT];
		static int m_rank[ADC_MAX_INPUT];
		static int m_inc;
	
		AnalogIn_DMA(GPIO_common GPIO_c);
		int read_b();											// Read (conversion)
		int read();												// Read (no conversion)
		operator int();										// Read (no conversion)
};

class AnalogOut
{
	private:
		
		GPIO_TypeDef* m_port;
		int m_pin;
	
	public:
		
		AnalogOut(GPIO_common GPIO_c);
		void write(int n);							  // Write
		AnalogOut& operator= (int n);			// Write shorhand
		int read();												// Read
		operator int();					// Read shorthand
};

#endif
