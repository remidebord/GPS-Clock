#include "PN532.h"
#include <string.h>

PN532 :: PN532(I2C* i2cx)
{
	m_i2c = i2cx;
	m_state = 0;
	m_timer = PN532_POLLING_TIME;
}

void PN532 :: update(void)
{
	char ack[] = {0x00, 0x00, 0xFF, 0x00, 0xFF};
	char buffer[MAX_I2C_BUF_LEN] = "";
	char data [MAX_I2C_BUF_LEN] = "";
	char length = 0, i = 0;
	
	switch(m_state)
	{
		case 0:
			
			if(m_timer == 0)
			{
				m_timer = PN532_POLLING_TIME;
				
				data[0] = 0x01; // Read 1 card (max 2)
				data[1] = 0x00; // 106 kbps type A (ISO/IEC14443 Type A)
				
				// Create frame inlist passive target (0x4A)
				length = this->createFrame(buffer, 0x4A, data, 2);
				
				// Send frame to PN532
				if(m_i2c->write(PN532_ADDRESS, buffer, length) == ERROR)
				{
					m_state = 0;
				}
				else
				{
					m_retry = 0;
					m_timer = PN532_TIMEOUT;
					m_state = 1;
				}
			}
			break;
			
		case 1:
			
			if(m_timer == 0)
			{
				// PN532 read (RDY)
				if(m_i2c->read(PN532_ADDRESS, buffer, 1) == ERROR)
				{
					m_timer = PN532_POLLING_TIME;
					m_state = 0;
				}
				else
				{
					m_timer = PN532_TIMEOUT;
					
					// PN532 ready ?
					if(buffer[0] == 0x01)
					{
						m_state = 2;
					}
					else
					{
						if(m_retry >= 3)
						{
							m_timer = PN532_POLLING_TIME;
							m_state = 0;
						}
						else m_retry++;
					}
				}
			}
			break;
			
		case 2:
			
			if(m_timer == 0)
			{
				// PN532 read
				if(m_i2c->read(PN532_ADDRESS, buffer, 7) == ERROR)
				{
					m_timer = PN532_POLLING_TIME;
					m_state = 0;
				}
				else
				{
					// ACK ?
					if(strcmp(ack, buffer+1) != 0)
					{
						m_timer = PN532_POLLING_TIME;
						m_state = 0;
					}
					else
					{
						m_timer = PN532_TIMEOUT;
						m_state = 3;
					}
				}
			}
			break;
			
		case 3:
			
			if(m_timer == 0)
			{
				// PN532 read
				if(m_i2c->read(PN532_ADDRESS, buffer, 26) == ERROR)
				{
					m_timer = PN532_POLLING_TIME;
					m_state = 0;
				}
				else
				{
					// RDY + PREAMBLE ?
					if((buffer[0] == 0x01) && (buffer[1] == 0x00) && (buffer[2] == 0x00) && (buffer[3] == 0xFF))
					{
						m_flag = 1;
						m_length = buffer[13];
						
						for(i = 0; i < m_length; i++)
						{
							m_uid[i] = buffer[i+14];
						}
						
						m_timer = PN532_POLLING_TIME;
						m_state = 0;
					}
					else
					{
						m_timer = PN532_TIMEOUT;
					}
				}
			}
			
			break;
	}
}

char PN532 :: getUID(char* uid)
{
	char i;
	
	// New card ?
	if(m_flag)
	{
		m_flag = 0;
		
		for(i = 0; i < m_length; i++)
		{
			uid[i] = m_uid[i];
		}
		
		return m_length;
	}
	else return 0;
}

