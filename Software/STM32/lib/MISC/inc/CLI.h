#ifndef __CLI_H
#define __CLI_H

/* includes ---------------------------------------------------------------- */
#include "Serial.h"

/* defines ----------------------------------------------------------------- */
#define CLI_INIT 0
#define CLI_IDLE 1

#define CLI_BUFFER_SIZE 32
#define CLI_BUFFER_NUMBER 3

/* class ------------------------------------------------------------------- */
class CLI
{
	private:
		
		Serial* m_serial;
		char m_state;
	
		char m_clear[CLI_BUFFER_SIZE+2];
		char m_buffer[3][CLI_BUFFER_SIZE];
		char m_index;
	
	public:
		
		CLI(Serial* serialx);
		void update(void);
};

#endif
