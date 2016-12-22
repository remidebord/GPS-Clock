/*!
 * \file Touch.cpp
 * \brief Touch sensors API.
 * \author Rémi.Debord
 * \version 1.0
 * \date 19 novembre 2013
 *
 * Touch library (TSC).
 *
 */

#include "Touch.h"

extern "C"
{
	int value_G[6];
	char auto_acq;
	char acq_flag;
	
	int GetGroupAndChannel(GPIO_TypeDef* port, int pin);
}

// Needed !
char Touch::tsc_flag;

/*!
 *  \brief Constructor
 *
 *  Touch constructor.
 *
 *  \param GPIO_c_touch_pin : touch pin
 *  \param GPIO_c_sampling_pin : sampling pin
 *
 */

Touch :: Touch(GPIO_common GPIO_c_touch_pin, GPIO_common GPIO_c_sampling_pin)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	int touchGxCx = 0;
	int samplingGxCx = 0;
	
	m_touch_port = GPIO_c_touch_pin.port;
	m_touch_pin = GPIO_c_touch_pin.pin;
	m_touch_pinSource = GPIO_c_touch_pin.pinSource;
	
	m_sampling_port = GPIO_c_sampling_pin.port;
	m_sampling_pin = GPIO_c_sampling_pin.pin;
	m_sampling_pinSource = GPIO_c_sampling_pin.pinSource;
	
	/* Enable GPIOx clock */
	if(m_touch_port == GPIOA) RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
	else if(m_touch_port == GPIOB) RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);
	else if(m_touch_port == GPIOC) RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOC, ENABLE);
	
	if(m_sampling_port == GPIOA) RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
	else if(m_sampling_port == GPIOB) RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);
	else if(m_sampling_port == GPIOC) RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOC, ENABLE);
	
	// Sampling capacitor IO (Alternate function Output Open-Drain)
	GPIO_InitStructure.GPIO_Pin = m_sampling_pin;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(m_sampling_port, &GPIO_InitStructure);
	
	// Channel or shield IO
	GPIO_InitStructure.GPIO_Pin = m_touch_pin;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_Init(m_touch_port, &GPIO_InitStructure);
	
	// Set alternate function (TS = AF_3)
	GPIO_PinAFConfig(m_sampling_port, m_sampling_pinSource, GPIO_AF_3);
	GPIO_PinAFConfig(m_touch_port, m_touch_pinSource, GPIO_AF_3);
	
	// Enable TSC clock
  RCC->AHBENR |= RCC_AHBENR_TSEN;
	
	touchGxCx = GetGroupAndChannel(m_touch_port, m_touch_pin);
	samplingGxCx = GetGroupAndChannel(m_sampling_port, m_sampling_pin);
	
	// Disable Schmitt trigger hysteresis
	TSC->IOHCR &= (~touchGxCx) & (~samplingGxCx);
	
	// Set Sampling Capacitor IOs
	TSC->IOSCR |= (samplingGxCx);
	
	// Enable the Gx_IOx used as channel
  TSC->IOCCR |= (touchGxCx);
  
	// Set both EOA and MCE interrupts
  TSC->IER |= 0x03;

  // Configure NVIC
  NVIC_SetPriority(TS_IRQn, 0);
  NVIC_EnableIRQ(TS_IRQn);
	
	// Enable acquisition on group X (Gx)
  if(touchGxCx < (1 << 4))
	{
		TSC->IOGCSR |= (1 << 0);
		m_value = &value_G[0];
	}
	else if(touchGxCx < (1 << 8))
	{
		TSC->IOGCSR |= (1 << 1);
		m_value = &value_G[1];
	}
	else if(touchGxCx < (1 << 12))
	{
		TSC->IOGCSR |= (1 << 2);
		m_value = &value_G[2];
	}
	else if(touchGxCx < (1 << 16))
	{
		TSC->IOGCSR |= (1 << 3);
		m_value = &value_G[3];
	}
	else if(touchGxCx < (1 << 20))
	{
		TSC->IOGCSR |= (1 << 4);
		m_value = &value_G[4];
	}
	else if(touchGxCx < (1 << 24))
	{
		TSC->IOGCSR |= (1 << 5);
		m_value = &value_G[5];
	}
	
	if(tsc_flag == 0)
	{
		// Enable TSC clock
		RCC->AHBENR |= RCC_AHBENR_TSEN;

		// TSC enabled
		TSC->CR = 0x01;

		// Set CTPH
		TSC->CR |= (uint32_t)((uint32_t)TSLPRM_TSC_CTPH << 28) & 0xF0000000;

		// Set CTPL
		TSC->CR |= (uint32_t)((uint32_t)TSLPRM_TSC_CTPL << 24) & 0x0F000000;

		// Set SpreadSpectrum
		TSC->CR |= (uint32_t)((uint32_t)TSLPRM_TSC_USE_SS << 16) & 0x00010000;
		TSC->CR |= (uint32_t)((uint32_t)TSLPRM_TSC_SSD << 17) & 0x00FE0000;
		TSC->CR |= (uint32_t)((uint32_t)TSLPRM_TSC_SSPSC << 15) & 0x00008000;

		// Set Prescaler
		TSC->CR |= (uint32_t)((uint32_t)TSLPRM_TSC_PGPSC << 12) & 0x00007000;

		// Set Max Count
		TSC->CR |= (uint32_t)((uint32_t)TSLPRM_TSC_MCV << 5) & 0x000000E0;

		// Set IO default in Output PP Low to discharge all capacitors
		TSC->CR &= (uint32_t)(~(1 << 4));
		
		// Set acquisition mode
		TSC->CR |= (uint32_t)((uint32_t)TSLPRM_TSC_AM << 2) & 0x00000004;
		
		// Start acquisition
		TSC->CR |= 0x02;
		
		tsc_flag = 1;
	}
}

