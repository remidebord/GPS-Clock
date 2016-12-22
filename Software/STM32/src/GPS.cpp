#include "GPS.h"
#include <stdio.h>
#include <string.h>

GPS :: GPS(Serial* serialx)
{
	m_serial = serialx;
}

void GPS :: update(void)
{
	char buffer[MAX_USART_RX_BUF_LEN] = {};
	char sentence[128] = {};
	char field[16] = {};
	int length;
		
	int timestamp = 0;
	int date = 0;
		
	length = m_serial->read(buffer);
	
	// Data received ?
	if(length > 0)
	{
		// Parse GPGGA
		if(this->get_sentence(buffer, (char*)"$GPGGA", sentence))
		{
			// Reload timeout
			m_timeout = NODATA_DELAY;
			
			// Verify checksum
			if(this->get_checksum(sentence) == this->calculate_checksum(sentence))
			{
				// Get time
				if(this->get_field(sentence, 1, field))
				{
					if(sscanf(field,"%6d", &timestamp) >= 0)
					{
						m_time.hours = timestamp / 10000;
						m_time.minutes = (timestamp / 100) % 100;
						m_time.seconds = timestamp % 100;
					}
				}
				
				// Get latitude
				if(this->get_field(sentence, 2, field))
				{
					if(sscanf(field,"%f", &m_position.latitude) <= 0)
					{
						m_position.latitude = 0;
					}
					else
					{
						// Convert DD.MM to DD 
						m_position.latitude = ((int)m_position.latitude / 100) + ((((int)m_position.latitude % 100) + (m_position.latitude - (int)m_position.latitude)) / 60);
					}
				}
				
				// North or south
				if(this->get_field(sentence, 3, field))
				{
					m_position.latitude = (field[0] == 'N') ? m_position.latitude : -m_position.latitude;
				}
				
				// Get longitude
				if(this->get_field(sentence, 4, field))
				{
					if(sscanf(field,"%f", &m_position.longitude) <= 0)
					{
						m_position.longitude = 0;
					}
					else
					{
						// Convert DD.MM to DD 
						m_position.longitude = ((int)m_position.longitude / 100) + ((((int)m_position.longitude % 100) + (m_position.longitude - (int)m_position.longitude)) / 60);
					}
				}
				
				// East or west
				if(this->get_field(sentence, 5, field))
				{
					m_position.longitude = (field[0] == 'E') ? m_position.longitude : -m_position.longitude;
				}
				
				// Get fix
				if(this->get_field(sentence, 6, field))
				{
					m_fix = field[0] - 0x30;
				}
				
				// Get satellite
				if(this->get_field(sentence, 7, field))
				{
					if(sscanf(field,"%2d", &m_satellite) <= 0)
					{
						m_satellite = 0;
					}
				}
				
				// Get altitude
				if(this->get_field(sentence, 9, field))
				{
					if(sscanf(field,"%f", &m_altitude) <= 0)
					{
						m_altitude = 0;
					}
				}
			}
		}
		
		// Parse GPRMC
		if(this->get_sentence(buffer, (char*)"$GPRMC", sentence))
		{
			// Reload timeout
			m_timeout = NODATA_DELAY;
			
			// Verify checksum
			if(this->get_checksum(sentence) == this->calculate_checksum(sentence))
			{
				// Get navigation state (A = ok, V = warning) 
				if(this->get_field(sentence, 2, field))
				{
					m_valid = field[0];
				}
				
				// Get speed (knots)
				if(this->get_field(sentence, 7, field))
				{
					if(sscanf(field,"%f", &m_speed) <= 0)
					{
						m_speed = 0;
					}
				}
				
				// Get track (heading)
				if(this->get_field(sentence, 8, field))
				{
					if(sscanf(field,"%f", &m_track) <= 0)
					{
						m_track = 0;
					}
				}
				
				// Get date
				if(this->get_field(sentence, 9, field))
				{
					if(sscanf(field,"%6d", &date) <= 0)
					{
						m_date.day = date / 10000;
						m_date.month = (date / 100) % 100;
						m_date.year = date % 100;
					}
				}
			}
		}
	}
	
	// Clear all values
	if(m_timeout == 0)
	{
		m_time.hours = 0;
		m_time.minutes = 0;
		m_time.seconds = 0;
		
		m_date.day = 0;
		m_date.month = 0;
		m_date.year = 0;
		
		m_position.latitude = 0;
		m_position.longitude = 0;
		
		m_fix = 0;
		m_satellite = 0;
		m_altitude = 0;
		m_speed = 0;
		m_track = 0; // heading
		m_valid = 0;
	}
}

