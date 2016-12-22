#ifndef __GPS_H
#define __GPS_H

/* includes ---------------------------------------------------------------- */
#include "Serial.h"
#include "RTC.h"

/* defines ----------------------------------------------------------------- */
#define KNOT 1,852 // km/h

#define KMH 	1
#define SMPH 	2

#define NODATA_DELAY 30000 // 30sec

/* structure --------------------------------------------------------------- */
struct Coordinate
{
	float latitude;
	float longitude;
};

/* class ------------------------------------------------------------------- */
class GPS
{
	private:
		
		Serial* m_serial;
		Time m_time;
		Date m_date;
		Coordinate m_position;
		char m_fix;
		int m_satellite;
		float m_altitude;
		float m_speed;
		float m_track; // heading
		char m_valid;
	
		int m_timeout;
	
	public:
		
		GPS(Serial* serialx);
		void update(void);
		char time(Time *t);
		char date(Date *d);
		void position(Coordinate *p);
		char fix(void);
		int satellite(void);
		float altitude(void);
		float speed(char units = KMH);
		float track(void);
		char valid(void);
		float distance(Coordinate c1, Coordinate c2);
	
		char calculate_checksum(char* sentence);
		char get_checksum(char* sentence);
		char get_field(char *in, char field, char* out);
		char get_sentence(char *in, char* sentence, char* out);
};

#endif
