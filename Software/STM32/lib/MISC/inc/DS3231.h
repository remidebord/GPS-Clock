#ifndef __DS3231_H
#define __DS3231_H

/* includes ---------------------------------------------------------------- */
#include "I2C.h"
#include "RTC.h"

/* defines ----------------------------------------------------------------- */
#define ADDR_SECONDS 0x00
#define ADDR_MINUTES 0x01
#define ADDR_HOURS	 0x02
#define ADDR_DAY		 0x03
#define ADDR_DATE		 0x04
#define ADDR_MONTH	 0x05
#define ADDR_YEAR		 0x06

#define ADDR_CONTROL 0x0E
#define ADDR_STATUS	 0x0F

#define ADDR_TEMP		 0x11

/* class ------------------------------------------------------------------- */
class DS3231
{
	private:
		
		I2C* m_i2c;
		uint8_t m_address;	
		uint8_t m_buffer[6];
	
	public:
		
		DS3231(I2C* i2cx);
		void set(Time time);
		void set(Date date);
		char get(Time *time);
		char get(Date *date);
		float temperature(void);
	
		static int dow(int year, int month, int day);
};

#endif
