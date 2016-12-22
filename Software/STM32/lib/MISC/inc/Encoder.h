#ifndef __ENCODER_H
#define __ENCODER_H

/* includes ---------------------------------------------------------------- */
#include "Digital.h"

/* defines ----------------------------------------------------------------- */

/* class ------------------------------------------------------------------- */
class Encoder
{
	private:
		
		InterruptIn* m_A;
		InterruptIn* m_B;
		char m_value;
		char m_old;
	
		int m_accum;
	
		int* m_variable;
		int m_limit;
		int m_n, m_p;
		char m_circular;
	
	public:
		
		Encoder(InterruptIn* A, InterruptIn* B);
		void update(void);
		void circular(char x);
		void attach(int* variable, int limit);
		int get(void);
		void irq(void);
	
		static int round(int number, int divisor);
};

#endif