char GPS :: time(Time *t)
{
	if((m_time.hours != 0) && (m_time.minutes != 0) && (m_time.seconds != 0))
	{
		t->hours = m_time.hours;
		t->minutes = m_time.minutes;
		t->seconds = m_time.seconds;
		
		return 1;
	}
	
	return 0;
}

char GPS :: date(Date *d)
{
	if((m_date.day != 0) && (m_date.month != 0) && (m_date.year != 0))
	{
		d->day = m_date.day;
		d->month = m_date.month;
		d->year = m_date.year;
		
		return 1;
	}
	
	return 0;
}

void GPS :: position(Coordinate *p)
{
	p->latitude = m_position.latitude;
	p->longitude = m_position.longitude;
}

char GPS :: fix(void)
{
	return m_fix;
}

int GPS :: satellite(void)
{
	return m_satellite;
}

float GPS :: altitude(void)
{
	return m_altitude;
}

float GPS :: speed(char units)
{
	float speed = 0;
	
	if(units == KMH)
	{
		speed = m_speed * KNOT;
	}
	else if(units == SMPH)
	{
		speed = m_speed;
	}
	
	return speed;
}

float GPS :: track(void)
{
	return m_track;
}

char GPS :: valid(void)
{
	if(m_valid == 'A')
	{
		return 1;
	}
	else return 0;
}

char GPS :: calculate_checksum(char* sentence)
{
	char checksum = 0;
	char* ptr1 = sentence;
	char* ptr2;
	char length, i;
	
	// Search '*'
	ptr2 = strchr(sentence, '*');
	
	if(ptr2 != NULL)
	{
		length = ptr2 - ptr1;
		
		// Calculate checksum (all characters between $ and *) 
		for(i = 1; i < length; i++)
		{
			checksum ^= sentence[i];
		}
	}
	
	return checksum;	
}

char GPS :: get_checksum(char* sentence)
{
	char checksum = 0;
	char* ptr;
	
	ptr = strchr(sentence, '*');
	
	if(ptr != NULL)
	{
		// Convert characters into hexa values
		checksum |= (ptr[1] >= 'A') ? (ptr[1] - 0x37) << 4 : (ptr[1] - 0x30) << 4;
		checksum |= (ptr[2] >= 'A') ? (ptr[2] - 0x37) : (ptr[2] - 0x30);
	}
	
	return checksum;	
}

char GPS :: get_field(char *in, char field, char* out)
{
	int length, i;
	char* ptr1 = in;
	char* ptr2;
	
	// Pass undesired field
	for(i = 0; i < field; i++)
	{
		ptr1 = strchr(ptr1+1, ',');
	}
	
	// ',' exist ?
	if(ptr1 != NULL)
	{
		// Search second ','
		ptr2 = strchr(ptr1+1, ',');

		// ',' exist ?
		if(ptr2 != NULL)
		{
			length = ptr2 - ptr1;
			
			// Copy field
			strncpy(out, ptr1+1, length-1);
			
			return length-1;
		}
	}
	
	return 0;
}

char GPS :: get_sentence(char *in, char* sentence, char* out)
{
	int length;
	char* ptr1;
	char* ptr2;
	
	// Get sub string
	ptr1 = strstr(in, sentence);
	
	// Sub string exist ?
	if(ptr1 != NULL)
	{
		// Search CR
		ptr2 = strchr(ptr1, 0x0D);
		
		// CR exist ?
		if(ptr2 != NULL)
		{
			length = ptr2 - ptr1;
			
			// Copy sub string
			strncpy(out, ptr1, length);
			
			return length;
		}
	}
	
	return 0;
}
