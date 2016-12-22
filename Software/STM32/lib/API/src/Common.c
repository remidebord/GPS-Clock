#include "Common.h"

// GPIO
const GPIO_common PA0 = {GPIOA, GPIO_Pin_0, GPIO_PinSource0, RCC_AHBPeriph_GPIOA, EXTI0_1_IRQn, EXTI_PortSourceGPIOA};
const GPIO_common PA1 = {GPIOA, GPIO_Pin_1, GPIO_PinSource1, RCC_AHBPeriph_GPIOA, EXTI0_1_IRQn, EXTI_PortSourceGPIOA};
const GPIO_common PA2 = {GPIOA, GPIO_Pin_2, GPIO_PinSource2, RCC_AHBPeriph_GPIOA, EXTI2_3_IRQn, EXTI_PortSourceGPIOA};
const GPIO_common PA3 = {GPIOA, GPIO_Pin_3, GPIO_PinSource3, RCC_AHBPeriph_GPIOA, EXTI2_3_IRQn, EXTI_PortSourceGPIOA};
const GPIO_common PA4 = {GPIOA, GPIO_Pin_4, GPIO_PinSource4, RCC_AHBPeriph_GPIOA, EXTI4_15_IRQn, EXTI_PortSourceGPIOA};
const GPIO_common PA5 = {GPIOA, GPIO_Pin_5, GPIO_PinSource5, RCC_AHBPeriph_GPIOA, EXTI4_15_IRQn, EXTI_PortSourceGPIOA};
const GPIO_common PA6 = {GPIOA, GPIO_Pin_6, GPIO_PinSource6, RCC_AHBPeriph_GPIOA, EXTI4_15_IRQn, EXTI_PortSourceGPIOA};
const GPIO_common PA7 = {GPIOA, GPIO_Pin_7, GPIO_PinSource7, RCC_AHBPeriph_GPIOA, EXTI4_15_IRQn, EXTI_PortSourceGPIOA};
const GPIO_common PA8 = {GPIOA, GPIO_Pin_8, GPIO_PinSource8, RCC_AHBPeriph_GPIOA, EXTI4_15_IRQn, EXTI_PortSourceGPIOA};
const GPIO_common PA9 = {GPIOA, GPIO_Pin_9, GPIO_PinSource9, RCC_AHBPeriph_GPIOA, EXTI4_15_IRQn, EXTI_PortSourceGPIOA};
const GPIO_common PA10 = {GPIOA, GPIO_Pin_10, GPIO_PinSource10, RCC_AHBPeriph_GPIOA, EXTI4_15_IRQn, EXTI_PortSourceGPIOA};
const GPIO_common PA11 = {GPIOA, GPIO_Pin_11, GPIO_PinSource11, RCC_AHBPeriph_GPIOA, EXTI4_15_IRQn, EXTI_PortSourceGPIOA};
const GPIO_common PA12 = {GPIOA, GPIO_Pin_12, GPIO_PinSource12, RCC_AHBPeriph_GPIOA, EXTI4_15_IRQn, EXTI_PortSourceGPIOA};
const GPIO_common PA13 = {GPIOA, GPIO_Pin_13, GPIO_PinSource13, RCC_AHBPeriph_GPIOA, EXTI4_15_IRQn, EXTI_PortSourceGPIOA};
const GPIO_common PA14 = {GPIOA, GPIO_Pin_14, GPIO_PinSource14, RCC_AHBPeriph_GPIOA, EXTI4_15_IRQn, EXTI_PortSourceGPIOA};
const GPIO_common PA15 = {GPIOA, GPIO_Pin_15, GPIO_PinSource15, RCC_AHBPeriph_GPIOA, EXTI4_15_IRQn, EXTI_PortSourceGPIOA};

