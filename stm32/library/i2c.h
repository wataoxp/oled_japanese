/*
 * i2c.h
 *
 *  Created on: Nov 5, 2024
 *      Author: wataoxp
 */

#ifndef INC_I2C_H_
#define INC_I2C_H_

#include <stdint.h>
#include "stm32g0xx_ll_i2c.h"

//I2Cスレーブメモリアドレス幅(byte)
#define I2C_MEMADD_SIZE_8BIT UINT8_C(1)
#define I2C_MEMADD_SIZE_16BIT UINT8_C(2)

//I2C_OAR1レジスタ設定用。7ビットアドレスの場合最下位ビット(OA１)は無視されるので１ビットシフトする
#define I2C_OA1_7BIT_Pos 1

typedef enum{
	TX_ISR,
	TX_Poll,
}I2C_Mode;

typedef struct{
	uint8_t *BufAdd;
	uint8_t BufSize;
}I2C_IsrTypedef;

/**
 * I2C Function Parameter
 * @param I2Cx...I2C Instance
 * @param address...SlaveAddress
 * @param data_*data...Writebuffer Address
 * @param *buffer...ReadDatabuffer Address
 * @param Reg...SlaveRegisterAddress
 * @param RegSize...RegisterSize
 * 		@arg I2C_MEMADD_SIZE_8BIT
 * 		@arg I2C_MEMADD_SIZE_16BIT
 * @param length...Send&Recv Datasize
 **/

/**
 * I2C_CR2 Register Config
 * @param I2Cx...I2C Instance
 * @param address...SlaveAddress
 * @param direction...I2C Transmission Direction
 * @arg LL_I2C_REQUEST_WRITE
 * @arg LL_I2C_REQUEST_READ
 * @param length...Transmission Size
 * @param EndMode...REPEAT STRATorSTOPF
 * @arg LL_I2C_MODE_SOFTEND...ReStart
 * @arg LL_I2C_MODE_AUTOEND...STOPF
 **/
static inline void CR2SetUP(I2C_TypeDef *I2Cx,uint8_t address,uint16_t direction,uint8_t length,uint32_t EndMode)
{
	uint32_t tmp = 0;
	tmp = (address & I2C_CR2_SADD)  | (direction & I2C_CR2_RD_WRN) | I2C_CR2_START | (length << I2C_CR2_NBYTES_Pos) | (EndMode & I2C_CR2_AUTOEND);

	MODIFY_REG(I2Cx->CR2,I2C_CR2_SADD|I2C_CR2_RD_WRN|I2C_CR2_NBYTES|I2C_CR2_START|I2C_CR2_AUTOEND,tmp);
}

//I2C MasterInit
void I2C_Config_Master(I2C_TypeDef *I2Cx);

//I2C SlaveInit
void I2C_Config_Slave(I2C_TypeDef *I2Cx,uint8_t SlaveAddr);

//ISRorPolling
void I2C_TransmitMode(I2C_TypeDef *I2Cx,I2C_Mode type);

//Single Write&RegAddress
void PushI2C_Mem_Write(I2C_TypeDef *I2Cx,uint8_t address,uint8_t data,uint16_t Reg,uint8_t RegSize);

//Sequential Write&RegAddress
void SeqI2C_Mem_Write(I2C_TypeDef *I2Cx,uint8_t address,uint8_t *data,uint16_t Reg,uint8_t RegSize,uint8_t length);

//Sequential Write
void I2C_Master_Transmit(I2C_TypeDef *I2Cx,uint8_t address,uint8_t *data,uint8_t length);

//Sequential Read&RegAddress
void I2C_Mem_Read(I2C_TypeDef *I2Cx,uint8_t address,uint8_t *buffer,uint16_t Reg,uint8_t RegSize,uint8_t length);


/* i2c_isr.c */


//ISR Write&RegAddress
void I2C_ISR_Mem_Write(I2C_TypeDef *I2Cx,uint8_t address,uint8_t *data,uint16_t Reg,uint8_t RegSize,uint8_t length);

//ISR Write
void I2C_ISR_Master_Transmit(I2C_TypeDef *I2Cx,uint8_t address,uint8_t *data,uint8_t length);

//ISR TX
uint8_t I2C_Master_IT(I2C_TypeDef *I2Cx);

//ISR Slave
uint8_t I2C_Slave_IT(I2C_TypeDef *I2Cx);

//ISR DataBufferGet
I2C_IsrTypedef* I2C_GetBufferAddress(void);

#endif /* INC_I2C_H_ */
