#ifndef __GPIO_H
#define __GPIO_H

/* includes ---------------------------------------------------------------- */
#include "stm32f0xx.h"

/* define ------------------------------------------------------------------ */
#define NULL 0

/* structure --------------------------------------------------------------- */
typedef struct
{
	GPIO_TypeDef* port;
	uint16_t pin;
	uint8_t pinSource;
	uint32_t peripheralClock;
	int16_t irqVector;
	uint8_t extiPort;
}GPIO_common;

typedef struct
{
	TIM_TypeDef* tim;
	char channel;
	char index;
	char af;
}TIM_common;

typedef struct
{
	I2C_TypeDef* bus;
	uint8_t* buffer;
	uint8_t* index;
	uint8_t* busy;
	int16_t irqVector;
	uint32_t peripheralClock;
}I2C_common;

// GPIO
extern const GPIO_common PA0;
extern const GPIO_common PA1;
extern const GPIO_common PA2;
extern const GPIO_common PA3;
extern const GPIO_common PA4;
extern const GPIO_common PA5;
extern const GPIO_common PA6;
extern const GPIO_common PA7;
extern const GPIO_common PA8;
extern const GPIO_common PA9;
extern const GPIO_common PA10;
extern const GPIO_common PA11;
extern const GPIO_common PA12;
extern const GPIO_common PA13;
extern const GPIO_common PA14;
extern const GPIO_common PA15;

extern const GPIO_common PB0;
extern const GPIO_common PB1;
extern const GPIO_common PB2;
extern const GPIO_common PB3;
extern const GPIO_common PB4;
extern const GPIO_common PB5;
extern const GPIO_common PB6;
extern const GPIO_common PB7;
extern const GPIO_common PB8;
extern const GPIO_common PB9;
extern const GPIO_common PB10;
extern const GPIO_common PB11;
extern const GPIO_common PB12;
extern const GPIO_common PB13;
extern const GPIO_common PB14;
extern const GPIO_common PB15;

extern const GPIO_common PC0;
extern const GPIO_common PC1;
extern const GPIO_common PC2;
extern const GPIO_common PC3;
extern const GPIO_common PC4;
extern const GPIO_common PC5;
extern const GPIO_common PC6;
extern const GPIO_common PC7;
extern const GPIO_common PC8;
extern const GPIO_common PC9;
extern const GPIO_common PC10;
extern const GPIO_common PC11;
extern const GPIO_common PC12;
extern const GPIO_common PC13;
extern const GPIO_common PC14;
extern const GPIO_common PC15;

extern const GPIO_common PD2;

extern const GPIO_common PF0;
extern const GPIO_common PF1;
extern const GPIO_common PF4;
extern const GPIO_common PF5;
extern const GPIO_common PF6;
extern const GPIO_common PF7;

// TIM
extern TIM_common TIM1_CH1;
extern TIM_common TIM1_CH2;
extern TIM_common TIM1_CH3;
extern TIM_common TIM1_CH4;
extern TIM_common TIM2_CH1;
extern TIM_common TIM2_CH2;
extern TIM_common TIM2_CH3;
extern TIM_common TIM2_CH4;
extern TIM_common TIM3_CH1;
extern TIM_common TIM3_CH2;
extern TIM_common TIM3_CH3;
extern TIM_common TIM3_CH4;
extern TIM_common TIM14_CH1;
extern TIM_common TIM15_CH1;
extern TIM_common TIM15_CH2;
extern TIM_common TIM16_CH1;
extern TIM_common TIM17_CH1;

// I2C
extern I2C_common I2C_1;
extern I2C_common I2C_2;

#endif
