/**
 * ****************************************************************************
 * @file slc_hal_gpio.c
 * @author SLC software team.
 * @brief
 * @version  1.0
 * ****************************************************************************
 *
 * THIS CODE IS LIMITED TO GUIDE THE APPLICATION AND IMPROVE THE EFFICIENCY OF RESEARCH AND DEVELOPMENT.
 * AS A RESULT, SLC IS NOT RESPONSIBLE FOR ANY CLAIMS ARISING DIRECTLY OR INDIRECTLY FROM SUCH FIRMWARE AND/OR CONTENT.
 *
 * Copyright (c) 2026 SLC Semiconductor all rights reserved.
 *
 * ****************************************************************************
 */
#include "slc_hal_gpio.h"
#include "reg_Dev.h"
#include "error_def.h"
#include "utility.h"
#include "hw_gpio.h"

/* GPIO func, works on normal mode */
hal_gpio_level_e slc_hal_gpio_get_value(hal_gpio_pin_e pin)
{
    if (pin >= HAL_GPIO_PIN_MAX)
        return HAL_GPIO_LEVEL_LOW;

    uint32_t value = 0;
    rom_hw_gpio_get_input_level(BIT(pin), &value);
    return (value ? HAL_GPIO_LEVEL_HIGH : HAL_GPIO_LEVEL_LOW);
}

void slc_hal_gpio_set_value(hal_gpio_pin_e pin, hal_gpio_level_e level)
{
    if (pin >= HAL_GPIO_PIN_MAX)
        return;

    rom_hw_gpio_set_output_level(BIT(pin), (EN_GPIO_OUTPUT_LEVEL_T)level);
}

void slc_hal_gpio_set_direction(hal_gpio_pin_e pin, hal_gpio_direction_e direction)
{
    if (pin >= HAL_GPIO_PIN_MAX)
        return;

    rom_hw_gpio_set_output_mode(BIT(pin), (EN_GPIO_PIN_MODE_T)direction);
}

void slc_hal_gpio_set_mode(hal_gpio_pin_e pin, hal_gpio_mode_e mode)
{
    if (pin >= HAL_GPIO_PIN_MAX)
        return;

    if (mode == HAL_GPIO_OPEN_DRAIN) {
        rom_hw_gpio_enable_open_drain(BIT(pin), true);
        rom_hw_gpio_enable_pull_mode(BIT(pin), false);
    } else if (mode == HAL_GPIO_PULL_UP) {
        rom_hw_gpio_enable_open_drain(BIT(pin), false);
        rom_hw_gpio_enable_pull_mode(BIT(pin), true);
    } else {
        rom_hw_gpio_enable_open_drain(BIT(pin), false);
        rom_hw_gpio_enable_pull_mode(BIT(pin), false);
    }
}

void slc_hal_gpio_set_iomux(hal_gpio_pin_e pin, hal_gpio_iomux_e iomux)
{
    if (pin >= HAL_GPIO_PIN_MAX)
        return;

    if (iomux == HAL_IOMUX_GPIO) {
        rom_hw_gpio_enable_iomux(BIT(pin), false);
        return;
    }

    rom_hw_gpio_enable_iomux(BIT(pin), true);
    rom_hw_gpio_set_iomux_mode(BIT(pin), (EN_GPIO_IOMUX_MODE_T)iomux);
}

void slc_hal_gpio_set_input_filter(uint32_t sample_cycle)
{
    if (sample_cycle > 256 || sample_cycle == 0) {
        sample_cycle = 1;   // only support 1~256 cycle
    }

    rom_hw_gpio_set_sample_divider(sample_cycle - 1);
}

void slc_hal_gpio_set_interrupt(hal_gpio_pin_e pin, hal_gpio_intr_e interrupt)
{
    if (pin >= HAL_GPIO_PIN_MAX)
        return;

    if (interrupt == HAL_GPIO_INTR_NONE) {
        rom_hw_gpio_clear_interrupt_mode(BIT(pin));
        rom_hw_gpio_disable_interrupt(BIT(pin));
        return;
    }

    rom_hw_gpio_set_interrupt_mode(BIT(pin), (EN_GPIO_INT_T)interrupt);
}

void slc_hal_gpio_enable_interrupt(hal_gpio_pin_e pin, bool enable)
{
    if (pin >= HAL_GPIO_PIN_MAX)
        return;

    if (enable) {
        rom_hw_gpio_enable_interrupt(BIT(pin));
    } else {
        rom_hw_gpio_disable_interrupt(BIT(pin));
    }
}

void slc_hal_gpio_clear_pin_interrupt(hal_gpio_pin_e pin)
{
    if (pin >= HAL_GPIO_PIN_MAX)
        return;

    rom_hw_gpio_clear_interrupt_flag(BIT(pin));
}

