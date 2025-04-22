/*
 * rcc.h
 *
 *  Created on: Jan 22, 2025
 *      Author: wataoxp
 */

#ifndef INC_RCC_H_
#define INC_RCC_H_

#include "periph.h"

#define SYSINT_ENABLE SysTick_CTRL_TICKINT_Msk
#define SYSINT_DISABLE 0

typedef struct{
	uint32_t Latency;
	uint32_t PLLSrc;
	uint32_t PLLM;
	uint32_t PLLN;
	uint32_t PLLR;
	uint32_t AHBdiv;
	uint32_t APBdiv;
	uint32_t SysClkSrc;
	uint32_t clock;
}RCC_InitTypedef;

static inline void SysTick_ConfigISR(uint8_t valid)
{
	SysTick->CTRL &= ~SysTick_CTRL_TICKINT_Msk;
	SysTick->CTRL |= valid;
}

void RCC_InitG0(RCC_InitTypedef *rcc);
uint32_t SysTick_Init(uint32_t Ticks);

/*
 * RCC_APBENR1レジスタについて
 * 同レジスタにあるDBGENビットは基本セットされている。
 * IDEのデバッグの構成->低電力モードのデバッグ(雑な和訳)が有効になっているので、これを無効にするとDBGENビットはクリアされたままになる
 */


#endif /* INC_RCC_H_ */
