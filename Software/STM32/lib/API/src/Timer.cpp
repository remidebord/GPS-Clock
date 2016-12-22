/*!
 * \file Timer.cpp
 * \brief Timer API.
 * \author Rémi.Debord
 * \version 1.0
 * \date 19 novembre 2013
 *
 * Timer library (PWM, Ticker, TimeOut).
 *
 */

#include "Timer.h"

extern "C"
{
	char timUpdateAttach[8]; // 7 + 1 basic timer
	void (*TimUpdateInterrupt[8])(void);
	
	char timCCAttach[17];		// 17 channels
	void (*TimCCInterrupt[17])(void);
}

/*!
 *  \brief Constructor
 *
 *  Ticker constructor.
 *
 *  \param TIMx : Timer number
 *
 */

Ticker :: Ticker(TIM_TypeDef* TIMx)
{
  NVIC_InitTypeDef NVIC_InitStructure;
  
	m_tim = TIMx;
	
  /* TIMx clock enable */
  if(m_tim == TIM1) RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
	else if(m_tim == TIM2) RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	else if(m_tim == TIM3) RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	else if(m_tim == TIM6) RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);
	else if(m_tim == TIM14) RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM14, ENABLE);
	else if(m_tim == TIM15) RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM15, ENABLE);
	else if(m_tim == TIM16) RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM16, ENABLE);
	else if(m_tim == TIM17) RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM17, ENABLE);

	/* IRQ Vector */
  if(m_tim == TIM1) m_irqVector = TIM1_BRK_UP_TRG_COM_IRQn;
	else if(m_tim == TIM2) m_irqVector = TIM2_IRQn;
	else if(m_tim == TIM3) m_irqVector = TIM3_IRQn;
	else if(m_tim == TIM6) m_irqVector = TIM6_DAC_IRQn;
	else if(m_tim == TIM14) m_irqVector = TIM14_IRQn;
	else if(m_tim == TIM15) m_irqVector = TIM15_IRQn;
	else if(m_tim == TIM16) m_irqVector = TIM16_IRQn;
	else if(m_tim == TIM17) m_irqVector = TIM17_IRQn;
	
	/* Determine index for function pointer array and flag */
	if(m_tim == TIM1)	m_index = 0;
	else if(m_tim == TIM2) m_index = 1;
	else if(m_tim == TIM3) m_index = 2;
	else if(m_tim == TIM6) m_index = 3;
	else if(m_tim == TIM14) m_index = 4;
	else if(m_tim == TIM15) m_index = 5;
	else if(m_tim == TIM16) m_index = 6;
	else if(m_tim == TIM17) m_index = 7;
	
  /* Enable the TIMx global Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = m_irqVector;
  NVIC_InitStructure.NVIC_IRQChannelPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}

/*!
 *  \brief Attach a function to ticker IRQ (callback)
 *
 *  Associate callback function to timer.
 *
 *  \param void(*f)(void) : function pointer (address)
 *  \param ms : periodic time (ms)
 *
 */

void Ticker :: attach(void(*f)(void), int ms)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	
	/* Attribute function adress to the function pointer */	
	TimUpdateInterrupt[m_index] = f;
	timUpdateAttach[m_index] = 1;
	
	/* Time base configuration */
  TIM_TimeBaseStructure.TIM_Prescaler = (SystemCoreClock / 10000) - 1; // 48 MHz / 10000 (100 us)
  TIM_TimeBaseStructure.TIM_Period = (ms * 10) - 1;
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseInit(m_tim, &TIM_TimeBaseStructure);
 
  /* Enable TIMx Update Interrupt */
  TIM_ITConfig(m_tim, TIM_IT_Update, ENABLE);
 
  /* Enable TIMx */
  TIM_Cmd(m_tim,ENABLE);
}

/*!
 *  \brief Attach a function to ticker IRQ (callback)
 *
 *  Associate callback function to timer.
 *
 *  \param void(*f)(void) : function pointer (address)
 *  \param ms : periodic time (us)
 *
 */

