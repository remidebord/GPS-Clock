#include "Average.h"
#include <stdlib.h>

Average :: Average(int maxSamples)
{
	// Dynamic allocation (with memory init to 0 != malloc) 
	m_values = (int*) calloc(maxSamples, sizeof(int));
	
	m_maxSamples = maxSamples;
	m_samples = 0;
	m_maximum = 0;
}

void Average :: insert(int value)
{
	if((m_config & FILL_ARRAY_AT_FIRST_INSERT) != 0)
	{
		m_config = m_config & (~FILL_ARRAY_AT_FIRST_INSERT);
		this->fill_array(value);
	}
	else
	{		
		m_values[m_samples] = value;
		m_samples = (m_samples + 1) % m_maxSamples;
	}
}

int Average :: get(void)
{
	int average = 0;
	int i = 0;
	
	for(i = 0; i < m_maxSamples; i++)
	{
		average += m_values[i];
	}
	
	average = average / m_maxSamples;
	
	return average;
}

void Average :: fill_array_at_first_insert(void)
{
	m_config = m_config | FILL_ARRAY_AT_FIRST_INSERT;
}

void Average :: fill_array(int value)
{
	int i;
	
	for(i = 0; i < m_maxSamples; i++)
	{
		m_values[i] = value;
	}
}

void Average :: clear(void)
{
	this->fill_array(0);
}

int Average :: size(void)
{
	return m_maxSamples;
}

int Average :: get_maximum(void)
{
	int maximum = 0;
	
	for(int i = 0; i < m_maxSamples; i++)
	{
		if(maximum < m_values[i])
		{
			maximum = m_values[i];
		}
	}
	
	return maximum;
}

int Average :: get_minimum(void)
{
	int minimum = 0x7FFFFFFF;
	
	for(int i = 0; i < m_maxSamples; i++)
	{
		if(minimum > m_values[i])
		{
			minimum = m_values[i];
		}
	}
	
	return minimum;
}

Average :: ~Average(void)
{
	free(m_values);
}
