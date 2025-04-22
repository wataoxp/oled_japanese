/*
 * rcc.c
 *
 *  Created on: Jan 22, 2025
 *      Author: wataoxp
 */
#include "rcc.h"

static uint32_t HCLK;

void RCC_InitG0(RCC_InitTypedef *rcc)
{
	LL_FLASH_SetLatency(rcc->Latency);
	while(LL_FLASH_GetLatency() != rcc->Latency);

	LL_RCC_HSI_Enable();
	while(LL_RCC_HSI_IsReady() != 1);

	LL_RCC_PLL_ConfigDomain_SYS(rcc->PLLSrc, rcc->PLLM, rcc->PLLN, rcc->PLLR);
	LL_RCC_PLL_Enable();
	LL_RCC_PLL_EnableDomain_SYS();
	while(LL_RCC_PLL_IsReady() != 1);

	LL_RCC_SetAHBPrescaler(rcc->AHBdiv);
	
	LL_RCC_SetSysClkSource(rcc->SysClkSrc);
	while(LL_RCC_GetSysClkSource() != (rcc->SysClkSrc << 3));

	LL_RCC_SetAPB1Prescaler(rcc->APBdiv);

	LL_SetSystemCoreClock(rcc->clock);
	HCLK = rcc->clock;
}
uint32_t SysTick_Init(uint32_t Ticks)
{
	if(HCLK == 0)
	{
		return 1;
	}
	SysTick->LOAD = (uint32_t)(HCLK/Ticks);
	SysTick->VAL = 0;
	SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_ENABLE_Msk;

	return 0;
}
