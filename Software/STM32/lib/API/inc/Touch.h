#ifndef __TOUCH_H
#define __TOUCH_H

/* includes ---------------------------------------------------------------- */
#include "stm32f0xx_gpio.h"
#include "stm32f0xx_rcc.h"
#include "Common.h"

/* defines ----------------------------------------------------------------- */
#define TSLPRM_TSC_CTPH 	(1) // 1: 2 x tPGCLK | Charge Transfer Pulse High (range=0..15)
#define TSLPRM_TSC_CTPL 	(1) // 1: 2 x tPGCLK | Charge Transfer Pulse Low (range=0..15)
#define TSLPRM_TSC_PGPSC 	(5) // 1: fPGCLK = fHCLK/32 | Pulse Generator Prescaler (range=0..7)

#define TSLPRM_TSC_IODEF 		(0)	// 0: Output push-pull low | TSC IOs default mode when no on-going acquisition (range=0..1)
#define TSLPRM_TSC_AM 			(0) // 0: Normal acquisition mode | Acquisition Mode (range=0..1)
#define TSLPRM_TSC_SYNC_PIN (0) // 0: PB08 | Synchronization Pin (range=0..1)
#define TSLPRM_TSC_SYNC_POL (0) // 0: Falling edge only | Synchronization Polarity (range=0..1)

#define TSLPRM_TSC_USE_SS (0) //Use Spread Spectrum (0=No, 1=Yes)
#define TSLPRM_TSC_SSD 		(0) // 0: 1 x tSSCLK | Spread Spectrum Deviation (range=0..127)
#define TSLPRM_TSC_SSPSC 	(0) // 0: fSSCLK = fHCLK | Spread Spectrum Prescaler (range=0..1)

#define TSLPRM_TSC_MCV 6 // 16383 | Acquisition measurement (range=255, 511, 1023, 2047, 8191, 16383)

#define TOUCH_RATIO 0.65

/* class ------------------------------------------------------------------- */
class Touch
{
	private:

		GPIO_TypeDef* m_touch_port;
		int m_touch_pin;
		int m_touch_pinSource;
	
		GPIO_TypeDef* m_sampling_port;
		int m_sampling_pin;
		int m_sampling_pinSource;
	
		int* m_value;
		short int m_threshold;
		static char tsc_flag;
	
	public:		
	
		Touch(GPIO_common GPIO_c_touch_pin, GPIO_common GPIO_c_sampling_pin);
		void calibrate(void);
		int get_value(void);
		char pressed(void);
		void set_auto_acq(char value);
		void start_acq(void);
	
};

#endif
