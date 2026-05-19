/**
 * ****************************************************************************
 * @file slc_hal_gpio.h
 * @author SLC software team.
 * @brief
 * @version  1.0
 * ****************************************************************************
 *
 * THIS CODE IS LIMITED TO GUIDE APPLICATION DEVELOPMENT AND INCREASE DEVELOPMENT SPEED.
 * AS A RESULT, SLC IS NOT RESPONSIBLE FOR ANY CLAIMS ARISING DIRECTLY OR INDIRECTLY FROM SUCH FIRMWARE AND/OR CONTENT.
 *
 * Copyright (c) 2026 SLC Semiconductor all rights reserved.
 *
 * ****************************************************************************
 */

#ifndef __SLC_HAL_GPIO_H__
#define __SLC_HAL_GPIO_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#define SLC_HAL_LPIO_MASK         (0xC0000)

typedef enum
{
    HAL_GPIO_LEVEL_LOW  = 0,    // 低电平
    HAL_GPIO_LEVEL_HIGH = 1,    // 高电平
} hal_gpio_level_e;

typedef enum
{
    HAL_GPIO_INPUT,
    HAL_GPIO_OUTPUT,
} hal_gpio_direction_e;

typedef enum
{
    HAL_GPIO_PULL_NONE,     // 悬空, 无上拉下拉
    HAL_GPIO_PULL_UP,       // 上拉
    HAL_GPIO_PULL_DOWN,     // 下拉(不支持)
    HAL_GPIO_OPEN_DRAIN,    // 开漏
} hal_gpio_mode_e;

typedef enum
{
    HAL_GPIO_PIN0,
    HAL_GPIO_PIN1,
    HAL_GPIO_PIN2,
    HAL_GPIO_PIN3,
    HAL_GPIO_PIN4,
    HAL_GPIO_PIN5,
    HAL_GPIO_PIN6,
    HAL_GPIO_PIN7,
    HAL_GPIO_PIN8,
    HAL_GPIO_PIN9,
    HAL_GPIO_PIN10,
    HAL_GPIO_PIN11,
    HAL_GPIO_PIN12,
    HAL_GPIO_PIN13,
    HAL_GPIO_PIN14,
    HAL_GPIO_PIN15,
    HAL_GPIO_PIN16,
    HAL_GPIO_PIN17,
    HAL_GPIO_PIN18,
    HAL_GPIO_PIN19,
    HAL_GPIO_PIN20,
    HAL_GPIO_PIN21,

    HAL_GPIO_PIN_MAX,
} hal_gpio_pin_e;

