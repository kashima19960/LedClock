#include "TM1637.h"
static void TM1637_WriteByte(uint8_t b);
const uint8_t numbersMap[] = {
    0x3f, // 0d
    0x06, // 1d
    0x5b, // 2d
    0x4f, // 3d
    0x66, // 4d
    0x6d, // 5d
    0x7d, // 6d
    0x07, // 7d
    0x7f, // 8d
    0x6f, // 9d
};

void delay(uint32_t i)
{
    for (; i > 0; i--)
    {
        for (int j = 0; j < 10; ++j)
        {
            // asm("nop");
        }
    }
}

void TM1637_GPIO_Init(void)
{
    TM1637_CLK_DIO_GPIO_CLK_ENABLE();

    GPIO_InitTypeDef gpio_init_struct;
    gpio_init_struct.Pin = TM1637_CLK_PIN | TM1637_DIO_PIN;
    gpio_init_struct.Mode = GPIO_MODE_OUTPUT_OD;
    gpio_init_struct.Pull = GPIO_NOPULL; // 硬件上外接了两个10K电阻到5V，不需要再上拉
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOF, &gpio_init_struct);
}

/*
！！！ TM1637使用的不是标准的I2C协议，无法使用stm32自带的I2C外设，因此这里使用GPIO模拟，
下面是根据手册原话总结出来的通信规则
1. 微处理器的数据通过两线总线接口和 TM1637 通信
2. 输入数据时当 CLK 是高电平时，DIO 上的信号必须保持不变；只有 CLK 上的时钟信号为低电平时，DIO 上的信号才能改变。
3. 数据输入的开始条件是 CLK为高电平时，DIO 由高变低
4. 结束条件是 CLK 为高时，DIO 由低电平变为高电平。
5. TM1637 的数据传输带有应答信号 ACK，当传输数据正确时，会在第八个时钟的下降沿，芯片内部会产生一个应答信号 ACK 将 DIO
管脚拉低，在第九个时钟结束之后释放 DIO 口线。

下面是根据手册里面的例程移植过来的程序
*/
static void TM1637_Start(void)
{
    TM1637_CLK(1);
    TM1637_DIO(1);
    delay(2);
    TM1637_DIO(0);
}

static void TM1637_Stop(void)
{
    TM1637_CLK(0);
    delay(2);
    TM1637_DIO(0);
    delay(2);
    TM1637_CLK(1);
    delay(2);
    TM1637_DIO(1);
}

static void TM1637_Ack(void)
{
    TM1637_CLK(0);
    delay(5);
    while (TM1637_READ_DIO)
        ;
    TM1637_CLK(1);
    delay(2);
    TM1637_CLK(0);
}

static uint8_t translateFromChar(char c)
{
	switch (c)
	{
	// Numbers
	case '0': return 0x3F;
	case '1': return 0x06;
	case '2': return 0x5B;
	case '3': return 0x4F;
	case '4': return 0x66;
	case '5': return 0x6D;
	case '6': return 0x7D;
	case '7': return 0x07;
	case '8': return 0x7F;
	case '9': return 0x6F;

	// Uppercase letters
	case 'A': return 0x77;
	case 'B': return 0x7C;
	case 'C': return 0x39;
	case 'D': return 0x5E;
	case 'E': return 0x79;
	case 'F': return 0x71;
	case 'H': return 0x76;
	case 'I': return 0x30;
	case 'J': return 0x1E;
	case 'L': return 0x38;
	case 'N': return 0x54;
	case 'O': return 0x5C;
	case 'P': return 0x73;
	case 'R': return 0x50;
	case 'S': return 0x6D;
	case 'T': return 0x44;
	case 'U': return 0x3E;

	// Lowercase letters
	case 'a': return 0x77;
	case 'b': return 0x7C;
	case 'c': return 0x58;
	case 'd': return 0x5E;
	case 'e': return 0x79;
	case 'f': return 0x71;
	case 'h': return 0x74;
	case 'i': return 0x10;
	case 'j': return 0x1E;
	case 'l': return 0x38;
	case 'n': return 0x54;
	case 'o': return 0x5C;
	case 'p': return 0x73;
	case 'r': return 0x50;
	case 's': return 0x6D;
	case 't': return 0x44;
	case 'u': return 0x1C;

	// Special characters
	case '-': return 0x40;
	case '_': return 0x08;
	default: return 0x00;
	}
}

uint32_t power(uint32_t x, uint8_t n)
{
    uint32_t rst = 1;
    while (n--)
    {
        rst *= x;
    }
    return rst;
}

void TM1637_Init(void)
{
    TM1637SetBrightness(8);
}

void TM1637SetBrightness(uint8_t brightness)
{
    // Brightness command:
    // 1000 0XXX = display off
    // 1000 1BBB = display on, brightness 0-7
    // X = don't care
    // B = brightness
    TM1637_Start();
    TM1637_WriteByte(0x87 + brightness); // 0x87=10000111
    TM1637_Ack();
    TM1637_Stop();
}

// TM1637ShowNumberRight(3, number, blinkControl ? 2 : 0xFF, 1);
void TM1637ShowNumberRight(uint8_t index, uint32_t num, uint8_t pointLocation, uint8_t isPaddingZero)
{

    uint8_t n = 6;
    do
    {
        uint32_t dig = num / power(10, n - 1);
        if (dig != 0)
        {
            break;
        }
    } while (--n);

    uint8_t digits[6];
    for (uint8_t i = 0; i < 6; i++)
    {
        if (i < n || isPaddingZero)
        {
            digits[i] = numbersMap[num % 10];
        }
        else
        {
            digits[i] = 0;
        }

        if (pointLocation == i)
        {
            digits[i] |= 0x80;
        }
        num /= 10;
    }

    TM1637_Start();
    TM1637_WriteByte(0x40);
    TM1637_Ack();
    TM1637_Stop();

    TM1637_Start();
    TM1637_WriteByte(0xc0);
    TM1637_Ack();

    for (uint8_t i = 0; i < 6; ++i)
    {
        if (i <= index)
        {
            TM1637_WriteByte(digits[index - i]);
        }
        else
        {
            TM1637_WriteByte(0);
        }

        TM1637_Ack();
    }

    TM1637_Stop();
}

void TM1637SetChar(uint8_t index, char c, uint8_t point)
{
    TM1637SetRawData(index, translateFromChar(c) | (point ? 0x80 : 0x00));
}

void TM1637SetRawData(uint8_t index, uint8_t data)
{
    TM1637_Start();
    TM1637_WriteByte(0x44); // 固定地址模式
    TM1637_Ack();
    TM1637_Stop();

    TM1637_Start();
    TM1637_WriteByte(0xC0 + index); // 固定地址模式
    TM1637_Ack();
    TM1637_WriteByte(data);
    TM1637_Ack();
    TM1637_Stop();
}

void TM1637_WriteByte(uint8_t DataByte)
{
    uint8_t i;
    for (i = 0; i < 8; ++i)
    {
        TM1637_CLK(0);
        if (DataByte & 0x01)
        {
            TM1637_DIO(1);
        }
        else
        {
            TM1637_DIO(0);
        }
        delay(3);
        DataByte >>= 1;
        TM1637_CLK(1);
        delay(3);
    }
}