void Ticker :: attach_us(void(*f)(void), int us)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	
	/* Attribute function adress to the function pointer */	
	TimUpdateInterrupt[m_index] = f;
	timUpdateAttach[m_index] = 1;
	
	/* Time base configuration */
  TIM_TimeBaseStructure.TIM_Prescaler = (SystemCoreClock / 1000000) - 1; // 48 MHz / 1000000 (1us)
  TIM_TimeBaseStructure.TIM_Period = us - 1;
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseInit(m_tim, &TIM_TimeBaseStructure);
 
  /* Enable TIMx Update Interrupt */
  TIM_ITConfig(m_tim, TIM_IT_Update, ENABLE);
 
  /* Enable TIMx */
  TIM_Cmd(m_tim,ENABLE);
}

/*!
 *  \brief Detach
 *
 *  Detach callback function
 *
 */

void Ticker :: detach()
{
	timUpdateAttach[m_index] = 0;
}

/*!
 *  \brief Constructor
 *
 *  TimeOut constructor.
 *
 *  \param TIMx : Timer number
 *
 */

TimeOut :: TimeOut(TIM_TypeDef* TIMx)
{
	NVIC_InitTypeDef NVIC_InitStructure;
  
	m_tim = TIMx;
	
  /* TIMx clock enable */
  if(m_tim == TIM1) RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
	else if(m_tim == TIM2) RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	else if(m_tim == TIM3) RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	else if(m_tim == TIM6) RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);
	else if(m_tim == TIM14) RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM14, ENABLE);
	else if(m_tim == TIM15) RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM15, ENABLE);
	else if(m_tim == TIM16) RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM16, ENABLE);
	else if(m_tim == TIM17) RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM17, ENABLE);

	/* IRQ Vector */
  if(m_tim == TIM1) m_irqVector = TIM1_BRK_UP_TRG_COM_IRQn;
	else if(m_tim == TIM2) m_irqVector = TIM2_IRQn;
	else if(m_tim == TIM3) m_irqVector = TIM3_IRQn;
	else if(m_tim == TIM6) m_irqVector = TIM6_DAC_IRQn;
	else if(m_tim == TIM14) m_irqVector = TIM14_IRQn;
	else if(m_tim == TIM15) m_irqVector = TIM15_IRQn;
	else if(m_tim == TIM16) m_irqVector = TIM16_IRQn;
	else if(m_tim == TIM17) m_irqVector = TIM17_IRQn;
	
	/* Determine index for function pointer array and flag */
	if(m_tim == TIM1)	m_index = 0;
	else if(m_tim == TIM2) m_index = 1;
	else if(m_tim == TIM3) m_index = 2;
	else if(m_tim == TIM6) m_index = 3;
	else if(m_tim == TIM14) m_index = 4;
	else if(m_tim == TIM15) m_index = 5;
	else if(m_tim == TIM16) m_index = 6;
	else if(m_tim == TIM17) m_index = 7;
	
  /* Enable the TIMx global Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = m_irqVector;
  NVIC_InitStructure.NVIC_IRQChannelPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}

/*!
 *  \brief Attach a function to timeOut IRQ (callback)
 *
 *  Associate callback function to timer.
 *
 *  \param void(*f)(void) : function pointer (address)
 *  \param ms : periodic time (ms)
 *
 */

void TimeOut :: attach(void(*f)(void), int ms)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	
	/* Attribute function adress to the function pointer */	
	TimUpdateInterrupt[m_index] = f;
	timUpdateAttach[m_index] = 1;
	
	/* Time base configuration */
  TIM_TimeBaseStructure.TIM_Prescaler = (SystemCoreClock / 10000) - 1; // 48 MHz / 10000 (100 us)
  TIM_TimeBaseStructure.TIM_Period = (ms * 10) - 1;
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0; //!!
  TIM_TimeBaseInit(m_tim, &TIM_TimeBaseStructure);
 
  /* Enable TIMx Update Interrupt */
  TIM_ITConfig(m_tim, TIM_IT_Update, ENABLE);
}

