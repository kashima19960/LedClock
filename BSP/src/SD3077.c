#include "SD3077.h"


/*
设BCD码的高4位（十位数）为 T，低4位（个位数）为 O
val为：val = T * 16 + O
十进制值是：bin = T * 10 + O。
两者相差6*T，因此函数的算法是： val - 6 * T
*/
#define bcd2bin(val) ((val) - 6 * ((val) >> 4))
//同理，加上6*T即可
#define bin2bcd(val) ((val) + 6 * ((val) / 10))


static void UnlockWriteProtect()
{
}

void TimeNow(DateTime *dateTime)
{
}

void SetTime(DateTime *dateTime)
{
}

void SetInterruptOuput(SD3077IntFreq freq)
{
}

void EnableSencodInterruptOuput()
{
}

void WriteBackData(uint8_t index, uint8_t *data, uint8_t size)
{
}

void ReadBackData(uint8_t index, uint8_t *data, uint8_t size)
{
}