const GPIO_common PB0 = {GPIOB, GPIO_Pin_0, GPIO_PinSource0, RCC_AHBPeriph_GPIOB, EXTI0_1_IRQn, EXTI_PortSourceGPIOB};
const GPIO_common PB1 = {GPIOB, GPIO_Pin_1, GPIO_PinSource1, RCC_AHBPeriph_GPIOB, EXTI0_1_IRQn, EXTI_PortSourceGPIOB};
const GPIO_common PB2 = {GPIOB, GPIO_Pin_2, GPIO_PinSource2, RCC_AHBPeriph_GPIOB, EXTI2_3_IRQn, EXTI_PortSourceGPIOB};
const GPIO_common PB3 = {GPIOB, GPIO_Pin_3, GPIO_PinSource3, RCC_AHBPeriph_GPIOB, EXTI2_3_IRQn, EXTI_PortSourceGPIOB};
const GPIO_common PB4 = {GPIOB, GPIO_Pin_4, GPIO_PinSource4, RCC_AHBPeriph_GPIOB, EXTI4_15_IRQn, EXTI_PortSourceGPIOB};
const GPIO_common PB5 = {GPIOB, GPIO_Pin_5, GPIO_PinSource5, RCC_AHBPeriph_GPIOB, EXTI4_15_IRQn, EXTI_PortSourceGPIOB};
const GPIO_common PB6 = {GPIOB, GPIO_Pin_6, GPIO_PinSource6, RCC_AHBPeriph_GPIOB, EXTI4_15_IRQn, EXTI_PortSourceGPIOB};
const GPIO_common PB7 = {GPIOB, GPIO_Pin_7, GPIO_PinSource7, RCC_AHBPeriph_GPIOB, EXTI4_15_IRQn, EXTI_PortSourceGPIOB};
const GPIO_common PB8 = {GPIOB, GPIO_Pin_8, GPIO_PinSource8, RCC_AHBPeriph_GPIOB, EXTI4_15_IRQn, EXTI_PortSourceGPIOB};
const GPIO_common PB9 = {GPIOB, GPIO_Pin_9, GPIO_PinSource9, RCC_AHBPeriph_GPIOB, EXTI4_15_IRQn, EXTI_PortSourceGPIOB};
const GPIO_common PB10 = {GPIOB, GPIO_Pin_10, GPIO_PinSource10, RCC_AHBPeriph_GPIOB, EXTI4_15_IRQn, EXTI_PortSourceGPIOB};
const GPIO_common PB11 = {GPIOB, GPIO_Pin_11, GPIO_PinSource11, RCC_AHBPeriph_GPIOB, EXTI4_15_IRQn, EXTI_PortSourceGPIOB};
const GPIO_common PB12 = {GPIOB, GPIO_Pin_12, GPIO_PinSource12, RCC_AHBPeriph_GPIOB, EXTI4_15_IRQn, EXTI_PortSourceGPIOB};
const GPIO_common PB13 = {GPIOB, GPIO_Pin_13, GPIO_PinSource13, RCC_AHBPeriph_GPIOB, EXTI4_15_IRQn, EXTI_PortSourceGPIOB};
const GPIO_common PB14 = {GPIOB, GPIO_Pin_14, GPIO_PinSource14, RCC_AHBPeriph_GPIOB, EXTI4_15_IRQn, EXTI_PortSourceGPIOB};
const GPIO_common PB15 = {GPIOB, GPIO_Pin_15, GPIO_PinSource15, RCC_AHBPeriph_GPIOB, EXTI4_15_IRQn, EXTI_PortSourceGPIOB};

const GPIO_common PC0 = {GPIOC, GPIO_Pin_0, GPIO_PinSource0, RCC_AHBPeriph_GPIOC, EXTI0_1_IRQn, EXTI_PortSourceGPIOC};
const GPIO_common PC1 = {GPIOC, GPIO_Pin_1, GPIO_PinSource1, RCC_AHBPeriph_GPIOC, EXTI0_1_IRQn, EXTI_PortSourceGPIOC};
const GPIO_common PC2 = {GPIOC, GPIO_Pin_2, GPIO_PinSource2, RCC_AHBPeriph_GPIOC, EXTI2_3_IRQn, EXTI_PortSourceGPIOC};
const GPIO_common PC3 = {GPIOC, GPIO_Pin_3, GPIO_PinSource3, RCC_AHBPeriph_GPIOC, EXTI2_3_IRQn, EXTI_PortSourceGPIOC};
const GPIO_common PC4 = {GPIOC, GPIO_Pin_4, GPIO_PinSource4, RCC_AHBPeriph_GPIOC, EXTI4_15_IRQn, EXTI_PortSourceGPIOC};
const GPIO_common PC5 = {GPIOC, GPIO_Pin_5, GPIO_PinSource5, RCC_AHBPeriph_GPIOC, EXTI4_15_IRQn, EXTI_PortSourceGPIOC};
const GPIO_common PC6 = {GPIOC, GPIO_Pin_6, GPIO_PinSource6, RCC_AHBPeriph_GPIOC, EXTI4_15_IRQn, EXTI_PortSourceGPIOC};
const GPIO_common PC7 = {GPIOC, GPIO_Pin_7, GPIO_PinSource7, RCC_AHBPeriph_GPIOC, EXTI4_15_IRQn, EXTI_PortSourceGPIOC};
const GPIO_common PC8 = {GPIOC, GPIO_Pin_8, GPIO_PinSource8, RCC_AHBPeriph_GPIOC, EXTI4_15_IRQn, EXTI_PortSourceGPIOC};
const GPIO_common PC9 = {GPIOC, GPIO_Pin_9, GPIO_PinSource9, RCC_AHBPeriph_GPIOC, EXTI4_15_IRQn, EXTI_PortSourceGPIOC};
const GPIO_common PC10 = {GPIOC, GPIO_Pin_10, GPIO_PinSource10, RCC_AHBPeriph_GPIOC, EXTI4_15_IRQn, EXTI_PortSourceGPIOC};
const GPIO_common PC11 = {GPIOC, GPIO_Pin_11, GPIO_PinSource11, RCC_AHBPeriph_GPIOC, EXTI4_15_IRQn, EXTI_PortSourceGPIOC};
const GPIO_common PC12 = {GPIOC, GPIO_Pin_12, GPIO_PinSource12, RCC_AHBPeriph_GPIOC, EXTI4_15_IRQn, EXTI_PortSourceGPIOC};
const GPIO_common PC13 = {GPIOC, GPIO_Pin_13, GPIO_PinSource13, RCC_AHBPeriph_GPIOC, EXTI4_15_IRQn, EXTI_PortSourceGPIOC};
const GPIO_common PC14 = {GPIOC, GPIO_Pin_14, GPIO_PinSource14, RCC_AHBPeriph_GPIOC, EXTI4_15_IRQn, EXTI_PortSourceGPIOC};
const GPIO_common PC15 = {GPIOC, GPIO_Pin_15, GPIO_PinSource15, RCC_AHBPeriph_GPIOC, EXTI4_15_IRQn, EXTI_PortSourceGPIOC};

