/*
 * ssd1309.c
 *
 *  Created on: Mar 1, 2025
 *      Author: wataoxp
 */
#include "ssd1309.h"

OLED::OLED(SPI& spi,OledParam &obj) : serial(spi),VAL(obj)
{
	;
}
/*** 3線式OLED制御関数 ***/
inline void OLED::SelectCommand(void)
{
	GPIO_CLEAR(VAL.DCport,VAL.DCpin);
}
inline void OLED::SelectData(void)
{
	GPIO_WRITE(VAL.DCport,VAL.DCpin);
}
inline void OLED::OledTransmit(uint8_t *data,uint32_t size)
{
	serial.Transfer(data, size);
}

/*** OLED 設定関数 ***/
void OLED::Begin(uint8_t Contrast)
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
			DISPALY_ON,
	};

	PinConfig();

	/* RESET Pulse */
	GPIO_CLEAR(VAL.RSTport,VAL.RSTpin);
	Delay(10);
	GPIO_WRITE(VAL.RSTport,VAL.RSTpin);
	Delay(100);

	SelectCommand();
	OledTransmit(init, sizeof(init));

	FontData = GetHandle();
	Table = GetKanjiStruct();
}

/*** リセットピン、およびDC(データorコマンド)用GPIO設定 ***/
RetCode OLED::PinConfig(void)
{
	uint32_t ret = 0;

	GPIO Select(VAL.DCport,VAL.DCpin);
	GPIO Reset(VAL.RSTport,VAL.RSTpin);

	ret = Select.Begin();
	ret = Reset.Begin();

	Select.SetParameter(LL_GPIO_PULL_NO, LL_GPIO_MODE_OUTPUT, LL_GPIO_SPEED_FREQ_LOW, LL_GPIO_OUTPUT_PUSHPULL);
	Reset.SetParameter(LL_GPIO_PULL_NO, LL_GPIO_MODE_OUTPUT, LL_GPIO_SPEED_FREQ_LOW, LL_GPIO_OUTPUT_PUSHPULL);

	Select.OutputInit();
	Reset.OutputInit();

	return (ret == 0)? RetCode::succses:RetCode::NotFoundPin;
}

void OLED::ClearLCD(void)
{
	memset(OLED_Buffer,0x00,sizeof(OLED_Buffer));
	UpdateFillDisplay();
}

/*** ピクセル単位ではなく、文字および行を指定する ***/
void OLED::SetCusor(uint8_t x,uint8_t y)
{
	if(x > OLED_MAX_COLUMN) x = 0;
	if(y > OLED_LIMIT_PAGE) y = 0;

	Xpoint = x * FontData->Width;
	Ypoint = y * FontData->Height;
}

/*** 文字コード取得、変換関数群 ***/

uint8_t OLED::GetCharacter(unsigned char *str,uint16_t *moji)
{
	uint8_t MojiSize = 3;
	uint32_t MultiByte = 0;

	MultiByte = (uint32_t)(str[0] << 16 | str[1] << 8 | str[2]);

	if(str[0] >= UTF_AREA_16)
	{
		*moji = MojiKanji(MultiByte);
	}
	else if(str[0] >= UTF_AREA_4_5 && MultiByte != UTF_TYOUON)
	{
		*moji = MojiKana_Katakana(MultiByte);
	}
	else if(str[0] < UTF_ASCII)
	{
		*moji = str[0];
		MojiSize = 1;
		MojiAscii();
	}
	else
	{
		*moji = MojiError(MultiByte);
	}

	return MojiSize;
}

inline uint16_t OLED::MojiKana_Katakana(uint32_t MultiByte)
{
	uint16_t Moji;

	if(MultiByte <= UTF_KANA_END)
	{
		FontData->index = Kana;
		FontData->OffSet = KANA_OFFSET;
		//「み」0x381BFの次で数値が飛ぶのでその為の分岐
		Moji = (MultiByte <= 0xE381BF)? (uint16_t)MultiByte - UTF_KANA_OFFSET:(uint16_t)MultiByte - (UTF_KANA_OFFSET + 0xC0);
	}
	else
	{
		FontData->index = Katakana;
		FontData->OffSet= KATAKANA_OFFSET;
		//「タ」0x382BFの次で数値が飛ぶのでその為の分岐
		Moji = (MultiByte <= 0xE382BF)? (uint16_t)MultiByte - UTF_KATAKANA_OFFSET:(uint16_t)MultiByte - (UTF_KATAKANA_OFFSET + 0xC0);
	}
	return Moji;
}
inline uint16_t OLED::MojiKanji(uint32_t MultiByte)
{
	uint16_t Moji;

	Moji = ConvUTF(MultiByte);

	FontData->index = Kanji;
	FontData->OffSet = KANJI_OFFSET + ((Moji >> 8) - EUC_AREA_16_START) * EUC_AREA_16_SPACE;
	//EUC用の処理。UTF→EUCの場合MojiがEUCコードなのでビットシフトで上位バイトを抽出して区を特定する。
	//FontData->OffSet = KANJI_OFFSET + (str[0] - EUC_AREA_16_START) * EUC_AREA_16_SPACE;

	if(Moji == 0)
	{
		Moji = MojiError(MultiByte);
	}

	return Moji;
}
inline void OLED::MojiAscii(void)
{
	FontData->index = Ascii;
	FontData->OffSet = ASCII_OFFSET;
}
inline uint8_t OLED::MojiError(uint32_t MultiByte)
{
	MojiAscii();
	//伸ばし棒(長音)はAscii配列の末尾
	return (uint8_t)(MultiByte != UTF_TYOUON)? EUC_ASCII_MAX:EUC_ASCII_MAX+1;
}

