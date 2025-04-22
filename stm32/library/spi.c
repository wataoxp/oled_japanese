/*
 * spi.c
 *
 *  Created on: Feb 21, 2025
 *      Author: wataoxp
 */
#include "spi.h"
#include "delay.h"

ErrorStatus SPI_Init(SPI_TypeDef *SPIx,SPI_InitTypedef *config)
{
	uint16_t CR1Reg = config->TransferDirection |  config->Mode |config->ClockPolarity | \
    		config->ClockPhase | config->NSS | config->SSI | config->BaudRate | config->BitOrder;

	if(LL_SPI_IsEnabled(SPIx) != 0)
	{
		return ERROR;
	}
	WRITE_REG(SPIx->CR1,CR1Reg);

    MODIFY_REG(SPIx->CR2,SPI_CR2_DS | SPI_CR2_SSOE,config->DataWidth | config->SSOE);

    if(config->DataWidth < LL_SPI_DATAWIDTH_9BIT)		//データサイズが8ビット以下ならFRXTHをセットする
    {
    	LL_SPI_SetRxFIFOThreshold(SPIx, LL_SPI_RX_FIFO_TH_QUARTER);
    }
    LL_SPI_DisableCRC(SPIx);

    LL_I2S_Disable(SPIx);								//I2SとSPIは排他的な関係

	LL_SPI_SetStandard(SPIx, LL_SPI_PROTOCOL_MOTOROLA);
	LL_SPI_DisableNSSPulseMgt(SPIx);

    return SUCCESS;
}
void SPI_StructInit(SPI_InitTypedef *config,uint32_t SPI_Mode,uint32_t NSS_Mode)
{
	config->TransferDirection = LL_SPI_FULL_DUPLEX;
	config->DataWidth = LL_SPI_DATAWIDTH_8BIT;
	config->ClockPolarity = LL_SPI_POLARITY_HIGH;
	config->ClockPhase = LL_SPI_PHASE_2EDGE;
	config->BitOrder = LL_SPI_MSB_FIRST;
	config->NSS = NSS_Mode;
	config->Mode = SPI_Mode;

	if(SPI_Mode == MSTR_MASTER)
	{
		config->SSI = SPI_CR1_SSI;
		config->SSOE = SSOE_OUTPUT_ENABLE;
		config->BaudRate = LL_SPI_BAUDRATEPRESCALER_DIV32;		//SPIクロック周波数
	}
	if(NSS_Mode == NSS_SOFT_CONTROL)
	{
		config->SSOE = SSOE_OUTPUT_DISABLE;
	}
}
uint8_t SPI_Transmit8(SPI_TypeDef *SPIx,uint8_t *buf,uint16_t length)
{
	uint8_t ret;

	LL_SPI_Enable(SPIx);

	while(LL_SPI_IsActiveFlag_BSY(SPIx));

	for(uint32_t i = 0;i < length;i++)
	{
		while(LL_SPI_IsActiveFlag_TXE(SPIx) == 0);
		LL_SPI_TransmitData8(SPIx, buf[i]);
	}

	while(LL_SPI_IsActiveFlag_BSY(SPIx));
	ret = LL_SPI_GetTxFIFOLevel(SPIx);		//0でない場合送信漏れがある

	LL_SPI_Disable(SPIx);

	return ret;
}
void SPI_MasterTransmitReceive8(SPI_TypeDef *SPIx,uint8_t *TXbuf,uint8_t *Rxbuf,uint32_t size)
{
	LL_SPI_Enable(SPIx);

	while(LL_SPI_IsActiveFlag_BSY(SPIx));

	for(uint32_t i = 0;i < size; i++)
	{
		while(LL_SPI_IsActiveFlag_TXE(SPIx) == 0);
		LL_SPI_TransmitData8(SPIx, TXbuf[i]);
		while(LL_SPI_IsActiveFlag_RXNE(SPIx) == 0);
		Rxbuf[i] = LL_SPI_ReceiveData8(SPIx);

		Delay(1);
	}
	while(LL_SPI_IsActiveFlag_BSY(SPIx));

	LL_SPI_Disable(SPIx);
}
