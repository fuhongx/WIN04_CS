/**
 * ****************************************************************************
 * @file hw_gpio.h
 * @author SLC(SC) software team. 
 * @brief 
 * @version  1.0
 * ****************************************************************************
 * 
 * Copyright (c) 2024 SLC Semiconductor all rights reserved.
 * 
 * ****************************************************************************
 */

#ifndef __HW_GPIO_H__
#define __HW_GPIO_H__

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
    EN_GPIO_IOMUX_MODE0         = 0,
    EN_GPIO_IOMUX_MODE1         = 1,
    EN_GPIO_IOMUX_MODE2         = 2,
    EN_GPIO_IOMUX_MODE3         = 3,
    EN_GPIO_IOMUX_MODE4         = 4,
    EN_GPIO_IOMUX_MODE5         = 5,
    EN_GPIO_IOMUX_MODE6         = 6,
    EN_GPIO_IOMUX_MODE7         = 7,
}EN_GPIO_IOMUX_MODE_T;

typedef enum
{
    EN_GPIO_PIN_MODE_INPUT       = 0,
    EN_GPIO_PIN_MODE_OUTPUT      = 1,
}EN_GPIO_PIN_MODE_T;

typedef enum
{
    EN_GPIO_OUTPUT_LEVEL_LO     =   0,
    EN_GPIO_OUTPUT_LEVEL_HI     =   1,
}EN_GPIO_OUTPUT_LEVEL_T;

typedef enum
{
    EN_GPIO_PULL_MODE_DOWN       =   0,
    EN_GPIO_PULL_MODE_UP         =   1,
}EN_GPIO_PULL_MODE_T;

typedef enum
{
    EN_GPIO_TURN_RATE_LO         =  0,
    EN_GPIO_TURN_RATE_HI         =  1,
}EN_GPIO_TURN_RATE_T;

typedef enum
{
    EN_GPIO_DRIVEN_STRENGTH0     = 0,
    EN_GPIO_DRIVEN_STRENGTH1     = 1,
    EN_GPIO_DRIVEN_STRENGTH2     = 2,
    EN_GPIO_DRIVEN_STRENGTH3     = 3,
}EN_GPIO_DRIVEN_STRENGTH_T;

typedef enum
{
    EN_GPIO_INT_LO_LEVEL        = 0,
    EN_GPIO_INT_HI_LEVEL        = 1,
    EN_GPIO_INT_RISING_EDGE     = 2,
    EN_GPIO_INT_FALLING_EDGE    = 3,
    EN_GPIO_INT_BOTH_EDGE       = 4,
}EN_GPIO_INT_T;

/* GPIO */
/**
 * @brief  Get GPIO input level
 *
 * @param u32Pin GPIO pin number, see reg_gio.h EN_GPIO_PIN_DEF_T for details
 * @param pu32Flag GPIO input level, 0: low level, 1: high level
 * @return EN_ERR_STA_T
 */
EN_ERR_STA_T rom_hw_gpio_get_input_level(uint32_t u32Pin, uint32_t *pu32Flag);

/**
 * @brief set GPIO output level
 *
 * @param u32Pin GPIO pin number, see reg_gio.h EN_GPIO_PIN_DEF_T for details
 * @param enMode GPIO level, see EN_GPIO_OUTPUT_LEVEL_T for details
 * @return EN_ERR_STA_T
 */
EN_ERR_STA_T rom_hw_gpio_set_output_level(uint32_t u32Pin, EN_GPIO_OUTPUT_LEVEL_T enMode);

/**
 * @brief  Enable or disable GPIO output function
 *
 * @param u32Pin GPIO pin number, see reg_gio.h EN_GPIO_PIN_DEF_T for details
 * @param bEnable true: enable output function; false: disable output function
 * @return EN_ERR_STA_T
 */
EN_ERR_STA_T rom_hw_gpio_enable_output(uint32_t u32Pin, bool bEnable);

