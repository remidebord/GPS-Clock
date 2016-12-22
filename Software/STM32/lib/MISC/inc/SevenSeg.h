#ifndef __SEVENSEG_H
#define __SEVENSEG_H

/* includes ---------------------------------------------------------------- */
#include "Digital.h"

/* defines ----------------------------------------------------------------- */
#define MAX_SEGMENTS 7
#define MAX_DIGITS	 10
#define REFRESH_TIME 20 // 20 ms => 50 fps

/* structure --------------------------------------------------------------- */
struct Effect
{
	int value;
	int timer;
	char flag;
	
	int select;
	char *buffer;
	int length;
	char shift;
};

/* variables --------------------------------------------------------------- */
const char loop[8] = {0x40, 0x02, 0x01, 0x20, 0x40, 0x04, 0x08, 0x10}; 
const char mad[8] = {0x3F, 0x7D, 0x7E, 0x5F, 0x3F, 0x7B, 0x77, 0x6F}; 
const char wave[6] = {0x01, 0x02, 0x04, 0x08, 0x10, 0x20};

/* class ------------------------------------------------------------------- */
class SevenSeg
{
	private:
		
		DigitalOut* m_segment[MAX_SEGMENTS];
		DigitalOut* m_digit;
	
	public:
		
		SevenSeg(DigitalOut* segment, DigitalOut* digit);
		void segments(char c);
		void set(char c);
		void set(int number);
		void on(void);
		void off(void);
};

class SevenSegGroup
{
	private:
		
		SevenSeg* m_digit[MAX_DIGITS];
		char m_buffer[MAX_DIGITS];
		char m_tmp[MAX_DIGITS];
		char m_size;
		char m_inc;
		char m_mux;
		int m_timer;

		Effect m_blink;
		Effect m_scroll;
		Effect m_effect;
	
	public:
		
		SevenSegGroup(void);
		void update(void);
		void add(SevenSeg* digit);
		void set(char* buffer, int length);
		void set(int number);
		void select(int segment);
		void blink(int ms);
		void blink(char select, int ms);
		void scroll(int ms);
		void effect(char* buffer, int length, int shift, int ms);
		void clear(void);
		void timer(void);
};

#endif
