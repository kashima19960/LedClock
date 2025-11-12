正在收集工作区信息根据代码分析，这个STM32F030F4P6项目使用了以下片上外设：

## 已使用的片上外设

1. **GPIO (通用I/O端口)**

   - SEC_INT_Pin (秒中断输入)
   - MODE_KEY_Pin (MODE按键输入)
   - SET_KEY_Pin (SET按键输入)
   - BUZZER_Pin (蜂鸣器输出)
   - TM1637数据/时钟线
   - SD3077 I2C接口线
2. **I2C1 (I2C接口)**

   - 用于与RTC模块 SD3077 通信
   - 用于读写RTC的备份寄存器
3. **ADC (模数转换器)**

   - 通道0：光敏电阻（环境光线检测）
   - 通道1：NTC热敏电阻（温度测量）
4. **DMA (直接内存访问)**

   - 用于ADC转换结果的DMA传输到 [`adcValue[2]`](Src/main.c)
5. **TIM3 (定时器3)**

   - 用于触发ADC定期转换
6. **TIM16 (定时器16)**

   - 自动亮度调节定时器 (LIGHT_CONTROL_TIMER)
   - 周期检测光线强度，自动调节显示亮度
7. **TIM17 (定时器17)**

   - 闹钟响铃控制定时器 (ALARM_CONTROL_TIMER)
   - 控制蜂鸣器的响铃节奏 (1ms中断周期)
8. **RCC (复位和时钟控制)**

   - 系统时钟配置（48MHz PLL）
   - HSI/HSI14/PLL初始化
9. **EXTI (外部中断)**

   - 用于GPIO中断处理（秒中断、按键中断）

## 未使用的主要外设

- UART/USART
- SPI
- CAN
- USB
- IWDG/WWDG
- RTC (片内，使用了外部SD3077 RTC替代)

根据项目中的代码和文档信息，这三个定时器的用途如下：

## TIM3 (`htim3`)

- **主要用途**: 作为ADC（模数转换器）的触发源
- **具体功能**: 周期性地触发ADC进行采样，用于读取光敏电阻和NTC热敏电阻的数值
- **实现方式**: 在 `main` 函数中通过 `HAL_TIM_Base_Start(&htim3)` 启动
- **配置特点**: 配置为更新触发输出（[TIM_TRGO_UPDATE](file://d:\coding\workspace\STM32F030F4-LEDClock\Drivers\STM32F0xx_HAL_Driver\Inc\stm32f0xx_hal_tim.h#L867-L868)），用于触发ADC转换

## TIM16 (`htim16`)

- **主要用途**: 自动亮度控制
- **具体功能**: 周期性中断，在中断回调函数中检查光敏电阻的ADC值，并根据预设阈值调整数码管显示亮度
- **实现方式**: 被宏定义为 `LIGHT_CONTROL_TIMER`，通过 `HAL_TIM_Base_Start_IT` 启动
- **配置特点**:
  - 1秒定时周期（预分频器1599，周期9999）
  - 使能中断，优先级为0

## TIM17 (`htim17`)

- **主要用途**: 闹钟响铃节奏控制
- **具体功能**: 当闹钟时间到达时启动，以1ms频率触发中断，精确控制蜂鸣器的鸣响和暂停，产生"嘀嘀嘀"的闹铃声效
- **实现方式**: 被宏定义为 `ALARM_CONTROL_TIMER`，在闹钟启动时通过 `alarmStart` 函数启动
- **配置特点**:
  - 1ms定时周期（预分频器15，周期999）
  - 使能中断，优先级为0

这三个定时器分别服务于项目的不同功能模块：TIM3用于环境感知（自动调光和温度检测），TIM16用于显示控制（亮度调节），TIM17用于闹钟功能（声音控制）。




正在收集工作区信息根据代码分析，这个项目使用的中断如下：

## 外部中断 (EXTI)

1. **EXTI0_1_IRQn** - EXTI Line 0 and 1 Interrupt

   - 用于 RTC 秒中断 (`SEC_INT_Pin` 在 PB1)
   - 详见 gpio.c 中的配置
2. **EXTI2_3_IRQn** - EXTI Line 2 and 3 Interrupt

   - 用于 MODE 键和 SET 键中断
   - 详见 gpio.c 中的配置

## 定时器中断

3. **TIM3_IRQn** - TIM3 Interrupt

   - 用于触发 ADC 转换（自动采样光敏电阻和 NTC 温度传感器）
4. **TIM16_IRQn** - TIM16 Interrupt

   - 自动亮度控制定时器
   - 周期性检测光线强度并调节显示亮度
5. **TIM17_IRQn** - TIM17 Interrupt

   - 闹钟响铃控制定时器
   - 控制蜂鸣器响铃节奏（1ms 中断周期）

## 其他中断

6. **DMA1_Channel1_IRQn** - DMA1 Channel 1 Interrupt

   - 用于 ADC DMA 传输
   - 详见 adc.c 中的 DMA 配置
7. **SysTick_IRQn** - System Tick Interrupt

   - 系统滴答定时器（由 HAL 库使用）

## 中断处理回调函数

主要的中断处理在 stm32f0xx_it.c 中：

- `HAL_GPIO_EXTI_Callback()` - GPIO 外部中断回调
- `HAL_TIM_PeriodElapsedCallback()` - 定时器周期中断回调
