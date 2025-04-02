/***
 * 東雲フォント、12X12の1バイト配列を2バイト配列に書き換えるファイル
 * 12X12のフォントなので、2バイトに収めた方が描画処理がラク
 * ArrayPoint、pArrayのみ適時変更
 * 
 * 注:漢字フォントの変換を行うと3000行の出力になるので時間がかかる
 * 加えてターミナルの出力行数上限を上げないと表示しきれない。
 ****/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "kana.h"
#include "katakana.h"
#include "sinonome_asci.h"
#include "kanji.h"

const uint8_t FontSize = 12 * 2;
const uint16_t ArrayPoint = 83;           //変換元の要素数
uint8_t (*pArray)[24] = kana12X12;


int main(void)
{
    uint32_t ArraySize = ArrayPoint * FontSize;     //全体のバイト数
    uint32_t OneArray = FontSize;                   //一次元配列１つ分のサイズ

    printf("kana12 = %d\n",ArraySize);
    printf("oneArray = %d\n",OneArray);
    printf("ArrayPoint = %d\n",ArrayPoint);

    uint16_t *Binary;
    uint16_t size = sizeof(uint16_t) * OneArray/2;  //二次元配列の1要素分のみ確保
    Binary = (uint16_t*)malloc(size);               //24*1バイト配列を2バイト配列にする

    for (uint16_t i = 0; i < ArrayPoint; i++)
    {
        memset(Binary,0,size);                      //配列を初期化する
        printf("{");
        for (uint16_t j = 0; j < OneArray; j++)
        {
            if(j % 2 == 0)
            {
                Binary[j/2] |= pArray[i][j] << 8;
            }
            else
            {
                Binary[j/2] |= pArray[i][j];
            }
        }
        for (uint16_t k = 0; k < OneArray/2; k++)    //二次元配列の1要素分のみ表示。ターミナル上に繰り返し表示して二次元配列っぽく見せる
        {
            printf("0x%04x,",Binary[k]);
            if(Binary[k] != (pArray[i][k*2] << 8 | pArray[i][k*2+1]))       //変換元、先が一致しているかチェック
            {
                printf("Not! %d,%d\n",i,k);
            }      
        }
        printf("},\n");
        
    }
    
    free(Binary);

    return 0;
}