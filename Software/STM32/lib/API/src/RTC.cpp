/*!
 * \file RTC.cpp
 * \brief RTC API.
 * \author Rémi.Debord
 * \version 1.0
 * \date 22 mars 2015
 *
 * RTC library.
 *
 */

#include "RTC.h"

extern "C"
{
	void (*RTCAlarmInterrupt)(void);
}

/*!
 *  \brief Constructor
 *
 *  Clock constructor.
 *
 */

CLOCK :: CLOCK(char type)
{
	RTC_InitTypeDef   RTC_InitStructure;
	
	// RTC clock (peripheral)
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
		
	// Enable access to RTC register (write access)
	PWR_BackupAccessCmd(ENABLE);
	
	// RTC not configured ?
	if (RTC_ReadBackupRegister(RTC_BKP_DR0) != BKP_VALUE)
	{
		if(type == RTC_LSI)
		{
			// Enable the LSI OSC 
			RCC_LSICmd(ENABLE);
			
			// Wait clock to be stable
			while(RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET);
			
			// Select LSI (40kHz)
			RCC_RTCCLKConfig(RCC_RTCCLKSource_LSI);

			// Enable RTC clock (LSI)
			RCC_RTCCLKCmd(ENABLE);
				
			// Wait for RTC APB registers synchronisation
			RTC_WaitForSynchro();
				
			// Init RTC (LSI = 40kHz) => 40000 / (100 * 400) = 1 Hz
			RTC_InitStructure.RTC_AsynchPrediv = 99; // 99 + 1 = 100
			RTC_InitStructure.RTC_SynchPrediv = 399; // 399 + 1 = 400 
			RTC_InitStructure.RTC_HourFormat = RTC_HourFormat_24;
		}
		else
		{			
			// Enable the LSE OSC 
			RCC_LSEConfig(RCC_LSE_ON);
			
			// Wait clock to be stable
			while(RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET);
			
			// Select LSE (32.768kHz)
			RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);
			
			// Enable RTC clock (LSE)
			RCC_RTCCLKCmd(ENABLE);
			
			// Wait for RTC APB registers synchronisation
			RTC_WaitForSynchro();
						
			// Init RTC (LSE = 32.768kHz) => 32768 / (128 * 256) = 1 Hz
			RTC_InitStructure.RTC_AsynchPrediv = 127; // 127 + 1 = 128
			RTC_InitStructure.RTC_SynchPrediv = 255; // 255 + 1 = 256 
			RTC_InitStructure.RTC_HourFormat = RTC_HourFormat_24;
		}
		
		RTC_Init(&RTC_InitStructure);
	}
	else
	{
		// RTC clock (peripheral)
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
		
		// Enable access to RTC register (write access)
		PWR_BackupAccessCmd(ENABLE);
		
		if(type == RTC_LSI)
		{
			// Enable the LSI OSC 
			RCC_LSICmd(ENABLE);
		}
		
		// Wait for RTC APB registers synchronisation
		RTC_WaitForSynchro();
	}
}

/*!
 *  \brief Set
 *
 *  Set time (hours, minutes, seconds).
 * 
 *  \param time : time structure
 */

void CLOCK :: set(Time time)
{
	RTC_TimeTypeDef   RTC_TimeStructure;
	
	RTC_TimeStructure.RTC_H12 = (time.hours >= 12) ? RTC_H12_AM : RTC_H12_PM;
	RTC_TimeStructure.RTC_Hours = time.hours;
	RTC_TimeStructure.RTC_Minutes = time.minutes;
	RTC_TimeStructure.RTC_Seconds = time.seconds;
	
	RTC_SetTime(RTC_Format_BIN, &RTC_TimeStructure);
}

/*!
 *  \brief Set
 *
 *  Set date (year, month, day).
 * 
 *  \param date : date structure
 */

void CLOCK :: set(Date date)
{
	RTC_DateTypeDef RTC_DateStructure;
	int weekDay = 0;
	
	// Get day of the week (dow)
	weekDay = CLOCK::dow(date.year, date.month, date.day);
	
	// Sunday is 0 (convert into rtc unit)
	weekDay = (weekDay == 0) ? RTC_Weekday_Sunday : weekDay;
	
	RTC_DateStructure.RTC_Date = date.day;
	RTC_DateStructure.RTC_Month = date.month;
	RTC_DateStructure.RTC_WeekDay = weekDay;
	RTC_DateStructure.RTC_Year = (date.year % 100);
	
	RTC_SetDate(RTC_Format_BIN, &RTC_DateStructure);
}

/*!
 *  \brief Get
 *
 *  Get time (hours, minutes, seconds).
 * 
 *  \param time : time structure pointer
 */

void CLOCK :: get(Time *time)
{
	RTC_TimeTypeDef   RTC_TimeStructure;
	
	RTC_GetTime(RTC_Format_BIN, &RTC_TimeStructure);
	
	time->hours = RTC_TimeStructure.RTC_Hours;
	time->minutes = RTC_TimeStructure.RTC_Minutes;
	time->seconds = RTC_TimeStructure.RTC_Seconds;
}

