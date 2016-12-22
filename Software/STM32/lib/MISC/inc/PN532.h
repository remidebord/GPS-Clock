#ifndef __PN532_H
#define __PN532_H

/* includes ---------------------------------------------------------------- */
#include "I2C.h"

/* defines ----------------------------------------------------------------- */
#define PN532_ADDRESS 0x48
#define PN532_TIMEOUT 2
#define PN532_POLLING_TIME 100 // 100 ms

/* class ------------------------------------------------------------------- */
class PN532
{
	private:
		
		I2C* m_i2c;
		char m_timer;
		char m_state;
		char m_retry;
		
		char m_uid[10];
		char m_length;
		char m_flag;
	
	public:
		
		PN532(I2C* i2cx);
		void update(void);
		char getUID(char* uid);
		char getFirmware(void);
		char setSAM(void);
		char createFrame(char* frame, char command, char* data, char length);
		void delay(int time);
		void timer(void);
};

#endif
