# 天猛星 MSPM0G3507 Keil5 模板
13232
3


65
5
本模板面向网表 `Netlist_Schematic1_2026-07-22.tel` 对应的天猛星 MSPM0G3507 底板，使用 ARM Compiler 5.06 和手动外设初始化。默认固件只初始化 80 MHz 系统时钟并点亮底板 LED1，不初始化电机、PWM、编码器、UART、OLED 或 MPU6050。

## 启动与默认 LED

启动链如下：

```text
Reset_Handler -> ARMCC __main -> main -> system_init -> board_led_init
```

- `system_init()` 复位并使能 GPIOA/GPIOB，使用 SYSOSC 驱动 SYSPLL，将 MCLK 配置为 80 MHz。
- `board_led_init()` 将 PB14 配置为 GPIO 输出并输出低电平，使 LED1 常亮。
- LED1 阳极经 1 kΩ 限流电阻连接 3.3 V，阴极连接 PB14，因此 PB14 低电平点亮、高电平熄灭。
- 核心板 LED 位于 PB22，但 PB22 同时连接 `IMU_MOSI`，因此不作为默认 LED。
- `ti_msp_dl_config.c/.h` 和 `empty.syscfg` 仅作参考，不参与 Keil 工程编译，也不得与 `system_init()` 混用。

## 已接入驱动的引脚

| 功能 | 引脚 | 驱动说明 |
| --- | --- | --- |
| LED1 | PB14 / PINCM31 | 默认 LED，低电平有效 |
| LED2 | PB18 / PINCM44 | 低电平有效 |
| LED3 | PA22 / PINCM47 | 低电平有效 |
| 核心板 LED | PB22 / PINCM50 | 低电平有效，与 IMU_MOSI 冲突 |
| UART0 | PA10 TX、PA11 RX | 默认 `printf` 目标，4 MHz MFCLK |
| UART1 | PA8 TX、PA9 RX | 按需初始化 |
| UART2 | PB15 TX、PB16 RX | 按需初始化 |
| UART3 | PA26 TX、PB13 RX | PA26 与 TIMG8_CH0 冲突 |
| OLED 软件 I2C | PB2 SCL、PB3 SDA | SSD1306，7 位地址 0x3C |
| MPU6050 软件 I2C | PA1 SCL、PA0 SDA | 7 位地址 0x68 |
| 电机 A | PA28 PWM、PA13/PB26 方向 | TIMG7_CH0 |
| 电机 B | PB20 PWM、PB9/PB7 方向 | TIMG12_CH0 |
| 编码器 1 | PB23 A、PB12 B | A 相下降沿中断 |
| 编码器 2 | PB4 A、PB5 B | A 相下降沿中断 |
| 加热 PWM | PB19 / TIMG7_CH1 | 仅登记通用 PWM 路由 |

## 其他网表网络

以下引脚已在 `ml_board.h` 集中定义，但模板不为其新增完整驱动：

| 网络 | 引脚 |
| --- | --- |
| IMU_SCLK / MOSI / MISO | PA17 / PB22 / PA16 |
| IMU_CS_A / CS_G / CS_P | PA25 / PA2 / PB6 |
| IMU_INT_A / INT_G / INT_T | PB1 / PB17 / PA23 |
| KEY_UP / DOWN / LEFT | PA14 / PA24 / PA15 |
| KEY_RIGHT / CENTER | PB25 / PB24 |
| BUZZER / ADC_read | PA21 / PA27 |
| AD0 / AD1 / AD2 | PB21 / PA30 / PB0 |
| C1 / C2 / C3 / C8 | PA31 / PA12 / PB8 / PB10 |

五向按键公共端接地，使用时必须配置为上拉输入，按下为低电平。

## PWM 路由与复用冲突

通用 PWM 驱动支持以下固定路由：

| 定时器通道 | 引脚 |
| --- | --- |
| TIMG0_CH0 / CH1 | PA12 / PA13 |
| TIMG6_CH0 / CH1 | PB6 / PB7 |
| TIMG7_CH0 / CH1 | PA28 / PB19 |
| TIMG8_CH0 / CH1 | PA26 / PA27 |
| TIMG12_CH0 / CH1 | PB20 / PB24 |

初始化时会登记驱动之间能够自动检测的资源冲突：

| 引脚或资源 | 冲突用途 |
| --- | --- |
| PA13 | 电机 AIN1 / TIMG0_CH1 |
| PB7 | 电机 BIN2 / TIMG6_CH1 |
| PA26 | UART3_TX / TIMG8_CH0 |
| PB22 | 核心板 LED / IMU_MOSI |
| PB24 | KEY_CENTER / TIMG12_CH1 |
| TIMG7 | CH0 电机 PWM / CH1 加热 PWM 共用频率 |

同一定时器的两个 PWM 通道必须使用相同频率；第二个通道请求不同频率时返回 `ML_STATUS_BUSY`。未提供驱动的核心板 LED、IMU SPI 和按键由应用层负责遵守冲突表。

## 驱动行为

- 初始化和传输函数使用 `ml_status_t` 返回参数错误、未初始化、超时、资源占用、I2C 无 ACK、缓冲区状态或设备未找到等状态。
- 软件 I2C 只主动拉低总线，高电平通过输入高阻释放，支持 ACK、时钟拉伸超时和 9 脉冲总线恢复。
- EXTI 同时支持 PA0-PA27 与 PB0-PB27；共享 `GROUP1_IRQHandler` 会分别处理 GPIOA 和 GPIOB 的全部待处理中断。
- UART RX 使用 64 字节环形缓冲区；满时丢弃新字节并累计溢出次数。
- 电机初始化先设置四个方向脚和两个 PWM 通道为安全零输出，接通电机电源前应先完成空载波形检查。
- 编码器计数为 `volatile int32_t`，应用应使用 `encoder_get_and_clear()` 原子读取并清零。

## 构建与验收

全量构建命令：

```text
F:\\xinkil\\UV4\\UV4.exe -r user\\project.uvprojx -j0
```

提交前应确认：

- 构建结果为 `0 Error(s), 0 Warning(s)`。
- 含中文的 `.c/.h` 文件为 UTF-8 BOM，注释不存在乱码或替换字符。
- 默认链接结果移除了未使用的 `motor_init`、`pid_control`、`OLED_Init`、`MPU6050_Init` 和 `SYSCFG_DL_init`。
- 硬件首先验收 LED1 PB14 常亮，再逐项验证 UART、OLED、IMU I2C、PWM 和 GPIOB 编码器中断。