const GPIO_common PD2 = {GPIOD, GPIO_Pin_2, GPIO_PinSource2, RCC_AHBPeriph_GPIOD, EXTI2_3_IRQn, EXTI_PortSourceGPIOD};

const GPIO_common PF0 = {GPIOF, GPIO_Pin_0, GPIO_PinSource0, RCC_AHBPeriph_GPIOF, EXTI0_1_IRQn, EXTI_PortSourceGPIOF};
const GPIO_common PF1 = {GPIOF, GPIO_Pin_1, GPIO_PinSource1, RCC_AHBPeriph_GPIOF, EXTI0_1_IRQn, EXTI_PortSourceGPIOF};
const GPIO_common PF4 = {GPIOF, GPIO_Pin_4, GPIO_PinSource4, RCC_AHBPeriph_GPIOF, EXTI4_15_IRQn, EXTI_PortSourceGPIOF};
const GPIO_common PF5 = {GPIOF, GPIO_Pin_5, GPIO_PinSource5, RCC_AHBPeriph_GPIOF, EXTI4_15_IRQn, EXTI_PortSourceGPIOF};
const GPIO_common PF6 = {GPIOF, GPIO_Pin_6, GPIO_PinSource6, RCC_AHBPeriph_GPIOF, EXTI4_15_IRQn, EXTI_PortSourceGPIOF};
const GPIO_common PF7 = {GPIOF, GPIO_Pin_7, GPIO_PinSource7, RCC_AHBPeriph_GPIOF, EXTI4_15_IRQn, EXTI_PortSourceGPIOF};

// TIM
TIM_common TIM1_CH1 = {TIM1, TIM_Channel_1, 0, GPIO_AF_2};
TIM_common TIM1_CH2 = {TIM1, TIM_Channel_2, 1, GPIO_AF_2};
TIM_common TIM1_CH3 = {TIM1, TIM_Channel_3, 2, GPIO_AF_2};
TIM_common TIM1_CH4 = {TIM1, TIM_Channel_4, 3, GPIO_AF_2};
TIM_common TIM2_CH1 = {TIM2, TIM_Channel_1, 4, GPIO_AF_2};
TIM_common TIM2_CH2 = {TIM2, TIM_Channel_2, 5, GPIO_AF_2};
TIM_common TIM2_CH3 = {TIM2, TIM_Channel_3, 6, GPIO_AF_2};
TIM_common TIM2_CH4 = {TIM2, TIM_Channel_4, 7, GPIO_AF_2};
TIM_common TIM3_CH1 = {TIM3, TIM_Channel_1, 8, GPIO_AF_1};
TIM_common TIM3_CH2 = {TIM3, TIM_Channel_2, 9, GPIO_AF_1};
TIM_common TIM3_CH3 = {TIM3, TIM_Channel_3, 10, GPIO_AF_1};
TIM_common TIM3_CH4 = {TIM3, TIM_Channel_4, 11, GPIO_AF_1};
TIM_common TIM14_CH1 = {TIM14, TIM_Channel_1, 12, GPIO_AF_4};
//TIM_common TIM15_CH1 = {TIM15, TIM_Channel_1, 13, GPIO_AF_1};
//TIM_common TIM15_CH2 = {TIM15, TIM_Channel_2, 14, GPIO_AF_1};
TIM_common TIM15_CH1 = {TIM15, TIM_Channel_1, 13, GPIO_AF_0};
TIM_common TIM15_CH2 = {TIM15, TIM_Channel_2, 14, GPIO_AF_0};
TIM_common TIM16_CH1 = {TIM16, TIM_Channel_1, 15, GPIO_AF_2};
TIM_common TIM17_CH1 = {TIM17, TIM_Channel_1, 16, GPIO_AF_2};

// I2C
I2C_common I2C_1 = {I2C1, 0, 0, 0, I2C1_IRQn, RCC_APB1Periph_I2C1};
I2C_common I2C_2 = {I2C2, 0, 0, 0, I2C2_IRQn, RCC_APB1Periph_I2C2};
