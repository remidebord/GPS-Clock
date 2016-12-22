/*!
 * \file I2C.cpp
 * \brief I2C API.
 * \author Rémi.Debord
 * \version 1.0
 * \date 19 novembre 2013
 *
 * I2C library (400kHz).
 *
 */

#include "I2C.h"

/*!
 *  \brief Constructor
 *
 *  I2C constructor.
 *
 *  \param i2cx : I2C number (I2C1 or I2C2)
 *  \param sda : data pin
 *  \param scl : clock pin
 *
 */

I2C :: I2C(I2C_common* i2cx, const GPIO_common* sda, const GPIO_common* scl, uint8_t address)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	I2C_InitTypeDef  I2C_InitStructure;
	
	m_i2c = i2cx;
	
	m_sda = sda;
	m_scl = scl;
	
	(m_i2c->buffer) = &m_buffer[0];
	(m_i2c->busy) = &m_busy;
	(m_i2c->index) = &m_index;
	
	m_busy = 0;
	m_index = 0;
	
	/* SDA/SCL GPIO clock enable */
  RCC_AHBPeriphClockCmd(m_sda->peripheralClock, ENABLE);
	RCC_AHBPeriphClockCmd(m_scl->peripheralClock, ENABLE);
	
	/* Connect pins to I2Cx Alternate Function mapping */
	GPIO_PinAFConfig(m_sda->port, m_sda->pinSource, GPIO_AF_1);
	GPIO_PinAFConfig(m_scl->port, m_scl->pinSource, GPIO_AF_1);

	/* GPIO Configuration SDA and SCL */
  GPIO_InitStructure.GPIO_Pin = m_sda->pin;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
	GPIO_Init(m_sda->port, &GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin = m_scl->pin;
  GPIO_Init(m_scl->port, &GPIO_InitStructure);
	
	/* Enable I2C interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = m_i2c->irqVector;
  NVIC_InitStructure.NVIC_IRQChannelPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
	
  /* I2C Peripheral clock enable */
  RCC_APB1PeriphClockCmd(m_i2c->peripheralClock, ENABLE);
	
	/* Configure the I2C clock source */
  if(m_i2c->bus == I2C1) RCC_I2CCLKConfig(RCC_I2C1CLK_HSI);
	
	/* I2C De-initialize (!important) */
  I2C_DeInit(m_i2c->bus);
	
  /* I2C configuration */
	I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;			/* I2C mode (I2C, SMBus, ...) */
	I2C_InitStructure.I2C_AnalogFilter = I2C_AnalogFilter_Enable;
  I2C_InitStructure.I2C_DigitalFilter = 0x00;
	I2C_InitStructure.I2C_OwnAddress1 = address;		/* MCU address */
  I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;			/* Acknowledge enable */
	
	if(m_i2c->bus == I2C1) I2C_InitStructure.I2C_Timing = 0x0030020B; 					// Speed !!! 400KHz | 8MHz (HSI) => 0x0030020A
  else if(m_i2c->bus == I2C2) I2C_InitStructure.I2C_Timing =  0x10950C27;		// Speed !!! 400KHz | 48MHz (PCCLK => HSE) => 0x10950C27
	
	I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;	/* 7-bit address is acknowledged */

  /* I2C Initialization */
  I2C_Init(m_i2c->bus, &I2C_InitStructure);

	/* IRQ configuration */
	if(address != 0x00)
	{
		I2C_ITConfig(m_i2c->bus, I2C_IT_ADDRI, ENABLE);
	}
	I2C_ITConfig(m_i2c->bus, I2C_IT_TXI, ENABLE);
	I2C_ITConfig(m_i2c->bus, I2C_IT_TCI, ENABLE);
	I2C_ITConfig(m_i2c->bus, I2C_IT_RXI, ENABLE);
	I2C_ITConfig(m_i2c->bus, I2C_IT_NACKI, ENABLE);
	I2C_ITConfig(m_i2c->bus, I2C_IT_STOPI, ENABLE);
	
  /* I2C ENABLE */
  I2C_Cmd(m_i2c->bus, ENABLE);
}

/*!
 *  \brief I2C Read (non blocking call)
 *
 *  Read (non blocking).
 *
 *  \param address : device address
 *  \param buffer : command byte(s)
 *  \param length : number of command byte 
 *
 *  \return 1 or 0
 */

