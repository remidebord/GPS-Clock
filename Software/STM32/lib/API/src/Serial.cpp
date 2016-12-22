/*!
 * \file Serial.cpp
 * \brief USART API.
 * \author Rémi.Debord
 * \version 2.0
 * \date 19 novembre 2013
 *
 * USART library.
 *
 */

#include "Serial.h"
#include <stdlib.h>

extern "C"
{
	Serial *serial1;
	Serial *serial2;

	volatile char USART1_usartRxTimer = 0;
	int USART1_index = 0;
	int USART1_length = 0;
	char USART1_TxBuffer[MAX_USART_RX_BUF_LEN] = {};
	
	volatile char USART2_usartRxTimer = 0;
	int USART2_index = 0;
	int USART2_length = 0;
	char USART2_TxBuffer[MAX_USART_RX_BUF_LEN] = {};
}

/*!
 *  \brief Constructor
 *
 *  Serial constructor.
 *
 *  \param USARTx : USART number (USART1 or USART2)
 *  \param GPIO_c_rx : RX pin
 *  \param GPIO_c_tx : TX pin
 *
 */

Serial :: Serial(USART_TypeDef* USARTx, GPIO_common GPIO_c_rx, GPIO_common GPIO_c_tx, GPIO_common GPIO_c_de)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	USART_InitTypeDef USART_InitStructure;

	m_rxPort = GPIO_c_rx.port;
	m_rxPin = GPIO_c_rx.pin;
	m_rxPinSource = GPIO_c_rx.pinSource;
	
	m_txPort = GPIO_c_tx.port;
	m_txPin = GPIO_c_tx.pin;
	m_txPinSource = GPIO_c_tx.pinSource;
	
	m_baudrate = DEFAULT_BAUDRATE;
	m_usart = USARTx;
	m_timeout = USART_ENDOFFRAME_RX_TIME;
	m_mode = USART_Mode_Rx | USART_Mode_Tx;
	
	m_rs485 = (GPIO_c_de.port != GPIOF) ? 1 : 0;
	
	//m_rxBuffer = (char*) calloc(MAX_USART_RX_BUF_LEN, sizeof(char));
	
	if(m_usart == USART1)
	{
		serial1 = this;
		m_rxTimer = &USART1_usartRxTimer;
		m_txIndex = &USART1_index;
		m_txLength = &USART1_length;
		m_txBuffer = USART1_TxBuffer;
	}
	else if(m_usart == USART2)
	{
		serial2 = this;
		m_rxTimer = &USART2_usartRxTimer;
		m_txIndex = &USART2_index;
		m_txLength = &USART2_length;
		m_txBuffer = USART2_TxBuffer;
	}
	
  /* Enable GPIO clock */
  if(m_rxPort == GPIOA) RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
	else if(m_rxPort == GPIOB) RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);
	else if(m_rxPort == GPIOC) RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOC, ENABLE);
	else if(m_rxPort == GPIOD) RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOD, ENABLE);
	
	if(m_txPort == GPIOA) RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
	else if(m_txPort == GPIOB) RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);
	else if(m_txPort == GPIOC) RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOC, ENABLE);
	else if(m_txPort == GPIOD) RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOD, ENABLE);
	
	if(m_rs485)
	{
		if(GPIO_c_de.port == GPIOA) RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
		else if(GPIO_c_de.port == GPIOB) RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);
		else if(GPIO_c_de.port == GPIOC) RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOC, ENABLE);
		else if(GPIO_c_de.port == GPIOD) RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOD, ENABLE);
	}
  
  /* Enable USART clock */
  if(m_usart == USART1) RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	else if(m_usart == USART2) RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

  /* Connect USART pins to alternate function */
  if(m_rxPort == GPIOA) GPIO_PinAFConfig(GPIOA, m_rxPinSource, GPIO_AF_1);
	else if(m_rxPort == GPIOB) GPIO_PinAFConfig(GPIOB, m_rxPinSource, GPIO_AF_0);
	
	if(m_txPort == GPIOA) GPIO_PinAFConfig(GPIOA, m_txPinSource, GPIO_AF_1);
	else if(m_txPort == GPIOB) GPIO_PinAFConfig(GPIOB, m_txPinSource, GPIO_AF_0);
	
	if(m_rs485)
	{
		if(GPIO_c_de.port == GPIOA) GPIO_PinAFConfig(GPIOA, GPIO_c_de.pinSource, GPIO_AF_1);
		else if(GPIO_c_de.port == GPIOB) GPIO_PinAFConfig(GPIOB, GPIO_c_de.pinSource, GPIO_AF_0);
	}
  
  /* Configure USART Tx and Rx as alternate function push-pull (optional DE) */
  GPIO_InitStructure.GPIO_Pin = m_rxPin;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Level_3;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
  GPIO_Init(m_rxPort, &GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin = m_txPin;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_Init(m_txPort, &GPIO_InitStructure);
	
	if(m_rs485)
	{
		GPIO_InitStructure.GPIO_Pin = GPIO_c_de.pin;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
		GPIO_Init(GPIO_c_de.port, &GPIO_InitStructure);
	}
	
  /* Enable the USARTx Interrupt */
  if(m_usart == USART1) NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	else if(m_usart == USART2) NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	
  NVIC_InitStructure.NVIC_IRQChannelPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
	
	/* USARTx configured as follow:
        - BaudRate = 9600 baud  
        - Word Length = 8 Bits
        - One Stop Bit
        - No parity
        - Hardware flow control disabled (RTS and CTS signals)
        - Receive and transmit enabled
  */
	
  USART_InitStructure.USART_BaudRate = m_baudrate;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = m_mode;

  USART_Init(m_usart, &USART_InitStructure);
	
  /* Enable USART TX and RX interrupt */
	//USART_ITConfig(m_usart, USART_IT_TXE, ENABLE);
  USART_ITConfig(m_usart, USART_IT_RXNE, ENABLE);
	
	// Disable Overrun detection
	USART_OverrunDetectionConfig(m_usart, USART_OVRDetection_Disable);
	
	if(m_rs485)
	{
		// Assertion time max value is (2^5)-1 = 31 (=> 2 bit time with oversampling by 16)
		USART_SetDEAssertionTime(m_usart, 31);
		USART_SetDEDeassertionTime(m_usart, 31);
		
		USART_DECmd(m_usart, ENABLE);
	}
	
	USART_Cmd(m_usart, ENABLE);
}

