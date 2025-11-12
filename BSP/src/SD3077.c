#include "sd3077.h"
#include "app_config.h"
I2C_HandleTypeDef g_iic_handle;
/*
sd3077硬件上接了，PA9--I2C1_SCL,PA10--I2C1_SDA,PB1--SEC-INT
*/
void sd3077_iic_init(void)
{
    g_iic_handle.Instance = I2C1;
    g_iic_handle.Init.Timing = 0x0000020B;
    g_iic_handle.Init.OwnAddress1 = 0;
    g_iic_handle.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
    g_iic_handle.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
    g_iic_handle.Init.OwnAddress2 = 0;
    g_iic_handle.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
    g_iic_handle.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
    g_iic_handle.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
    if (HAL_I2C_Init(&g_iic_handle) != HAL_OK)
    {
        Error_Handler();
    }

    if (HAL_I2CEx_ConfigAnalogFilter(&g_iic_handle, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
    {
        Error_Handler();
    }

    if (HAL_I2CEx_ConfigDigitalFilter(&g_iic_handle, 0) != HAL_OK)
    {
        Error_Handler();
    }
}
void sec_int_gpio_init(void)
{
    GPIO_InitTypeDef gpio_init_struct = {0};
    //SD3077_SEC_INT_GPIO_CLK_ENABLE();
    gpio_init_struct.Pin = SEC_INT_PIN;
    gpio_init_struct.Mode = GPIO_MODE_IT_FALLING;
    gpio_init_struct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(SEC_INT_GPIO_PORT, &gpio_init_struct);
    HAL_NVIC_SetPriority(EXTI0_1_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(EXTI0_1_IRQn);
}

void HAL_I2C_MspInit(I2C_HandleTypeDef *i2cHandle)
{
    GPIO_InitTypeDef gpio_init_struct = {0};
    if (i2cHandle->Instance == I2C1)
    {
        SD3077_IIC_GPIO_CLK_ENABLE();
        SD3077_IIC_CLK_ENABLE();
        gpio_init_struct.Pin = GPIO_PIN_9 | GPIO_PIN_10;
        gpio_init_struct.Mode = GPIO_MODE_AF_OD;
        gpio_init_struct.Pull = GPIO_PULLUP;
        gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;
        gpio_init_struct.Alternate = GPIO_AF4_I2C1;
        HAL_GPIO_Init(GPIOA, &gpio_init_struct);
    }
}

/**
 * @brief       解除SD3077写保护
 * @param       无
 * @retval      无
 * @note        必须按照顺序: WRTC1(CTR2) -> WRTC2(CTR1) -> WRTC3(CTR1)
 */
static void unlock_write_protect(void)
{
    uint8_t ctr1_data;
    uint8_t ctr2_data;

    /* 读取当前控制寄存器值 */
    HAL_I2C_Mem_Read(&SD3077_IIC_HANDLE, SD3077_IIC_ADDR_READ, SD3077_REG_CTR1, 1, &ctr1_data, 1, HAL_MAX_DELAY);
    HAL_I2C_Mem_Read(&SD3077_IIC_HANDLE, SD3077_IIC_ADDR_READ, SD3077_REG_CTR2, 1, &ctr2_data, 1, HAL_MAX_DELAY);

    /* 步骤1: 先置WRTC1=1 (CTR2的bit7) */
    ctr2_data |= 0x80; /* 0x80 = 10000000 */
    HAL_I2C_Mem_Write(&SD3077_IIC_HANDLE, SD3077_IIC_ADDR_WRITE, SD3077_REG_CTR2, 1, &ctr2_data, 1, HAL_MAX_DELAY);

    /* 步骤2: 后置WRTC2=1 (CTR1的bit2) */
    ctr1_data |= 0x04; /* 0x04 = 00000100 */
    HAL_I2C_Mem_Write(&SD3077_IIC_HANDLE, SD3077_IIC_ADDR_WRITE, SD3077_REG_CTR1, 1, &ctr1_data, 1, HAL_MAX_DELAY);

    /* 步骤3: 最后置WRTC3=1 (CTR1的bit7) */
    ctr1_data |= 0x80; /* 0x80 = 10000000 */
    HAL_I2C_Mem_Write(&SD3077_IIC_HANDLE, SD3077_IIC_ADDR_WRITE, SD3077_REG_CTR1, 1, &ctr1_data, 1, HAL_MAX_DELAY);
}

/**
 * @brief       启用SD3077写保护
 * @param       无
 * @retval      无
 * @note        必须按照顺序: WRTC2/WRTC3(CTR1) -> WRTC1(CTR2)
 */
static void lock_write_protect(void)
{
    uint8_t ctr1_data, ctr2_data;

    /* 读取当前控制寄存器值 */
    HAL_I2C_Mem_Read(&SD3077_IIC_HANDLE, SD3077_IIC_ADDR_READ, SD3077_REG_CTR1, 1, &ctr1_data, 1, HAL_MAX_DELAY);
    HAL_I2C_Mem_Read(&SD3077_IIC_HANDLE, SD3077_IIC_ADDR_READ, SD3077_REG_CTR2, 1, &ctr2_data, 1, HAL_MAX_DELAY);

    /* 步骤1: 先清除WRTC2和WRTC3 (CTR1的bit2和bit7) */
    ctr1_data &= ~0x84; /* 0x84 = 10000100, 清除bit7和bit2 */
    HAL_I2C_Mem_Write(&SD3077_IIC_HANDLE, SD3077_IIC_ADDR_WRITE, SD3077_REG_CTR1, 1, &ctr1_data, 1, HAL_MAX_DELAY);

    /* 步骤2: 后清除WRTC1 (CTR2的bit7) */
    ctr2_data &= ~0x80; /* 清除bit7 */
    HAL_I2C_Mem_Write(&SD3077_IIC_HANDLE, SD3077_IIC_ADDR_WRITE, SD3077_REG_CTR2, 1, &ctr2_data, 1, HAL_MAX_DELAY);
}
/*
实时时钟数据寄存器是7字节(0x00~0x06)的存储器，它以BCD 码方式存贮包括年、月、日、星期、时、分、
秒的数据。
*/
void time_now(DateTime *dateTime)
{
    uint8_t data[7];
    HAL_I2C_Mem_Read(&SD3077_IIC_HANDLE, SD3077_IIC_ADDR_READ, 0x00, 1, data, 7, HAL_MAX_DELAY);
    dateTime->seconds = bcd2bin(data[0]);
    dateTime->minutes = bcd2bin(data[1]);

    if (data[2] >> 7) // D7=1 24小时制 0 12小时制
    {
        dateTime->ampm = HOUR24;
        dateTime->hours = bcd2bin(data[2] & 0x7F); // 0x7F = 01111111
    }
    else // 12小时制，根据D5判断AM/PM
    {
        dateTime->ampm = (data[2] & 0x20) >> 5;
        dateTime->hours = bcd2bin(data[2] & 0x1F);
    }

    dateTime->dayOfWeek = bcd2bin(data[3]);
    dateTime->dayOfMonth = bcd2bin(data[4]);
    dateTime->month = bcd2bin(data[5]);
    dateTime->year = bcd2bin(data[6]);
}

void set_time(DateTime *dateTime)
{ // 解除写保护
    unlock_write_protect();

    uint8_t data[7];
    data[0] = bin2bcd(dateTime->seconds);
    data[1] = bin2bcd(dateTime->minutes);
    data[2] = bin2bcd(dateTime->hours);
    data[3] = bin2bcd(dateTime->dayOfWeek);
    data[4] = bin2bcd(dateTime->dayOfMonth);
    data[5] = bin2bcd(dateTime->month);
    data[6] = bin2bcd(dateTime->year);

    if ((dateTime->ampm) == HOUR24)
    {
        data[2] |= 0x80;
    }
    else
    {
        data[2] |= (dateTime->ampm) << 5;
    }

    HAL_I2C_Mem_Write(&SD3077_IIC_HANDLE, SD3077_IIC_ADDR_WRITE, 0x00, 1, data, 7, HAL_MAX_DELAY);
    lock_write_protect();
}

void set_interrupt_output(SD3077IntFreq freq)
{
    // 解除写保护
    unlock_write_protect();

    // 读出控制寄存器2和3
    uint8_t data[2];
    HAL_I2C_Mem_Read(&SD3077_IIC_HANDLE, SD3077_IIC_ADDR_READ, SD3077_REG_CTR2, 1, data, 2, HAL_MAX_DELAY);

    // 允许频率中断
    data[0] |= 0x01;
    // 选择频率中断输出
    data[0] |= 0x20;
    data[0] &= 0xEF;

    // 设置频率为1Hz
    data[1] &= 0xF0;
    data[1] |= freq;

    // 设置频率为1秒
    //	data[1] |= 0x0F;

    // 控制寄存器回写
    HAL_I2C_Mem_Write(&SD3077_IIC_HANDLE, SD3077_IIC_ADDR_WRITE, SD3077_REG_CTR2, 1, data, 2, HAL_MAX_DELAY);
}

/*
当 INTFE=1 时频率中断被允许;INTFE=O 时频率中断被禁止
当设置 INTS1=1、INTSO=O时,即允许频率中断从INT脚输出．频率中断没有标志位.
INT脚输出频率中断由控制寄存器3 中的FS3、FS2、FS1、FS0位来选择确定,1111表示1秒中断
*/
void enable_second_interrupt_output()
{ // 解除写保护
    unlock_write_protect();

    // 读出控制寄存器2和3
    uint8_t data[2];
    HAL_I2C_Mem_Read(&SD3077_IIC_HANDLE, SD3077_IIC_ADDR_READ, SD3077_REG_CTR2, 1, data, 2, HAL_MAX_DELAY);

    // 允许频率中断
    data[0] |= 0x01; // 0x01=00000001,置INTFE=1
    // 选择频率中断输出,设置INTS1=1,INTS0=0
    data[0] |= 0x20; // 0x20=00100000
    data[0] &= 0xEF; // 0xEF=11101111

    // 设置频率为1Hz
    //	data[1] &= 0xF0;
    //	data[1] |= 0x0A;

    // 设置频率为1秒
    data[1] |= 0x0F; // 0x0F=00001111

    // 控制寄存器回写
    HAL_I2C_Mem_Write(&SD3077_IIC_HANDLE, SD3077_IIC_ADDR_WRITE, SD3077_REG_CTR2, 1, data, 2, HAL_MAX_DELAY);
    lock_write_protect();
}

/*
读写SD3077用户RAM(70bytes),范围:0x2C~0x71
*/
void write_backup_data(uint8_t index, uint8_t *data, uint8_t size)
{
    if (index > 69)
    {
        return;
    }
    // 解除写保护
    unlock_write_protect();

    // 写入备份寄存器
    HAL_I2C_Mem_Write(&SD3077_IIC_HANDLE, SD3077_IIC_ADDR_WRITE, SD3077_REG_SRAM_START + index, 1, data, size,
                      HAL_MAX_DELAY);
    lock_write_protect();
}

void read_backup_data(uint8_t index, uint8_t *data, uint8_t size)
{
    if (index > 69)
    {
        return;
    }

    // 从备份寄存器读出
    HAL_I2C_Mem_Read(&SD3077_IIC_HANDLE, SD3077_IIC_ADDR_READ, SD3077_REG_SRAM_START + index, 1, data, size,
                     HAL_MAX_DELAY);
}
