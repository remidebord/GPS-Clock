/*!
 * \file Watchdog.cpp
 * \brief Watchdog API.
 * \author Rémi.Debord
 * \version 1.0
 * \date 12 mars 2015
 *
 * Watchdog library (IWDG).
 *
 */

#include "Watchdog.h"

/*!
 *  \brief Constructor
 *
 *  Watchdog constructor.
 *
 *  \param timeout : watchdog reload value (> 6ms and < 24000ms)
 *
 */

Watchdog :: Watchdog(int timeout)
{
	// Enable watchdog write access
	IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
	
	// Watchdog clock (LSI = 40kHz)
	IWDG_SetPrescaler(IWDG_Prescaler_256);
	
	// 40kHz / 256 = 156 Hz => 6.4 ms
	// Reload value can be 6.4ms to 6.4 x 4096 = 26.21s
	
	// Set reload value (ms)
	IWDG_SetReload(timeout / ((1000 * 256) / 40000));
	
	// Wait reload value flag
	while(IWDG_GetFlagStatus(IWDG_FLAG_RVU) != SET);
	
	// Disable watchdog write access
	IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
	
	// Enable watchdog
	IWDG_Enable();
}

/*!
 *  \brief Reload
 *
 *  Watchdog reload (if the watchdog counter reach 0, it reset the microcontroller).
 *
 */

void Watchdog :: reload(void)
{
	IWDG_ReloadCounter();
}
