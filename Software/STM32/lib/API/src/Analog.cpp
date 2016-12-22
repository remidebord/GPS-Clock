/*!
 * \file Analog.cpp
 * \brief Analog API.
 * \author Rémi.Debord
 * \version 1.1
 * \date 19 novembre 2013
 *
 * Analog library (ADC and DAC).
 *
 */

#include "Analog.h"

ADC_TypeDef* AnalogIn_DMA::m_adc[ADC_MAX_INPUT] = {0};
int AnalogIn_DMA::m_channels[ADC_MAX_INPUT] = {0};
int AnalogIn_DMA::m_rank[ADC_MAX_INPUT] = {0};
int AnalogIn_DMA::m_inc = 1;

/*!
 *  \brief Constructor
 *
 *  AnalogIn constructor (ADC).
 *
 *  \param GPIO_c : ADC pin
 *
 */

AnalogIn :: AnalogIn(GPIO_common GPIO_c)
{
	ADC_InitTypeDef       ADC_InitStructure;
  GPIO_InitTypeDef      GPIO_InitStructure;
	
	m_adc = ADC1; // STM32F0 have Only 1 ADC
	m_port = GPIO_c.port;
	m_pin = GPIO_c.pin;
	m_channel = GetChannel(GPIO_c.port, GPIO_c.pin);
	m_value = 0;
	
  /* GPIOx Periph clock enable */
	if(m_port == GPIOA) RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
	else if(m_port == GPIOB) RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);
	else if(m_port == GPIOC) RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOC, ENABLE);
	else if(m_port == GPIOD) RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOD, ENABLE);
	else if(m_port == GPIOF) RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOF, ENABLE);
	
	/* ADCx clock */
	if(m_adc == ADC1) RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);	
  
	/* Configure "pin" in input mode */
  GPIO_InitStructure.GPIO_Pin = m_pin;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(m_port, &GPIO_InitStructure);
	
	/* ADCx DeInit */  
  ADC_DeInit(m_adc); 
  
	/* ADCx Init */
  ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
  ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
  ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;    
  ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T1_TRGO;
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
  ADC_InitStructure.ADC_ScanDirection = ADC_ScanDirection_Upward;
  ADC_Init(m_adc, &ADC_InitStructure);
	
	//ADC_ChannelConfig(m_adc, m_channel, ADC_SampleTime_13_5Cycles);
	ADC_ChannelConfig(m_adc, m_channel, ADC_SampleTime_55_5Cycles);
	
	/* ADC Calibration */
  ADC_GetCalibrationFactor(m_adc);

  /* Enable ADCx */
  ADC_Cmd(m_adc, ENABLE);
	
	/* Wait the ADCEN falg */
  while(!ADC_GetFlagStatus(m_adc, ADC_FLAG_ADEN));
}

/*!
 *  \brief Read ADC value
 *
 *  Convert voltage and get result.
 *
 *  \return m_value: ADC value
 */

int AnalogIn :: read()
{
	__disable_irq();
	
	/* Stop conversion */
	ADC_StopOfConversion(m_adc);
	
	/* ADC ready ? */
	//while(ADC_GetFlagStatus(m_adc, ADC_FLAG_ADRDY) == RESET);
	
	/* Reset sequence (ADSTART = 0) */
  m_adc->CHSELR &= 0x0000;
	
	/* Configure the ADC Channel (ADSTART = 0) */
  m_adc->CHSELR |= m_channel;
	
	/* Start conversion on the selected channel */
	ADC_StartOfConversion(m_adc);
	
	/* Wait the end of conversion (EOC) */
	while((ADC_GetFlagStatus(m_adc, ADC_FLAG_EOC) == RESET) && (ADC_GetFlagStatus(m_adc, ADC_FLAG_OVR) == RESET));
	
	if(ADC_GetFlagStatus(m_adc, ADC_FLAG_OVR) != RESET)
	{
		ADC_ClearFlag(m_adc, ADC_FLAG_OVR);
		
		/* Stop conversion */
		ADC_StopOfConversion(m_adc);
		
		__enable_irq();
		
		return 0;
	}
	else
	{
		/* Automatic stop */
	
		/* Store value */
		m_value = ADC_GetConversionValue(m_adc);
		
		__enable_irq();
		
		/* Return the converted value */
		return m_value;
	}
}

/*!
 *  \brief Read ADC value (no conversion)
 *
 *  Get last value converted.
 *
 *  \return m_value: ADC value
 */

AnalogIn :: operator int()
{
	return m_value;
}

/*!
 *  \brief Constructor
 *
 *  AnalogIn_DMA constructor (ADC).
 *  Need to be declared in channel order !!!
 *
 *  \param GPIO_c : ADC pin
 *
 */

__IO uint16_t value[ADC_MAX_INPUT];

