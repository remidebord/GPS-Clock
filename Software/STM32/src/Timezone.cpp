#include "Timezone.h"
#include <math.h>

const TimezoneInfo fra_01 = {46.135629, 2.703838, 492871, 1};

const TimezoneInfo database[] = {fra_01};

Timezone :: Timezone(void)
{
	m_size = sizeof(database)/sizeof(TimezoneInfo);
	m_state = 0;
	m_inc = 0;
}

char Timezone :: offset(Time *t, Coordinate coord)
{
	double distance = 0;
	
	switch(m_state)
	{
		case 0:
			m_coord.latitude = coord.latitude;
			m_coord.longitude = coord.longitude;
			m_inc = 0;
			m_state = 1;
			break;
		
		case 1:
			if(m_inc < m_size)
			{
				distance = this->haversine(m_coord.latitude, m_coord.longitude, database[m_inc].latitude, database[m_inc].longitude);
				
				if(distance <= database[m_inc].radius)
				{
					m_offset = database[m_inc].offset;
					t->hours += m_offset;
					
					m_inc = 0;
					m_state = 0;
					return 1;
				}
				
				m_inc++;
			}
			else
			{
				m_state = 2;
			}
			break;
			
		case 2:
			m_offset = this->round((m_coord.longitude / 7.5) / 2);
			t->hours += m_offset;
			
			m_inc = 0;
			m_state = 0;
			return 1;
			break;
	}
	
	return 0;
}

double Timezone :: haversine(double th1, double ph1, double th2, double ph2)
{
	double dx, dy, dz;
	ph1 -= ph2;
	ph1 *= TO_RAD, th1 *= TO_RAD, th2 *= TO_RAD;
 
	dz = sin(th1) - sin(th2);
	dx = cos(ph1) * cos(th1) - cos(th2);
	dy = sin(ph1) * cos(th1);
	
	return asin(sqrt(dx * dx + dy * dy + dz * dz) / 2) * 2 * EARTH_RADIUS;
}

float Timezone :: round(float value)
{
	int number = value * 10;
	int divisor = 10;
	
	int half = divisor / 2;
	float result = 0;
	
	if(number < 0) half = -half;
	
	result = ((number + half) - ((number + half) % divisor));
	
	return (result/10);
}
