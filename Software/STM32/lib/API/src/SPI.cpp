/*!
 * \file SPI.cpp
 * \brief Serial Peripheral Interface API.
 * \author Rémi.Debord
 * \version 1.0
 * \date 19 novembre 2013
 *
 * SPI library.
 *
 */

#include "SPI.h"

/*!
 *  \brief Constructor
 *
 *  SPI contructor.
 *
 *  \param SPIx : SPI number (SPI1 or SPI2)
 *  \param GPIO_c_cs : chip select pin
 *  \param GPIO_c_sck : clock pin
 *  \param GPIO_c_mosi : data out pin
 *  \param GPIO_c_miso : data in pin
 *
 */

SPI :: SPI(SPI_TypeDef* SPIx, GPIO_common GPIO_c_cs, GPIO_common GPIO_c_sck, GPIO_common GPIO_c_mosi, GPIO_common GPIO_c_miso)
{
	this->init(SPIx, GPIO_c_cs, GPIO_c_sck, GPIO_c_mosi, GPIO_c_miso);
}

/*!
 *  \brief Constructor
 *
 *  SPI contructor (no parameter)
 *
 */

SPI :: SPI(void)
{
}

/*!
 *  \brief Initialize SPI
 *
 *  SPI initialization.
 *
 *  \param SPIx : SPI number (SPI1 or SPI2)
 *  \param GPIO_c_cs : chip select pin
 *  \param GPIO_c_sck : clock pin
 *  \param GPIO_c_mosi : data out pin
 *  \param GPIO_c_miso : data in pin
 *
 */

void SPI :: init(SPI_TypeDef* SPIx, GPIO_common GPIO_c_cs, GPIO_common GPIO_c_sck, GPIO_common GPIO_c_mosi, GPIO_common GPIO_c_miso)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
  SPI_InitTypeDef   SPI_InitStructure;
	
	m_spi = SPIx;
	
	m_csPort = GPIO_c_cs.port;
	m_csPin = GPIO_c_cs.pin;
	
	m_sckPort = GPIO_c_sck.port;
	m_sckPin = GPIO_c_sck.pin;
	m_sckPinSource = GPIO_c_sck.pinSource;
	
	m_mosiPort = GPIO_c_mosi.port;
	m_mosiPin = GPIO_c_mosi.pin;
	m_mosiPinSource = GPIO_c_mosi.pinSource;
	
	m_misoPort = GPIO_c_miso.port;
	m_misoPin = GPIO_c_miso.pin;
	m_misoPinSource = GPIO_c_miso.pinSource;

  /* GPIO Periph clock enable */
  if(m_csPort == GPIOA) RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
	else if(m_csPort == GPIOB) RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);
	else if(m_csPort == GPIOC) RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOC, ENABLE);
	else if(m_csPort == GPIOD) RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOD, ENABLE);
	
  if(m_sckPort == GPIOA) RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
	else if(m_sckPort == GPIOB) RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);
	else if(m_sckPort == GPIOC) RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOC, ENABLE);
	else if(m_sckPort == GPIOD) RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOD, ENABLE);
	
	if(m_mosiPort == GPIOA) RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
	else if(m_mosiPort == GPIOB) RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);
	else if(m_mosiPort == GPIOC) RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOC, ENABLE);
	else if(m_mosiPort == GPIOD) RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOD, ENABLE);
	
	if(m_misoPort == GPIOA) RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
	else if(m_misoPort == GPIOB) RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);
	else if(m_misoPort == GPIOC) RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOC, ENABLE);
	else if(m_misoPort == GPIOD) RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOD, ENABLE);

  /* SD_SPI Periph clock enable */
  if(m_spi == SPI1) RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
	else if(m_spi == SPI2) RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);

  /* Configure SCK pin */
  GPIO_InitStructure.GPIO_Pin = m_sckPin;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
  GPIO_Init(m_sckPort, &GPIO_InitStructure);
	
  /* Configure MOSI pin */
  GPIO_InitStructure.GPIO_Pin = m_mosiPin;
  GPIO_Init(m_mosiPort, &GPIO_InitStructure);
	
  /* Configure MISO pin */
  GPIO_InitStructure.GPIO_Pin = m_misoPin;
  GPIO_Init(m_misoPort, &GPIO_InitStructure);

  /* Configure CS pin */
  GPIO_InitStructure.GPIO_Pin = m_csPin;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(m_csPort, &GPIO_InitStructure);

  /* Alternate function */
  GPIO_PinAFConfig(m_sckPort, m_sckPinSource, GPIO_AF_0);
	GPIO_PinAFConfig(m_mosiPort, m_mosiPinSource, GPIO_AF_0);
  GPIO_PinAFConfig(m_misoPort, m_misoPinSource, GPIO_AF_0);
  
  /*!< SPI Config */
  SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
  SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
  SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
  SPI_InitStructure.SPI_CPOL = SPI_CPOL_High; 		//SPI_CPOL_High;
  SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge; 	//SPI_CPHA_2Edge;
  SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
  SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;

  SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB; //SPI_FirstBit_LSB;
  SPI_InitStructure.SPI_CRCPolynomial = 7;
  SPI_Init(m_spi, &SPI_InitStructure);
  
  //SPI_RxFIFOThresholdConfig(SPI, SPI_RxFIFOThreshold_QF);
	
	/* Disable CRC */
	SPI_CalculateCRC(m_spi, DISABLE);
	
	/* Disable SPIx NSS output for master mode */
  SPI_SSOutputCmd(m_spi, DISABLE);
  
  SPI_Cmd(m_spi, ENABLE); /*!< SPI enable */
}

/*!
 *  \brief Change chip select state
 *
 *  Modify chip select state (CS).
 *
 *  \param x: cs state
 */

void SPI :: cs(char x)
{
	if(x == 0) GPIO_ResetBits(m_csPort, m_csPin);
	else GPIO_SetBits(m_csPort, m_csPin);
}

/*!
 *  \brief SPI write (blocking)
 *
 *  Send a character.
 *
 *  \param c: character to send
 */

void SPI :: write(char c)
{
	/* Wait until the transmit buffer is empty */
  while (SPI_I2S_GetFlagStatus(m_spi, SPI_I2S_FLAG_TXE) == RESET);
	
	SPI_SendData8(m_spi, c);
}

/*!
 *  \brief SPI read (blocking)
 *
 *  Read a character.
 *
 *  \return character read
 */

char SPI :: read(void)
{
	/* Wait until a data is received */
  while (SPI_I2S_GetFlagStatus(m_spi, SPI_I2S_FLAG_RXNE) == RESET);
	
	return SPI_ReceiveData8(m_spi);
}
