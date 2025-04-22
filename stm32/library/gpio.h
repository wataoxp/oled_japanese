/*
 * gpio.h
 *
 *  Created on: Jan 12, 2025
 *      Author: wataoxp
 */

#ifndef INC_GPIO_H_
#define INC_GPIO_H_

#include "periph.h"

//#define GPIO_BASE  (0x00000400UL)
//#define GPIO_SET_PORT(x) ((GPIO_TypeDef *)(IOPORT_BASE + GPIO_BASE * x))

#define GPIO_WRITE(GPIOx,Pin) (GPIOx->BSRR = 1 << Pin)
#define GPIO_CLEAR(GPIOx,Pin) (GPIOx->BRR = 1 << Pin)
#define GPIO_READ(GPIOx,Pin) ((GPIOx->IDR & 1 << Pin) >> Pin)

typedef enum{
	init_Success,
	init_Failed,
}GPIO_Code;

typedef struct{
	uint32_t PinPos;
	uint32_t Mode;
	uint32_t Speed;
	uint32_t OutputType;
	uint32_t Pull;
	uint32_t Alternate;
}GPIO_InitTypedef;

static inline void GPIO_SetPinSpeed(GPIO_TypeDef *GPIOx,uint32_t pinpos,uint32_t speed)
{
	MODIFY_REG(GPIOx->OSPEEDR,GPIO_OSPEEDR_OSPEED0 << (pinpos * 2),(speed << (pinpos * 2)));
}
static inline void GPIO_SetPinPull(GPIO_TypeDef *GPIOx,uint32_t pinpos,uint32_t pull)
{
	MODIFY_REG(GPIOx->PUPDR,GPIO_PUPDR_PUPD0 << (pinpos * 2),(pull << (pinpos * 2)));
}
static inline void GPIO_SetPinMode(GPIO_TypeDef *GPIOx,uint32_t pinpos,uint32_t mode)
{
	MODIFY_REG(GPIOx->MODER,GPIO_MODER_MODE0 << (pinpos * 2),(mode << (pinpos * 2)));
}
static inline void GPIO_SetOutputPinType(GPIO_TypeDef *GPIOx,uint32_t pinmask,uint32_t outputType)
{
	MODIFY_REG(GPIOx->OTYPER,pinmask,(pinmask * outputType));
}
static inline void GPIO_SetAlternate0_7(GPIO_TypeDef* GPIOx,uint32_t pinpos,uint32_t alternate)
{
	MODIFY_REG(GPIOx->AFR[0],GPIO_AFRL_AFSEL0 << (pinpos * 4),alternate << (pinpos * 4));
}
static inline void GPIO_SetAlternate8_15(GPIO_TypeDef* GPIOx,uint32_t pinpos,uint32_t alternate)
{
	pinpos -= 8;
	MODIFY_REG(GPIOx->AFR[1],GPIO_AFRH_AFSEL8 << (pinpos * 4),alternate << (pinpos * 4));
}

uint32_t GetPortNumber(GPIO_TypeDef *GPIOx);
GPIO_Code GPIO_OutputInit(GPIO_TypeDef *GPIOx,GPIO_InitTypedef *InitStruct);
GPIO_Code GPIO_InputInit(GPIO_TypeDef *GPIOx,uint32_t PinPos,uint32_t Mode,uint32_t Pull);


#endif /* INC_GPIO_H_ */