/*!
 *  \brief Attach a function to timeOut IRQ (callback)
 *
 *  Associate callback function to timer.
 *
 *  \param void(*f)(void) : function pointer (address)
 *  \param ms : periodic time (us)
 *
 */

void TimeOut :: attach_us(void(*f)(void), int us)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	
	/* Attribute function adress to the function pointer */	
	TimUpdateInterrupt[m_index] = f;
	timUpdateAttach[m_index] = 1;
	
	/* Time base configuration */
  TIM_TimeBaseStructure.TIM_Prescaler = (SystemCoreClock / 1000000) - 1; // 48 MHz / 1000000 (1us)
  TIM_TimeBaseStructure.TIM_Period = us - 1;
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0; //!!
  TIM_TimeBaseInit(m_tim, &TIM_TimeBaseStructure);
 
  /* Enable TIMx Update Interrupt */
  TIM_ITConfig(m_tim, TIM_IT_Update, ENABLE);
}

/*!
 *  \brief Detach
 *
 *  Detach callback function.
 *
 */

void TimeOut :: detach()
{
	timUpdateAttach[m_index] = 0;

	/* Disable TIMx */
  TIM_Cmd(m_tim,DISABLE);
}

/*!
 *  \brief Start counter
 *
 *  Launch timeout counter.
 *
 */

void TimeOut :: start(void)
{
	/* Reset counter */
	TIM_SetCounter(m_tim, 0);
	
	/* Enable TIMx */
  TIM_Cmd(m_tim,ENABLE);
}

/*!
 *  \brief Constructor
 *
 *  Constructeur de la classe Timer (used a 32 bit timer like TIM2!)
 *
 *  \param TIMx : Timer number
 *
 */

Timer :: Timer(TIM_TypeDef* TIMx)
{ 
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	
	m_tim = TIMx;
	
  /* TIMx clock enable */
  if(m_tim == TIM1) RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
	else if(m_tim == TIM2) RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	else if(m_tim == TIM3) RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	else if(m_tim == TIM6) RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);
	else if(m_tim == TIM14) RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM14, ENABLE);
	else if(m_tim == TIM15) RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM15, ENABLE);
	else if(m_tim == TIM16) RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM16, ENABLE);
	else if(m_tim == TIM17) RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM17, ENABLE);
	
	TIM_TimeBaseStructure.TIM_Prescaler = (SystemCoreClock / 1000000) - 1; // 48 MHz / 1000000 (1us)
  
	// 32 bits timer ?
	if(m_tim == TIM2)
	{
		TIM_TimeBaseStructure.TIM_Period = 0xFFFFFFFF;
	}
	else
	{
		TIM_TimeBaseStructure.TIM_Period = 0xFFFF;
	}
	
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0; //!!
  TIM_TimeBaseInit(m_tim, &TIM_TimeBaseStructure);
}

/*!
 *  \brief Start counter
 *
 *  Lancement du compteur
 *
 */

void Timer :: start(void)
{
	/* Enable TIMx */
  TIM_Cmd(m_tim, ENABLE);
}

/*!
 *  \brief Stop counter
 *
 *  Arrêt du compteur
 *
 */

void Timer :: stop(void)
{
	/* Enable TIMx */
  TIM_Cmd(m_tim, DISABLE);
}

/*!
 *  \brief Reset counter
 *
 *  Remise à zéro de la valeur du compteur
 *
 */

void Timer :: reset(void)
{
	/* Reset counter */
	TIM_SetCounter(m_tim, 0);
}

/*!
 *  \brief Get the time passed in seconds
 *
 *  Temps passé en secondes
 *
 */

int Timer :: read(void)
{
	return (TIM_GetCounter(m_tim) / (1000*1000));
}

/*!
 *  \brief Get the time passed in mili-seconds
 *
 *  Temps passé en milli-secondes
 *
 */

int Timer :: read_ms(void)
{
	return (TIM_GetCounter(m_tim) / 1000);
}

/*!
 *  \brief Get the time passed in micro-seconds
 *
 *  Temps passé en micro-secondes
 *
 */

int Timer :: read_us(void)
{
	return TIM_GetCounter(m_tim);
}

