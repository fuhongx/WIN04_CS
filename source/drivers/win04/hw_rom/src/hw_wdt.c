/**
 * ****************************************************************************
 * @file hw_wdt.c
 * @author SLC(SC) software team.
 * @brief
 * @version  1.0
 * ****************************************************************************
 *
 * THIS CODE IS LIMITED TO GUIDE THE APPLICATION AND IMPROVE THE EFFICIENCY OF RESEARCH AND DEVELOPMENT.
 * AS A RESULT, SILICONDUCTOR IS NOT RESPONSIBLE FOR ANY CLAIMS ARISING DIRECTLY OR INDIRECTLY FROM SUCH FIRMWARE AND/OR CONTENT.
 *
 * Copyright (c) 2025 Siliconductor Semiconductor all rights reserved.
 *
 * ****************************************************************************
 */

#include "utility.h"
#include "hw_wdt.h"
#include "hw_sysctrl.h"

EN_ERR_STA_T rom_hw_wdt_enable_write(bool bEnable)
{
    WDT->WDT_WPR = bEnable;

    return EN_ERROR_STA_OK;
}

EN_ERR_STA_T rom_hw_wdt_set_feed_mode(EN_WDT_FEED_MODE_T enMode)
{
    uint8_t u8Cfg = WDT->WDT_SCR;
    u8Cfg &= ~(WDT_SCR_FEED_MODE_MASK << WDT_SCR_FEED_MODE_SHIFT);
    u8Cfg |= ((enMode & WDT_SCR_FEED_MODE_MASK)<< WDT_SCR_FEED_MODE_SHIFT);
    WDT->WDT_SCR = u8Cfg & WDT_SCR_REG_MASK;
    return EN_ERROR_STA_OK;
}

EN_ERR_STA_T rom_hw_wdt_set_reset_mode(EN_WDT_RST_MODE_T enMode)
{
    uint8_t u8Cfg = WDT->WDT_SCR;
    u8Cfg &= ~(WDT_SCR_RST_EN_MASK << WDT_SCR_RST_EN_SHIFT);
    u8Cfg |= ((enMode & WDT_SCR_RST_EN_MASK)<< WDT_SCR_RST_EN_SHIFT);
    WDT->WDT_SCR = u8Cfg & WDT_SCR_REG_MASK;
    return EN_ERROR_STA_OK;
}

EN_ERR_STA_T rom_hw_wdt_set_counter(uint32_t u32Counter)
{
    WDT->WDT_LDR = u32Counter;
    WDT->WDT_CDR = u32Counter;
    return EN_ERROR_STA_OK;
}

EN_ERR_STA_T rom_hw_wdt_get_counter(uint32_t *pu32Counter)
{
    if(NULL == pu32Counter)
    {
        return EN_ERROR_STA_INVALID;
    }

    *pu32Counter = WDT->WDT_CDR;

    return EN_ERROR_STA_OK;
}

EN_ERR_STA_T rom_hw_wdt_set_interrupt_mask(uint8_t u8Mask)
{
    WDT->WDT_IMR = u8Mask & WDT_IMR_REG_MASK;
    return EN_ERROR_STA_OK;
}

EN_ERR_STA_T rom_hw_wdt_enable_interrupt(uint8_t u8Int)
{
    WDT->WDT_IER = u8Int & WDT_IER_REG_MASK;
    return EN_ERROR_STA_OK;
}

EN_ERR_STA_T rom_hw_wdt_disable_interrupt(uint8_t u8Int)
{
    WDT->WDT_IER &= ~(u8Int & WDT_IER_REG_MASK);
    return EN_ERROR_STA_OK;
}

EN_ERR_STA_T rom_hw_wdt_get_interrup_flag(uint8_t *pu8Flag)
{
    if(NULL == pu8Flag)
    {
        return EN_ERROR_STA_INVALID;
    }

    *pu8Flag = WDT->WDT_ISR;

    return EN_ERROR_STA_OK;
}

