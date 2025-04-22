/*
 * delay.c
 *
 *  Created on: Jan 6, 2025
 *      Author: wataoxp
 */
#include "delay.h"

void Delay(uint32_t nTime)
{
	__IO uint32_t tmp = SysTick->CTRL;
	((void)tmp);

	while(nTime)
	{
		if ((SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk) != 0U)
		{
			nTime--;
		}
	}
}
