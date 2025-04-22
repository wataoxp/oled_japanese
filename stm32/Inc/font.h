/*
 * font.h
 *
 *  Created on: Mar 4, 2025
 *      Author: wataoxp
 */

#ifndef INC_FONT_H_
#define INC_FONT_H_

#include <stdint.h>

#define OLED_DATA_ONEBYTEx24

#define EUC_ASCII_MAX 0x7F
#define EUC_AREA_4 0xA4
#define EUC_AREA_5 0xA5
#define EUC_AREA_ERROR 0xD0

#define ASCII_OFFSET 0x20
#define KANA_OFFSET 0xA4A1
#define KATAKANA_OFFSET 0xA5A1
#define KANJI_OFFSET 0xB0A1

#define EUC_AREA_16_START 0xB0
#define EUC_AREA_16_SPACE 0xA2


#ifdef OLED_DATA_ONEBYTEx24
typedef const uint8_t (*JisFont)[24];
#else
typedef const uint16_t (*JisFont)[12];
#endif

typedef struct{
	uint8_t Height;
	uint8_t Width;
	uint16_t OffSet;
	uint8_t index;
	uint8_t Space;
}Font_Typedef;

typedef enum{
	Ascii,
	Kana,
	Katakana,
	Kanji,
}JisFont_index;

Font_Typedef *GetHandle(void);
JisFont GetFontData(uint8_t index);

#endif /* INC_FONT_H_ */
