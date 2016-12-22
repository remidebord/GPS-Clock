/*!
 * \file DS3231.cpp
 * \brief DS3231 I2C RTC API.
 * \author Rémi.Debord
 * \version 1.0
 * \date 26 juin 2015
 *
 * DS3231 High Precision RTC library.
 *
 */

#include "DS3231.h"

/*!
 *  \brief Constructor
 *
 *  I2C constructor.
 *
 *  \param i2cx : I2C object address
 *
 */

DS3231 :: DS3231(I2C* i2cx)
{
	m_i2c = i2cx;
	m_address = 0xD0;
	
	m_buffer[0] = ADDR_HOURS;
	
	// Get hour mode (12 or 24h)
	m_i2c->write_b(m_address, m_buffer, 1);
	m_i2c->read(m_address, m_buffer, 1);
	
	// 12 hour mode selected ?
	if((m_buffer[0] & 0x80) == 0)
	{
		// Go to 24 hour mode
		m_buffer[0] |= 0x80;
		m_i2c->write_b(m_address, m_buffer, 1);
	}
}

/*!
 *  \brief Set
 *
 *  Set time (hours, minutes, seconds).
 * 
 *  \param time : time structure
 */

void DS3231 :: set(Time time)
{
	m_buffer[0] = ADDR_SECONDS;
	m_buffer[1] = ((time.seconds / 10) << 4) + (time.seconds % 10);
	m_buffer[2] = ((time.minutes / 10) << 4) + (time.minutes % 10);
	m_buffer[3] = ((time.hours / 10) << 4) + (time.hours % 10);
	
	// 24h mode
	m_buffer[3] |= 0x80;
	
	m_i2c->write_b(m_address, m_buffer, 4);
}

/*!
 *  \brief Set
 *
 *  Set date (year, month, day).
 * 
 *  \param date : date structure
 */

void DS3231 :: set(Date date)
{
	date.year = date.year % 100;
	
	m_buffer[0] = ADDR_DATE;
	m_buffer[1] = ((date.day / 10) << 4) + (date.day % 10);
	m_buffer[2] = ((date.month / 10) << 4) + (date.month % 10);
	m_buffer[3] = ((date.year / 10) << 4) + (date.year % 10);
	
	m_i2c->write_b(m_address, m_buffer, 4);
}

/*!
 *  \brief Get
 *
 *  Get time (hours, minutes, seconds).
 * 
 *  \param time : time structure pointer
 */

char DS3231 :: get(Time *time)
{
	char result = 0;
	
	m_buffer[0] = ADDR_SECONDS;
	
	result = m_i2c->write_b(m_address, m_buffer, 1);

	result = m_i2c->read(m_address, m_buffer, 3);
	
	time->seconds = ((m_buffer[0] >> 4) * 10) + (m_buffer[0] & 0x0F);
	time->minutes = ((m_buffer[1] >> 4) * 10) + (m_buffer[1] & 0x0F);
	time->hours = (((m_buffer[2] & 0x30) >> 4) * 10) + (m_buffer[2] & 0x0F);
	
	return result;
}

/*!
 *  \brief Get
 *
 *  Get date (year, month, day).
 * 
 *  \param date : date structure pointer
 */

char DS3231 :: get(Date *date)
{
	char result = 0;
	
	m_buffer[0] = ADDR_DATE;
	
	result = m_i2c->write_b(m_address, m_buffer, 1);

	result = m_i2c->read(m_address, m_buffer, 3);
	
	date->day = ((m_buffer[0] >> 4) * 10) + (m_buffer[0] & 0x0F);
	date->month = (((m_buffer[1] & 0x30) >> 4) * 10) + (m_buffer[1] & 0x0F);
	date->year = ((m_buffer[2] >> 4) * 10) + (m_buffer[2] & 0x0F);
	
	date->year += 2000;
	
	return result;
}

/*!
 *  \brief Get temperature
 *
 *  Get temperature.
 * 
 *  \param date : date structure pointer
 */

float DS3231 :: temperature(void)
{
	m_buffer[0] = ADDR_TEMP;
	
	m_i2c->write_b(m_address, m_buffer, 1);

	m_i2c->read(m_address, m_buffer, 2);
	
	return (float)(m_buffer[0] + (0.25 * (m_buffer[1] >> 6)));
}

/*!
 *  \brief dow
 *
 *  Get day of the week (0 = Sunday, 1 = Monday, ...).
 * 
 *  \param year : year (20XX)
 *  \param month : month
 *  \param day : day
 *  \return day of the week [0:6]
 */

int DS3231 :: dow (int year, int month, int day)
{
	static int t[] = {0, 3, 2, 5, 0, 3, 5, 1, 4, 6, 2, 4};
	
	year -= month < 3;

	return (year + year/4 - year/100 + year/400 + t[month-1] + day) % 7;
}