uint8_t I2C :: read(uint8_t address, uint8_t* buffer, uint8_t length)
{
	uint8_t result = 1;
	uint32_t timeout = length * I2C_TIMEOUT;
	uint8_t i = 0;
	
	/* Periheral busy ? */
	if(I2C_GetFlagStatus(m_i2c->bus, I2C_FLAG_BUSY) || m_busy)
	{
		result = 0;
	}
	else
	{
		m_index = 0;
		m_busy = 1;
		
		/* Send first Start and control byte (slave address + R/W) */
		I2C_TransferHandling(m_i2c->bus, address, length, I2C_SoftEnd_Mode, I2C_Generate_Start_Read);	
		
		/* Wait data transfer */
		while(m_index != length)
		{
			if((timeout--) == 0)
			{
				m_index = length;
				result = 0;
			}
		}
		
		m_busy = 0;
	
		/* Copy data */
		for(i = 0; i < length; i++)
		{
			buffer[i] = m_buffer[i];
		}
	}
	
	return result;
}

/*!
 *  \brief I2C Read slave(non blocking call)
 *
 *  Return data received (slave).
 *
 *  \param buffer : data
 *  \return length
 */

uint8_t I2C :: read_s(uint8_t* buffer)
{
	uint8_t length = 0;
	uint8_t i = 0;
	
	/* Periheral busy ? */
	if(I2C_GetFlagStatus(m_i2c->bus, I2C_FLAG_BUSY) || m_busy)
	{
		length = 0;
	}
	else
	{
		/* Data received ? */
		if(m_index > 0)
		{
			length = m_index;
			
			for(i = 0; i < length; i++)
			{
				buffer[i] = m_buffer[i];
			}
			
			m_index = 0;
		}
	}

	return length;
}

/*!
 *  \brief I2C Write (non blocking call)
 *
 *  Write data to i2c peripheral (non blocking).
 *
 *  \param address : device address
 *  \param buffer : command byte(s)
 *  \param length : number of command byte 
 *
 *  \return 1 or 0
 */

uint8_t I2C :: write(uint8_t address, uint8_t* buffer, uint8_t length)
{
	uint8_t result = SUCCESS;
	uint8_t i = 0;
	
	/* Periheral busy ? */
	if(I2C_GetFlagStatus(m_i2c->bus, I2C_FLAG_BUSY) || m_busy)
	{
		result = 0;
	}
	else
	{
		m_index = 0;
		m_busy = 1;
		
		/* Copy data to i2c buffer */
		for(i = 0; i < length; i++)
		{
			m_buffer[i] = buffer[i];
		}
		
		/* Send first Start and control byte (slave address + R/W) */
		I2C_TransferHandling(m_i2c->bus, address, length, I2C_SoftEnd_Mode, I2C_Generate_Start_Write);
	}
	
	return result;
}

/*!
 *  \brief I2C Write (blocking call)
 *
 *  Write data (blocking).
 *
 *  \param address : device address
 *  \param buffer : command byte(s)
 *  \param length : number of command byte 
 *
 *  \return 1 or 0
 */
 
uint8_t I2C :: write_b(uint8_t address, uint8_t* buffer, uint8_t length)
{
	uint8_t result = 1;
	uint8_t i = 0;
	
	/* Periheral busy ? */
	if(I2C_GetFlagStatus(m_i2c->bus, I2C_FLAG_BUSY) || m_busy)
	{
		result = 0;
	}
	else
	{
		m_index = 0;
		m_busy = 1;
		
		/* Copy data to i2c buffer */
		for(i = 0; i < length; i++)
		{
			m_buffer[i] = buffer[i];
		}
		
		/* Send first Start and control byte (slave address + R/W) */
		I2C_TransferHandling(m_i2c->bus, address, length, I2C_SoftEnd_Mode, I2C_Generate_Start_Write);
		
		while(m_busy);		
	}
	
	return result;
}

/*!
 *  \brief I2C busy
 *
 *  I2C is busy ?
 *
 *  \return 1 if busy else 0
 */