/**
 * @brief Define GPIO IOMUX mode.
 * ----------------------------------------------------------------------------------------------------------
 *        |  MODE0    |  MODE1     |  MODE2     |  MODE3     |  MODE4    |  MODE5    |  MODE6    |  MODE7    |
 * GPIO0  | UART0_TX  | UART0_RX   | SPI0_CS    | SPI0_CS    | I2C0_SDA  | I2C0_SCK  | PWM1_OUT  | TRX_SW    |
 * GPIO1  | UART0_RX  | UART0_TX   | SPI0_CLK   | SPI0_CLK   | I2C0_SCK  | I2C0_SDA  | PWM2_OUT  | TRX_SW    |
 * GPIO2  | UART1_TX  | UART0_CTS  | SPI0_SI    | SPI0_SO    | I2C1_SDA  | I2C1_SCK  | PWM3_OUT  | TRX_SW    |
 * GPIO3  | UART1_RX  | UART0_RTS  | SPI0_SO    | SPI0_SI    | I2C1_SCK  | I2C1_SDA  |   NULL    | TRX_SW    |
 * GPIO4  | UART0_TX  | UART1_CTS  | SPI1_CS    | SPI1_CS    | I2C0_SDA  | I2C0_SCK  | PWM1_OUT  | TRX_SW    |
 * GPIO5  | UART0_RX  | UART1_RTS  | SPI1_CLK   | SPI1_CLK   | I2C0_SCK  | I2C0_SDA  | PWM2_OUT  | TRX_SW    |
 * GPIO6  | UART1_TX  | UART1_RX   | SPI1_SI    | SPI1_SO    | I2C1_SDA  | I2C1_SCK  | PWM3_OUT  | TRX_SW    |
 * GPIO7  | UART1_RX  | UART1_TX   | SPI1_SO    | SPI1_SI    | I2C1_SCK  | I2C1_SDA  |   NULL    | TRX_SW    |
 * GPIO8  | UART0_TX  | UART0_RX   | SPI0_CS    | SPI0_CS    | I2C0_SDA  | I2C0_SCK  | PWM1_OUT  | TRX_SW    |
 * GPIO9  | UART0_RX  | UART0_TX   | SPI0_CLK   | SPI0_CLK   | I2C0_SCK  | I2C0_SDA  | PWM2_OUT  | TRX_SW    |
 * GPIO10 | UART1_TX  | UART0_CTS  | SPI0_SI    | SPI0_SO    | I2C1_SDA  | I2C1_SCK  | PWM3_OUT  | TRX_SW    |
 * GPIO11 | UART1_RX  | UART0_RTS  | SPI0_SO    | SPI0_SI    | I2C1_SCK  | I2C1_SDA  |   NULL    | TRX_SW    |
 * GPIO12 | UART0_TX  | UART1_CTS  | SPI1_CS    | SPI1_CS    | I2C0_SDA  | I2C0_SCK  | PWM1_OUT  | TRX_SW    |
 * GPIO13 | UART0_RX  | UART1_RTS  | SPI1_CLK   | SPI1_CLK   | I2C0_SCK  | I2C0_SDA  | PWM2_OUT  | TRX_SW    |
 * GPIO14 | UART1_TX  | UART1_RX   | SPI1_SI    | SPI1_SO    | I2C1_SDA  | I2C1_SCK  | PWM3_OUT  | TRX_SW    |
 * GPIO15 | UART1_RX  | UART1_TX   | SPI1_SO    | SPI1_SI    | I2C1_SCK  | I2C1_SDA  |   NULL    | TRX_SW    |
 * GPIO16 | UART0_TX  | UART0_RX   | None       | None       | I2C0_SDA  | I2C0_SCK  | PWM1_OUT  | TRX_SW    | 
 * GPIO17 | UART0_RX  | UART0_TX   | None       | None       | I2C0_SCK  | I2C0_SDA  | PWM2_OUT  | TRX_SW    |
 * GPIO18 | LPUART_TX | LPUART_TX  | LPUART_TX  | LPUART_TX  | LPUART_TX | LPUART_TX | LPUART_TX | LPUART_TX |
 * GPIO19 | LPUART_RX | LPUART_RX  | LPUART_RX  | LPUART_RX  | LPUART_RX | LPUART_RX | LPUART_RX | LPUART_RX |
 * GPIO20 |    SWC    |    SWC     |    SWC     |    SWC     |    SWC    |    SWC    |    SWC    |    SWC    |
 * GPIO21 |    SWD    |    SWD     |    SWD     |    SWD     |    SWD    |    SWD    |    SWD    |    SWD    |
 * ----------------------------------------------------------------------------------------------------------
 */
typedef enum
{
    HAL_IOMUX_MODE0,
    HAL_IOMUX_MODE1,
    HAL_IOMUX_MODE2,
    HAL_IOMUX_MODE3,
    HAL_IOMUX_MODE4,
    HAL_IOMUX_MODE5,
    HAL_IOMUX_MODE6,
    HAL_IOMUX_MODE7,

    HAL_IOMUX_GPIO,  // 作为普通GPIO使用
} hal_gpio_iomux_e;

typedef enum {
    HAL_GPIO_INTR_LOW_LEVEL     = 0,    // 低电平触发
    HAL_GPIO_INTR_HIGH_LEVEL    = 1,    // 高电平触发
    HAL_GPIO_INTR_RISING_EDGE   = 2,    // 上升沿触发
    HAL_GPIO_INTR_FALLING_EDGE  = 3,    // 下降沿触发
    HAL_GPIO_INTR_BOTH_EDGE     = 4,    // 上升沿和下降沿都触发

    HAL_GPIO_INTR_NONE,                 // 不使能中断
} hal_gpio_intr_e;

typedef enum {
    HAL_GPIO_INTR_STA_NONE,         // 无中断
    HAL_GPIO_INTR_STA_LEVEL,        // 电平中断
    HAL_GPIO_INTR_STA_RISING,       // 上升沿中断
    HAL_GPIO_INTR_STA_FALLING,      // 下降沿中断
} hal_gpio_intr_sta_e;