void OLED::WriteChar(uint16_t Asci)
{
	uint8_t Page = DIV8(Ypoint);
	uint16_t moji = Asci - FontData->OffSet;
	JisFont Text = GetFontData(FontData->index);

	LL_FLASH_EnablePrefetch();

	if(Page == 1 || Page == 4)										//2行目と5行目でコピーすると上の行の下側4ドットを上書きしてしまう
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
	LL_FLASH_DisablePrefetch();
}
void OLED::WriteString(char *str,uint8_t size)
{
	uint8_t colBase = Xpoint;			//描画前の座標を記憶
	uint8_t PageBase = DIV8(Ypoint);
	uint8_t Byte = 0;
	uint16_t moji;

	while(Byte < size)
	{
		Byte += GetCharacter((uint8_t*)&str[Byte], &moji);
		WriteChar(moji);
		Xpoint += FontData->Width;
	}
	SendUpdatePoint(colBase, Xpoint-1, PageBase, PageBase+1);		//nカラムまで書く場合(n-1)カラムを指定。mページに書き込む際は必ず(m+1)ページまで使う

	UpdatePointDisplay(colBase, Xpoint-colBase, PageBase);

	Xpoint = 0;
}
void OLED::SendUpdatePoint(uint8_t x1,uint8_t x2,uint8_t y1,uint8_t y2)
{
	uint8_t cusor[] = {SET_COLUMN,x1,x2,SET_PAGE,y1,y2};

	SelectCommand();
	OledTransmit(cusor, sizeof(cusor));
}

/***
	書き込むエリアだけを更新する関数
	col&pageBase:XおよびY座標の描画開始点。
	colはバッファの開始点。pageは何番目の配列かを指定。

	colSize:有効データ数。Base+(Width*文字数)で描画完了点。
	pageEnd:フォントを変えない限りは必ずpageBase+1。
***/
void OLED::UpdatePointDisplay(uint8_t colBase,uint8_t colSize,uint8_t pageBase)
{
	SelectData();
	OledTransmit(&OLED_Buffer[pageBase][colBase], colSize);
	OledTransmit(&OLED_Buffer[pageBase+1][colBase], colSize);
}
/*** 画面全体を更新する関数 ***/
void OLED::UpdateFillDisplay(void)
{
	SendUpdatePoint(0, OLED_MAX_COLUMN, 0, OLED_MAX_PAGE);
	SelectData();
	OledTransmit((uint8_t*)OLED_Buffer, sizeof(OLED_Buffer));
}

/******* ex.漢字のUTFコードをEUC(JIS)に変換 ******/
uint16_t OLED::ConvUTF(uint32_t Kanji)
{
    uint16_t Seach = 0;

    while(Seach < UTF_MAX_SIZE)
    {
        if(Kanji == Table[Seach].utf)
        {
            break;
        }
        else
        {
            Seach++;
        }
    }

    //第一水準外の漢字は0を返す
    return (Seach != UTF_MAX_SIZE)? Table[Seach].euc:0;
}

//EUC用
#if 0
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
#endif
//2025-05-06以前
#if 0
inline bool OLED::IsByteSize(unsigned char c)
{
    return (c >= UTF_AREA_4_5);
}
uint8_t OLED::GetCharacter(unsigned char *str,uint16_t *moji)
{
	uint8_t MojiSize = 0;
	uint32_t MultiByte = 0;

	if(IsByteSize(str[0]))
	{
		MojiSize = 3;
		MultiByte = (uint32_t)(str[0] << 16 | str[1] << 8 | str[2]);

		if(MultiByte <= UTF_KANA_END)
		{
			FontData->index = Kana;
			FontData->OffSet = KANA_OFFSET;
			//「み」0x381BFの次で数値が飛ぶのでその為の分岐
			*moji = (MultiByte <= 0xE381BF)? (uint16_t)MultiByte - UTF_KANA_OFFSET:(uint16_t)MultiByte - (UTF_KANA_OFFSET + 0xC0);
		}
		else if(MultiByte <= UTF_KATAKANA_END)
		{
			FontData->index = Katakana;
			FontData->OffSet= KATAKANA_OFFSET;
			//「タ」0x382BFの次で数値が飛ぶのでその為の分岐
			*moji = (MultiByte <= 0xE382BF)? (uint16_t)MultiByte - UTF_KATAKANA_OFFSET:(uint16_t)MultiByte - (UTF_KATAKANA_OFFSET + 0xC0);
		}
		else if(MultiByte == 0xE383BC)		//伸ばし棒'ー'をAsciコード-として認識。-のフォントデータを伸ばし棒に変更
		{
			FontData->index = Ascii;
			FontData->OffSet = ASCII_OFFSET;
			*moji = '-';
		}
		else
		{
			//漢字
			*moji = ConvUTF(MultiByte);
			if(*moji != 0)					//0はエラー
			{
				FontData->index = Kanji;
				//EUC用の処理。UTF→EUCの場合*mojiがEUCコードなのでビットシフトが必要
				//FontData->OffSet = KANJI_OFFSET + (str[0] - EUC_AREA_16_START) * EUC_AREA_16_SPACE;
				FontData->OffSet = KANJI_OFFSET + ((*moji >> 8) - EUC_AREA_16_START) * EUC_AREA_16_SPACE;
			}
			else
			{
				MojiSize = 3;
				*moji = EUC_ASCII_MAX;
				FontData->index = Ascii;
				FontData->OffSet = ASCII_OFFSET;
			}
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
			MojiSize = 3;				//なぜ2なのだ
			*moji = EUC_ASCII_MAX;
		}
		FontData->index = Ascii;
		FontData->OffSet = ASCII_OFFSET;
	}
	return MojiSize;
}
#endif
