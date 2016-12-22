/*!
 * \file Power.cpp
 * \brief Power API.
 * \author Rémi.Debord
 * \version 1.0
 * \date 12 mars 2015
 *
 * Power management library.
 *
 * RUN mode (default) @ 48MHz => 12 mA
 * STOP mode => 3.7 uA
 * STANDY mode => 1.1 uA
 *
 */

#include "Power.h"

/*!
 *  \brief Constructor
 *
 *  Power constructor.
 *
 */

Power :: Power(void)
{
  // Enable the PWR clock
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
}

/*!
 *  \brief Stop
 *
 *  Stop mode (with clocks init).
 *
 */

void Power :: stop(void)
{
	// Stop mode
	PWR_EnterSTOPMode(PWR_Regulator_LowPower, PWR_STOPEntry_WFI);
	
	// Init clocks after Stop mode exit
	SystemInit();
}

/*!
 *  \brief Standby
 *
 *  Standby mode (lowest consumption).
 *
 *  \param pin : wake up pin (1 = PA0, 2 = PC13)
 */

void Power :: standby(char pin)
{
	// Enable WKUP pin
	if(pin == 1)
	{
		// PA0
		PWR_WakeUpPinCmd(PWR_WakeUpPin_1, ENABLE);
	}
	else if(pin == 2)
	{
		// PC13
		PWR_WakeUpPinCmd(PWR_WakeUpPin_2, ENABLE);
	}
	
	// Request to enter STANDBY mode
	PWR_EnterSTANDBYMode();
}
