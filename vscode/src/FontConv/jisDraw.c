/***************
 * 2バイトフォントデータを用いて*で描画するプログラム
 * ウェブ上で正しく読めるようにUTF-8にしています。
 * 実際に使う際にはEUC-JPに切り替えてから使ってください。
 **************/

 #include <stdio.h>
 #include <string.h>
 #include <stdint.h>
 #include <stdbool.h>
 
 typedef uint16_t (*pArray)[12];
 pArray WordArray;
 uint16_t FontOffset;
 
 #include "kanaTwoByte.h" 
 #include "KatakanaTwoByte.h"
 #include "AsciiTwoByte.h"
 #include "kanjiTwoByte.h"
 
 #define ASCII_OFFSET 32U
 #define KANA_OFFSET 0xA4A1   
 #define KATAKANA_OFFSET 0xA5A1
 #define KANJI_OFFSET 0xB0A1
 
 #define EUC_ASCII_MAX 0x7F
 #define EUC_AREA_4 0xA4
 #define EUC_AREA_5 0xA5
 #define EUC_AREA_ERROR 0xD0
 
 #define EUC_AREA_16_START 0xB0
 #define EUC_AREA_16_SPACE 0xA2
 
 
 static inline bool OLED_IsByteSize(unsigned char c)
 {
     return (c >= EUC_AREA_4 && c < EUC_AREA_ERROR);
 }
 
 void Draw(uint16_t moji,uint16_t offset);
 /****
  * 第一水準漢字は0xb0~0xcfの31区。b0を引くことで0～31の区を求める
  * 0xa2は各区毎の空白。区番号*a2で今の区に該当する正しい値を求める
  */
 uint8_t OLED_GetCharacter(unsigned char *str, uint16_t *moji);
 
 
 int main(void)
 {
     unsigned char str[] = "あAア亜";
     unsigned char *Pstr = str;
     uint32_t length = strlen(str);
     uint8_t Bytes;
     uint16_t moji;
     
 
     for (uint16_t i = 0; i < strlen(str); i++)
     {
         printf("str[%d]:0x%x\n",i,(uint8_t)str[i]);
     }
 
     uint8_t i = 0;
 
     while (i < length)
     {
         Bytes = OLED_GetCharacter(Pstr+i,&moji);
         printf("moji:0x%x,Bytes:0x%x\n",moji,Bytes);
         Draw(moji,FontOffset);
         i += Bytes;
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
     uint8_t Moji_Size = 0;
 
     if (OLED_IsByteSize(str[0]))
     {
         Moji_Size = 2;
         *moji = (uint16_t)(str[0] << 8 | str[1]);
 
         switch (str[0])
         {
         case EUC_AREA_4:
             WordArray = kana12X12;
             FontOffset = KANA_OFFSET;
             break;
         case EUC_AREA_5:
             WordArray = katakana12X12;
             FontOffset = KATAKANA_OFFSET;
             break;
         default:
             WordArray = kanji12X12;
             FontOffset = KANJI_OFFSET + (str[0] - EUC_AREA_16_START) * EUC_AREA_16_SPACE;
             break;
         }
     }
     else if (str[0] < EUC_ASCII_MAX)
     {
         Moji_Size = 1;
         *moji = (uint16_t)str[0];
 
         WordArray = asci12X12;
         FontOffset = ASCII_OFFSET;
     }
     else;
     
     return Moji_Size;
     
 }