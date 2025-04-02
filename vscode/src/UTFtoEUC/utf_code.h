#include <stdint.h>

typedef struct{
    uint16_t euc;
    uint32_t utf;
}UTFtoEUC;

const UTFtoEUC *GetKanjiStruct(void);
