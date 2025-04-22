/*
 * exti.c
 *
 *  Created on: Jan 28, 2025
 *      Author: wataoxp
 */
#include "exti.h"

void EXTI_SetSource(uint32_t port,uint32_t pin)
{
	uint32_t Reg = port & 0x03;
	uint32_t Bit = MOD(pin,4) * 8;

	MODIFY_REG(EXTI->EXTICR[Reg],EXTI_EXTICR1_EXTI0 << Bit, port << Bit);
}

void EXTI_Init(LL_EXTI_InitTypeDef *InitStruct)
{
	switch (InitStruct->Mode)
	{
	case LL_EXTI_MODE_IT:
		LL_EXTI_DisableEvent_0_31(InitStruct->Line_0_31);
		LL_EXTI_EnableIT_0_31(InitStruct->Line_0_31);
		break;
	case LL_EXTI_MODE_EVENT:
		LL_EXTI_DisableIT_0_31(InitStruct->Line_0_31);
		LL_EXTI_EnableEvent_0_31(InitStruct->Line_0_31);
		break;
	case LL_EXTI_MODE_IT_EVENT:
		LL_EXTI_EnableIT_0_31(InitStruct->Line_0_31);
		LL_EXTI_EnableEvent_0_31(InitStruct->Line_0_31);
		break;
	default:
		break;
	}
	switch(InitStruct->Trigger)
	{
	case LL_EXTI_TRIGGER_RISING:
		LL_EXTI_DisableFallingTrig_0_31(InitStruct->Line_0_31);
		LL_EXTI_EnableRisingTrig_0_31(InitStruct->Line_0_31);
		break;
	case LL_EXTI_TRIGGER_FALLING:
		LL_EXTI_DisableRisingTrig_0_31(InitStruct->Line_0_31);
		LL_EXTI_EnableFallingTrig_0_31(InitStruct->Line_0_31);
		break;
	case LL_EXTI_TRIGGER_RISING_FALLING:
		LL_EXTI_EnableRisingTrig_0_31(InitStruct->Line_0_31);
		LL_EXTI_EnableFallingTrig_0_31(InitStruct->Line_0_31);
		break;
	default:
		break;
	}
}