uint8_t I2C :: busy(void)
{
	uint8_t result = 0;
	
	if(I2C_GetFlagStatus(m_i2c->bus, I2C_FLAG_BUSY) || m_busy)
	{
		result = 1;
	}
	else
	{
		result = 0;
	}
	
	return result;
}

/*!
 *  \brief I2C IRQ handler
 */

extern "C"
{	
	void I2C1_IRQHandler(void)
	{
		/* Address matched (slave) */
		if(I2C_GetITStatus(I2C1, I2C_IT_ADDR))
		{
			I2C_GetAddressMatched(I2C1);
			
			if(I2C_GetTransferDirection(I2C1) == I2C_Direction_Receiver)
			{
				// TODO:
				// + Callback (send data to master)
				// + Set transmit IT
			}
			
			I2C_ClearFlag(I2C1, I2C_IT_ADDR);
		}
		
		/* Transmit complete */
		if(I2C_GetITStatus(I2C1, I2C_IT_TC))
		{
			I2C_GenerateSTOP(I2C1, ENABLE);		
		}
		
		/* Transmit buffer interrupt status */
		if(I2C_GetITStatus(I2C1, I2C_IT_TXIS))
		{
			I2C_SendData(I2C1, (uint8_t) I2C_1.buffer[*(I2C_1.index)]);
			*(I2C_1.index) = *(I2C_1.index) + 1;
		}
		
		/* Receive buffer interrupt status */
		if(I2C_GetITStatus(I2C1, I2C_IT_RXNE))
		{
			I2C_1.buffer[*(I2C_1.index)] = I2C_ReceiveData(I2C1);
			
			if(*(I2C_1.index) < 0x7F) *(I2C_1.index) = *(I2C_1.index) + 1;
			else *(I2C_1.index) = 0;
		}
		
		/* Clear NACK error */
		if(I2C_GetITStatus(I2C1, I2C_IT_NACKF))
		{
			I2C_ClearITPendingBit(I2C1, I2C_IT_NACKF);
			*(I2C_1.busy) = 0;
		}
		
		/* Stop generation end */
		if(I2C_GetITStatus(I2C1, I2C_IT_STOPF))
		{
			I2C_ClearITPendingBit(I2C1, I2C_IT_STOPF);
			I2C_GenerateSTOP(I2C1, DISABLE);
			*(I2C_1.busy) = 0;
		}
	}
	
	void I2C2_IRQHandler(void)
	{
		/* Address matched (slave) */
		if(I2C_GetITStatus(I2C2, I2C_IT_ADDR))
		{
			I2C_GetAddressMatched(I2C2);
			
			if(I2C_GetTransferDirection(I2C2) == I2C_Direction_Receiver)
			{
				// TODO:
				// + Callback (send data to master)
				// + Set transmit IT
			}
			
			I2C_ClearFlag(I2C2, I2C_IT_ADDR);
		}
		
		/* Transmit complete */
		if(I2C_GetITStatus(I2C2, I2C_IT_TC))
		{
			I2C_GenerateSTOP(I2C2, ENABLE);		
		}
		
		/* Transmit buffer interrupt status */
		if(I2C_GetITStatus(I2C2, I2C_IT_TXIS))
		{
			I2C_SendData(I2C2, (uint8_t) I2C_2.buffer[*(I2C_2.index)]);
			*(I2C_2.index) = *(I2C_2.index) + 1;
		}
		
		/* Receive buffer interrupt status */
		if(I2C_GetITStatus(I2C2, I2C_IT_RXNE))
		{			
			I2C_2.buffer[*(I2C_2.index)] = I2C_ReceiveData(I2C2);

			if(*(I2C_2.index) < 0x7F) *(I2C_2.index) = *(I2C_2.index) + 1;
			else *(I2C_2.index) = 0;
		}
		
		/* Clear NACK error */
		if(I2C_GetITStatus(I2C2, I2C_IT_NACKF))
		{
			I2C_ClearITPendingBit(I2C2, I2C_IT_NACKF);
			*(I2C_2.busy) = 0;
		}
		
		/* Stop generation end */
		if(I2C_GetITStatus(I2C2, I2C_IT_STOPF))
		{
			I2C_ClearITPendingBit(I2C2, I2C_IT_STOPF);
			I2C_GenerateSTOP(I2C2, DISABLE);
			*(I2C_2.busy) = 0;
		}
	}
}
