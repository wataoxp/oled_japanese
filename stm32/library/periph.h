/*
 * periph.h
 *
 *  Created on: Feb 17, 2025
 *      Author: wataoxp
 */

#ifndef SRC_PERIPH_H_
#define SRC_PERIPH_H_

#define STM32G0xx

#ifdef STM32G0xx
#include "stm32g0xx.h"

/* System Headers */
#include "stm32g0xx_ll_rcc.h"
#include "stm32g0xx_ll_bus.h"
#include "stm32g0xx_ll_system.h"
#include "stm32g0xx_ll_exti.h"
#include "stm32g0xx_ll_cortex.h"
#include "stm32g0xx_ll_utils.h"
#include "stm32g0xx_ll_pwr.h"
#include "stm32g0xx_ll_dma.h"
#include "stm32g0xx_ll_tim.h"
#include "stm32g0xx_ll_gpio.h"

typedef enum{
	Pin0,
	Pin1,
	Pin2,
	Pin3,
	Pin4,
	Pin5,
	Pin6,
	Pin7,
	Pin8,
	Pin9,
	Pin10,
	Pin11,
	Pin12,
	Pin13,
	Pin14,
	Pin15,
}GPIO_Pin;

typedef enum{
	PORTA,
	PORTB,
	PORTC,
	PORTD,
	PORTF = 5,
}GPIO_Port;

typedef enum{
	HighPriority = 0,
	MidPriority,
	LowPriority,
}NVIC_Prioritys;


#endif

#endif /* SRC_PERIPH_H_ */
