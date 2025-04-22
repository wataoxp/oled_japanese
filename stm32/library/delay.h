/*
 * delay.h
 *
 *  Created on: Jan 6, 2025
 *      Author: wataoxp
 */

#ifndef INC_DELAY_H_
#define INC_DELAY_H_

#include "stm32g0xx.h"
#include "core_cm0plus.h"
#include "stm32g0xx_ll_utils.h"

#define SYSTEM_CLOCK 64000000		//HCLK
#define MICRO_SECOND 1000000		//1s = 1000000us
#define MILL_SECOND 1000			//1s = 1000ms
#define MICRO_LOAD (SYSTEM_CLOCK/MICRO_SECOND)	// HCLK/MICRO_SECOND = 1usでカウントされる値
#define MILL_LOAD (SYSTEM_CLOCK/MILL_SECOND)	// HCLK/MILL_SECOND = 1msでカウントされる値

static inline void Init1usTick(void)
{
	SysTick->LOAD  = (uint32_t)(MICRO_LOAD - 1UL);
}
static inline void DeInit1usTick(void)
{
	SysTick->LOAD  = (uint32_t)(MILL_LOAD - 1UL);
}
//デフォルトはSystemClock_Config()のLL_Init1msTick()が適用されるので、1msカウントになっている。
void Delay(uint32_t nTime);

#endif /* INC_DELAY_H_ */
