/*!
 * \file SevenSeg.cpp
 * \brief Seven segment API.
 * \author Rémi.Debord
 * \version 1.0
 * \date 13 juin 2015
 *
 * Seven segment library.
 *
 */

#include "SevenSeg.h"
#include "AsciiMap.h"

#include <stdio.h>
#include <string.h>

/*!
 *  \brief Constructor
 *
 *  SevenSeg constructor.
 *
 */

SevenSeg :: SevenSeg(DigitalOut* segment, DigitalOut* digit)
{
	char i = 0;
	
	for(i = 0; i < MAX_SEGMENTS; i++)
	{
		m_segment[i] = segment++;
	}
	
	m_digit = digit;
}

/*!
 *  \brief Segments
 *
 *  Set segments value.
 *
 *  (msb) dot g f e d c b a (lsb)
 *
 *  \param c : segments values
 */

void SevenSeg :: segments(char c)
{
	char i = 0;
	char value = 0;
	
	for(i = 0; i < MAX_SEGMENTS; i++)
	{
		value = ((c & (0x01 << i)) > 0) ? 1 : 0;
			
		m_segment[i]->write(value);
	}
}

/*!
 *  \brief Set
 *
 *  Set segments from an ASCII characters.
 *
 *  \param c : ASCII character
 */

void SevenSeg :: set(char c)
{	
	if(c < 0x7F)
	{
		this->segments(digitAsciiMap[c]);
	}
}

/*!
 *  \brief Set
 *
 *  Set segments from an integer value.
 *
 *  \param number : integer
 */

void SevenSeg :: set(int number)
{
	this->set((char) ((number%10)+0x30));
}

/*!
 *  \brief On
 *
 *  Switch the digit ON.
 *
 */

void SevenSeg :: on(void)
{
	m_digit->write(0);
}

/*!
 *  \brief Off
 *
 *  Switch the digit OFF.
 *
 */

void SevenSeg :: off(void)
{
	m_digit->write(1);
}

/*!
 *  \brief Constructor
 *
 *  SevenSeg constructor.
 *
 */

SevenSegGroup :: SevenSegGroup(void)
{
	// Init value(s)
	m_size = 0;
	m_inc = 0;
	m_timer = 0;
}

/*!
 *  \brief Update
 *
 *  Manage digits display.
 *
 */

void SevenSegGroup :: update(void)
{	
	char i = 0;
	
	// Refresh ?
	if(m_timer == 0)
	{
		m_timer = m_mux;
		
		// SevenSeg object inserted ?
		if(m_size > 0)
		{
			// Special effects using segments ?
			if(m_effect.value)
			{
				m_digit[m_inc]->segments(m_buffer[m_inc]);
			}
			// Display ASCII characters ?
			else
			{
				m_digit[m_inc]->set(m_buffer[m_inc]);
			}
			
			// Switch digits (Only 1 turn ON)
			for(i = 0; i < m_size; i++)
			{
				// Digit to switch ON ?
				if(i == m_inc)
				{
					// Digit selected for blinking ? and blink effect activated ?
					if(((m_blink.select & (0x01 << i)) != 0) && (m_blink.flag))
					{
						m_digit[m_inc]->set((char)0x00);
					}
					
					m_digit[i]->on();
				}
				// Switch OFF others digits
				else
				{
					m_digit[i]->off();
				}
			}
			
			m_inc = (m_inc + 1) % m_size;
		}
	}
	
	// Blink ?
	if((m_blink.timer == 0) && m_blink.value)
	{
		m_blink.timer = m_blink.value;
		
		m_blink.flag = (m_blink.flag) ? 0 : 1;
	}
	
	// Scroll ?
	if((m_scroll.timer == 0) && m_scroll.value)
	{
		m_scroll.timer = m_scroll.value;
		
		for(i = 0; i < m_size; i++)
		{			
			m_tmp[i] = m_buffer[(i+1)%m_size];
		}
		
		for(i = 0; i < m_size; i++)
		{			
			m_buffer[i] = m_tmp[i];
		}
	}
	
	// Loading ?
	if((m_effect.timer == 0) && m_effect.value)
	{
		m_effect.timer = m_effect.value;
		
		m_effect.flag = (m_effect.flag + 1) % m_effect.length;
		
		for(i = 0; i < m_size; i++)
		{
			m_buffer[i] = m_effect.buffer[(m_effect.flag+(m_effect.shift*i))%m_effect.length];
		}
	}
}

