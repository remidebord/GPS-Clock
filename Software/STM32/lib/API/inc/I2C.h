#ifndef __I2C_H
#define __I2C_H

/* includes ---------------------------------------------------------------- */
#include "stm32f0xx_gpio.h"
#include "stm32f0xx_rcc.h"
#include "stm32f0xx_i2c.h"
#include "Common.h"

/* defines ----------------------------------------------------------------- */
#define I2C_MCU_ADDRESS		0x00
#define I2C_TIMEOUT				0x1800	// 0x1800 / 48MHz (SystemCoreClock) = 0.12 ms

/* class ------------------------------------------------------------------- */
class I2C
{
	private:
		
		I2C_common* m_i2c;
		const GPIO_common* m_sda;
		const GPIO_common* m_scl;
		
		uint8_t m_buffer[64];
		uint8_t m_index;
		uint8_t m_busy;
		
	public:
		
		I2C(I2C_common* i2cx, const GPIO_common* sda, const GPIO_common* scl, uint8_t address = I2C_MCU_ADDRESS);
		uint8_t read(uint8_t address, uint8_t* command, uint8_t size);
		uint8_t read_s(uint8_t* buffer);
		uint8_t write(uint8_t address, uint8_t* command, uint8_t size);
		uint8_t write_b(uint8_t address, uint8_t* command, uint8_t size);
		uint8_t busy(void);
};

#endif