/*!
 *  \brief Constructor
 *
 *  PwmOut counter.
 *
 *  \param GPIO_c : pwm pin
 *  \param freq : pwm frequency
 *  \param PWM_c : timer
 *
 */

PwmOut :: PwmOut(GPIO_common GPIO_c, int freq, TIM_common PWM_c)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	
	int prescaler = 0;
	int channelPulse = 0;
	
	m_port = GPIO_c.port;
	m_pin = GPIO_c.pin;
	m_pinSource = GPIO_c.pinSource;
	
	m_tim = PWM_c.tim;
	m_channel = PWM_c.channel;
	m_index = PWM_c.index;
	
	m_freq = freq;
	m_dutyCycle = 0.0;

  /* GPIOx Clocks enable */
  if(m_port == GPIOA) RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
	else if(m_port == GPIOB) RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);
	else if(m_port == GPIOC) RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOC, ENABLE);
	else if(m_port == GPIOD) RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOC, ENABLE);
  
  /* GPIOx Configuration: Channel x as alternate function push-pull */
  GPIO_InitStructure.GPIO_Pin = m_pin;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP ;
  GPIO_Init(m_port, &GPIO_InitStructure);
  
  /* Alternate function */
	GPIO_PinAFConfig(m_port, m_pinSource, PWM_c.af);
	
	/* PWM freq superior at 1 kHz or TIM2 (32bit) used ? */
	if((m_freq < FREQ_LOW_MAX)) prescaler = PRESCALER_LOW_FREQ;
	else prescaler = 1;
	
	/* pwm period */
	m_timerPeriod = ((SystemCoreClock / prescaler) / m_freq) - 1;

  /* TIM1 clock enable */
  if(m_tim == TIM1) RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
	if(m_tim == TIM15) RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM15, ENABLE);
	if(m_tim == TIM16) RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM16, ENABLE);
	if(m_tim == TIM17) RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM17, ENABLE);
	if(m_tim == TIM2) RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	if(m_tim == TIM3) RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	if(m_tim == TIM14) RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM14, ENABLE);
  
  /* Time Base configuration */
  if(prescaler == 1) TIM_TimeBaseStructure.TIM_Prescaler = 0;
	else TIM_TimeBaseStructure.TIM_Prescaler = PRESCALER_LOW_FREQ;
	
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseStructure.TIM_Period = m_timerPeriod;
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;

  TIM_TimeBaseInit(m_tim, &TIM_TimeBaseStructure);

  /* Channel x Configuration in PWM mode */
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;
  TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;

	/* Calculate pulse width */
	channelPulse = (((1 - m_dutyCycle)*100) * (m_timerPeriod - 1)) / 100;

  TIM_OCInitStructure.TIM_Pulse = channelPulse;
  
	switch(m_channel)
	{
		case TIM_Channel_1: TIM_OC1Init(m_tim, &TIM_OCInitStructure); break;
		case TIM_Channel_2: TIM_OC2Init(m_tim, &TIM_OCInitStructure); break;
		case TIM_Channel_3: TIM_OC3Init(m_tim, &TIM_OCInitStructure); break;
		case TIM_Channel_4: TIM_OC4Init(m_tim, &TIM_OCInitStructure); break;
	}

  /* TIMx counter enable */
  TIM_Cmd(m_tim, ENABLE);

  /* TIMx Main Output Enable */
  if((m_tim == TIM1) || (m_tim == TIM15) || (m_tim == TIM16) || (m_tim == TIM17))
	{
		TIM_CtrlPWMOutputs(m_tim, ENABLE);
	}
}

/*!
 *  \brief Enable timer IRQ
 *
 *  Interrupt activation.
 *
 */

