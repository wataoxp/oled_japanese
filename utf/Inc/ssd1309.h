/*
 * ssd1309.h
 *
 *  Created on: Mar 1, 2025
 *      Author: wataoxp
 */

#ifndef INC_SSD1309_H_
#define INC_SSD1309_H_

#include "spi.h"
#include "font.h"
#include "gpio.h"
#include "delay.h"

#include <string.h>
#include <stdbool.h>
#include <stdint.h>

typedef enum{
	Normal,
	Inverse,
}OLED_BackColor;

typedef enum{
	String_OverRun,
}OLED_ErrorStatus;

#define SSD1306_ADDR (0x3C << 1)

#define OLED_COLUMN 128
#define OLED_PAGE 8
#define OLED_MAX_COLUMN (OLED_COLUMN - 1)
#define OLED_MAX_PAGE (OLED_PAGE - 1)

#define OLED_LIMIT_PAGE 4

#define DIV8(n) (n >> 3)
#define MOD8(n) (n & 7)

/* Control Byte.forI2C */
//Ϣ³�������ޥ��
#define REG_CMD 0x00
#define REG_DATA 0x40

//���楳�ޥ��+1�Х���������
#define PUSH_CTRL 0x80
#define PUSH_DATA 0xC0
/* I2C end */

/* SSD1309 Command table */

//LCD CONTRAST 2ByteCommand
#define CONTRAST_CMD 0x81		//���楳�ޥ��
#define CONTRAST_SET(x) (x & 0xFF)

//GDDRAM command
#define GDDRAM_DRAW 0xA4		//GDDRAM�����Ƥ�ɽ��
#define GDDRAM_FORCE 0xA5		//GDDRAM�����Ƥ�̵�뤷�ƶ���Ū������������

//Display Inverse
#define NORMAL_DISPLAY 0xA6		//�̾�ɽ��
#define INVERSE_DISPALY 0xA7	//ȿžɽ��

//SetDisplay
#define DISPLAY_OFF 0xAE		//�ǥ����ץ쥤OFF�����꡼�ץ⡼��
#define DISPALY_ON 0xAF			//�ǥ����ץ쥤ON

/* MemoryMode SetCommand */

//Mode 2ByteCommand
#define SET_MEMORYMODE 0x20		//���楳�ޥ��

#define HORIZONTAL_MODE 0x00		//��ʿ���������衣Column��λ���ɥ쥹�ޤǡ���λ���ɥ쥹��ã���Page+1
#define VERTICAL_MODE 0x01			//��ľ���������衣Page��λ���ɥ쥹�ޤǡ���λ���ɥ쥹��ã���Column+1
#define PAGEADDRESSING_MODE 0x02	//��ʿ���������衣Column��λ���ɥ쥹�ޤǡ���ã���Column��0���᤹

//Horizontal&Vertical Mode 3ByteCommand
#define SET_COLUMN 0x21			//Column�γ��ϡ���λ���ɥ쥹��³��������
#define SET_PAGE 0x22			//Page�γ��ϡ���λ���ɥ쥹��³��������

//PageAddressingMode
#define COLUMN_LOWER_BIT 0x00	//Column���ϥ��ɥ쥹�β���4�ӥåȤ���ꤹ��
#define COLUMN_HIGHER_BIT 0x10	//������Ͼ�̥ӥåȡ���פ�0x3F(127)��Ķ���ƤϤʤ�ʤ�
#define PAGE_ADDR_SET(x) (0xB0 | (x & 0x7))		//Page���ϥ��ɥ쥹����ꤹ��

/* MemoryMode end */

//StartLine Set
#define START_LINE(x) (0x40 | (x & 0x3F))		//�Ĥ����賫������ɥå�ñ�̤ǻ��ꤹ�롣

//ViewLine direction
#define SEGMENT_RIGHT 0xA0		//SEG�Ƚ�����
#define SEGMENT_LEFT 0xA1		//SEG�ȵ�����
#define COLUMM_UPPER 0xC0		//0~Ratio�Ԥޤ�ɽ��
#define COLUMM_LOWER 0xC8		//Ratio~0�Ԥޤ�ɽ��

//ɽ���Կ� 2Bytecommnad
#define SET_RATIO 0xA8		//RATIO+1��ɽ���Կ�
#define RATIO_MAX 0x3F		//63
#define RATIO_HERF 0x1F		//31
#define RATIO_MIN 0x0F		//15

//ɽ�����ϹԤ�������ư���� 2Bytecommnad
#define ROW_OFFSET 0xD3
#define ROW_MAX 0x3F		//Row63����ɽ���������ϰϤ�Ķ�����硢���Υǡ�����Row0����ɽ������롣
#define ROW_HERF 0x1F		//Row31����ɽ��
#define ROW_MIN 0x00		//Row0����ɽ��

//�ϡ��ɥ������������ޥ�� 2ByteCommand
#define SET_COM 0xDA			//SET_COM�θ�˲���4�ĤΤ����줫������
#define COM_00	0x02
#define COM_01	0x12
#define COM_10	0x22
#define COM_11	0x32

//DispalyClock&Osilator
#define LCD_OSC_DIV 0xD5		//���ȿ���⤯���Ƥ���̤�����Ĥ��������ä�
#define SET_OSC_DIV(osc,div) ((osc & 0x0F) | (div & 0x0F))	//���4�ӥåȤ����ȿ�������4�ӥåȤ�ʬ����

//ChargePump
#define CHARGEPUMP_SET 0x8D	//���ľ�Υ���ǥ󥵤��Ѥ���7.5V���������Ƥ���
#define PUMP_OFF 0x10
#define PUMP_ON 0x14		//ON�ˤ��ʤ��Ȳ��̤�ɽ������ʤ�(1309�Ǥ�����ʤ��Ƥ�OK���ǡ��������Ȥˤ⵭�Ҥʤ�)

/* Function */

void StringLCD(char *str,uint8_t size);

void SetHandle(SPI_TypeDef *SPIx,GPIO_TypeDef *PortDC,uint8_t PinDC);
void OLEDinit(uint8_t Contrast);
void ClearLCD(uint32_t inverse);

void WriteChar(uint16_t Asci);
void WriteString(char *str,uint8_t size);
void SetCusor(uint8_t x,uint8_t y);
void UpdateFillDisplay(void);

/* ��α
 *
#define I2C_AUTOEND LL_I2C_MODE_AUTOEND
#define I2C_SOFTEND LL_I2C_MODE_SOFTEND		//255�Х��Ȥ�Ķ����ǡ����������������

//#define SET_VCOM 0xDB
//#define VCOM_77x 0x20
 */

#endif /* INC_SSD1309_H_ */
