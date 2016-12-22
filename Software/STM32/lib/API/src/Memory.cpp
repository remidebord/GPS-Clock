/*!
 * \file Memory.cpp
 * \brief Memory API.
 * \author Rémi.Debord
 * \version 1.1
 * \date 12 mars 2015
 *
 * Memory library (Read and write on flash memory).
 *
 * Write operation: ~22ms (with page erase), else ~120us
 * Read operation: ~2us
 *
 * How to use: Read from memory at startup, and store ALL data periodically
 *
 */

#include "Memory.h"

/*!
 *  \brief Constructor
 *
 *  Memory constructor.
 *
 *  \param base : start address
 *
 */

Memory :: Memory(unsigned int base)
{
	m_base = base;
}

/*!
 *  \brief Write
 *
 *  Write 4 bytes at specified address.
 *  Warning: these operation erase the entire page
 *
 *  \param address : 0 to 256
 *  \param data : 4 bytes
 *
 */

void Memory :: write(int address, int data)
{
	FLASH_Unlock();
	
	FLASH_ClearFlag(FLASH_FLAG_EOP|FLASH_FLAG_PGERR|FLASH_FLAG_WRPERR|FLASH_FLAG_BSY);
	
	// We can only write on erased byte !
	FLASH_ErasePage(m_base);
	
	FLASH_ProgramWord((m_base + (address * 4)), data);
	
	m_address = 4;
	
	FLASH_Lock();
}

/*!
 *  \brief Write
 *
 *  Write 4 bytes at specified address.
 *  Warning: work only on if we write on erased bytes
 *
 *  \param data : 4 bytes
 *
 */

void Memory :: write(int data)
{	
	FLASH_Unlock();
	
	FLASH_ClearFlag(FLASH_FLAG_EOP|FLASH_FLAG_PGERR|FLASH_FLAG_WRPERR|FLASH_FLAG_BSY);
	
	FLASH_ProgramWord((m_base + m_address), data);
	
	m_address += 4;
	
	FLASH_Lock();
}

/*!
 *  \brief Write
 *
 *  Write 4 bytes at specified address.
 *
 *  \param address : data address (0 to 256)
 *	\return data : 4 bytes
 */

int Memory :: read(int address)
{
	int* value = 0;
	
	value = (int*)(m_base + (address * 4));
	
	return *value;
}

/*!
 *  \brief Erase
 *
 *  Erase page.
 *
 */

void Memory :: erase(void)
{
	FLASH_Unlock();
	
	FLASH_ClearFlag(FLASH_FLAG_EOP|FLASH_FLAG_PGERR|FLASH_FLAG_WRPERR|FLASH_FLAG_BSY);
	
	FLASH_ErasePage(m_base);
	
	m_address = 0;
	
	FLASH_Lock();
}