AnalogIn_DMA :: AnalogIn_DMA(GPIO_common GPIO_c)
{
  ADC_InitTypeDef     ADC_InitStructure;
	GPIO_InitTypeDef    GPIO_InitStructure;
  DMA_InitTypeDef     DMA_InitStructure;
	
	char i = 0, j = 1;
  
	m_port = GPIO_c.port;
	m_pin = GPIO_c.pin;
	m_index = m_inc - 1;
	m_adc[m_index] = ADC1; // STM32F0 have Only 1 ADC
	m_channels[m_index] = GetChannel(GPIO_c.port, GPIO_c.pin);
	m_channel = GetIndex(m_channels[m_index]);
	m_rank[m_channel] = m_inc;
	
	// Sort rank by channel index
	for(i = 0; i < ADC_MAX_INPUT; i++)
	{
		if(m_rank[i] != 0)
		{
			m_rank[i] = j++;
		}
	}
	
	/* Stop conversion */
	ADC_StopOfConversion(m_adc[m_index]);
	
	/* Disable ADCx */
  ADC_Cmd(m_adc[m_index], DISABLE);  
	
	/* ADCx DeInit */
  ADC_DeInit(m_adc[m_index]);
	
	/* Disable ADC_DMA */
  ADC_DMACmd(m_adc[m_index], DISABLE);
	
  /* DMA1 Channel1 disable */
  DMA_Cmd(DMA1_Channel1, DISABLE);
	
  /* GPIOx Periph clock enable */
	if(m_port == GPIOA) RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
	else if(m_port == GPIOB) RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);
	else if(m_port == GPIOC) RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOC, ENABLE);
	else if(m_port == GPIOD) RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOD, ENABLE);
	else if(m_port == GPIOF) RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOF, ENABLE);
	
	/* Configure "pin" in input mode */
  GPIO_InitStructure.GPIO_Pin = m_pin;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(m_port, &GPIO_InitStructure);
	
	/* ADCx clock */
	if(m_adc[m_index] == ADC1) RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
  
  /* DMA1 clock enable */
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1 , ENABLE);
  
  /* DMA1 Channel1 Config */
  DMA_DeInit(DMA1_Channel1);
  DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&m_adc[m_index]->DR;
  DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)value;
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
  DMA_InitStructure.DMA_BufferSize = m_inc++;
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
  DMA_InitStructure.DMA_Priority = DMA_Priority_High;
  DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
  DMA_Init(DMA1_Channel1, &DMA_InitStructure);
  
  /* DMA1 Channel1 enable */
  DMA_Cmd(DMA1_Channel1, ENABLE);
  
  /* ADC DMA request in circular mode */
  ADC_DMARequestModeConfig(m_adc[m_index], ADC_DMAMode_Circular);
  
  /* Enable ADC_DMA */
  ADC_DMACmd(m_adc[m_index], ENABLE);
  
  /* Initialize ADC structure */
  ADC_StructInit(&ADC_InitStructure);
  
  /* Configure the ADC1 in continous mode withe a resolutuion equal to 12 bits  */
  ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
  ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
  ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
  ADC_InitStructure.ADC_ScanDirection = ADC_ScanDirection_Upward;
  ADC_Init(m_adc[m_index], &ADC_InitStructure);
 
	/* Channel(s) configuration */
	for(i = 0; i < (m_inc - 1); i++)
	{
		ADC_ChannelConfig(m_adc[i], m_channels[i], ADC_SampleTime_55_5Cycles);
	}
	
  /* ADC Calibration */
  //ADC_GetCalibrationFactor(m_adc[m_index]); // !!! Array out of alignment by 1 position
  
  /* Enable ADCx */
  ADC_Cmd(m_adc[m_index], ENABLE);     
  
  /* Wait the ADCEN falg */
  while(!ADC_GetFlagStatus(m_adc[m_index], ADC_FLAG_ADEN));
	
	/* Channel(s) configuration */
	for(i = 0; i < (m_inc - 1); i++)
	{
		ADC_ChannelConfig(m_adc[i], m_channels[i], ADC_SampleTime_55_5Cycles);
	}
  
  /* ADCx regular Software Start Conv */
  ADC_StartOfConversion(m_adc[m_index]);
}

/*!
 *  \brief Read ADC value (blocking)
 *
 *  Convert voltage and get result.
 *
 *  \return m_value: ADC value
 */

int AnalogIn_DMA :: read_b()
{
	/* Test DMA1 TC flag */
	while((DMA_GetFlagStatus(DMA1_FLAG_TC1)) == RESET ); 
    
	/* Clear DMA TC flag */
	DMA_ClearFlag(DMA1_FLAG_TC1);
    
	return m_value = value[m_rank[m_channel]-1];
}

/*!
 *  \brief Read ADC value
 *
 *  Convert voltage and get result.
 *
 *  \return m_value: ADC value
 */

int AnalogIn_DMA :: read()
{
	return m_value = value[m_rank[m_channel]-1];
}

