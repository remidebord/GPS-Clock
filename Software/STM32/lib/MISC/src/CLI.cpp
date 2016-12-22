#include "CLI.h"
#include <stdio.h>
#include <string.h>

const char welcome[64] = "\rCommand line interface 1.0\nAuthor: Remi Debord\n\n> ";

CLI :: CLI(Serial* serialx)
{
	int i = 0;
	
	m_serial = serialx;
	m_state = CLI_INIT;
	m_index = 0;
	
	m_serial->baud(115200);
	
	for(i = 0; i < CLI_BUFFER_SIZE+1; i++)
	{
		m_clear[i] = 0x20;
	}
}

void CLI :: update(void)
{
	char buffer[MAX_USART_RX_BUF_LEN] = {};
	int length = 0;
	int space = 0;
		
	switch(m_state)
	{
		case CLI_INIT:
			
			m_serial->write((char*)welcome, strlen(welcome));
		
			m_state = CLI_IDLE;
			
			break;
		
		case CLI_IDLE:
			
			length = m_serial->read(buffer);
		
			if(length > 0)
			{
				// Carrier return or new line ?
				if((buffer[0] == 0x0D) || (buffer[0] == 0x0A))
				{
					m_serial->write((char*)"\n> ", 3);
					
					m_index = (m_index + 1) % CLI_BUFFER_NUMBER;
					
					memset(m_buffer[m_index], 0x00, CLI_BUFFER_SIZE);
				}
				// Backspace ?
				else if(buffer[0] == 0x08)
				{
					length = strlen(m_buffer[m_index]);
					
					if(length > 0)
					{
						m_buffer[m_index][length-1] = 0x00;
					}
					
					memset(buffer, 0x00, MAX_USART_RX_BUF_LEN);
					
					sprintf(buffer, "\r%s\r> %s", m_clear, m_buffer[m_index]);
					
					// Echo input
					m_serial->write(buffer, strlen(buffer));
				}
				// Arrow ?
				else if((length == 3) && (buffer[0] == 0x1B) && (buffer[1] == 0x5B))
				{
					// Up ?
					if(buffer[2] == 0x41)
					{
						m_index = (m_index + 1) % CLI_BUFFER_NUMBER;
					}
					// Down ?
					else if(buffer[2] == 0x42)
					{
						if(m_index == 0) m_index = CLI_BUFFER_NUMBER;
						
						m_index--;
					}
					
					memset(buffer, 0x00, MAX_USART_RX_BUF_LEN);
					
					sprintf(buffer, "\r%s\r> %s", m_clear, m_buffer[m_index]);
					
					// Echo input
					m_serial->write(buffer, strlen(buffer));
				}
				else
				{
					// Compute available space in m_buffer
					space = CLI_BUFFER_SIZE - 1 - strlen(m_buffer[m_index]);
					
					// Buffer overflow ?
					if(length > space)
					{
						length = space;
					}
					
					// New characters to add ?
					if(length > 0)
					{
						// Store in m_buffer
						strncpy(m_buffer[m_index]+strlen(m_buffer[m_index]), buffer, length);
						
						memset(buffer, 0x00, MAX_USART_RX_BUF_LEN);
					
						sprintf(buffer, "\r> %s", m_buffer[m_index]);
						
						// Echo input
						m_serial->write(buffer, strlen(buffer));
					}
				}
			}
			
		break;
	}
}