/*!
 *  \brief Constructor
 *
 *  Serial constructor (Rx or Tx only).
 *
 *  \param USARTx : USART number (USART1 or USART2)
 *  \param GPIO_c_pin : Rx or Tx pin
 *  \param mode : Rx or Tx
 *
 */

Serial :: Serial(USART_TypeDef* USARTx, GPIO_common GPIO_c_pin, char mode)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	
	m_baudrate = DEFAULT_BAUDRATE;
	m_usart = USARTx;
	m_timeout = USART_ENDOFFRAME_RX_TIME;
	m_mode = mode;
	
	//m_rxBuffer = (char*) calloc(MAX_USART_RX_BUF_LEN, sizeof(char));
	
	if(m_usart == USART1)
	{
		serial1 = this;
		m_rxTimer = &USART1_usartRxTimer;
		m_txIndex = &USART1_index;
		m_txLength = &USART1_length;
		m_txBuffer = USART1_TxBuffer;
	}
	else if(m_usart == USART2)
	{
		serial2 = this;
		m_rxTimer = &USART2_usartRxTimer;
		m_txIndex = &USART2_index;
		m_txLength = &USART2_length;
		m_txBuffer = USART2_TxBuffer;
	}
	
  /* Enable GPIO clock */
	if(GPIO_c_pin.port == GPIOA) RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
	else if(GPIO_c_pin.port == GPIOB) RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);
	else if(GPIO_c_pin.port == GPIOC) RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOC, ENABLE);
	else if(GPIO_c_pin.port == GPIOD) RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOD, ENABLE);
  
  /* Enable USART clock */
  if(m_usart == USART1) RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	else if(m_usart == USART2) RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

  /* Connect USART pins to alternate function */
	if(GPIO_c_pin.port == GPIOA) GPIO_PinAFConfig(GPIOA, GPIO_c_pin.pinSource, GPIO_AF_1);
	else if(GPIO_c_pin.port == GPIOB) GPIO_PinAFConfig(GPIOB, GPIO_c_pin.pinSource, GPIO_AF_0);
  
  /* Configure USART Tx or Rx as alternate function push-pull */
  GPIO_InitStructure.GPIO_Pin = GPIO_c_pin.pin;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Level_3;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
  GPIO_Init(GPIO_c_pin.port, &GPIO_InitStructure);
	
  /* Enable the USARTx Interrupt */
  if(m_usart == USART1) NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	else if(m_usart == USART2) NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	
  NVIC_InitStructure.NVIC_IRQChannelPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
	
	/* USARTx configured as follow:
        - BaudRate = 9600 baud  
        - Word Length = 8 Bits
        - One Stop Bit
        - No parity
        - Hardware flow control disabled (RTS and CTS signals)
        - Receive and transmit enabled
  */
	
  USART_InitStructure.USART_BaudRate = m_baudrate;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; 
  USART_InitStructure.USART_Mode = m_mode;

  USART_Init(m_usart, &USART_InitStructure);
	
  /* Enable USART TX or RX interrupt */
	if((m_mode & USART_Mode_Rx) != 0)
	{
		USART_ITConfig(m_usart, USART_IT_RXNE, ENABLE);
	}
	else if((m_mode & USART_Mode_Tx) != 0)
	{
		//USART_ITConfig(m_usart, USART_IT_TXE, ENABLE);
	}  
	
	// Disable Overrun detection
	USART_OverrunDetectionConfig(m_usart, USART_OVRDetection_Disable);
	
	USART_Cmd(m_usart, ENABLE);
}

/*!
 *  \brief Change baudrate
 *
 *  Modify serial port speed.
 *
 *  \param baudrate: usar speed
 */

