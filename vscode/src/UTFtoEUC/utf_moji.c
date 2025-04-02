/****
 * (vscode)そのまま実行しても通らないです。
 * 一旦実行してからターミナル上で過去のコマンドを呼び出し(↑を押す)
 * コマンド上のutf_moji.cの後にスペースを1つ空けて「utf_code.c」を追加すれば通ります。
 ****/
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#include "kanaTwoByte.h" 
#include "KatakanaTwoByte.h"
#include "AsciiTwoByte.h"
#include "kanjiTwoByte.h"

#include "utf_code.h"

//pArrayはポインタ。uint16_t*12ずつアクセスする
//()を忘れると「uint16_t型のポインタを12個持つ配列」になってしまう
typedef uint16_t (*pArray)[12];
pArray WordArray;
uint32_t FontOffset;
const UTFtoEUC *Table;

#define ASCII_OFFSET 32U
#define KANA_OFFSET 0xa4a1   
#define KATAKANA_OFFSET 0xA5A1
#define KANJI_OFFSET 0xb0a1

#define ASCII_MAX 0x7F
#define EUC_AREA_4 0xA4
#define EUC_AREA_5 0xA5
#define EUC_AREA_ERROR 0xD0

#define EUC_AREA_16_START 0xB0
#define EUC_AREA_16_SPACE 0xA2

typedef enum{
    UTF_AREA_4_5 = 0xE3,
    UTF_KANA_OFFSET = 0xE2DCE0,
    UTF_KANA_END = 0xE38293,

    UTF_KATAKANA_OFFSET = 0xE2DD00,
    UTF_KATAKANA_END = 0xE383B6,

    UTF_MAX_SIZE = 2965,
}UTF_List;
/****
 * ・UTF_AREA_4_5 
 * JIS4~5区のUTF-8先頭バイト
 * 
 * ・UTF_KANA_OFFSET
 * JIS04-01のUTF-8コードが0xE38181なので
 * EUC = 0xE38181 - x
 * x = 0xE38181 - EUC
****/

static inline bool OLED_IsByteSize(unsigned char c)
{
    return (c >= UTF_AREA_4_5);
}

void Draw(uint16_t moji,uint16_t offset);
uint8_t OLED_GetCharacter(unsigned char *str, uint16_t *moji);
int16_t ConvUTF(uint32_t Kanji);

int main(void)
{
    unsigned char str[] = "寿限無ごこうのスリKIre";
    unsigned char *Pstr = str;
    uint32_t length = strlen(str);
    uint8_t Bytes;
    uint16_t moji;
    uint8_t i = 0;

    Table = GetKanjiStruct();
    
    for (uint16_t i = 0; i < strlen(str); i++)
    {
        printf("str[%d]:0x%x\n",i,(uint8_t)str[i]);
    }

    while (i < length)
    {
        Bytes = OLED_GetCharacter(Pstr+i,&moji);
        if (moji != 0xFFFF)
        {
            printf("moji:0x%x,Bytes:0x%x,offset = 0x%x\n",moji,Bytes,FontOffset);
            Draw(moji,FontOffset);
            i += Bytes;
        }
        else
        {
            break;
        }
    }

    return 0;
}

void Draw(uint16_t moji,uint16_t offset)
{  
    uint16_t Moji = moji - offset;

    printf("moji = 0x%x\n",Moji);

    for (uint8_t i = 0; i < 12; i++)
    {
        for (uint8_t j = 0; j < 12; j++)
        {
            if (WordArray[Moji][i] << j & 0x8000)
            {
                printf("*");
            }
            else
            {
                printf(" ");
            }
        }
        printf("\n");
    }
}

uint8_t OLED_GetCharacter(unsigned char *str, uint16_t *moji) 
{
    //文字サイズ数。この数ずつ文字列のポインタを進める
    uint8_t Moji_Size = 0;
    //UTF-8コード格納変数
    uint32_t MultiByte = 0;

    if (OLED_IsByteSize(str[0]))
    {
        Moji_Size = 3;
        //strはMoji_Sizeずつ進む。なのでn回目の呼び出し時には&str[0]は元の文字列の先頭アドレスから(n * Moji_Size)分進んだ点を指している。
        //厳密には上記の例は文字列すべてがマルチバイト文字であるときのみ。Ascii文字が混じる場合はMoji_Sizeが変わるため。
        MultiByte = (uint32_t)(str[0] << 16 | str[1] << 8 | str[2]);

        //漢字以外は単純な演算でEUCに変換する
        //ひらがな、カタカナは途中でコードが飛ぶので三項演算子している
        if (MultiByte <= UTF_KANA_END)
        {
            WordArray = kana12X12;
            FontOffset = KANA_OFFSET;
            *moji = (MultiByte <= 0xE381BF)? (uint16_t)MultiByte - UTF_KANA_OFFSET:(uint16_t)MultiByte - (UTF_KANA_OFFSET + 0xC0);
        }
        else if(MultiByte <= UTF_KATAKANA_END)
        {
            WordArray = katakana12X12;
            FontOffset = KATAKANA_OFFSET;
            *moji = (MultiByte <= 0xE382BF)? (uint16_t)MultiByte - UTF_KATAKANA_OFFSET:(uint16_t)MultiByte - (UTF_KATAKANA_OFFSET + 0xC0);
        }
        else
        {
            *moji = ConvUTF(MultiByte);         //漢字はここでEUCに変換する
            if(*moji != 0xFFFF)                 //-1を符号無し型に入れるとFFFFになる。あんまよくないけど
            {
                WordArray = kanji12X12;
                FontOffset = KANJI_OFFSET + ((*moji >> 8) - EUC_AREA_16_START) * EUC_AREA_16_SPACE;
            }
            else
            {
                printf("0x%X<-その文字はありません！\n",MultiByte);
            }
        }
    }
    else if (str[0] < ASCII_MAX)
    {
        Moji_Size = 1;
        *moji = (uint16_t)str[0];

        WordArray = asci12X12;
        FontOffset = ASCII_OFFSET;
    }
    else;
    
    return Moji_Size;
}
int16_t ConvUTF(uint32_t Kanji)
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

    return (Seach != UTF_MAX_SIZE)? Table[Seach].euc:-1;
}