EN_ERR_STA_T rom_hw_wdt_clear_interrupt_flag(uint8_t u8Flag)
{
    uint8_t u8Cfg = WDT->WDT_ISR;
    WDT->WDT_ISR = u8Flag & u8Cfg;
    return EN_ERROR_STA_OK;
}

EN_ERR_STA_T rom_hw_wdt_feed_dog(void)
{
    WDT->WDT_FDR = IWDT_FDR_WRITE_VAL;
    return EN_ERROR_STA_OK;
}

//------------------------------------------------------------------------------
//IWDT Functions
//------------------------------------------------------------------------------
EN_ERR_STA_T rom_hw_iwdt_enable_write(bool bEnable)
{
    IWDT->WDT_WPR = bEnable;
    rom_utility_delay_us(LP_RW_DELAY);

    return EN_ERROR_STA_OK;
}

EN_ERR_STA_T rom_hw_iwdt_set_feed_mode(EN_WDT_FEED_MODE_T enMode)
{
    uint8_t u8Cfg = IWDT->WDT_SCR;
    rom_utility_delay_us(LP_RW_DELAY);
    u8Cfg &= ~(WDT_SCR_FEED_MODE_MASK << WDT_SCR_FEED_MODE_SHIFT);
    u8Cfg |= ((enMode & WDT_SCR_FEED_MODE_MASK)<< WDT_SCR_FEED_MODE_SHIFT);
    IWDT->WDT_SCR = u8Cfg & WDT_SCR_REG_MASK;
    return EN_ERROR_STA_OK;
}

EN_ERR_STA_T rom_hw_iwdt_set_reset_mode(EN_WDT_RST_MODE_T enMode)
{
    uint8_t u8Cfg = IWDT->WDT_SCR;
    rom_utility_delay_us(LP_RW_DELAY);
    u8Cfg &= ~(WDT_SCR_RST_EN_MASK << WDT_SCR_RST_EN_SHIFT);
    u8Cfg |= ((enMode & WDT_SCR_RST_EN_MASK)<< WDT_SCR_RST_EN_SHIFT);
    IWDT->WDT_SCR = u8Cfg & WDT_SCR_REG_MASK;
    return EN_ERROR_STA_OK;
}

EN_ERR_STA_T rom_hw_iwdt_set_counter(uint32_t u32Counter)
{
    IWDT->WDT_LDR = u32Counter;
    IWDT->WDT_CDR = u32Counter;
    return EN_ERROR_STA_OK;
}

EN_ERR_STA_T rom_hw_iwdt_get_counter(uint32_t *pu32Counter)
{
    if(NULL == pu32Counter)
    {
        return EN_ERROR_STA_INVALID;
    }

    *pu32Counter = IWDT->WDT_CDR;

    return EN_ERROR_STA_OK;
}

EN_ERR_STA_T rom_hw_iwdt_set_interrupt_mask(uint8_t u8Mask)
{
    IWDT->WDT_IMR = u8Mask & WDT_IMR_REG_MASK;
    return EN_ERROR_STA_OK;
}

EN_ERR_STA_T rom_hw_iwdt_enable_interrupt(uint8_t u8Int)
{
    IWDT->WDT_IER = u8Int & WDT_IER_REG_MASK;
    return EN_ERROR_STA_OK;
}

EN_ERR_STA_T rom_hw_iwdt_disable_interrupt(uint8_t u8Int)
{
    IWDT->WDT_IER &= ~(u8Int & WDT_IER_REG_MASK);
    return EN_ERROR_STA_OK;
}

EN_ERR_STA_T rom_hw_iwdt_get_interrup_flag(uint8_t *pu8Flag)
{
    if(NULL == pu8Flag)
    {
        return EN_ERROR_STA_INVALID;
    }

    *pu8Flag = IWDT->WDT_ISR;

    return EN_ERROR_STA_OK;
}

