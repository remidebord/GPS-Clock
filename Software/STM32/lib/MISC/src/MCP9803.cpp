/*!
 * \file MCP9803.cpp
 * \brief MCP9803 I2C temperature sensor API.
 * \author Rémi.Debord
 * \version 1.0
 * \date 01 juillet 2015
 *
 * MCP9803 I2C temperature sensor library.
 *
 */

#include "MCP9803.h"

/*!
 *  \brief Constructor
 *
 *  MCP9803 constructor.
 *
 *  \param i2cx : I2C object address
 *
 */

MCP9803 :: MCP9803(I2C* i2cx, char address)
{
	char buffer[8] = {0};
	
	m_i2c = i2cx;
	m_address = address;
	
	buffer[0] = 0x01;		// Configuration register (0x00)
	buffer[1] = 0x60;		// ADC resolution 12 bits	
	
	m_i2c->write_b(m_address, buffer, 2);	
}

/*!
 *  \brief Get
 *
 *  Get temperature (Celsius degrees).
 * 
 *  \return temperature
 */

float MCP9803 :: get(void)
{
		char buffer[8] = {0};
	
		buffer[0] = 0x00;	// Temperature register (0x00)
		
		m_i2c->write_b(m_address, buffer, 1);
		m_i2c->read(m_address, buffer, 2);
		
		// Conversion
    m_temperature = buffer[0];
    m_temperature += ((buffer[1] & 0x80) >> 7)*0.5;
    m_temperature += ((buffer[1] & 0x40) >> 6)*0.25;
    m_temperature += ((buffer[1] & 0x20) >> 5)*0.125;
    m_temperature += ((buffer[1] & 0x10) >> 4)*0.0625;
		
		return m_temperature;
}

