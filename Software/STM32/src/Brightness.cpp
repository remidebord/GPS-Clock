/*!
 * \file Brightness.cpp
 * \brief Brightness API.
 * \author Rémi.Debord
 * \version 1.0
 * \date 5 aout 2015
 *
 * Brightness library.
 *
 */

#include "Brightness.h"

/*!
 *  \brief Constructor
 *
 *  Led constructor.
 *
 */

Brightness :: Brightness(GPIO_common GPIO_c, int freq, TIM_common PWM_c): m_output(GPIO_c, freq, PWM_c)
{
	m_value = BRIGHTNESS_MAX;
	
	this->set();
}

/*!
 *  \brief Set
 *
 *  Set Brightness.
 */

void Brightness :: set(void)
{
	if(m_value > BRIGHTNESS_MAX) m_value = BRIGHTNESS_MAX;
	if(m_value < BRIGHTNESS_MIN) m_value = BRIGHTNESS_MIN;
	
	m_output.write((float)((int)cie[m_value*10])/100);
}

/*!
 *  \brief Set
 *
 *  Set Brightness.
 */

void Brightness :: set(int value)
{
	m_value = value;
	
	if(m_value > BRIGHTNESS_MAX) m_value = BRIGHTNESS_MAX;
	if(m_value < BRIGHTNESS_MIN) m_value = BRIGHTNESS_MIN;
	
	m_output.write((float)((int)cie[m_value*10])/100);
}

/*!
 *  \brief Value
 *
 *  Get Brightness value.
 *
 *  \return n : 0 to 100 (%)
 */

int* Brightness :: value(void)
{
	return &m_value;
}
