#ifndef __MAIN_H
#define __MAIN_H

/* includes ------------------------------------------------------------------*/
#include "stm32f0xx.h"
#include <stdio.h>
#include <string.h>

#include "Delay.h"
#include "Digital.h"
#include "Timer.h"
#include "Systick.h"
#include "Analog.h"
#include "I2C.h"
#include "Serial.h"
#include "SPI.h"
#include "Touch.h"
#include "Watchdog.h"
#include "Memory.h"
#include "Power.h"
#include "RTC.h"

#include "SevenSeg.h"
#include "Encoder.h"
#include "DS3231.h"
#include "GPS.h"
#include "Button.h"
#include "Brightness.h"
#include "Timezone.h"

/* defines --------------------------------------------------------------------*/
#define TIME_MODE 			0x00
#define SET_TIME_MODE 	0x01
#define GPS_SYNC_MODE		0x02

#define TIME_MODE_TIME	0x00
#define TIME_MODE_TEMP	0x01

#define SET_TIME_MODE_INIT 		0x00
#define SET_TIME_MODE_MINUTES 0x01
#define SET_TIME_MODE_HOURS 	0x02

/* variables ------------------------------------------------------------------*/
int state[2];
int timeout;
float temperature;
Coordinate position;
Time time;
Time temp[2];

/* functions ------------------------------------------------------------------*/
void GlobalBaseTime(void);
void EncoderRightirqA(void);
void EncoderRightirqB(void);
void EncoderLeftirqA(void);
void EncoderLeftirqB(void);

#endif /* __MAIN_H */
