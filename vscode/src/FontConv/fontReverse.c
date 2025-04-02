/*********
 * 2バイトのフォントデータをSSD130x用の文字データに変換するプログラム
**********/

#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "kanaTwoByte.h"
#include "KatakanaTwoByte.h"
#include "kanjiTwoByte.h"
#include "AsciiTwoByte.h"

typedef uint16_t (*JisFont)[12];
JisFont WordArray;
uint16_t FontOffset;
uint8_t PixArray[2][12];

void DrawPixel(uint8_t x,uint8_t y)
{
    PixArray[y/8][x] |= 1 << y % 8;
}

int main(void)
{
    uint16_t moji;
    
    uint16_t ArraySize = sizeof(kana12X12)/24;
    WordArray = kana12X12;

    for (moji = 0; moji < ArraySize; moji++)
    {
        for (uint16_t i = 0; i < 12; i++)
        {
            for (uint16_t j = 0; j < 12; j++)
            {
                if (WordArray[moji][i] << j & 0x8000)
                {
                    DrawPixel(j,i);
                }
            }
        }
        printf("{");
        for (uint16_t i = 0; i < 12; i++)
        {
            printf("0x%02X,",PixArray[0][i]);
        }
        for (uint16_t i = 0; i < 12; i++)
        {
            printf("0x%02X,",PixArray[1][i]);
        }
        printf("},");
        memset(PixArray,0,sizeof(PixArray));
        printf("\n");
    }
    printf("moji = %d\n",moji);
    
    return 0;
}


