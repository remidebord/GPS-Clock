#ifndef __DELAY_H
#define __DELAY_H

/* includes ----------------------------------------------------------------- */
#include "stm32f0xx.h"

/* functions ---------------------------------------------------------------- */
void Delay(__IO uint32_t nTime);
void TimingDelay_Decrement(void);

#endif /* __DELAY_H */