/* ----------------------------------------------------------------------------------------------------------
 * GPIO API, works on normal mode; 正常模式下工作
 * ---------------------------------------------------------------------------------------------------------- */

/**
 * @brief Set GPIO pin direction, 设置GPIO引脚方向
 *
 * @param pin GPIO pin number, GPIO引脚编号, see hal_gpio_pin_e enum for details
 * @param direction GPIO direction to set, 设置的GPIO方向, see hal_gpio_direction_e enum for details
 */
void slc_hal_gpio_set_direction(hal_gpio_pin_e pin, hal_gpio_direction_e direction);

/**
 * @brief Set GPIO input filter sample cycle, 设置GPIO输入毛刺过滤采样周期
 *
 * @param sample_cycle Sample cycle count, 毛刺过滤采样周期数，范围1-256
 */
void slc_hal_gpio_set_input_filter(uint32_t sample_cycle);

/**
 * @brief Get GPIO pin level, 获取GPIO引脚电平
 *
 * @param pin GPIO pin number, GPIO引脚编号, see hal_gpio_pin_e enum for details
 * @return GPIO level, GPIO电平, see hal_gpio_level_e enum for details
 */
hal_gpio_level_e slc_hal_gpio_get_value(hal_gpio_pin_e pin);

/**
 * @brief Set GPIO pin level, 设置GPIO引脚电平
 *
 * @param pin GPIO pin number, GPIO引脚编号, see hal_gpio_pin_e enum for details
 * @param level GPIO level to set, 设置的GPIO电平, see hal_gpio_level_e enum for details
 */
void slc_hal_gpio_set_value(hal_gpio_pin_e pin, hal_gpio_level_e level);

/**
 * @brief Set GPIO pin interrupt mode, 设置GPIO引脚中断触发模式
 *
 * @param pin GPIO pin number, GPIO引脚编号, see hal_gpio_pin_e enum for details
 * @param interrupt GPIO interrupt mode to set, 设置的GPIO中断触发模式, see hal_gpio_intr_e enum for details
 */
void slc_hal_gpio_set_interrupt(hal_gpio_pin_e pin, hal_gpio_intr_e interrupt);

/**
 * @brief Enable or disable GPIO pin interrupt, 使能或禁用GPIO引脚中断
 *
 * @param pin GPIO pin number, GPIO引脚编号, see hal_gpio_pin_e enum for details
 * @param enable true to enable interrupt, false to disable interrupt, 是否使能中断，true为使能，false为禁用
 */
void slc_hal_gpio_enable_interrupt(hal_gpio_pin_e pin, bool enable);

/**
 * @brief Clear GPIO single pin interrupt flag, 清除单一GPIO引脚中断标志
 *
 * @param pin GPIO pin number, GPIO引脚编号, see hal_gpio_pin_e enum for details
 */
void slc_hal_gpio_clear_pin_interrupt(hal_gpio_pin_e pin);

/**
 * @brief Get GPIO pin interrupt status, 获取GPIO引脚中断状态
 *
 * @return GPIO interrupt status, GPIO中断状态, see hal_gpio_intr_sta_e enum for details
 */
hal_gpio_intr_sta_e slc_hal_gpio_get_pin_interrupt_sta(hal_gpio_pin_e pin);

/**
 * @brief Clear all GPIO interrupts, 清除所有GPIO引脚中断标志
 */
void slc_hal_gpio_clear_all_interrupts(void);

/**
 * @brief Get all GPIO interrupts status, 获取所有GPIO引脚中断状态
 *
 * @return GPIO interrupt status bitmap, GPIO中断状态位，每一位对应一个GPIO引脚的中断状态
 */
uint32_t slc_hal_gpio_get_all_interrupts(void);

/**
 * @brief Set GPIO pin mode, 设置GPIO引脚上下拉模式
 *
 * @param pin GPIO pin number, GPIO引脚编号, see hal_gpio_pin_e enum for details
 * @param mode GPIO mode to set, 设置的GPIO模式, see hal_gpio_mode_e enum for details
 */
void slc_hal_gpio_set_mode(hal_gpio_pin_e pin, hal_gpio_mode_e mode);

