#ifndef __SD3077_H
#define __SD3077_H

#include "stm32f0xx_hal.h"

/* IIC句柄 */
#define SD3077_IIC_HANDLE      g_iic_handle
extern I2C_HandleTypeDef SD3077_IIC_HANDLE;
extern DMA_HandleTypeDef g_dma_adc_handle;
#define SD3077_IIC_CLK_ENABLE() __HAL_RCC_I2C1_CLK_ENABLE()
#define SD3077_IIC_GPIO_CLK_ENABLE() __HAL_RCC_GPIOA_CLK_ENABLE()
#define SD3077_SEC_INT_GPIO_CLK_ENABLE  __HAL_RCC_GPIOB_CLK_ENABLE()
/*
设BCD码的高4位（十位数）为 T，低4位（个位数）为 O
val为：val = T * 16 + O
十进制值是：bin = T * 10 + O。
两者相差6*T，因此函数的算法是： val - 6 * T
*/
#define bcd2bin(val) ((val) - 6 * ((val) >> 4))
/* 同理，加上6*T即可 */
#define bin2bcd(val) ((val) + 6 * ((val) / 10))

#define SD3077_SEC_INT_PIN GPIO_PIN_1
#define SD3077_SEC_INT_GPIO_PORT GPIOB


/* 设备地址 */
#define SD3077_IIC_ADDR_7BIT   ((uint8_t)0x32) /* 7位设备地址: 0110010 */
#define SD3077_IIC_ADDR_READ   ((uint8_t)0x65) /* 8位读地址: 01100101 */
#define SD3077_IIC_ADDR_WRITE  ((uint8_t)0x64) /* 8位写地址: 01100100 */


/*
 * ============================================================================
 * 控制寄存器 (0FH - 1FH)
 * ============================================================================
 */

/* 控制寄存器1 (0FH)
 * D7:    WRTC3 (写保护位3)
 * D6:    OSF   (停振标志位, 1=曾发生停振)
 * D5:    INTAF (报警中断标志位, 1=中断发生)
 * D4:    INTDF (倒计时中断标志位, 1=中断发生)
 * D3:    BLF   (电池欠压标志位, 1=Vbat<2.2V)
 * D2:    WRTC2 (写保护位2)
 * D1:    PMF   (电源模式标志位, 0=VDD供电, 1=VBAT供电)
 * D0:    RTCF  (上电指示位, 1=首次上电)
 */
#define SD3077_REG_CTR1             (0x0F)

/* 控制寄存器2 (10H)
 * D7:    WRTC1 (写保护位1, 写保护顺序: 1->2->3)
 * D6:    IM    (中断模式, 0=单事件报警, 1=周期性报警)
 * D5:    INTS1 (INT引脚输出选择1)
 * D4:    INTS0 (INT引脚输出选择0, 00=电量报警, 01=报警, 10=频率, 11=倒计时)
 * D3:    FOBAT (VBAT模式下INT输出使能, 1=允许)
 * D2:    INTDE (倒计时中断允许, 1=允许)
 * D1:    INTAE (时间报警中断允许, 1=允许)
 * D0:    INTFE (频率中断允许, 1=允许)
 */
#define SD3077_REG_CTR2             (0x10)

/* 控制寄存器3 (11H)
 * D7:    ARST  (中断标志自动复位, 1=读0FH时自动清除INTAF/INTDF)
 * D6:    F32K  (32KHz方波输出控制, 1=禁止, 0=允许)
 * D5:    TDS1  (倒计时时钟源选择1)
 * D4:    TDS0  (倒计时时钟源选择0, 00=4096Hz, 01=1024Hz, 10=1秒, 11=1分钟)
 * D3:    FS3   (频率中断输出选择3)
 * D2:    FS2   (频率中断输出选择2)
 * D1:    FS1   (频率中断输出选择1)
 * D0:    FS0   (频率中断输出选择0)
 */
#define SD3077_REG_CTR3             (0x11)

#define SD3077_REG_UNUSED_12H       (0x12) /* CTTF (只读RAM) */
#define SD3077_REG_TIMER_CNT_L      (0x13) /* 24位倒计时定时器低字节 (TD7-TD0) */
#define SD3077_REG_TIMER_CNT_M      (0x14) /* 24位倒计时定时器中字节 (TD15-TD8) */
#define SD3077_REG_TIMER_CNT_H      (0x15) /* 24位倒计时定时器高字节 (TD23-TD16) */

/* 温度寄存器 (16H)
 * D7:    TM7 (符号位, 1=负温)
 * D6-D0: TM6-TM0 (温度值, 单位°C)
 */
#define SD3077_REG_TEMP             (0x16)

/* IIC控制寄存器 (17H)
 * D7:    BATIIC (VBAT模式下IIC通信允许, 1=允许, 0=禁止)
 * D6-D0: 0 (固定)
 */
#define SD3077_REG_IIC_CTRL         (0x17)

/* 充电寄存器 (18H)
 * D7:    ENCH (充电使能, 1=允许, 0=禁止)
 * D6-D2: 0 (固定)
 * D1:    Charge1 (充电内阻选择1, 00=10KΩ, 01=5KΩ, 10=2KΩ, 11=断开)
 * D0:    Charge0 (充电内阻选择0)
 */
#define SD3077_REG_CHARGE           (0x18)

/* 控制寄存器4 (19H)
 * D7:    INTS_E2 (扩展中断选择2)
 * D6:    INTS_E1 (扩展中断选择1, 当INTS1/0=00时选择INT脚功能)
 * D5:    INTS_E0 (扩展中断选择0, 011=电池低压报警, 100=电池高压报警)
 * D4:    0 (固定, 必须为0)
 * D3:    INTTHE (高温报警使能, 1=允许)
 * D2:    INTTLE (低温报警使能, 1=允许)
 * D1:    INTBHE (电池高压报警使能, 1=允许, >3.3V)
 * D0:    INTBLE (电池低压报警使能, 1=允许, <2.2V)
 */
