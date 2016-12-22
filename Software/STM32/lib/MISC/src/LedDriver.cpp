#include "LedDriver.h"
#include <string.h>

LedDriver :: LedDriver(I2C* i2cx, char address)
{
	m_i2c = i2cx;
	m_address = address;
}

void LedDriver :: init(void)
{
	char buffer[2];
	char i;
	
	buffer[0] = 0x00; // Mode register 1
	buffer[1] = 0x01; // Normal mode, all call enabled

	m_i2c->write_b(m_address, buffer, 2);

	buffer[0] = 0x01; // Mode register 2
	buffer[1] = 0x02; // Totem pole outputs
	
	m_i2c->write_b(m_address, buffer, 2);
	
	for(i = 0x14; i <= 0x17; i++)
	{
		buffer[0]= i;			// LEDOUTx
		buffer[1]= 0xFF;	// Pwm control
		
		m_i2c->write_b(m_address, buffer, 2);
	}
}

void LedDriver :: setColor(char red, char green, char blue)
{
	int i;
	char buffer[PWM_OUTPUT+1];
  
  for(i = 0; i < RGB_OUTPUT; i++)
	{
		m_led[i][0] = blue;
		m_led[i][1] = green;
		m_led[i][2] = red;
	}
	
	buffer[0] = 0x82;  
	memcpy(buffer+1, m_led, PWM_OUTPUT);
	
	m_i2c->write(m_address, buffer, (PWM_OUTPUT + 1));
}

void LedDriver :: setColorAll(char red, char green, char blue)
{
	int i;
	char led[RGB_OUTPUT][3];
	char buffer[PWM_OUTPUT+1];
  
  for(i = 0; i < RGB_OUTPUT; i++)
	{
		// Inversion pour led du milieu
		if(i != 2)
		{
			led[i][0] = blue;
			led[i][1] = green;
			led[i][2] = red;
		}
		else
		{
			led[i][0] = red;
			led[i][1] = green;
			led[i][2] = blue;
		}
	}
	
	buffer[0] = 0x82;  
	memcpy(buffer+1, led, PWM_OUTPUT);
	
	m_i2c->write(ALLCALL_ADRESS, buffer, (PWM_OUTPUT + 1));
}

void LedDriver :: setLedColor(int led, char red, char green, char blue)
{
	if(led < RGB_OUTPUT)
	{
		m_led[led][0] = blue;
		m_led[led][1] = green;
		m_led[led][2] = red;
	}
}

void LedDriver :: setLedColor2(int led, char red, char green, char blue)
{
	memset(m_led, 0x00, PWM_OUTPUT);
	
	if(led < RGB_OUTPUT)
	{
		m_led[led][0] = blue;
		m_led[led][1] = green;
		m_led[led][2] = red;
	}
}

void LedDriver :: update(void)
{
	char buffer[PWM_OUTPUT+1];
	
	buffer[0] = 0x82;
	memcpy(buffer+1, m_led, PWM_OUTPUT);
	
	m_i2c->write(m_address, buffer, (PWM_OUTPUT + 1));
}

void LedDriver :: updateAll(void)
{
	char buffer[PWM_OUTPUT+1];
	
	buffer[0] = 0x82;
	memcpy(buffer+1, m_led, PWM_OUTPUT);
	
	m_i2c->write(ALLCALL_ADRESS, buffer, (PWM_OUTPUT + 1));
}

void LedDriver :: setOutput(char out, char value)
{
	char buffer[2];
	
	buffer[0] = out + 0x02;
	buffer[1] = value;
	
	m_i2c->write(m_address, buffer, 2);
}
