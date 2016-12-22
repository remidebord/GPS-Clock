#ifndef __RTC_H
#define __RTC_H

/* includes ---------------------------------------------------------------- */
#include "stm32f0xx_rtc.h"

/* defines ----------------------------------------------------------------- */
#define RTC_LSI 0x01
#define RTC_LSE 0x02

#define BKP_VALUE 0x32F0 

/* structures -------------------------------------------------------------- */
struct Time
{
	int hours;
	int minutes;
	int seconds;
};

struct Date
{
	int year;
	char month;
	char day;
};

/* class ------------------------------------------------------------------- */
class CLOCK
{
	private:
	
	public:
		
		CLOCK(char type = RTC_LSI);
		void set(Time time);
		void set(Date date);
		void get(Time *time);
		void get(Date *date);
		void alarm(int seconds, void(*f)(void));
	
		static int dow(int year, int month, int day);
};

#endif