/*!
 *  \brief Add
 *
 *  Add a digit (seven segment) to the group.
 *
 *  \param digit: Seven segment object
 */

void SevenSegGroup :: add(SevenSeg* digit)
{
	m_digit[m_size++] = digit;
	
	m_mux = REFRESH_TIME / m_size;
}

/*!
 *  \brief Set (string)
 *
 *  Set an ASCII string to display.
 *
 *  \param buffer: ASCII string
 *  \param length: ASCII string length
 */

void SevenSegGroup :: set(char* buffer, int length)
{
	char i = 0;
	
	length = (length > m_size) ? m_size : length;
	
	for(i = 0; i < length; i++)
	{
		m_buffer[i] = buffer[i];
	}
}

/*!
 *  \brief Set (number)
 *
 *  Set a number to display.
 *
 *  \param number
 */

void SevenSegGroup :: set(int number)
{
	char buffer[MAX_DIGITS] = {0};
	
	sprintf(buffer, "%%0%dd", m_size);
	
	sprintf(m_buffer, buffer, number);
}

/*!
 *  \brief Blink
 *
 *  All the seven segment in the group blink at the specified delay.
 *
 *  \param ms : blink delay (ms)
 */

void SevenSegGroup :: blink(int ms)
{
	if(m_blink.value != ms)
	{
		m_blink.select = 0xFFFF;
		
		m_blink.value = (ms <= 0) ? 0 : ms;
		m_blink.flag = 0;
		
		m_blink.timer = m_blink.value;
	}
}

/*!
 *  \brief Blink
 *
 *  Select one seven segment to blink at the specified delay.
 *
 *  \param select : 0 to n seven segment
 *  \param ms : blink delay (ms)
 */

void SevenSegGroup :: blink(char select, int ms)
{
	m_blink.select |= 0x01 << select;
	
	if(m_blink.value != ms)
	{		
		m_blink.value = (ms <= 0) ? 0 : ms;
		m_blink.flag = 0;
		
		m_blink.timer = m_blink.value;
	}
}

/*!
 *  \brief Scroll
 *
 *  Scroll text displayed on the seven segments (right to left)
 *
 *  \param ms : scroll delay (ms)
 */

void SevenSegGroup :: scroll(int ms)
{
	if(m_scroll.value != ms)
	{
		m_scroll.value = (ms <= 0) ? 0 : ms;
		m_scroll.flag = 0;
		
		m_scroll.timer = m_scroll.value;
	}
}

/*!
 *  \brief Effect
 *
 *  User specified effect (address directly the segments).
 *
 *  \param buffer : array with special value
 *  \param length : array length
 *  \param shift : shift
 *  \param ms : delay
 */

void SevenSegGroup :: effect(char* buffer, int length, int shift, int ms)
{
	if(m_effect.value != ms)
	{
		m_effect.buffer = buffer;
		m_effect.length = length;
		m_effect.shift = shift;
		
		m_effect.value = (ms <= 0) ? 0 : ms;
		m_effect.flag = 0;
		
		m_effect.timer = m_effect.value;
	}
}

/*!
 *  \brief Clear
 *
 *  Disable any effect.
 *
 */

void SevenSegGroup :: clear(void)
{
	m_blink.value = 0;
	m_blink.select = 0;
	
	m_scroll.value = 0;
	m_effect.value = 0;
}

/*!
 *  \brief Manage SevenSegGroup base time 
 *
 *  Note: Place it into a ticker callback of 1 ms
 *
 */

void SevenSegGroup :: timer(void)
{
	if(m_timer) m_timer--;
	
	if(m_blink.timer) m_blink.timer--;
	if(m_scroll.timer) m_scroll.timer--;
	if(m_effect.timer) m_effect.timer--;
}
