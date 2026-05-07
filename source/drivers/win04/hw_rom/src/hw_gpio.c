/**
 * ****************************************************************************
 * @file hw_gpio.c
 * @author SLC(SC) software team.
 * @brief
 * @version  1.0
 * ****************************************************************************
 *
 * Copyright (c) 2024 SLC Semiconductor all rights reserved.
 *
 * ****************************************************************************
 */

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>

#include "reg_Dev.h"
#include "error_def.h"
#include "utility.h"

#include "hw_gpio.h"

EN_ERR_STA_T rom_hw_gpio_get_input_level(uint32_t u32Pin, uint32_t *pu32Flag)
{
    if (NULL == pu32Flag)
    {
        return EN_ERROR_STA_INVALID;
    }

    uint32_t u32FlagTemp = GPIO->INPUT_LEVEL & GPIO_INPUT_EN_REG_MASK;

    *pu32Flag = ((u32FlagTemp & u32Pin) != 0) ? 1 : 0;

    return EN_ERROR_STA_OK;
}

EN_ERR_STA_T rom_hw_gpio_set_output_level(uint32_t u32Pin, EN_GPIO_OUTPUT_LEVEL_T enMode)
{

    if (EN_GPIO_OUTPUT_LEVEL_HI == enMode)
    {
        GPIO->OUTPUT_LEVEL |= u32Pin & GPIO_OUTPUT_LEVEL_REG_MASK;
    }
    else
    {
        GPIO->OUTPUT_LEVEL &= ~(u32Pin & GPIO_OUTPUT_LEVEL_REG_MASK);
    }

    return EN_ERROR_STA_OK;
}

EN_ERR_STA_T rom_hw_gpio_enable_output(uint32_t u32Pin, bool bEnable)
{
    if (bEnable)
    {
        GPIO->OUTPUT_EN |= u32Pin;
    }
    else
    {
        GPIO->OUTPUT_EN &= ~(u32Pin);
    }

    return EN_ERROR_STA_OK;
}

EN_ERR_STA_T rom_hw_gpio_enable_input(uint32_t u32Pin, bool bEnable)
{

    if (bEnable)
    {
        GPIO->INPUT_EN |= u32Pin & GPIO_INPUT_EN_REG_MASK;
    }
    else
    {
        GPIO->INPUT_EN &= ~(u32Pin & GPIO_INPUT_EN_REG_MASK);
    }

    return EN_ERROR_STA_OK;
}

EN_ERR_STA_T rom_hw_gpio_set_output_mode(uint32_t u32Pin, EN_GPIO_PIN_MODE_T enMode)
{
    if (enMode == EN_GPIO_PIN_MODE_INPUT)
    {
        // Disable output & Enable Input
        GPIO->OUTPUT_EN &= ~(u32Pin);
        GPIO->INPUT_EN |= u32Pin & GPIO_INPUT_EN_REG_MASK;
    }
    else
    {
        GPIO->INPUT_EN &= ~(u32Pin & GPIO_INPUT_EN_REG_MASK);
        GPIO->OUTPUT_EN |= u32Pin;
    }

    return EN_ERROR_STA_OK;
}

EN_ERR_STA_T rom_hw_gpio_enable_open_drain(uint32_t u32Pin, bool bEnable)
{
    if (bEnable)
    {
        GPIO->OD_EN |= u32Pin & GPIO_OD_EN_REG_MASK;
    }
    else
    {
        GPIO->OD_EN &= ~(u32Pin & GPIO_OD_EN_REG_MASK);
    }

    return EN_ERROR_STA_OK;
}

EN_ERR_STA_T rom_hw_gpio_enable_pull_mode(uint32_t u32Pin, bool bEnable)
{
    if (bEnable)
    {
        GPIO->PULL_EN |= u32Pin & GPIO_PULL_EN_REG_MASK;
    }
    else
    {
        GPIO->PULL_EN &= ~(u32Pin & GPIO_PULL_EN_REG_MASK);
    }
    return EN_ERROR_STA_OK;
}

EN_ERR_STA_T rom_hw_gpio_enable_iomux(uint32_t u32Pin, bool bEnable)
{
    if (bEnable)
    {
        // Set to 1
        GPIO->IO_MUX_EN |= u32Pin & GPIO_IO_MUX_EN_REG_MASK;
    }
    else
    {
        GPIO->IO_MUX_EN &= ~(u32Pin & GPIO_IO_MUX_EN_REG_MASK);
    }

    return EN_ERROR_STA_OK;
}

