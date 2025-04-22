#ifndef __MAIN_H
#define __MAIN_H

#include "periph.h"

#include "gpio.h"
#include "rcc.h"
#include "delay.h"

#include "spi.h"

typedef struct{
	uint32_t initoled : 1;
	uint32_t cusor : 1;
	uint32_t kanji : 1;

	uint32_t initgpio : 1;
}InitErrorCheck;

#endif
