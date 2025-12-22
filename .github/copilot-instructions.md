# LEDClock - STM32F030F4P6 嵌入式项目

## 项目架构

这是一个基于 STM32F030F4P6 的 LED 时钟项目，使用 **Keil uVision** 构建。核心功能包括时间显示、闹钟、温度测量和自动亮度调节。

### 目录结构
- **App/** - 应用层代码（状态机、中断处理、显示逻辑）
- **BSP/** - 板级支持包（外设驱动：TM1637、SD3077、ADC、GPIO、定时器等）
- **Drivers/** - STM32 HAL 库（CMSIS 和 HAL 驱动）
- **Output/** - 编译输出（.axf, .hex, .lst 等）
- **Docs/** - 项目文档（`编码规范.md` 定义了代码风格）

## 核心设计模式

### 状态机架构
项目使用 `display_mode` 枚举驱动主状态机（定义在 [app_config.h](../App/app_config.h)）：
- 显示模式：`MODE_SHOW_TIME` → `MODE_SHOW_SECOND` → `MODE_SHOW_TEMPERTURE`
- 设置模式：`MODE_SET_HOUR` → `MODE_SET_ALARM_HOUR` → `MODE_SET_BRIGHTNESS` 等共 16 种状态
- 状态转换由 MODE 键触发，在 [app_modekey.c](../App/app_modekey.c) 中实现

### 中断驱动架构
使用回调注册机制（参考 [exti.h](../BSP/inc/exti.h)）：
```c
// 注册中断回调函数
register_exti_interrupt_callback(exti_interrupt_handler);  // GPIO 中断
register_timer_interrupt_callback(tim_interrupt_handler);  // 定时器中断
```

中断处理流程：
1. HAL 中断服务程序（ISR）在 BSP 层触发
2. 调用已注册的回调函数（在 [app_interrupt_handler.c](../App/app_interrupt_handler.c)）
3. 回调函数根据中断源（秒中断/按键中断）分发到具体处理函数

### 三定时器分工
- **TIM3** - 触发 ADC 采样（光敏电阻和 NTC 热敏电阻）
- **TIM16** (`LIGHT_CONTROL_TIMER`) - 1秒周期自动亮度调节
- **TIM17** (`ALARM_CONTROL_TIMER`) - 1ms 周期控制闹钟响铃节奏

### 数据持久化
使用 SD3077 RTC 芯片的备份寄存器（12字节）保存设置：
- 索引定义在 [app_config.h](../App/app_config.h)（`BAK_*_INDEX` 常量）
- 断电标识：`POWER_DOWN_IND_DATA` (0xFA)
- 保存内容：闹钟设置、亮度配置、温度显示参数等

## 代码规范（严格遵守）

参考 [Docs/编码规范.md](../Docs/编码规范.md)：

1. **行长度限制 80 列**，超出需换行并对齐操作符
2. **命名约定**：
   - **分层策略**（重要）：
     - BSP 层硬件句柄：使用 `g_` 前缀（如 `g_adc_handle`、`g_tim16_handle`）
     - App 层状态变量：使用语义前缀 + 无 `g_` 前缀（如 `is_alarming`、`last_mode_key_press_time`）
   - 宏定义：全大写 + 下划线（如 `STRONG_BRIGHTNESS_VALUE`）
   - 函数/变量：小写 + 下划线（如 `mode_key_pressed()`）
   - 语义前缀约定：`is_`（布尔状态）、`last_`（时间戳）提高可读性
3. **缩进和空格**：4 空格缩进，运算符两侧加空格，函数间必须空行
4. **注释风格**：
   - 文件头使用 `/* ... */` 说明功能
   - 函数前添加功能说明（如 [adc.c](../BSP/src/adc.c#L12-L19)）
   - 行内注释用 `//`，放在代码右侧或上方
5. **禁止规则**：一行多条语句、多重赋值 `a = b = 0`

## 关键开发工作流

### 构建和调试
- **IDE**: Keil uVision（项目文件：`LedClock.uvprojx`）
- **调试器**: ST-Link（配置在 `DebugConfig/Target_1_STM32F030F4Px.dbgconf`）
- **输出文件**: `Output/LedClock.axf`（ELF 可执行文件）

### 添加新外设驱动
1. 在 `BSP/inc/` 添加头文件，`BSP/src/` 添加实现
2. 在 [bsp_config.h](../BSP/inc/bsp_config.h) 定义引脚和常量
3. 在 `stm32f0xx_hal_msp.c` 添加 HAL MSP 回调（如 `HAL_XXX_MspInit`）
4. 更新 Keil 项目文件以包含新源文件

### 修改显示逻辑
- 显示更新函数集中在 [app_display.c](../App/app_display.c)
- TM1637 驱动提供 `tm1637_display_byte()` 控制 4 位数码管
- 闪烁效果通过全局变量 `blink_control`（秒中断翻转）实现

### 调整状态机
1. 在 [app_config.h](../App/app_config.h) 的 `display_mode` 枚举添加新状态
2. 在 [app_modekey.c](../App/app_modekey.c) 更新 MODE 键逻辑
3. 在 [app_setkey.c](../App/app_setkey.c) 添加 SET 键对应操作
4. 在 [app_display.c](../App/app_display.c) 实现新状态的显示刷新
5. 更新 [app_settings.c](../App/app_settings.c) 保存逻辑（如需持久化）

## 外部依赖

- **SD3077** - I2C RTC 芯片（提供秒中断和备份寄存器）
- **TM1637** - 4 位 7 段数码管驱动芯片（位驱动协议，非标准 I2C）
- **NTC 热敏电阻** - 通过 ADC 通道 1 采样，使用查表法（`temperature_map[]`）转换温度

## 常见陷阱

1. **DMA 数据对齐**：ADC DMA 配置为 32 位宽（[adc.c](../BSP/src/adc.c#L72-L73)），但 `adc_value[]` 是 `uint32_t` 数组
2. **定时器句柄别名**：使用 `LIGHT_CONTROL_TIMER_HANDLE` 而非直接访问 `g_tim16_handle`
3. **HAL 回调命名**：MSP 回调函数必须精确命名为 `HAL_XXX_MspInit/MspDeInit`（弱符号覆盖）
4. **工厂测试模式**：`ENABLE_FACTORY_TEST` 宏在 [main.c](../App/main.c#L17) 控制测试入口，生产版本需关闭

## 项目文档

- [README.md](../README.md) - 外设使用清单和定时器功能说明
- [编码规范.md](../Docs/编码规范.md) - 完整的代码风格指南（373 行）
- `Output/LEDClock_项目报告.md` - 项目总结报告（如存在）
