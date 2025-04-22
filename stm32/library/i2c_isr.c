/*
 * i2c_isr.c
 *
 *  Created on: Mar 1, 2025
 *      Author: wataoxp
 */
#include "i2c.h"
#include <string.h>

/* 割り込み送信用のバッファ */
static uint8_t SendBuffer[128];
static uint8_t SendSize;
/* 割り込み受信用のバッファ */
static uint8_t Buf[UINT8_MAX];
static uint8_t BufSize = 0;
static I2C_IsrTypedef obj = {
		.BufAdd = Buf,
		.BufSize = 0,
};

void I2C_TransmitMode(I2C_TypeDef *I2Cx,I2C_Mode type)
{
	if(type == TX_ISR)
	{
		LL_I2C_EnableIT_TX(I2Cx);
		LL_I2C_EnableIT_STOP(I2Cx);
		__NVIC_EnableIRQ(I2C2_IRQn);
	}
	else
	{
		LL_I2C_DisableIT_TX(I2Cx);
		LL_I2C_DisableIT_STOP(I2Cx);
		__NVIC_DisableIRQ(I2C2_IRQn);
	}
}

void I2C_ISR_Mem_Write(I2C_TypeDef *I2Cx,uint8_t address,uint8_t *data,uint16_t Reg,uint8_t RegSize,uint8_t length)
{
	uint8_t i = 0;
	while(LL_I2C_IsActiveFlag_BUSY(I2Cx) != 0);

	CR2SetUP(I2Cx, address, LL_I2C_REQUEST_WRITE, RegSize+length, LL_I2C_MODE_AUTOEND);

	if(RegSize == I2C_MEMADD_SIZE_8BIT)
	{
		Buf[i++] = (uint8_t)Reg & 0xFF;
	}
	else
	{
		Buf[i++] = (uint8_t)(Reg >> 8);
		Buf[i++] = (uint8_t)Reg & 0xFF;
	}
	memcpy(Buf+i,data,length);
}
void I2C_ISR_Master_Transmit(I2C_TypeDef *I2Cx,uint8_t address,uint8_t *data,uint8_t length)
{
	while(LL_I2C_IsActiveFlag_BUSY(I2Cx) != 0);

	CR2SetUP(I2Cx, address, LL_I2C_REQUEST_WRITE, length, LL_I2C_MODE_AUTOEND);

	memcpy(SendBuffer,data,length);
}
uint8_t I2C_Slave_IT(I2C_TypeDef *I2Cx)
{
	static uint32_t Direction;

	if(BufSize >= sizeof(Buf))
	{
		return 1;
	}

	if(LL_I2C_IsActiveFlag_ADDR(I2Cx))
	{
		LL_I2C_ClearFlag_ADDR(I2Cx);
		Direction = LL_I2C_GetTransferDirection(I2Cx);
	}
	else if(LL_I2C_IsActiveFlag_RXNE(I2Cx) && Direction != I2C_ISR_DIR_Msk)
	{
		Buf[BufSize++] = LL_I2C_ReceiveData8(I2Cx);
	}
	else if(LL_I2C_IsActiveFlag_TXIS(I2Cx) && Direction == I2C_ISR_DIR_Msk)
	{
		LL_I2C_TransmitData8(I2Cx, 10);
	}
	else if(LL_I2C_IsActiveFlag_STOP(I2Cx))
	{
		LL_I2C_ClearFlag_STOP(I2Cx);
		Direction = 0;
	}

	return 0;
}
uint8_t I2C_Master_IT(I2C_TypeDef *I2Cx)
{
	if(SendSize >= sizeof(SendBuffer))
	{
		return 1;
	}

	if(LL_I2C_IsActiveFlag_TXIS(I2Cx) || LL_I2C_IsActiveFlag_RXNE(I2Cx))
	{
		if(LL_I2C_GetTransferRequest(I2Cx) ==  I2C_CR2_RD_WRN)
		{
			SendBuffer[SendSize++] = LL_I2C_ReceiveData8(I2Cx);
		}
		else
		{
			LL_I2C_TransmitData8(I2Cx, SendBuffer[SendSize++]);
		}
	}
	else if(LL_I2C_IsActiveFlag_STOP(I2Cx))
	{
		LL_I2C_ClearFlag_STOP(I2Cx);
		SendSize = 0;
	}

	return 0;
}
I2C_IsrTypedef* I2C_GetBufferAddress(void)
{
	obj.BufSize = BufSize;
	BufSize = 0;

	return &obj;
}