#define SD3077_REG_CTR4             (0x19)

/* 控制寄存器5 (1AH)
 * D7:    BAT8_VAL (电池电量ADC结果的最高位, Bit8)
 * D6-D2: 0 (固定)
 * D1:    BHF (电池高压标志位, 1=高压)
 * D0:    BLF (电池低压标志位, 1=低压, 同0FH[D3])
 */
#define SD3077_REG_CTR5             (0x1A)

/* 电池电量ADC结果低8位 (1BH)
 * 注: 完整的9位电池电压值 = (CTR5[D7]<<8) | BAT_VAL
 * 例如0x135=309d=3.09V
 */
#define SD3077_REG_BAT_VAL          (0x1B)

#define SD3077_REG_TEMP_ALARM_L     (0x1C) /* 低温报警阈值 (D7=符号) */
#define SD3077_REG_TEMP_ALARM_H     (0x1D) /* 高温报警阈值 (D7=符号) */
#define SD3077_REG_TEMP_HIST_L      (0x1E) /* 历史最低温度 (D7=符号) */
#define SD3077_REG_TEMP_HIST_H      (0x1F) /* 历史最高温度 (D7=符号) */ 

/*
 * ============================================================================
 * 历史温度时间戳 (20H - 2BH)
 * ============================================================================
 */
#define SD3077_REG_HIST_L_MIN       (0x20) /* 历史低温: 分钟 (BCD)       */
#define SD3077_REG_HIST_L_HOUR      (0x21) /* 历史低温: 小时 (BCD)       */
#define SD3077_REG_HIST_L_WEEK      (0x22) /* 历史低温: 星期             */
#define SD3077_REG_HIST_L_DAY       (0x23) /* 历史低温: 日 (BCD)         */
#define SD3077_REG_HIST_L_MONTH     (0x24) /* 历史低温: 月 (BCD)         */
#define SD3077_REG_HIST_L_YEAR      (0x25) /* 历史低温: 年 (BCD)         */

#define SD3077_REG_HIST_H_MIN       (0x26) /* 历史高温: 分钟 (BCD)       */
#define SD3077_REG_HIST_H_HOUR      (0x27) /* 历史高温: 小时 (BCD)       */
#define SD3077_REG_HIST_H_WEEK      (0x28) /* 历史高温: 星期             */
#define SD3077_REG_HIST_H_DAY       (0x29) /* 历史高温: 日 (BCD)         */
#define SD3077_REG_HIST_H_MONTH     (0x2A) /* 历史高温: 月 (BCD)         */
#define SD3077_REG_HIST_H_YEAR      (0x2B) /* 历史高温: 年 (BCD)         */

/*
 * ============================================================================
 * 用户SRAM (2CH - 71H)
 * ============================================================================
 */
#define SD3077_REG_SRAM_START       (0x2C) /* 70字节用户SRAM起始地址     */
#define SD3077_REG_SRAM_END         (0x71) /* 70字节用户SRAM结束地址     */

/*
 * ============================================================================
 * 芯片ID (72H - 79H)
 * ============================================================================
 */
#define SD3077_REG_ID_START         (0x72) /* 8字节ID起始地址 (只读)     */
#define SD3077_REG_ID_YEAR          (0x72) /* 生产年份                   */
#define SD3077_REG_ID_MONTH         (0x73) /* 生产月份                   */
#define SD3077_REG_ID_DAY           (0x74) /* 生产日期                   */
#define SD3077_REG_ID_MACHINE       (0x75) /* 生产机台号                 */
#define SD3077_REG_ID_ORDER_1       (0x76) /* 生产工单号1                */
#define SD3077_REG_ID_ORDER_2       (0x77) /* 生产工单号2                */
#define SD3077_REG_ID_SERIAL_1      (0x78) /* 工单内序号1                */
#define SD3077_REG_ID_SERIAL_2      (0x79) /* 工单内序号2                */
#define SD3077_REG_ID_END           (0x79) /* 8字节ID结束地址            */

typedef enum
{
    AM = 0x00,
    PM = 0x01,
    HOUR24 = 0x02
} AM_PM;

typedef struct
{
    uint8_t year;
    uint8_t month;
    uint8_t dayOfMonth;
    uint8_t dayOfWeek;
    uint8_t hours;
    uint8_t minutes;
    uint8_t seconds;
    AM_PM ampm;
} DateTime;

/**
 * SD3077的中断输出频率定义，为0即关闭中断输出
 */
typedef enum
{
    F_0_HZ       = 0x00,
    F_4096_HZ    = 0x02,
    F_1024_HZ    = 0x03,
    F_64_HZ      = 0x04,
    F_32_HZ      = 0x05,
    F_16_HZ      = 0x06,
    F_8_HZ       = 0x07,
    F_4_HZ       = 0x08,
    F_2_HZ       = 0x09,
    F_1_HZ       = 0x0A,
    F_0_5_HZ     = 0x0B,
    F_0_25_HZ    = 0x0C,
    F_0_125_HZ   = 0x0D,
    F_0_0625_HZ  = 0x0E,
    F_1_S        = 0x0F,
} SD3077IntFreq;

void time_now(DateTime *dateTime);
void set_time(DateTime *dateTime);
void set_interrupt_output(SD3077IntFreq freq);
void enable_second_interrupt_output();
void write_backup_data(uint8_t index, uint8_t *data, uint8_t size);
void read_backup_data(uint8_t index, uint8_t *data, uint8_t size);
void sec_int_gpio_init(void);
void sd3077_iic_init(void);
#endif