/**
 * @brief Set GPIO pin iomux, 设置GPIO引脚复用功能
 *
 * @param pin GPIO pin number, GPIO引脚编号, see hal_gpio_pin_e enum for details
 * @param iomux GPIO iomux mode to set, 设置的GPIO复用模式, see hal_gpio_iomux_e enum for details
 */
void slc_hal_gpio_set_iomux(hal_gpio_pin_e pin, hal_gpio_iomux_e iomux);


/* ----------------------------------------------------------------------------------------------------------
 * LPIO API(same as GPIO API), works on low power mode; 低功耗模式下工作
 * ---------------------------------------------------------------------------------------------------------- */

/**
 * @brief Set LPIO pin direction, 设置LPIO引脚方向
 *
 * @param pin LPIO pin number, LPIO引脚编号, see hal_gpio_pin_e enum for details
 * @param direction LPIO direction to set, 设置的LPIO方向, see hal_gpio_direction_e enum for details
 */
void slc_hal_lpio_set_direction(hal_gpio_pin_e pin, hal_gpio_direction_e direction);

/**
 * @brief Get LPIO pin level, 设置LPIO引脚电平
 *
 * @param pin LPIO pin number, LPIO引脚编号, see hal_gpio_pin_e enum for details
 * @param level LPIO level to set, 设置的LPIO电平, see hal_gpio_level_e enum for details
 */
void slc_hal_lpio_set_value(hal_gpio_pin_e pin, hal_gpio_level_e level);

/**
 * @brief Get LPIO pin level, 获取LPIO引脚电平
 * @note: 该接口获取的状态仅为唤醒事件发生时的电平状态，并非实时电平状态
 *
 * @param pin LPIO pin number, LPIO引脚编号, see hal_gpio_pin_e enum for details
 * @return LPIO level, LPIO电平, see hal_gpio_level_e enum for details
 */
hal_gpio_level_e slc_hal_lpio_get_value(hal_gpio_pin_e pin);

/**
 * @brief Set LPIO pin mode(only support pull-up and floating), 设置LPIO引脚上下拉模式(仅支持上拉和悬空)
 *
 * @param pin LPIO pin number, LPIO引脚编号, see hal_gpio_pin_e enum for details
 * @param mode LPIO mode to set, 设置的LPIO模式, see hal_gpio_mode_e enum for details
 */
void slc_hal_lpio_set_mode(hal_gpio_pin_e pin, hal_gpio_mode_e mode);

/**
 * @brief Set LPIO pin iomux, 设置LPIO引脚复用功能(仅支持Pin18和Pin19)
 *
 * @param pin LPIO pin number, LPIO引脚编号, see hal_gpio_pin_e enum for details
 * @param iomux LPIO iomux mode to set, 设置的LPIO复用模式, see hal_gpio_iomux_e enum for details
 */
void slc_hal_lpio_set_iomux(hal_gpio_pin_e pin, hal_gpio_iomux_e iomux);

/**
 * @brief Set LPIO pin interrupt mode, 设置LPIO引脚中断触发模式(仅支持电平触发)
 *
 * @param pin LPIO pin number, LPIO引脚编号, see hal_gpio_pin_e enum for details
 * @param interrupt LPIO interrupt mode to set, 设置的LPIO中断触发模式, see hal_gpio_intr_e enum for details
 */
void slc_hal_lpio_set_interrupt(hal_gpio_pin_e pin, hal_gpio_intr_e interrupt);

/**
 * @brief Enable or disable LPIO pin interrupt, 使能或禁用LPIO引脚中断
 *
 * @param pin LPIO pin number, LPIO引脚编号, see hal_gpio_pin_e enum for details
 * @param enable true to enable interrupt, false to disable interrupt, 是否使能中断，true为使能，false为禁用
 */
void slc_hal_lpio_enable_interrupt(hal_gpio_pin_e pin, bool enable);

/**
 * @brief Clear all LPIO interrupts, 清除所有LPIO引脚中断标志
 */
void slc_hal_lpio_clear_interrupt(void);

/**
 * @brief Get all LPIO interrupts status, 获取所有LPIO引脚中断状态
 *
 * @return LPIO interrupt status, LPIO中断状态位，仅显示总的中断状态，不区分具体引脚
 */
uint32_t slc_hal_lpio_get_interrupt_sta(void);

#ifdef __cplusplus
}
#endif

#endif  /* __SLC_HAL_GPIO_H__ */