void PwmOut :: enable_irq(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	int irqVector;

	/* Disable TIMx */
  TIM_Cmd(m_tim,DISABLE);
	
	/* IRQ Vector */
  if(m_tim == TIM1)	irqVector = TIM1_BRK_UP_TRG_COM_IRQn;
	else if(m_tim == TIM2) irqVector = TIM2_IRQn;
	else if(m_tim == TIM3) irqVector = TIM3_IRQn;
	else if(m_tim == TIM14) irqVector = TIM14_IRQn;
	else if(m_tim == TIM15) irqVector = TIM15_IRQn;
	else if(m_tim == TIM16) irqVector = TIM16_IRQn;
	else if(m_tim == TIM17) irqVector = TIM17_IRQn;
	
  /* Enable the TIMx global Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = irqVector;
  NVIC_InitStructure.NVIC_IRQChannelPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
	
	/* Enable TIMx Update Interrupt */
  switch(m_channel)
	{
		case TIM_Channel_1: TIM_ITConfig(m_tim, TIM_IT_CC1, ENABLE); break;
		case TIM_Channel_2: TIM_ITConfig(m_tim, TIM_IT_CC2, ENABLE); break;
		case TIM_Channel_3: TIM_ITConfig(m_tim, TIM_IT_CC3, ENABLE); break;
		case TIM_Channel_4: TIM_ITConfig(m_tim, TIM_IT_CC4, ENABLE); break;
	}
  
	/* Enable TIMx */
  TIM_Cmd(m_tim,ENABLE);
}

/*!
 *  \brief Attach a function to PwmOut IRQ (callback)
 *
 *  Associate callback function to timer.
 *
 *  \param void(*f)(void) : function pointer (address)
 *
 */

void PwmOut :: attach(void(*f)(void))
{
	/* Attribute function adress to the function pointer */	
	TimCCInterrupt[m_index] = f;
	timCCAttach[m_index] = 1;
}

/*!
 *  \brief Detach
 *
 *  Detach callback function
 *
 */

void PwmOut :: detach(void)
{
	timCCAttach[m_index] = 0;
}

/*!
 *  \brief Modify pwm frequency
 *
 *  Modify pwm frequency.
 *
 *  \param n: pwm frequency (Hz)
 */

void PwmOut :: freq(int n)
{
	int prescaler;
	
	m_freq = n;
	
	/* Update prescaler if needed */
	if(m_freq < FREQ_LOW_MAX) prescaler = PRESCALER_LOW_FREQ;
	else prescaler = 1;
	
	/* pwm period */
	m_timerPeriod = ((SystemCoreClock / prescaler) / m_freq) - 1;
	
	if(prescaler == 1) prescaler = 0;
	
	TIM_PrescalerConfig(m_tim, prescaler, TIM_PSCReloadMode_Immediate);
		
	/* Set pwm period */
	TIM_SetAutoreload(m_tim, m_timerPeriod);
	
	/* Restart counter */
	TIM_SetCounter(m_tim, 0);
	
	/* !important: Set duty cycle */
	this->write(m_dutyCycle);
}

/*!
 *  \brief Modify pwm duty cycle
 *
 *  Change duty cycle.
 *
 *  \param n: pwm duty cycle (0.01 to 1.00)
 */

void PwmOut :: write(float n)
{	
	int channelPulse;
	
	/* overflow et underflow protection */
	if(n >= 1) n = 1;
	else if(n <= 0) n = 0;
	
	/* Store duty cycle value */
	m_dutyCycle = n;
	
	/* Calculate pulse width */
	channelPulse = (((1 - m_dutyCycle)*100) * (m_timerPeriod - 1)) / 100;
	
	/* Set pulse width to the TIMx channel */
	switch(m_channel)
	{
		case TIM_Channel_1: m_tim->CCR1 = channelPulse; break;
		case TIM_Channel_2: m_tim->CCR2 = channelPulse; break;
		case TIM_Channel_3: m_tim->CCR3 = channelPulse; break;
		case TIM_Channel_4: m_tim->CCR4 = channelPulse; break;
	}
}

/*!
 *  \brief Modify pwm duty cycle (shorthand)
 *
 *  Change duty cycle.
 *
 *  \param n: pwm duty cycle (0.01 to 1.00)
 */

PwmOut& PwmOut ::  operator= (float n)
{
	this->write(n);
	
	return *this;
}

