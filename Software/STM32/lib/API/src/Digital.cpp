/*!
 * \file Digital.cpp
 * \brief Digital API.
 * \author Rémi.Debord
 * \version 1.2
 * \date 19 novembre 2013
 *
 * Digital library (DigitalIn, DigitalOut and InterruptIn).
 *
 */

#include "Digital.h"

extern "C"
{
	void (*ExternalInterruptCallback[16])(void);
}

/*!
 *  \brief Constructor
 *
 *  DigitalIn constructor.
 *
 *  \param GPIO_c : GPIO pin
 *
 */

DigitalIn :: DigitalIn(const GPIO_common* gpio)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	
	m_gpio = gpio;
	
  /* GPIOx Periph clock enable */
	RCC_AHBPeriphClockCmd(m_gpio->peripheralClock, ENABLE);
	
  /* Configure "pin" in input mode */
  GPIO_InitStructure.GPIO_Pin = m_gpio->pin;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Level_3;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;	//GPIO_PuPd_NOPULL;
	
  GPIO_Init(m_gpio->port, &GPIO_InitStructure);
}

/*!
 *  \brief Change input mode
 *
 *  Modify input mode (no pull, pull up, ...).
 *
 *  \param pull : pull mode
 *
 */

void DigitalIn :: mode(GPIOPuPd_TypeDef pull)
{
	GPIO_TypeDef* port = m_gpio->port;
	uint32_t tmp = m_gpio->pinSource << 1;
	
	/* GPIO pull-up/down */
	port->PUPDR &= ~((uint32_t)0x03 << tmp);
	port->PUPDR |= ((uint32_t)pull << tmp);
}

/*!
 *  \brief DigitalIn read
 *
 *  Read GPIO state (0 or 1).
 *
 *  \return 0 or 1
 *
 */

uint32_t DigitalIn :: read()
{
	uint32_t value = m_gpio->port->IDR & m_gpio->pin;
	
	if(value != 0)
	{
		value = 1;
	}
	
	return value;
}

/*!
 *  \brief DigitalIn read (shorthand)
 *
 *  Read GPIO state (0 or 1).
 *
 *  \return 0 or 1
 *
 */

DigitalIn :: operator uint32_t()
{	
	return this->read();
}

/*!
 *  \brief Constructor
 *
 *  DigitalOut constructor.
 *
 *  \param GPIO_c : GPIO pin
 *
 */

DigitalOut :: DigitalOut(const GPIO_common* gpio)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	
	m_gpio = gpio;
	
  /* GPIOx Periph clock enable */
	RCC_AHBPeriphClockCmd(m_gpio->peripheralClock, ENABLE);
	
  /* Configure "pin" in output pushpull mode */
  GPIO_InitStructure.GPIO_Pin = m_gpio->pin;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Level_3;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	
  GPIO_Init(m_gpio->port, &GPIO_InitStructure);
}

/*!
 *  \brief Constructor
 *
 *  DigitalOut constructor.
 *
 *  \param GPIO_c : GPIO pin
 *	\param type: Push pull or Open drain
 *
 */

DigitalOut :: DigitalOut(const GPIO_common* gpio, GPIOOType_TypeDef type)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	
	m_gpio = gpio;
	
  /* GPIOx Periph clock enable */
	RCC_AHBPeriphClockCmd(m_gpio->peripheralClock, ENABLE);
	
  /* Configure "pin" in output pushpull mode */
  GPIO_InitStructure.GPIO_Pin = m_gpio->pin;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = type;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Level_3;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	
  GPIO_Init(m_gpio->port, &GPIO_InitStructure);
}

/*!
 *  \brief DigitalOut write
 *
 *  Write GPIO new state (0 or 1).
 *
 *  \param value : GPIO state
 *
 */

void DigitalOut :: write(uint32_t value)
{
	if(value == 0)
	{
		m_gpio->port->BRR = m_gpio->pin;
	}
	else
	{
		m_gpio->port->BSRR = m_gpio->pin;
	}
}

/*!
 *  \brief DigitalOut read
 *
 *  Read GPIO state (0 or 1).
 *
 *  \return 0 or 1
 *
 */

uint32_t DigitalOut :: read()
{
	uint32_t value = m_gpio->port->ODR & m_gpio->pin;
	
	if(value != 0)
	{
		value = 1;
	}
	
	return value;
}

/*!
 *  \brief DigitalOut write (shorthand)
 *
 *  Write GPIO new state (0 or 1).
 *
 */

DigitalOut& DigitalOut ::  operator= (uint32_t n)
{
	this->write(n);
	
	return *this;
}

/*!
 *  \brief DigitalOut read (shorthand)
 *
 *  Read GPIO state (0 or 1).
 *
 *  \return 0 or 1
 *
 */

DigitalOut :: operator uint32_t()
{
	return this->read();
}

/*!
 *  \brief Constructor
 *
 *  InterruptIn constructor.
 *
 *  \param gpio : GPIO pin
 *
 */

