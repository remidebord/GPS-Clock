#include "Delay.h"

static __IO uint32_t timingDelay;

/*!
 *  \brief Delay
 *
 *  delay in millisecond
 *
 *  \param nTime : delay value (ms)
 */

void Delay(__IO uint32_t nTime)
{
  timingDelay = nTime;

  while(timingDelay != 0);
}

/*!
 *  \brief TimingDelayDecrement
 *
 *  Decrement the delay value
 *
 */

void TimingDelay_Decrement(void)
{
  if(timingDelay != 0x00)
  { 
    timingDelay--;
  }
}
