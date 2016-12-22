#ifndef __MEMORY_H
#define __MEMORY_H

/* includes ---------------------------------------------------------------- */
#include "stm32f0xx_flash.h"

/* defines ----------------------------------------------------------------- */
#define MEMORY_START_ADDRESS (0x0800FC00) // 0x10000 - 0x400 = 0xFC00 (64kB - 1kB)

/* class ------------------------------------------------------------------- */
class Memory
{
	private:
		
		unsigned int m_base;
		unsigned int m_address;
	
	public:
		
		Memory(unsigned int base);
		void write(int address, int data);
		void write(int data);
		int read(int address);
		void erase (void);
};

#endif