EN_ERR_STA_T rom_hw_iwdt_clear_interrupt_flag(uint8_t u8Flag)
{
    uint8_t u8Cfg = IWDT->WDT_ISR;
    IWDT->WDT_ISR = u8Flag & u8Cfg;
    return EN_ERROR_STA_OK;
}

EN_ERR_STA_T rom_hw_iwdt_feed_dog(void)
{
    IWDT->WDT_FDR = IWDT_FDR_WRITE_VAL;
    return EN_ERROR_STA_OK;
}

EN_ERR_STA_T rom_hw_iwdt_set_clock_division(uint32_t u32Div)
{
    IWDT->IWDT_DLL = u32Div;
    return EN_ERROR_STA_OK;
}

EN_ERR_STA_T rom_hw_iwdt_timer_enable(bool bEnable)
{
    IWDT->IWDT_TER = bEnable;
    return EN_ERROR_STA_OK;
}

EN_ERR_STA_T rom_hw_wdt_init(stWdtHandle_t *pstHandle, stWdtInit_t *pstInit)
{
    if((NULL == pstHandle) | (NULL == pstInit))
    {
        return EN_ERROR_STA_INVALID;
    }

    if(WDT == pstHandle)
    {
        //Init WDT
        rom_hw_wdt_enable_write(false);
        rom_hw_wdt_set_counter(pstInit->u32Counter);
        rom_hw_wdt_set_feed_mode(pstInit->enFeedMode);
        rom_hw_wdt_set_reset_mode(pstInit->enRstMode);

        if(EN_WDT_FEED_MODE_FEED == pstInit->enFeedMode) {
            rom_hw_wdt_feed_dog();
        } else {
            rom_hw_wdt_enable_interrupt(EN_WDT_INT_MATCH);
            rom_hw_wdt_set_interrupt_mask(0);
        }
        rom_hw_wdt_enable_write(true);
    }
    else
    {
        rom_hw_iwdt_enable_write(false);
        rom_hw_iwdt_set_counter(pstInit->u32Counter);
        rom_hw_iwdt_set_reset_mode(pstInit->enRstMode);
        rom_hw_iwdt_set_feed_mode(pstInit->enFeedMode);
        rom_hw_iwdt_set_clock_division(pstInit->u32IwdtDiv);


        if(EN_WDT_FEED_MODE_FEED == pstInit->enFeedMode) {
            rom_hw_iwdt_feed_dog();
        } else {
            rom_hw_iwdt_enable_interrupt(EN_WDT_INT_MATCH);
            rom_hw_iwdt_set_interrupt_mask(0);
        }
        rom_hw_iwdt_timer_enable(true);
        rom_hw_iwdt_enable_write(true);
    }

    return EN_ERROR_STA_OK;
}

EN_ERR_STA_T rom_hw_wdt_deinit(stWdtHandle_t *pstHandle)
{
    if(NULL == pstHandle)
    {
        return EN_ERROR_STA_INVALID;
    }

    if(WDT == pstHandle)
    {
        rom_hw_wdt_enable_write(false);
        rom_hw_wdt_set_reset_mode(EN_WDT_RST_MODE_NONE);
        rom_hw_wdt_set_feed_mode(EN_WDT_FEED_MODE_FEED);

        rom_hw_wdt_disable_interrupt(WDT_IER_REG_MASK);
        rom_hw_wdt_enable_write(true);
    }
    else
    {
        rom_hw_iwdt_enable_write(false);
        rom_hw_iwdt_timer_enable(false);
        rom_hw_iwdt_set_reset_mode(EN_WDT_RST_MODE_NONE);
        rom_hw_iwdt_set_feed_mode(EN_WDT_FEED_MODE_FEED);

        rom_hw_iwdt_disable_interrupt(WDT_IER_REG_MASK);
        rom_hw_iwdt_enable_write(true);
    }

    return EN_ERROR_STA_OK;
}