InterruptIn :: InterruptIn(const GPIO_common* gpio) : DigitalIn(gpio)
{
	EXTI_InitTypeDef   EXTI_InitStructure;
  NVIC_InitTypeDef   NVIC_InitStructure;
	
	/* Enable SYSCFG clock */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

  /* Connect EXTI Line to "pin" */
	SYSCFG_EXTILineConfig(m_gpio->extiPort, m_gpio->pinSource);
	
  /* Configure EXTI "pin" */
  EXTI_InitStructure.EXTI_Line = (uint32_t) m_gpio->pin;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling; //EXTI_Trigger_Rising;  
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);

  /* Enable and set EXTI "pin" Interrupt to the lowest priority */
  NVIC_InitStructure.NVIC_IRQChannel = m_gpio->irqVector;
  NVIC_InitStructure.NVIC_IRQChannelPriority = 0x00;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}

/*!
 *  \brief Set callback function on a rising edge
 *
 *  Associate rising edge to callback function.
 *
 *  \param void(*f)(void) : function address
 *
 */

void InterruptIn :: rise(void(*f)(void))
{
	/* Attribute function adress to the function pointer */
	ExternalInterruptCallback[m_gpio->pinSource] = f;
	
	/* Clear Rising Falling edge configuration */
	EXTI->RTSR &= ~((uint32_t) m_gpio->pin);
	EXTI->FTSR &= ~((uint32_t) m_gpio->pin);
	
	/* Rising edge */
	EXTI->RTSR |= ((uint32_t) m_gpio->pin);
}

/*!
 *  \brief Set callback function on a falling edge
 *
 *  Associate falling edge to callback function.
 *
 *  \param void(*f)(void) : function address
 *
 */

void InterruptIn :: fall(void(*f)(void))
{	
	/* Attribute function adress to the function pointer */
	ExternalInterruptCallback[m_gpio->pinSource] = f;
	
	/* Clear Rising Falling edge configuration */
	EXTI->RTSR &= ~((uint32_t) m_gpio->pin);
	EXTI->FTSR &= ~((uint32_t) m_gpio->pin);
	
	/* Falling edge */
	EXTI->FTSR |= ((uint32_t) m_gpio->pin);
}

/*!
 *  \brief Set callback function on a rising and a falling edge
 *
 *  Associate rising/falling edge to callback function.
 *
 *  \param void(*f)(void) : function address
 *
 */

void InterruptIn :: risefall(void(*f)(void))
{	
	/* Attribute function adress to the function pointer */
	ExternalInterruptCallback[m_gpio->pinSource] = f;
	
	/* Clear Rising Falling edge configuration */
	EXTI->RTSR &= ~((uint32_t) m_gpio->pin);
	EXTI->FTSR &= ~((uint32_t) m_gpio->pin);
	
	/* Rising Falling edge */
	EXTI->RTSR |= ((uint32_t) m_gpio->pin);
	EXTI->FTSR |= ((uint32_t) m_gpio->pin);
}

/*!
 *  \brief External interrupt IRQ handler
 */

extern "C"
{
	void EXTI0_1_IRQHandler(void)
	{
		uint32_t extiLine = 0;
		uint8_t i = 0;
		
		for(i = 0; i <= 1; i++)
		{
			extiLine = ((uint32_t)0x01 << i);
			
			if(EXTI_GetITStatus(extiLine) != RESET)
			{
				/* Callback ? */
				if(ExternalInterruptCallback[i] != NULL)
				{
					(*ExternalInterruptCallback[i])();
				}
				
				/* Clear the EXTI line X pending bit */
				EXTI_ClearITPendingBit(extiLine);
			}			
		}
	}
	
	void EXTI2_3_IRQHandler(void)
	{
		uint32_t extiLine = 0;
		uint8_t i = 0;
		
		for(i = 2; i <= 3; i++)
		{
			extiLine = ((uint32_t)0x01 << i);
			
			if(EXTI_GetITStatus(extiLine) != RESET)
			{
				/* Callback ? */
				if(ExternalInterruptCallback[i] != NULL)
				{
					(*ExternalInterruptCallback[i])();
				}
				
				/* Clear the EXTI line X pending bit */
				EXTI_ClearITPendingBit(extiLine);
			}			
		}
	}
	
	void EXTI4_15_IRQHandler(void)
	{
		uint32_t extiLine = 0;
		uint8_t i = 0;
		
		for(i = 4; i <= 15; i++)
		{
			extiLine = ((uint32_t)0x01 << i);
			
			if(EXTI_GetITStatus(extiLine) != RESET)
			{
				/* Callback ? */
				if(ExternalInterruptCallback[i] != NULL)
				{
					(*ExternalInterruptCallback[i])();
				}
				
				/* Clear the EXTI line X pending bit */
				EXTI_ClearITPendingBit(extiLine);
			}			
		}
	}
}