EN_ERR_STA_T rom_hw_gpio_set_iomux_mode(uint32_t u32Pin, EN_GPIO_IOMUX_MODE_T enMode)
{
    uint8_t i = 0;
    uint32_t u32Cfg = 0;

    for (; i < GPIO_MAX_PIN_NUM; i++)
    {
        // Find pin
        if (u32Pin & (0x1 << i))
        {
            if (i <= GPIO_IO_MUX0_MAX_PIN)
            {
                // 0~7
                u32Cfg = GPIO->IO_MUX0;
                u32Cfg &= ~(GPIO_IO_MUX_BIT_MASK << (i * GPIO_IO_MUX_BIT_WIDE));
                u32Cfg |= ((enMode & GPIO_IO_MUX_BIT_MASK) << (i * GPIO_IO_MUX_BIT_WIDE));
                GPIO->IO_MUX0 = u32Cfg;
            }
            else if ((i > GPIO_IO_MUX0_MAX_PIN) & (i <= GPIO_IO_MUX1_MAX_PIN))
            {
                // 8~15
                u32Cfg = GPIO->IO_MUX1;
                u32Cfg &= ~(GPIO_IO_MUX_BIT_MASK << ((i - 8) * GPIO_IO_MUX_BIT_WIDE));
                u32Cfg |= ((enMode & GPIO_IO_MUX_BIT_MASK) << ((i - 8) * GPIO_IO_MUX_BIT_WIDE));
                GPIO->IO_MUX1 = u32Cfg;
            }
            else
            {
                // 16 ~ 21
                u32Cfg = GPIO->IO_MUX2;
                u32Cfg &= ~(GPIO_IO_MUX_BIT_MASK << ((i - 16) * GPIO_IO_MUX_BIT_WIDE));
                u32Cfg |= ((enMode & GPIO_IO_MUX_BIT_MASK) << ((i - 16) * GPIO_IO_MUX_BIT_WIDE));
                GPIO->IO_MUX2 = u32Cfg;
            }
        }
    }

    return EN_ERROR_STA_OK;
}

EN_ERR_STA_T rom_hw_gpio_set_sample_divider(uint8_t u8Div)
{
    //(APB0_SYS_CLK / u8Div) + 1
    GPIO->GPIO_FCR = u8Div & GPIO_SAMPLE_DIV_REG_MASK;
    return EN_ERROR_STA_OK;
}

EN_ERR_STA_T rom_hw_gpio_enable_interrupt(uint32_t u32Pin)
{
    GPIO->INT_MASK |= u32Pin & GPIO_INT_MASK_REG_MASK;

    return EN_ERROR_STA_OK;
}

EN_ERR_STA_T rom_hw_gpio_disable_interrupt(uint32_t u32Pin)
{
    GPIO->INT_MASK &= ~(u32Pin & GPIO_INT_MASK_REG_MASK);

    return EN_ERROR_STA_OK;
}

EN_ERR_STA_T rom_hw_gpio_set_interrupt_mode(uint32_t u32Pin, EN_GPIO_INT_T enMode)
{
    switch (enMode)
    {
        case EN_GPIO_INT_LO_LEVEL:
        {

            GPIO->INT_TYPE |= u32Pin;
            GPIO->LEVEL_INT_SEL &= (~(u32Pin)&GPIO_INT_TYPE_REG_MASK);
            break;
        }

        case EN_GPIO_INT_HI_LEVEL:
        {
            GPIO->INT_TYPE |= u32Pin;
            GPIO->LEVEL_INT_SEL |= (u32Pin);
            break;
        }

        case EN_GPIO_INT_RISING_EDGE:
        {
            GPIO->INT_TYPE &= ((~u32Pin) & GPIO_INT_TYPE_REG_MASK);
            GPIO->RISE_EDGE_INT_EN |= u32Pin;
            break;
        }

        case EN_GPIO_INT_FALLING_EDGE:
        {
            GPIO->INT_TYPE &= ((~u32Pin) & GPIO_INT_TYPE_REG_MASK);
            GPIO->FALL_EDGE_INT_EN |= u32Pin;
            break;
        }

        case EN_GPIO_INT_BOTH_EDGE:
        {
            GPIO->INT_TYPE &= ((~u32Pin) & GPIO_INT_TYPE_REG_MASK);
            GPIO->RISE_EDGE_INT_EN |= u32Pin;
            GPIO->FALL_EDGE_INT_EN |= u32Pin;
            break;
        }

        default:
            break;
    }

    return EN_ERROR_STA_OK;
}

EN_ERR_STA_T rom_hw_gpio_clear_interrupt_mode(uint32_t u32Pin)
{
    // Return to default.
    GPIO->INT_MASK &= ~(u32Pin & GPIO_INT_MASK_REG_MASK);
    GPIO->INT_TYPE &= ((~u32Pin) & GPIO_INT_TYPE_REG_MASK);
    GPIO->LEVEL_INT_SEL &= (~(u32Pin)&GPIO_INT_TYPE_REG_MASK);
    GPIO->RISE_EDGE_INT_EN &= ((~u32Pin) & GPIO_RISE_EDGE_INT_EN_REG_MASK);
    GPIO->FALL_EDGE_INT_EN &= ((~u32Pin) & GPIO_FALL_EDGE_INT_EN_REG_MASK);
    return EN_ERROR_STA_OK;
}

EN_ERR_STA_T rom_hw_gpio_get_interrupt_flag(uint32_t *pu32Flag)
{
    if (NULL == pu32Flag)
    {
        return EN_ERROR_STA_INVALID;
    }

    *pu32Flag = GPIO->INT_FLAG;

    return EN_ERROR_STA_OK;
}

