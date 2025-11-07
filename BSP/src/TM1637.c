#include "TM1637.h"

#define SCLK_Set() (TM1637_SCLK_PORT->BSRR = TM1637_SCLK_PIN)
#define SCLK_Reset()  (TM1637_SCLK_PORT->BRR = TM1637_SCLK_PIN)
#define SDIO_Set() (TM1637_SDIO_PORT->BSRR = TM1637_SDIO_PIN)
#define SDIO_Reset()  (TM1637_SDIO_PORT->BRR = TM1637_SDIO_PIN)

#ifndef GPIO_ResetPin
#define GPIO_ResetPin(GPIOx, PIN) (GPIOx -> BRR = PIN);
#endif
#ifndef GPIO_SetPin
#define GPIO_SetPin(GPIOx, PIN)   (GPIOx -> BSRR = PIN);
#endif

static void start(void);
static void stop(void);
static void waitACK(void);
static void WriteByte(uint8_t b);
static void delay(uint32_t i);
static uint8_t translateFromChar(char c);

const uint8_t numbersMap[] = {
    0x3f,
	0x06,
	0x5b,
	0x4f,
	0x66,
	0x6d,
	0x7d,
	0x07,
    0x7f,
	0x6f,
};

uint32_t power(uint32_t x, uint8_t n) {
}

void TM1637Init(void)
{
}

void TM1637SetBrightness(uint8_t brightness)
{
}

void TM1637ShowNumberLeft(uint8_t index, uint32_t num, uint8_t pointLocation)
{
}

void TM1637ShowNumberRight(uint8_t index, uint32_t num, uint8_t pointLocation, uint8_t isPaddingZero)
{
}

void TM1637SetChar(uint8_t index, char c, uint8_t point)
{
}

void TM1637SetRawData(uint8_t index, uint8_t data)
{
}
