#include "headfile.h"

/*
 * 天猛星 G3507 底板主要引脚（依据 Netlist_Schematic1_2026-07-22.tel）
 *
 * LED1: PB14，低电平点亮。LED 阳极经限流电阻接 3.3 V，阴极接 PB14。
 * LED2: PB18，低电平点亮；LED3: PA22，低电平点亮。
 * 核心板 LED: PB22，低电平点亮，但与 IMU_MOSI 复用。
 *
 * UART0: TX PA10, RX PA11
 * UART1: TX PA8,  RX PA9
 * UART2: TX PB15, RX PB16
 * UART3: TX PA26, RX PB13
 *
 * OLED 软件 I2C: SCL PB2, SDA PB3
 * MPU6050 软件 I2C: SCL PA1, SDA PA0
 *
 * 电机 A: PWM PA28/TIMG7_CH0, AIN1 PA13, AIN2 PB26
 * 电机 B: PWM PB20/TIMG12_CH0, BIN1 PB9, BIN2 PB7
 * 编码器 1: E1A PB23, E1B PB12
 * 编码器 2: E2A PB4,  E2B PB5
 *
 * 默认程序只初始化并点亮 LED1，不初始化其他外设。
 */

int main(void)
{
    if (system_init() != ML_STATUS_OK) {
        while (1) {
        }
    }
    if (board_led_init() != ML_STATUS_OK) {
        while (1) {
        }
    }

    delay_ms(500U);

    while (1) {
        __WFI();
    }
}