/*!
 *  \brief Touch sensor calibration 
 *
 *  Calibrate sensor.
 *
 */

void Touch :: calibrate(void)
{
	if(auto_acq != 1)
	{
		this->start_acq();
		
		while(acq_flag != 0);
	}
	
	m_threshold = *m_value;
}

/*!
 *  \brief Get touch sensor value 
 *
 *  Get touch value.
 *
 *  \return m_value: sensor value
 */

int Touch :: get_value(void)
{
	return *m_value;
}

/*!
 *  \brief Get touch sensor value 
 *
 *  Get touch state.
 *
 *  \return 1 if the sensor is pressed else 0
 */

char Touch :: pressed(void)
{
	if(*m_value <= (m_threshold * TOUCH_RATIO))
	{
		return 1;
	}
	else return 0;
}

/*!
 *  \brief Set automatic acquisition 
 *
 *  Continuous acquisition.
 *
 */

void Touch :: set_auto_acq(char value)
{
	auto_acq = value;
}

/*!
 *  \brief Start acquisition 
 *
 *  New acquisition.
 *
 */

void Touch :: start_acq(void)
{
	if(acq_flag == 0)
	{
		acq_flag = 1;
		
		// Start acquisition
		TSC->CR |= 0x02;
	}
}

/*!
 *  \brief TS IRQ handler
 */

extern "C"
{	
	void TS_IRQHandler(void)
	{		
		// Max count error
		if(TSC->ISR & 2) TSC->ICR = 2;
			
		// Wait for a end of acquisition
		if(TSC->ISR & 1)
		{
			acq_flag = 0;
			
			// Clear EOAC flag
			TSC->ICR |= 1;
				
			// Counter register
			value_G[0] = (TSC->IOGXCR[0]);
			value_G[1] = (TSC->IOGXCR[1]);
			value_G[2] = (TSC->IOGXCR[2]);
			value_G[3] = (TSC->IOGXCR[3]);
			value_G[4] = (TSC->IOGXCR[4]);
			value_G[5] = (TSC->IOGXCR[5]);
				
			if(auto_acq)
			{
				acq_flag = 1;
				
				// Start acquisition
				TSC->CR |= 0x02;
			}
		}
	}
	
	int GetGroupAndChannel(GPIO_TypeDef* port, int pin)
	{
		int value = 0;
		
		if(port == GPIOA)
		{
			switch(pin)
			{
				case GPIO_Pin_0: value = (1 << 0); break; // G1_IO1
				case GPIO_Pin_1: value = (1 << 1); break; // G1_IO2
				case GPIO_Pin_2: value = (1 << 2); break; // G1_IO3
				case GPIO_Pin_3: value = (1 << 3); break; // G1_IO4
						
				case GPIO_Pin_4: value = (1 << 4); break; // G2_IO1
				case GPIO_Pin_5: value = (1 << 5); break; // G2_IO2
				case GPIO_Pin_6: value = (1 << 6); break; // G2_IO3
				case GPIO_Pin_7: value = (1 << 7); break; // G2_IO4
						
				case GPIO_Pin_9: value = (1 << 12); break; // G4_IO1
				case GPIO_Pin_10: value = (1 << 13); break; // G4_IO2
				case GPIO_Pin_11: value = (1 << 14); break; // G4_IO3
				case GPIO_Pin_12: value = (1 << 15); break; // G4_IO4
			}
		}
		else if(port == GPIOB)
		{	
			switch(pin)
			{
				case GPIO_Pin_0: value = (1 << 9); break; // G3_IO2
				case GPIO_Pin_1: value = (1 << 10); break; // G3_IO3
				case GPIO_Pin_2: value = (1 << 11); break; // G3_IO4
						
				case GPIO_Pin_3: value = (1 << 16); break; // G5_IO1
				case GPIO_Pin_4: value = (1 << 17); break; // G5_IO2
				case GPIO_Pin_6: value = (1 << 18); break; // G5_IO3
				case GPIO_Pin_7: value = (1 << 19); break; // G5_IO4		
						
				case GPIO_Pin_11: value = (1 << 20); break; // G6_IO1
				case GPIO_Pin_12: value = (1 << 21); break; // G6_IO1
				case GPIO_Pin_13: value = (1 << 22); break; // G6_IO1
				case GPIO_Pin_14: value = (1 << 23); break; // G6_IO1
			}
		}					
		else if(port == GPIOC)
		{
			switch(pin)
			{
				case GPIO_Pin_5: value = (1 << 8); break; //G3_IO1
			}		
		}
		
		return value;
	}
}