EN_ERR_STA_T rom_hw_gpio_clear_interrupt_flag(uint32_t u32Flag)
{
    uint32_t u32Cfg = GPIO->INT_FLAG;
    u32Cfg &= u32Flag;
    GPIO->INT_FLAG |= u32Cfg;
    return EN_ERROR_STA_OK;
}

EN_ERR_STA_T rom_hw_gpio_get_edge_interrupt_flag(uint32_t *u32Flag, bool bRisingFlag)
{
    if (bRisingFlag)
    {
        *u32Flag = GPIO->RISE_EDGE_INT_FLAG;
    }
    else
    {
        *u32Flag = GPIO->FALL_EDGE_INT_FLAG;
    }

    return EN_ERROR_STA_OK;
}

/* LPIO func */
EN_ERR_STA_T rom_hw_lpio_get_input_level(uint32_t u32Pin, uint32_t *pu32Flag)
{
    if (NULL == pu32Flag)
    {
        return EN_ERROR_STA_INVALID;
    }

    uint32_t u32FlagTemp = LPIO->INPUT_LEVEL & GPIO_INPUT_EN_REG_MASK;

    *pu32Flag = ((u32FlagTemp & u32Pin) != 0) ? 1 : 0;

    return EN_ERROR_STA_OK;
}

EN_ERR_STA_T rom_hw_lpio_set_output_level(uint32_t u32Pin, EN_GPIO_OUTPUT_LEVEL_T enMode)
{

    if (EN_GPIO_OUTPUT_LEVEL_HI == enMode)
    {
        LPIO->OUTPUT_LEVEL |= u32Pin & GPIO_OUTPUT_LEVEL_REG_MASK;
    }
    else
    {
        LPIO->OUTPUT_LEVEL &= ~(u32Pin & GPIO_OUTPUT_LEVEL_REG_MASK);
    }

    return EN_ERROR_STA_OK;
}

EN_ERR_STA_T rom_hw_lpio_set_pin_direction(uint32_t u32Pin, EN_GPIO_PIN_MODE_T enMode)
{
    if (enMode == EN_GPIO_PIN_MODE_INPUT)
    {
        // Disable output & Enable Input
        LPIO->OUTPUT_EN &= ~(u32Pin);
        LPIO->INPUT_EN |= u32Pin & GPIO_INPUT_EN_REG_MASK;
    }
    else
    {
        LPIO->INPUT_EN &= ~(u32Pin & GPIO_INPUT_EN_REG_MASK);
        LPIO->OUTPUT_EN |= u32Pin;
    }

    return EN_ERROR_STA_OK;
}

EN_ERR_STA_T rom_hw_lpio_enable_pull_mode(uint32_t u32Pin, bool bEnable)
{
    if (bEnable)
    {
        LPIO->PULL_EN |= u32Pin & GPIO_PULL_EN_REG_MASK;
    }
    else
    {
        LPIO->PULL_EN &= ~(u32Pin & GPIO_PULL_EN_REG_MASK);
    }
    return EN_ERROR_STA_OK;
}

EN_ERR_STA_T rom_hw_lpio_enable_iomux(uint32_t u32Pin, bool bEnable)
{
    if (bEnable)
    {
        // Set to 1
        LPIO->IO_MUX_EN |= u32Pin & GPIO_IO_MUX_EN_REG_MASK;
    }
    else
    {
        LPIO->IO_MUX_EN &= ~(u32Pin & GPIO_IO_MUX_EN_REG_MASK);
    }

    return EN_ERROR_STA_OK;
}

EN_ERR_STA_T rom_hw_lpio_enable_interrupt(uint32_t u32Pin)
{
    LPIO->INT_EN |= u32Pin & GPIO_INT_MASK_REG_MASK;

    return EN_ERROR_STA_OK;
}

EN_ERR_STA_T rom_hw_lpio_disable_interrupt(uint32_t u32Pin)
{
    LPIO->INT_EN &= ~(u32Pin & GPIO_INT_MASK_REG_MASK);

    return EN_ERROR_STA_OK;
}

EN_ERR_STA_T rom_hw_lpio_set_interrupt_mode(uint32_t u32Pin, EN_GPIO_INT_T enMode)
{
    switch (enMode)
    {
        case EN_GPIO_INT_LO_LEVEL:
        {
            LPIO->LEVEL_INT_SEL &= (~(u32Pin)&GPIO_INT_TYPE_REG_MASK);
            break;
        }

        case EN_GPIO_INT_HI_LEVEL:
        {
            LPIO->LEVEL_INT_SEL |= (u32Pin);
            break;
        }

        default:
            break;
    }

    return EN_ERROR_STA_OK;
}

void rom_hw_lpio_clear_interrupt(uint32_t sta)
{
    LPIO->INT_STA |= sta;
}

uint32_t rom_hw_lpio_get_interrupt_status(void)
{
    return LPIO->INT_STA;
}