/*!
 *  \brief Get
 *
 *  Get date (year, month, day).
 * 
 *  \param date : date structure pointer
 */

void CLOCK :: get(Date *date)
{
	RTC_DateTypeDef RTC_DateStructure;
	
	RTC_GetDate(RTC_Format_BIN, &RTC_DateStructure);
	
	date->year = RTC_DateStructure.RTC_Year + 2000;
	date->month = RTC_DateStructure.RTC_Month;
	date->day = RTC_DateStructure.RTC_Date;
}

/*!
 *  \brief Alarm
 *
 *  Set alarm in seconds.
 * 
 *  \param seconds : alarm value
 *  \param f : callback function
 */

void CLOCK :: alarm(int seconds, void(*f)(void))
{
	NVIC_InitTypeDef NVIC_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure;
	RTC_AlarmTypeDef RTC_AlarmStructure;
	RTC_TimeTypeDef  RTC_TimeStructure;
	
	int min = seconds / 60;
	int sec = seconds % 60;
	
	RTCAlarmInterrupt = f;
	
	// Enable EXTI (alarm)
	EXTI_InitStructure.EXTI_Line = EXTI_Line17;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;  
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	
	EXTI_Init(&EXTI_InitStructure);
	
	// Enable alarm global Interrupt
  NVIC_InitStructure.NVIC_IRQChannel = RTC_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	
  NVIC_Init(&NVIC_InitStructure);	
	
	// Get time
	RTC_GetTime(RTC_Format_BIN, &RTC_TimeStructure);
	
	if((RTC_TimeStructure.RTC_Seconds + sec) >= 60)
	{
		RTC_TimeStructure.RTC_Minutes++;	
	}
	
	RTC_TimeStructure.RTC_Seconds = (RTC_TimeStructure.RTC_Seconds + sec) % 60;
	
	if((RTC_TimeStructure.RTC_Minutes + min) >= 60)
	{
		RTC_TimeStructure.RTC_Hours++;
	}
	
	RTC_TimeStructure.RTC_Minutes = (RTC_TimeStructure.RTC_Minutes + min) % 60;
	
	if(RTC_TimeStructure.RTC_Hours >= 24)
	{
		RTC_TimeStructure.RTC_Hours = 0;
		RTC_TimeStructure.RTC_H12 = RTC_H12_AM;
	}
	
	// Set alarm
	RTC_AlarmStructure.RTC_AlarmTime.RTC_H12 = RTC_TimeStructure.RTC_H12;
	RTC_AlarmStructure.RTC_AlarmTime.RTC_Hours = RTC_TimeStructure.RTC_Hours;
	RTC_AlarmStructure.RTC_AlarmTime.RTC_Minutes = RTC_TimeStructure.RTC_Minutes;
	RTC_AlarmStructure.RTC_AlarmTime.RTC_Seconds = RTC_TimeStructure.RTC_Seconds;
	
	RTC_AlarmStructure.RTC_AlarmDateWeekDay = 1; // Not used
	RTC_AlarmStructure.RTC_AlarmDateWeekDaySel = RTC_AlarmDateWeekDaySel_Date;
	
	// If rtc hour + minutes + seconds == alarm hour + minutes + seconds => interrupt !
	RTC_AlarmStructure.RTC_AlarmMask = RTC_AlarmMask_DateWeekDay;

	// Disable Alarm A
	RTC_AlarmCmd(RTC_Alarm_A, DISABLE);
	
	// Disable Alarm A interrupt
	RTC_ITConfig(RTC_IT_ALRA, DISABLE);
	
	RTC_SetAlarm(RTC_Format_BIN, RTC_Alarm_A, &RTC_AlarmStructure);	
	
	// Enable Alarm A
	RTC_AlarmCmd(RTC_Alarm_A, ENABLE);	
			
	// Enable Alarm A interrupt
	RTC_ITConfig(RTC_IT_ALRA, ENABLE);
		
	// Clear flag
	RTC_ClearFlag(RTC_IT_ALRA);
}

/*!
 *  \brief dow
 *
 *  Get day of the week (0 = Sunday, 1 = Monday, ...).
 * 
 *  \param year : year (20XX)
 *  \param month : month
 *  \param day : day
 *  \return day of the week [0:6]
 */

int CLOCK :: dow (int year, int month, int day)
{
	static int t[] = {0, 3, 2, 5, 0, 3, 5, 1, 4, 6, 2, 4};
	
	year -= month < 3;

	return (year + year/4 - year/100 + year/400 + t[month-1] + day) % 7;
}

/*!
 *  \brief RTC IRQ handler
 */

extern "C"
{	
	void RTC_IRQHandler(void)
	{
		// RTC Alarm A
		if (RTC_GetITStatus(RTC_IT_ALRA) != RESET)
		{
			// Clear flag
			RTC_ClearITPendingBit(RTC_IT_ALRA);
			
			// Callback function
			if(RTCAlarmInterrupt) (*RTCAlarmInterrupt)();
		}
		
		EXTI_ClearITPendingBit(EXTI_Line17);
	}
}
