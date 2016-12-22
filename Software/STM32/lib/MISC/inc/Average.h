#ifndef __AVERAGE_H
#define __AVERAGE_H

/* includes ----------------------------------------------------------------- */
/* defines ------------------------------------------------------------------ */
//#define AVERAGE_MAX_SAMPLES 20

#define FILL_ARRAY_AT_FIRST_INSERT 0x01
#define EXTREMUM_AUTO_CLEAR 0x02

/* class -------------------------------------------------------------------- */
class Average
{
	private:
		
		//int m_values[AVERAGE_MAX_SAMPLES];
		int *m_values;	
		int m_maxSamples;
		int m_samples;
		int m_maximum;
		int m_minimum;
		char m_config;
	
	public:
		
		Average(int maxSamples);
		void insert(int value);
		int get(void);
		void fill_array_at_first_insert(void);
		void fill_array(int value);
		void clear(void);
		int size(void);
		int get_maximum(void);
		void clear_maximum(void);
		int get_minimum(void);
		void clear_minimum(void);
		void extremum_auto_clear(void);
		~Average(void);
	
};

#endif /* __AVERAGE_H */
