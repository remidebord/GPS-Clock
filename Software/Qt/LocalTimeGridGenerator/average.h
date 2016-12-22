#ifndef __AVERAGE_H
#define __AVERAGE_H

class Average
{
	private:
		
		//int m_values[AVERAGE_MAX_SAMPLES];
		int *m_values;	
		int m_maxSamples;
		int m_samples;
	
	public:
		
		Average(int maxSamples);
		void insert(int value);
		int get(void);
		~Average(void);
	
};

#endif /* __AVERAGE_H */