/*!
 *  \brief Read ADC value (no conversion)
 *
 *  Get last value converted.
 *
 *  \return m_value: ADC value
 */

AnalogIn_DMA :: operator int()
{
	return m_value = value[m_rank[m_channel]-1];
}

/*!
 *  \brief Get ADC channel
 *
 *  Get ADC channel.
 *
 *  \return m_value: ADC value
 */

int GetChannel(GPIO_TypeDef* port, int pin)
{
	int channel = 0;
	
	if(port == GPIOA)
	{
		switch(pin)
		{
			case GPIO_Pin_0: channel = ADC_Channel_0; break;
			case GPIO_Pin_1: channel = ADC_Channel_1; break;
			case GPIO_Pin_2: channel = ADC_Channel_2; break;
			case GPIO_Pin_3: channel = ADC_Channel_3; break;
			case GPIO_Pin_4: channel = ADC_Channel_4; break;
			case GPIO_Pin_5: channel = ADC_Channel_5; break;
			case GPIO_Pin_6: channel = ADC_Channel_6; break;
			case GPIO_Pin_7: channel = ADC_Channel_7; break;
		}
	}	
	else if(port == GPIOB)
	{
		switch(pin)
		{
			case GPIO_Pin_0: channel = ADC_Channel_8; break;
			case GPIO_Pin_1: channel = ADC_Channel_9; break;
		}
	}	
	else if(port == GPIOC)
	{
		switch(pin)
		{
			case GPIO_Pin_0: channel = ADC_Channel_10; break;
			case GPIO_Pin_1: channel = ADC_Channel_11; break;
			case GPIO_Pin_2: channel = ADC_Channel_12; break;
			case GPIO_Pin_3: channel = ADC_Channel_13; break;
			case GPIO_Pin_4: channel = ADC_Channel_14; break;
			case GPIO_Pin_5: channel = ADC_Channel_15; break;
		}
	}
	else if(port == GPIOF)
	{
		channel = ADC_Channel_Vrefint; 
	}
	
	return channel;
}

/*!
 *  \brief Get index
 *
 *  Get index.
 *
 *  \return m_value: ADC value
 */

int GetIndex(int x)
{
	char i;
	
	for(i = 0; i < (sizeof(int)* 8); i++)
	{	
		if((x & (0x01 << i)) != 0)
		{
			return i;
		}	
	}
	
	return 0;
}

/*!
 *  \brief Constructor
 *
 *  AnalogOut constructor (DAC).
 *
 *  \param GPIO_c : DAC pin
 *
 */

AnalogOut :: AnalogOut(GPIO_common GPIO_c)
{
	DAC_InitTypeDef    DAC_InitStructure;
  GPIO_InitTypeDef   GPIO_InitStructure;

	// STM32F0 have Only 1 DAC
	m_port = GPIO_c.port;
	m_pin = GPIO_c.pin;
	
  /* GPIOx Periph clock enable */
	if(m_port == GPIOA) RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
	else if(m_port == GPIOB) RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);
	else if(m_port == GPIOC) RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOC, ENABLE);
	else if(m_port == GPIOD) RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOD, ENABLE);
	else if(m_port == GPIOF) RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOF, ENABLE);
  
	/* Configure "pin" in input mode */
  GPIO_InitStructure.GPIO_Pin = m_pin;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(m_port, &GPIO_InitStructure);

  /* Enable DAC clock */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE);
		
  /* DAC channel1 Configuration */
  DAC_InitStructure.DAC_Trigger = DAC_Trigger_None;
  DAC_InitStructure.DAC_OutputBuffer = DAC_OutputBuffer_Enable;
	
  /* DAC Channel1 Init */
  DAC_Init(DAC_Channel_1, &DAC_InitStructure);
  
  /* Enable DAC Channel1 */
  DAC_Cmd(DAC_Channel_1, ENABLE);
}

/*!
 *  \brief DAC write
 *
 *  Modify output voltage.
 *
 *  \param n: DAC value
 */

void AnalogOut :: write(int n)
{
	DAC_SetChannel1Data(DAC_Align_12b_R, n);
}

/*!
 *  \brief DAC write (shorthand)
 *
 *  Modify output voltage.
 *
 *  \param n: DAC value
 */

AnalogOut& AnalogOut ::  operator= (int n)
{
	DAC_SetChannel1Data(DAC_Align_12b_R, n);
	return *this;
}

/*!
 *  \brief DAC read
 *
 *  Read voltage.
 *
 *  \return DAC value
 */

int AnalogOut :: read()
{
	return DAC_GetDataOutputValue(DAC_Channel_1);
}

/*!
 *  \brief DAC read (shorthand)
 *
 *  Read voltage.
 *
 *  \return DAC value
 */

AnalogOut :: operator int()
{
	return DAC_GetDataOutputValue(DAC_Channel_1);
}
