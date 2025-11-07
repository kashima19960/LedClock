#include "SD3077.h"

static uint8_t bcd2bin(uint8_t val)
{
    return val - 6 * (val >> 4);
}
static uint8_t bin2bcd(uint8_t val)
{
    return val + 6 * (val / 10);
}

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
