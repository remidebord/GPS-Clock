/*!
 * \file Button.cpp
 * \brief Button API.
 * \author Rémi.Debord
 * \version 1.0
 * \date 26 juin 2015
 *
 * Button library.
 *
 */

#include "Button.h"

/*!
 *  \brief Constructor
 *
 *  Button constructor.
 *
 *  \param pin : button pin
 *
 */

Button :: Button(GPIO_common pin): m_button(&pin)
{
	m_value = 0;
	m_state = WAIT_PRESS;
	m_timer = 0;
}

/*!
 *  \brief Update
 *
 *  Manage button state.
 *
 */

void Button :: update(void)
{
	switch(m_state)
	{
		case WAIT_PRESS:
			if((m_button) && (m_value == 0))
			{
				// Debounce
				if(m_timer >= DEBOUNCE_TIME)
				{
					m_timer = 0;
					m_state = WAIT_RELEASE;
				}
			}
			else
			{
				m_timer = 0;
			}
			break;
		
		case WAIT_RELEASE:
			if(m_button == 0)
			{
				// Short press ?
				if(m_timer < LONG_PRESS_TIME)
				{
					m_value = SHORT_PRESS;
				}
				// Long press ?
				else
				{
					m_value = LONG_PRESS;
				}
				
				m_state = WAIT_PRESS;
				m_timer = 0;
			}
			// Continuous press ?
			else if((m_button) && (m_timer >= CONTINUOUS_PRESS_TIME))
			{
				m_value = CONTINUOUS_PRESS;
				m_state = WAIT_CONTINUOUS;
			}
			break;
			
		case WAIT_CONTINUOUS:
			if(m_button == 0)
			{
				m_state = WAIT_PRESS;
				m_timer = 0;
			}
	}
}

/*!
 *  \brief Get
 *
 *  Get button state (Short, long and continuous press).
 *
 *  \return state: see Button.h
 */

char Button :: get(void)
{
	char value = m_value;
	
	m_value = 0;
	
	return value;
}

/*!
 *  \brief Manage Button base time 
 *
 *  Note: Place it into a ticker callback of 1 ms
 *
 */

void Button :: timer(void)
{
	if(m_timer < 10000) m_timer++;
}
