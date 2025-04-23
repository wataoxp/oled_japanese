/*
 * ssd1309.c
 *
 *  Created on: Mar 1, 2025
 *      Author: wataoxp
 */
#include "ssd1309.h"
#include "main.h"

#define CS_AUTO_CONTROL

static SPI_TypeDef *lcdSPI;
static GPIO_TypeDef *DC_Port;
static uint8_t DC_Pin;
static uint8_t OLED_Buffer[OLED_PAGE][OLED_COLUMN] = {0};
static Font_Typedef *FontData;

static uint8_t Xpoint = 0;
static uint8_t Ypoint = 0;

static void OLED_GPIO_Config(GPIO_TypeDef *GPIOx,uint32_t Pos);
static void UpdatePointDisplay(uint8_t colBase,uint8_t colSize,uint8_t pageBase);
static void SendUpdatePoint(uint8_t x1,uint8_t x2,uint8_t y1,uint8_t y2);
static uint8_t EUC_GetCharacter(unsigned char *str,uint16_t *moji);

static inline void SelectCommand(void)
{
	GPIO_CLEAR(DC_Port,DC_Pin);
}
static inline void SelectData(void)
{
	GPIO_WRITE(DC_Port,DC_Pin);
}
static inline void OLED_SPI_Transmit(uint8_t *data,uint32_t size)
{
	SPI_Transmit8(lcdSPI, data, size);
}
void SetHandle(SPI_TypeDef *SPIx,GPIO_TypeDef *PortDC,uint8_t PinDC)
{
	lcdSPI = SPIx;
	DC_Port = PortDC;
	DC_Pin = PinDC;
}
void OLEDinit(uint8_t Contrast)
{
	uint8_t init[] = {
			DISPLAY_OFF,
			SET_RATIO,RATIO_MAX,		//Row number of lines
			ROW_OFFSET,ROW_MIN,			//Row StartLine
			START_LINE(0),
			SEGMENT_LEFT,				//Under
			COLUMM_LOWER,				//Direction
			SET_COM,COM_01,
			CONTRAST_CMD,CONTRAST_SET(Contrast),
			GDDRAM_DRAW,
			NORMAL_DISPLAY,
			LCD_OSC_DIV,SET_OSC_DIV(0x0,0x0),
			SET_MEMORYMODE,HORIZONTAL_MODE,
			//CHARGEPUMP_SET,PUMP_ON,
			DISPALY_ON,
	};

	OLED_GPIO_Config(GPIOA, Pin3);		//RES
	OLED_GPIO_Config(GPIOA, Pin5);		//DC...DATAorCMD

	/* RESET Pulse */

	GPIO_CLEAR(GPIOA,Pin3);
	Delay(10);
	GPIO_WRITE(GPIOA,Pin3);
	Delay(100);

	SelectCommand();

	OLED_SPI_Transmit(init, sizeof(init));

	FontData = GetHandle();
}
static void OLED_GPIO_Config(GPIO_TypeDef *GPIOx,uint32_t Pos)
{
	uint32_t Periphs = 0;
	GPIO_InitTypedef init;
	init.PinPos = Pos;
	init.Pull = LL_GPIO_PULL_NO;
	init.Mode = LL_GPIO_MODE_OUTPUT;
	init.Speed = LL_GPIO_SPEED_FREQ_LOW;
	init.OutputType = LL_GPIO_OUTPUT_PUSHPULL;

	Periphs = GetPortNumber(GPIOx);

	if(LL_IOP_GRP1_IsEnabledClock(Periphs) == 0)
	{
		LL_IOP_GRP1_EnableClock(Periphs);
	}

	GPIO_OutputInit(GPIOx, &init);
}
void ClearLCD(uint32_t inverse)
{
	uint8_t color;

	color = (inverse == Normal)?	0x00:0xFF;
	memset(OLED_Buffer,color,sizeof(OLED_Buffer));

	UpdateFillDisplay();
}
static inline bool EUC_IsByteSize(unsigned char c)
{
    return (c >= EUC_AREA_4 && c < EUC_AREA_ERROR);
}
static uint8_t EUC_GetCharacter(unsigned char *str,uint16_t *moji)
{
	uint8_t MojiSize = 0;

	if(EUC_IsByteSize(str[0]))
	{
		MojiSize = 2;
		*moji = (uint16_t)(str[0] << 8 | str[1]);

		switch(str[0])
		{
		case EUC_AREA_4:
			FontData->index = Kana;
			FontData->OffSet = KANA_OFFSET;
			break;
		case EUC_AREA_5:
			FontData->index = Katakana;
			FontData->OffSet= KATAKANA_OFFSET;
			break;
		default:
			FontData->index = Kanji;
			FontData->OffSet = KANJI_OFFSET + (str[0] - EUC_AREA_16_START) * EUC_AREA_16_SPACE;
			break;
		}
	}
	else
	{
		if(str[0] < EUC_ASCII_MAX)		//Ascii文字
		{
			MojiSize = 1;
			*moji = str[0];
		}
		else							//JIS第一水準に無い文字、エラー
		{
			MojiSize = 2;
			*moji = EUC_ASCII_MAX;
		}
		FontData->index = Ascii;
		FontData->OffSet = ASCII_OFFSET;
	}
	return MojiSize;
}
void WriteChar(uint16_t Asci)
{
	uint8_t Page = DIV8(Ypoint);
	uint16_t moji = Asci - FontData->OffSet;
	JisFont Text = GetFontData(FontData->index);

	FLASH->ACR |= FLASH_ACR_PRFTEN;
	if(Page == 1 || Page == 4)										//2行目と4行目
	{
		for(uint8_t i = 0; i < 12;i++)
		{
			OLED_Buffer[Page][Xpoint+i] |= Text[moji][i] << 4;		//LSBが表示上の上側になるので上位側にシフトする
			OLED_Buffer[Page+1][Xpoint+i] |= Text[moji][i+12] << 4 | ((Text[moji][i] & 0xF0) >> 4);
		}
	}
	else
	{
		memcpy(&OLED_Buffer[Page][Xpoint],Text[moji],12);			//1文字の上側8ドットを描画(24バイトの前半)
		memcpy(&OLED_Buffer[Page+1][Xpoint],Text[moji]+12,12);		//１文字の下側4ドットを描画(24バイトの後半)
	}
	FLASH->ACR &= ~FLASH_ACR_PRFTEN;
	Xpoint += FontData->Width;
}
void WriteString(char *str,uint8_t size)
{
	uint8_t colBase = Xpoint;			//描画前の座標を記憶
	uint8_t PageBase = DIV8(Ypoint);
	uint8_t Byte = 0;
	uint16_t moji;

	while(Byte < size)
	{
		Byte += EUC_GetCharacter((uint8_t*)&str[Byte], &moji);
		WriteChar(moji);
	}
	SendUpdatePoint(colBase, Xpoint-1, PageBase, PageBase+1);		//nカラムまで書く場合(n-1)カラムを指定する

	UpdatePointDisplay(colBase, Xpoint-colBase, PageBase);

	Xpoint = 0;
}
static void SendUpdatePoint(uint8_t x1,uint8_t x2,uint8_t y1,uint8_t y2)
{
	uint8_t cusor[] = {SET_COLUMN,x1,x2,SET_PAGE,y1,y2};

	SelectCommand();
	OLED_SPI_Transmit(cusor, sizeof(cusor));
}
//ピクセル単位ではなく、文字および行を指定する
void SetCusor(uint8_t x,uint8_t y)
{
	if(x > OLED_MAX_COLUMN) x = 0;
	if(y > OLED_LIMIT_PAGE) y = 0;

	Xpoint = x * FontData->Width;
	Ypoint = y * FontData->Height;
}
/***
	col&pageBase:XおよびY座標の描画開始点。
	colはバッファの開始点。pageは何番目の配列かを指定している。

	colSize:有効データ数。Baseと足すと描画完了点。
	pageEnd:フォントを変えない限りは必ずpageBase+1
***/
static void UpdatePointDisplay(uint8_t colBase,uint8_t colSize,uint8_t pageBase)
{
	SelectData();
	OLED_SPI_Transmit(&OLED_Buffer[pageBase][colBase], colSize);
	OLED_SPI_Transmit(&OLED_Buffer[pageBase+1][colBase], colSize);
}
void UpdateFillDisplay(void)
{
	SendUpdatePoint(0, OLED_MAX_COLUMN, 0, OLED_MAX_PAGE);
	SelectData();
	OLED_SPI_Transmit((uint8_t*)OLED_Buffer, sizeof(OLED_Buffer));
}

/********* ex.改行コードを認識、伸ばし棒をハイフンに変換 *******/
void StringLCD(char *str,uint8_t size)
{
	uint8_t Byte = 0;
	uint16_t moji;

	SetCusor(0, 0);

	while(Byte < size)
	{
		if((str[Byte] << 8 | str[Byte+1]) == 0xA1BC)
		{
			FontData->index = Ascii;
			FontData->OffSet = ASCII_OFFSET;
			WriteChar('-');
			Byte += 2;
		}
		else if(str[Byte] == '\n')
		{
			Xpoint = 0;
			Ypoint += 12;
			Byte++;
		}
		else
		{
			Byte += EUC_GetCharacter((uint8_t*)&str[Byte], &moji);
			WriteChar(moji);
		}
	}
	UpdateFillDisplay();
}
