#ifndef __MCP9803_H
#define __MCP9803_H

/* includes ---------------------------------------------------------------- */
#include "I2C.h"

/* defines ----------------------------------------------------------------- */

/* class ------------------------------------------------------------------- */
class MCP9803
{
	private:
		
		I2C* m_i2c;
		char m_address;
		float m_temperature;
	
	public:
		
		MCP9803(I2C* i2cx, char address);
		float get(void);
};

#endif