/**
 * @brief  Enable or disable GPIO input function
 *
 * @param u32Pin GPIO pin number, see reg_gio.h EN_GPIO_PIN_DEF_T for details
 * @param bEnable true: enable input function; false: disable input function
 * @return EN_ERR_STA_T
 */
EN_ERR_STA_T rom_hw_gpio_enable_input(uint32_t u32Pin, bool bEnable);

/**
 * @brief  Set GPIO pin direction
 *
 * @param u32Pin GPIO pin number, see reg_gio.h EN_GPIO_PIN_DEF_T for details
 * @param enMode GPIO pin direction, see EN_GPIO_PIN_MODE_T for details
 * @return EN_ERR_STA_T
 */
EN_ERR_STA_T rom_hw_gpio_set_output_mode(uint32_t u32Pin, EN_GPIO_PIN_MODE_T enMode);
EN_ERR_STA_T rom_hw_gpio_enable_open_drain(uint32_t u32Pin, bool bEnable);
EN_ERR_STA_T rom_hw_gpio_enable_pull_mode(uint32_t u32Pin, bool bEnable);

/**
 * @brief  Enable or disable GPIO iomux function
 *
 * @param u32Pin  GPIO pin number, see reg_gio.h EN_GPIO_PIN_DEF_T for details
 * @param bEnable true: enable iomux function; false: disable iomux function
 * @return EN_ERR_STA_T
 */
EN_ERR_STA_T rom_hw_gpio_enable_iomux(uint32_t u32Pin, bool bEnable);

/**
 * @brief  Set GPIO iomux mode
 *
 * @param u32Pin GPIO pin number, see reg_gio.h EN_GPIO_PIN_DEF_T for details
 * @param enMode GPIO iomux mode, see EN_GPIO_IOMUX_MODE_T for details
 * @return EN_ERR_STA_T
 */
EN_ERR_STA_T rom_hw_gpio_set_iomux_mode(uint32_t u32Pin, EN_GPIO_IOMUX_MODE_T enMode);
EN_ERR_STA_T rom_hw_gpio_set_sample_divider(uint8_t u8Div);
EN_ERR_STA_T rom_hw_gpio_enable_interrupt(uint32_t u32Pin);
EN_ERR_STA_T rom_hw_gpio_disable_interrupt(uint32_t u32Pin);
EN_ERR_STA_T rom_hw_gpio_set_interrupt_mode(uint32_t u32Pin, EN_GPIO_INT_T enMode);
EN_ERR_STA_T rom_hw_gpio_clear_interrupt_mode(uint32_t u32Pin);
EN_ERR_STA_T rom_hw_gpio_get_interrupt_flag(uint32_t *pu32Flag);
EN_ERR_STA_T rom_hw_gpio_clear_interrupt_flag(uint32_t u32Flag);
EN_ERR_STA_T rom_hw_gpio_get_edge_interrupt_flag(uint32_t *u32Flag, bool bRisingFlag);

/* LPIO */
EN_ERR_STA_T rom_hw_lpio_get_input_level(uint32_t u32Pin, uint32_t *pu32Flag);
EN_ERR_STA_T rom_hw_lpio_set_output_level(uint32_t u32Pin, EN_GPIO_OUTPUT_LEVEL_T enMode);
EN_ERR_STA_T rom_hw_lpio_set_pin_direction(uint32_t u32Pin, EN_GPIO_PIN_MODE_T enMode);
EN_ERR_STA_T rom_hw_lpio_enable_pull_mode(uint32_t u32Pin, bool bEnable);
EN_ERR_STA_T rom_hw_lpio_enable_iomux(uint32_t u32Pin, bool bEnable);
EN_ERR_STA_T rom_hw_lpio_enable_interrupt(uint32_t u32Pin);
EN_ERR_STA_T rom_hw_lpio_disable_interrupt(uint32_t u32Pin);
EN_ERR_STA_T rom_hw_lpio_set_interrupt_mode(uint32_t u32Pin, EN_GPIO_INT_T enMode);
void rom_hw_lpio_clear_interrupt(uint32_t sta);
uint32_t rom_hw_lpio_get_interrupt_status(void);

#endif
