#include "average.h"
#include <stdlib.h>

Average :: Average(int maxSamples)
{
	// Dynamic allocation (with memory init to 0 != malloc) 
	m_values = (int*) calloc(maxSamples, sizeof(int));
	
	m_maxSamples = maxSamples;
	m_samples = 0;
}

void Average :: insert(int value)
{
    m_values[m_samples] = value;
    m_samples = (m_samples + 1) % m_maxSamples;
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

Average :: ~Average(void)
{
	free(m_values);
}
