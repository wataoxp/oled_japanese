/*
 * ssd1309.h
 *
 *  Created on: Mar 1, 2025
 *      Author: wataoxp
 */

#ifndef INC_SSD1309_H_
#define INC_SSD1309_H_

#include "main.h"
#include "spi.h"
#include "utf_code.h"
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
//連続送信コマンド
#define REG_CMD 0x00
#define REG_DATA 0x40

//制御コマンド+1バイト送信時
#define PUSH_CTRL 0x80
#define PUSH_DATA 0xC0
/* I2C end */

/* SSD1309 Command table */

//LCD CONTRAST 2ByteCommand
#define CONTRAST_CMD 0x81		//制御コマンド
#define CONTRAST_SET(x) (x)

//GDDRAM command
#define GDDRAM_DRAW 0xA4		//GDDRAMの内容を表示
#define GDDRAM_FORCE 0xA5		//GDDRAMの内容を無視して強制的に全画面点灯

//Display Inverse
#define NORMAL_DISPLAY 0xA6		//通常表示
#define INVERSE_DISPALY 0xA7	//反転表示

//SetDisplay
#define DISPLAY_OFF 0xAE		//ディスプレイOFF。スリープモード
#define DISPALY_ON 0xAF			//ディスプレイON

/* MemoryMode SetCommand */

//Mode 2ByteCommand
#define SET_MEMORYMODE 0x20		//制御コマンド

#define HORIZONTAL_MODE 0x00		//水平方向へ描画。Column終了アドレスまで。終了アドレス到達後はPage+1
#define VERTICAL_MODE 0x01			//垂直方向へ描画。Page終了アドレスまで。終了アドレス到達後はColumn+1
#define PAGEADDRESSING_MODE 0x02	//水平方向へ描画。Column終了アドレスまで。到達後はColumnを0に戻す

//Horizontal&Vertical Mode 3ByteCommand
#define SET_COLUMN 0x21			//Columnの開始・終了アドレスを続けて送る
#define SET_PAGE 0x22			//Pageの開始・終了アドレスを続けて送る

//PageAddressingMode
#define COLUMN_LOWER_BIT 0x00	//Column開始アドレスの下位4ビットを指定する
#define COLUMN_HIGHER_BIT 0x10	//こちらは上位ビット。合計で0x3F(127)を超えてはならない
#define PAGE_ADDR_SET(x) (0xB0 | (x & 0x7))		//Page開始アドレスを指定する

/* MemoryMode end */

//StartLine Set
#define START_LINE(x) (0x40 | (x & 0x3F))		//縦の描画開始点をドット単位で指定する。

//ViewLine direction
#define SEGMENT_RIGHT 0xA0		//SEGと順方向
#define SEGMENT_LEFT 0xA1		//SEGと逆方向
#define COLUMM_UPPER 0xC0		//0~Ratio行まで表示
#define COLUMM_LOWER 0xC8		//Ratio~0行まで表示

//表示行数 2Bytecommnad
#define SET_RATIO 0xA8		//RATIO+1が表示行数
#define RATIO_MAX 0x3F		//63
#define RATIO_HERF 0x1F		//31
#define RATIO_MIN 0x0F		//15

//表示開始行を指定数移動する 2Bytecommnad
#define ROW_OFFSET 0xD3
#define ROW_MAX 0x3F		//Row63から表示。描画範囲を超える場合、次のデータはRow0から表示される。
#define ROW_HERF 0x1F		//Row31から表示
#define ROW_MIN 0x00		//Row0から表示

//ハードウェア構成コマンド 2ByteCommand
#define SET_COM 0xDA			//SET_COMの後に下記4つのいずれかを送る
#define COM_00	0x02
#define COM_01	0x12
#define COM_10	0x22
#define COM_11	0x32

//DispalyClock&Osilator
#define LCD_OSC_DIV 0xD5		//周波数を高くしても画面がちらつくだけだった
#define SET_OSC_DIV(osc,div) ((osc & 0x0F) | (div & 0x0F))	//上位4ビットが周波数。下位4ビットが分周比

//ChargePump
#define CHARGEPUMP_SET 0x8D	//基板上のコンデンサを用いて7.5Vを生成している
#define PUMP_OFF 0x10
#define PUMP_ON 0x14		//ONにしないと画面は表示されない(1309では送らなくてもOK？データシートにも記述なし)

enum class RetCode : uint8_t{
	succses,
	failed,
	NotFoundPin,
};

typedef struct{
	GPIO_TypeDef *RSTport;
	uint32_t RSTpin;
	GPIO_TypeDef *DCport;
	uint32_t DCpin;
}OledParam;

class OLED{
private:
	SPI& serial;
	OledParam& VAL;
	uint8_t OLED_Buffer[OLED_PAGE][OLED_COLUMN] = {0};
	uint8_t Xpoint = 0;
	uint8_t Ypoint = 0;
	Font_Typedef *FontData;
	const UTFtoEUC *Table;
	inline void SelectCommand(void);
	inline void SelectData(void);
	inline void OledTransmit(uint8_t *data,uint32_t size);
	RetCode PinConfig(void);

	/*** 文字コード判定 ***/
	uint8_t GetCharacter(unsigned char *str,uint16_t *moji);
	inline uint16_t MojiKana_Katakana(uint32_t MultiByte);
	inline uint16_t MojiKanji(uint32_t MultiByte);
	inline void MojiAscii(void);
	inline uint8_t MojiError(uint32_t MultiByte);

	void SendUpdatePoint(uint8_t x1,uint8_t x2,uint8_t y1,uint8_t y2);
	void UpdatePointDisplay(uint8_t colBase,uint8_t colSize,uint8_t pageBase);
	uint16_t ConvUTF(uint32_t Kanji);

public:
	OLED(SPI& spi,OledParam &obj);
	void Begin(uint8_t Contrast);
	void ClearLCD(void);
	void SetCusor(uint8_t x,uint8_t y);
	void WriteChar(uint16_t Asci);
	void WriteString(char *str,uint8_t size);
	void UpdateFillDisplay(void);
};

/* Function */
extern "C"{
void StringLCD(char *str,uint8_t size);

void SetHandle(SPI_TypeDef *SPIx,GPIO_TypeDef *PortDC,uint8_t PinDC);
void OLEDinit(uint8_t Contrast);
void ClearLCD(uint32_t inverse);

void WriteChar(uint16_t Asci);
void WriteString(char *str,uint8_t size);
void SetCusor(uint8_t x,uint8_t y);
void UpdateFillDisplay(void);
}


#endif /* INC_SSD1309_H_ */
