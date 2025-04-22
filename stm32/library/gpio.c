/*
 * gpio.c
 *
 *  Created on: Jan 12, 2025
 *      Author: wataoxp
 */

#include "gpio.h"

uint32_t GetPortNumber(GPIO_TypeDef *GPIOx)
{
	uint32_t Periphs = 0;

	if(GPIOx == GPIOA)
	{
		Periphs = 1 << PORTA;
	}
	else if(GPIOx == GPIOB)
	{
		Periphs = 1 << PORTB;
	}
	else if(GPIOx == GPIOC)
	{
		Periphs = 1 << PORTC;
	}
	else if(GPIOx == GPIOD)
	{
		Periphs = 1 << PORTD;
	}
	else if(GPIOx == GPIOF)
	{
		Periphs = 1 << PORTF;
	}
	else
	{
		Periphs = 0;
	}

	return Periphs;
}

GPIO_Code GPIO_OutputInit(GPIO_TypeDef *GPIOx,GPIO_InitTypedef *InitStruct)
{
	if((InitStruct->PinPos == Pin13) || (InitStruct->PinPos == Pin14))
	{
		if(GPIOx == GPIOA)
		{
			return init_Failed;
		}
	}
	//WRITE_REG(GPIOx->BRR,1 << InitStruct->PinPos);
	GPIO_CLEAR(GPIOx,InitStruct->PinPos);
	GPIO_SetPinSpeed(GPIOx, InitStruct->PinPos, InitStruct->Speed);
	GPIO_SetOutputPinType(GPIOx, (1 << InitStruct->PinPos), InitStruct->OutputType);

	if(InitStruct->Mode == LL_GPIO_MODE_ALTERNATE)
	{
		if(InitStruct->PinPos < Pin8)
		{
			GPIO_SetAlternate0_7(GPIOx, InitStruct->PinPos, InitStruct->Alternate);
		}
		else
		{
			GPIO_SetAlternate8_15(GPIOx, InitStruct->PinPos, InitStruct->Alternate);
		}
	}
	GPIO_SetPinPull(GPIOx, InitStruct->PinPos, InitStruct->Pull);
	GPIO_SetPinMode(GPIOx, InitStruct->PinPos, InitStruct->Mode);

	return init_Success;
}
GPIO_Code GPIO_InputInit(GPIO_TypeDef *GPIOx,uint32_t PinPos,uint32_t Mode,uint32_t Pull)
{
	uint32_t Periphs = 0;

	if((PinPos == Pin13) || (PinPos == Pin14))
	{
		if(GPIOx == GPIOA)
		{
			return init_Failed;
		}
	}
	Periphs = GetPortNumber(GPIOx);

	if(LL_IOP_GRP1_IsEnabledClock(Periphs) == 0)
	{
		LL_IOP_GRP1_EnableClock(Periphs);
	}
	GPIO_SetPinPull(GPIOx, PinPos, Pull);
	GPIO_SetPinMode(GPIOx, PinPos, Mode);

	return init_Success;
}
