#ifndef __TIMEZONE_H
#define __TIMEZONE_H

/* includes ---------------------------------------------------------------- */
#include "RTC.h"
#include "GPS.h"

/* defines ----------------------------------------------------------------- */
#define PI (3.1415926536)
#define EARTH_RADIUS 6371000 // meters
#define TO_RAD (PI / 180)

#define TIMEZONE_IDLE			0x00
#define DATABASE_OFFSET 	0x01
#define LONGITUDE_OFFSET 	0x02

/* structure --------------------------------------------------------------- */
struct TimezoneInfo
{
	float latitude;
	float longitude;
	int radius;
	int offset;
};

/* class ------------------------------------------------------------------- */
class Timezone
{
	private:
		
		Coordinate m_coord;
		float m_offset;
		int m_size;
		int m_inc;
		char m_state;
	
	public:
		
		Timezone(void);
		void update(void);
		char offset(Time *t, Coordinate coord);
		double haversine(double th1, double ph1, double th2, double ph2);
		float round(float value);
};

#endif