char PN532 :: getFirmware(void)
{
	char ack[] = {0x00, 0x00, 0xFF, 0x00, 0xFF};
	char buffer[MAX_I2C_BUF_LEN];
	char length = 0;

	// Create frame get firmware version (0x02)
	length = this->createFrame(buffer, 0x02, 0, 0);

	this->delay(PN532_TIMEOUT);

	// Send frame to PN532
	if(m_i2c->write_b(PN532_ADDRESS, buffer, length) == ERROR)
	{
		m_timer = PN532_POLLING_TIME;
		return ERROR;
	}
					
	this->delay(PN532_TIMEOUT);
	
	// PN532 read (RDY)
	if(m_i2c->read(PN532_ADDRESS, buffer, 1) == ERROR)
	{
		m_timer = PN532_POLLING_TIME;
		return ERROR;
	}
	else
	{
		// PN532 not ready ?
		if(buffer[0] != 0x01)
		{
			m_timer = PN532_POLLING_TIME;
			return ERROR;
		}
	}
	
	this->delay(PN532_TIMEOUT);
	
	// PN532 read
	if(m_i2c->read(PN532_ADDRESS, buffer, 7) == ERROR)
	{
		m_timer = PN532_POLLING_TIME;
		return ERROR;
	}
	else
	{
		// ACK ?
		if(strcmp(ack, buffer+1) != 0)
		{
			m_timer = PN532_POLLING_TIME;
			return ERROR;
		}
	}
	
	this->delay(PN532_TIMEOUT);
					
	// PN532 read
	if(m_i2c->read(PN532_ADDRESS, buffer, 14) == ERROR)
	{
		m_timer = PN532_POLLING_TIME;
		return ERROR;
	}
	else
	{
		m_timer = PN532_POLLING_TIME;
		return ((char)((buffer[9] << 4) + buffer[10]));
	}
}

char PN532 :: setSAM(void)
{
	char ack[] = {0x00, 0x00, 0xFF, 0x00, 0xFF};
	char buffer[MAX_I2C_BUF_LEN];
	char data[MAX_I2C_BUF_LEN];
	char length = 0;

	data[0] = 0x01; // Normal mode
  data[1] = 0x00; // Timeout not used (Virtual card mode only)
  data[2] = 0x01; // Use IRQ pin

	// Create frame SAM configuration (0x14)
	length = this->createFrame(buffer, 0x14, data, 3);

	this->delay(PN532_TIMEOUT);

	// Send frame to PN532
	if(m_i2c->write_b(PN532_ADDRESS, buffer, length) == ERROR)
	{
		m_timer = PN532_POLLING_TIME;
		return ERROR;
	}
					
	this->delay(PN532_TIMEOUT);
	
	// PN532 read (RDY)
	if(m_i2c->read(PN532_ADDRESS, buffer, 1) == ERROR)
	{
		m_timer = PN532_POLLING_TIME;
		return ERROR;
	}
	else
	{
		// PN532 not ready ?
		if(buffer[0] != 0x01)
		{
			m_timer = PN532_POLLING_TIME;
			return ERROR;
		}
	}
	
	this->delay(PN532_TIMEOUT);
	
	// PN532 read
	if(m_i2c->read(PN532_ADDRESS, buffer, 7) == ERROR)
	{
		m_timer = PN532_POLLING_TIME;
		return ERROR;
	}
	else
	{
		// ACK ?
		if(strcmp(ack, buffer+1) != 0)
		{
			m_timer = PN532_POLLING_TIME;
			return ERROR;
		}
	}
	
	this->delay(PN532_TIMEOUT);
					
	// PN532 read
	if(m_i2c->read(PN532_ADDRESS, buffer, 10) == ERROR)
	{
		m_timer = PN532_POLLING_TIME;
		return ERROR;
	}
	else
	{	
		m_timer = PN532_POLLING_TIME;
		
		if(buffer[7] == 0x15)
		{
			return SUCCESS;
		}
		else
		{
			return ERROR;
		}
	}
}

char PN532 :: createFrame(char* frame, char command, char* data, char length)
{
	char checksum = 0xFF;
	char i;
	
	frame[0] = 0x00;	// Preamble (PREAMBLE)
	frame[1] = 0x00;	// Start of packet code (START CODE)
	frame[2] = 0xFF;
	
	frame[3] = length + 2;			// Length (LEN)
	frame[4] = (~frame[3]) + 1; // Length checksum (LCS)
	frame[5] = 0xD4;						// Frame identifier (TFI)
	frame[6] = command;
	
	checksum += (frame[5] + frame[6]);
	
	for(i = 0; i < length; i++)
	{
		frame[7 + i] = data[i];
		checksum += data[i];
	}
	
	frame[7 + length] = (~checksum) & 0x00FF;	// Data checksum (DCS)
	frame[8 + length] = 0x00;
	
	return (9 + length);
}

void PN532 :: delay(int time)
{
	m_timer = time;
	while(m_timer);
}

void PN532 :: timer(void)
{
	if(m_timer) m_timer--;
}