/*!
 *  \brief Read pwm duty cycle
 *
 *  Get duty cycle.
 *
 *  \return m_dutyCycle: duty cycle (0.01 to 1.00)
 */

float PwmOut :: read()
{
	return m_dutyCycle;
}

/*!
 *  \brief Read pwm duty cycle (shorthand)
 *
 *  Get duty cycle.
 *
 *  \return m_dutyCycle: duty cycle (0.01 to 1.00)
 */

PwmOut :: operator float()
{
	return this->read();
}

/*!
 *  \brief Timer IRQ handler
 */

extern "C"
{	
	void TIM1_BRK_UP_TRG_COM_IRQHandler(void)
	{	
		if (TIM_GetITStatus(TIM1, TIM_IT_Update) != RESET)
		{
			/* Call function */
			if(timUpdateAttach[0]) (*TimUpdateInterrupt[0])();
			
			TIM_ClearITPendingBit(TIM1, TIM_IT_Update);
		}
		
		if (TIM_GetITStatus(TIM1, TIM_IT_CC1) != RESET)
		{
			/* Call function */
			if(timCCAttach[0]) (*TimCCInterrupt[0])();		
			
			TIM_ClearITPendingBit(TIM1, TIM_IT_CC1);
		}
		
		if (TIM_GetITStatus(TIM1, TIM_IT_CC2) != RESET)
		{
			/* Call function */
			if(timCCAttach[1]) (*TimCCInterrupt[1])();		
			
			TIM_ClearITPendingBit(TIM1, TIM_IT_CC2);
		}
		
		if (TIM_GetITStatus(TIM1, TIM_IT_CC3) != RESET)
		{
			/* Call function */
			if(timCCAttach[2]) (*TimCCInterrupt[2])();		
			
			TIM_ClearITPendingBit(TIM1, TIM_IT_CC3);
		}
		
		if (TIM_GetITStatus(TIM1, TIM_IT_CC4) != RESET)
		{
			/* Call function */
			if(timCCAttach[3]) (*TimCCInterrupt[3])();		
			
			TIM_ClearITPendingBit(TIM1, TIM_IT_CC4);
		}
	}
	
	void TIM2_IRQHandler(void)
	{	
		if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)
		{
			/* Call function */
			if(timUpdateAttach[1]) (*TimUpdateInterrupt[1])();
			
			TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
		}
		
		if (TIM_GetITStatus(TIM2, TIM_IT_CC1) != RESET)
		{
			/* Call function */
			if(timCCAttach[4]) (*TimCCInterrupt[4])();		
			
			TIM_ClearITPendingBit(TIM2, TIM_IT_CC1);
		}
		
		if (TIM_GetITStatus(TIM2, TIM_IT_CC2) != RESET)
		{
			/* Call function */
			if(timCCAttach[5]) (*TimCCInterrupt[5])();		
			
			TIM_ClearITPendingBit(TIM2, TIM_IT_CC2);
		}
		
		if (TIM_GetITStatus(TIM2, TIM_IT_CC3) != RESET)
		{
			/* Call function */
			if(timCCAttach[6]) (*TimCCInterrupt[6])();		
			
			TIM_ClearITPendingBit(TIM2, TIM_IT_CC3);
		}
		
		if (TIM_GetITStatus(TIM2, TIM_IT_CC4) != RESET)
		{
			/* Call function */
			if(timCCAttach[7]) (*TimCCInterrupt[7])();		
			
			TIM_ClearITPendingBit(TIM2, TIM_IT_CC4);
		}
	}
	
	void TIM3_IRQHandler(void)
	{	
		if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)
		{
			/* Call function */
			if(timUpdateAttach[2]) (*TimUpdateInterrupt[2])();				
			
			TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
		}
		
		if (TIM_GetITStatus(TIM3, TIM_IT_CC1) != RESET)
		{
			/* Call function */
			if(timCCAttach[8]) (*TimCCInterrupt[8])();		
			
			TIM_ClearITPendingBit(TIM3, TIM_IT_CC1);
		}
		
		if (TIM_GetITStatus(TIM3, TIM_IT_CC2) != RESET)
		{
			/* Call function */
			if(timCCAttach[9]) (*TimCCInterrupt[9])();		
			
			TIM_ClearITPendingBit(TIM3, TIM_IT_CC2);
		}
		
		if (TIM_GetITStatus(TIM3, TIM_IT_CC3) != RESET)
		{
			/* Call function */
			if(timCCAttach[10]) (*TimCCInterrupt[10])();		
			
			TIM_ClearITPendingBit(TIM3, TIM_IT_CC3);
		}
		
		if (TIM_GetITStatus(TIM3, TIM_IT_CC4) != RESET)
		{
			/* Call function */
			if(timCCAttach[11]) (*TimCCInterrupt[11])();		
			
			TIM_ClearITPendingBit(TIM3, TIM_IT_CC4);
		}
	}
	
	void TIM6_DAC_IRQHandler(void)
	{	
		if (TIM_GetITStatus(TIM6, TIM_IT_Update) != RESET)
		{
			/* Call function */
			if(timUpdateAttach[3]) (*TimUpdateInterrupt[3])();		
			
			TIM_ClearITPendingBit(TIM6, TIM_IT_Update);
		}
	}
	
	void TIM14_IRQHandler(void)
	{	
		if (TIM_GetITStatus(TIM14, TIM_IT_Update) != RESET)
		{
			/* Call function */
			if(timUpdateAttach[4]) (*TimUpdateInterrupt[4])();		
			
			TIM_ClearITPendingBit(TIM14, TIM_IT_Update);
		}
		
		if (TIM_GetITStatus(TIM14, TIM_IT_CC1) != RESET)
		{	
			/* Call function */
			if(timCCAttach[12]) (*TimCCInterrupt[12])();	
			
			TIM_ClearITPendingBit(TIM14, TIM_IT_CC1);
		}
	}
	
	void TIM15_IRQHandler(void)
	{	
		if (TIM_GetITStatus(TIM15, TIM_IT_Update) != RESET)
		{
			/* Call function */
			if(timUpdateAttach[5]) (*TimUpdateInterrupt[5])();	
			
			TIM_ClearITPendingBit(TIM15, TIM_IT_Update);
		}

		if (TIM_GetITStatus(TIM15, TIM_IT_CC1) != RESET)
		{	
			/* Call function */
			if(timCCAttach[13]) (*TimCCInterrupt[13])();	
			
			TIM_ClearITPendingBit(TIM15, TIM_IT_CC1);
		}
		
		if (TIM_GetITStatus(TIM15, TIM_IT_CC2) != RESET)
		{	
			/* Call function */
			if(timCCAttach[14]) (*TimCCInterrupt[14])();	
			
			TIM_ClearITPendingBit(TIM15, TIM_IT_CC2);
		}
	}
	
	void TIM16_IRQHandler(void)
	{	
		if (TIM_GetITStatus(TIM16, TIM_IT_Update) != RESET)
		{
			/* Call function */
			if(timUpdateAttach[6]) (*TimUpdateInterrupt[6])();
			
			TIM_ClearITPendingBit(TIM16, TIM_IT_Update);
		}
		
		if (TIM_GetITStatus(TIM16, TIM_IT_CC1) != RESET)
		{	
			/* Call function */
			if(timCCAttach[15]) (*TimCCInterrupt[15])();	
			
			TIM_ClearITPendingBit(TIM16, TIM_IT_CC1);
		}
	}
	
	void TIM17_IRQHandler(void)
	{	
		if (TIM_GetITStatus(TIM17, TIM_IT_Update) != RESET)
		{
			/* Call function */
			if(timUpdateAttach[7]) (*TimUpdateInterrupt[7])();		
			
			TIM_ClearITPendingBit(TIM17, TIM_IT_Update);
		}
		
		if (TIM_GetITStatus(TIM17, TIM_IT_CC1) != RESET)
		{	
			/* Call function */
			if(timCCAttach[16]) (*TimCCInterrupt[16])();	
			
			TIM_ClearITPendingBit(TIM17, TIM_IT_CC1);
		}
	}
}