hal_gpio_intr_sta_e slc_hal_gpio_get_pin_interrupt_sta(hal_gpio_pin_e pin)
{
    uint32_t flags = 0;
    rom_hw_gpio_get_interrupt_flag(&flags);
    if ((flags & BIT(pin)) == 0)
        return HAL_GPIO_INTR_STA_NONE;

    rom_hw_gpio_get_edge_interrupt_flag(&flags, true);
    if ((flags & BIT(pin)) != 0)
        return HAL_GPIO_INTR_STA_RISING;

    rom_hw_gpio_get_edge_interrupt_flag(&flags, false);
    if ((flags & BIT(pin)) != 0)
        return HAL_GPIO_INTR_STA_FALLING;

    return HAL_GPIO_INTR_STA_LEVEL;
}

void slc_hal_gpio_clear_all_interrupts(void)
{
    rom_hw_gpio_clear_interrupt_flag(GPIO_INT_TYPE_REG_MASK);
}

uint32_t slc_hal_gpio_get_all_interrupts(void)
{
    uint32_t flags = 0;
    rom_hw_gpio_get_interrupt_flag(&flags);
    return flags;
}

/* LPIO func, works on low power mode */
void slc_hal_lpio_set_direction(hal_gpio_pin_e pin, hal_gpio_direction_e direction)
{
    if (pin >= HAL_GPIO_PIN_MAX)
        return;

    rom_hw_lpio_set_pin_direction(BIT(pin), (EN_GPIO_PIN_MODE_T)direction);
}

void slc_hal_lpio_set_value(hal_gpio_pin_e pin, hal_gpio_level_e level)
{
    if (pin >= HAL_GPIO_PIN_MAX)
        return;

    rom_hw_lpio_set_output_level(BIT(pin), (EN_GPIO_OUTPUT_LEVEL_T)level);
}

hal_gpio_level_e slc_hal_lpio_get_value(hal_gpio_pin_e pin)
{
    if (pin >= HAL_GPIO_PIN_MAX)
        return HAL_GPIO_LEVEL_LOW;

    uint32_t value = 0;
    rom_hw_lpio_get_input_level(BIT(pin), &value);
    return (value ? HAL_GPIO_LEVEL_HIGH : HAL_GPIO_LEVEL_LOW);
}

void slc_hal_lpio_set_mode(hal_gpio_pin_e pin, hal_gpio_mode_e mode)
{
    if ((pin >= HAL_GPIO_PIN_MAX) || (mode == HAL_GPIO_PULL_DOWN) || (mode == HAL_GPIO_OPEN_DRAIN))
        return;

    if (mode == HAL_GPIO_PULL_UP) {
        rom_hw_lpio_enable_pull_mode(BIT(pin), true);
    } else {
        rom_hw_lpio_enable_pull_mode(BIT(pin), false);
    }
}

void slc_hal_lpio_set_iomux(hal_gpio_pin_e pin, hal_gpio_iomux_e iomux)
{
    if (pin >= HAL_GPIO_PIN_MAX || ((BIT(pin) & SLC_HAL_LPIO_MASK) == 0))
        return;

    if (iomux == HAL_IOMUX_GPIO) {
        rom_hw_lpio_enable_iomux(BIT(pin), false);
    } else {
        rom_hw_lpio_enable_iomux(BIT(pin), true);
    }
}

void slc_hal_lpio_set_interrupt(hal_gpio_pin_e pin, hal_gpio_intr_e interrupt)
{
    if (pin >= HAL_GPIO_PIN_MAX)
        return;

    if ((interrupt == HAL_GPIO_INTR_LOW_LEVEL) || (interrupt == HAL_GPIO_INTR_HIGH_LEVEL))
        rom_hw_lpio_set_interrupt_mode(BIT(pin), (interrupt == HAL_GPIO_INTR_LOW_LEVEL) ? EN_GPIO_INT_HI_LEVEL : EN_GPIO_INT_LO_LEVEL);
}

void slc_hal_lpio_enable_interrupt(hal_gpio_pin_e pin, bool enable)
{
    if (pin >= HAL_GPIO_PIN_MAX)
        return;

    if (enable) {
        rom_hw_lpio_enable_interrupt(BIT(pin));
    } else {
        rom_hw_lpio_disable_interrupt(BIT(pin));
    }
}

void slc_hal_lpio_clear_interrupt(void)
{
    rom_hw_lpio_clear_interrupt(GPIO_INT_TYPE_REG_MASK);
}

uint32_t slc_hal_lpio_get_interrupt_sta(void)
{
    return rom_hw_lpio_get_interrupt_status();
}