void Serial :: baud(int baudrate)
{
	USART_InitTypeDef USART_InitStructure;
	
	m_baudrate = baudrate;
	
	/* Disable USART ! */
	USART_Cmd(m_usart, DISABLE);
	
	USART_InitStructure.USART_BaudRate = m_baudrate;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = m_mode;

  USART_Init(m_usart, &USART_InitStructure);
	
	if(m_rs485)
	{
		USART_SetDEAssertionTime(m_usart, 31);
		USART_SetDEDeassertionTime(m_usart, 31);
	}
	
	USART_Cmd(m_usart, ENABLE);
}

/*!
 *  \brief Receive character
 *
 *  Get character and place it in a circular buffer.
 *
 */

void Serial :: receive(void)
{
	m_rxBuffer[m_rxHead++] = USART_ReceiveData(m_usart);
			
	/* To avoid buffer overflow */
	if(m_rxHead >= MAX_USART_RX_BUF_LEN)
	{
		m_rxHead = 0;
	}
	
	/* Reload timer (delay between two bytes) */
	*m_rxTimer = m_timeout;
}

/*!
 *  \brief USART write (non blocking call)
 *
 *  Send data to serial port.
 *
 *  \param buffer: data array
 *  \param length: data length
 */

int Serial :: write(char *buffer, int length)
{
	int i;
	
	if((m_mode & USART_Mode_Tx) == 0) return 0;
	
	if(*m_txLength != 0) return 0;
	
	*m_txLength = length;
	*m_txIndex = 0;
	
	/* Copy data to USART buffer */
	for(i = 0; i < length; i++, buffer++)
	{
		*(m_txBuffer + i) = *buffer;
	}
	
	USART_ITConfig(m_usart, USART_IT_TXE, ENABLE);
	
	return 1;
}

/*!
 *  \brief USART write (blocking call)
 *
 *  Send data to serial port (blocking).
 *
 *  \param buffer: data array
 *  \param length: data length
 */

int Serial :: write_b(char *buffer, int length)
{
	int i;
	
	if((m_mode & USART_Mode_Tx) == 0) return 0;
	
	if(*m_txLength != 0) return 0;
	
	*m_txLength = length;
	*m_txIndex = 0;
	
	/* Copy data to USART buffer */
	for(i = 0; i < length; i++, buffer++)
	{
		*(m_txBuffer + i) = *buffer;
	}
	
	USART_ITConfig(m_usart, USART_IT_TXE, ENABLE);
	
	while(*m_txLength != 0);
	
	return 1;
}

/*!
 *  \brief USART read
 *
 *  Read data.
 *
 *  \param buffer: data array (out)
 *  \return length
 */

int Serial :: read(char *buffer)
{
	int i = 0;
	int length = 0;
	
	if((m_mode & USART_Mode_Rx) == 0) return 0;
	
	if(*m_rxTimer > 0)
	{
		return 0;
	}
	else
	{
		/* Reload timer */
		*m_rxTimer = m_timeout;
		
		/* Get head and tail position */
		if(m_rxHead >= m_rxTail) length = m_rxHead - m_rxTail;
		else length = m_rxHead + (MAX_USART_RX_BUF_LEN - m_rxTail);
		
		/* head != tail */
		if(m_rxHead == m_rxTail) return 0;
		else
		{			
			for(i = 0; i < length; i++, buffer++)
			{
				*buffer = m_rxBuffer[m_rxTail++];
				
				if(m_rxTail >= MAX_USART_RX_BUF_LEN)
				{
					m_rxTail = 0;
				}
			}
				
			return length;
		}
	}
}


/*!
 *  \brief USART busy
 *
 *  Get usart status.
 *
 *  \return >0 if busy else 0
 */

char Serial :: busy(void)
{
	if((char)USART_GetFlagStatus(m_usart, USART_FLAG_BUSY) ||
	   (char)!USART_GetFlagStatus(m_usart, USART_FLAG_TXE))
	{
		return 1;
	}
	
	return 0;
}

/*!
 *  \brief Max delay between two bytes
 *
 *  Timeout between two bytes.
 *
 *  \param value: delay in ms
 */

void Serial :: timeout(char value)
{
	m_timeout = value;
}

/*!
 *  \brief USART IRQ handler
 */

extern "C"
{
	void USART1_IRQHandler(void)
	{
		if(USART_GetITStatus(USART1, USART_IT_TXE) != RESET)
		{   
			USART_SendData(USART1, USART1_TxBuffer[USART1_index++]);

			if(USART1_index == USART1_length)
			{
				USART1_length = 0;
				USART_ITConfig(USART1, USART_IT_TXE, DISABLE);
			}
		}

		if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
		{
			serial1->receive();
		}
	}
	
	void USART2_IRQHandler(void)
	{
		if(USART_GetITStatus(USART2, USART_IT_TXE) != RESET)
		{   
			USART_SendData(USART2, USART2_TxBuffer[USART2_index++]);

			if(USART2_index == USART2_length)
			{
				USART2_length = 0;
				USART_ITConfig(USART2, USART_IT_TXE, DISABLE);
			}
		}
		
		if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
		{			
			serial2->receive();
		}
	}
}
