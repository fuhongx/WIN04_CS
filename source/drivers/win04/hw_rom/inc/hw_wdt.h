/**
 * ****************************************************************************
 * @file hw_wtd.h
 * @author SLC(SC) software team.
 * @brief
 * @version  1.0
 * ****************************************************************************
 *
 * Copyright (c) 2024 SLC Semiconductor all rights reserved.
 *
 * ****************************************************************************
 */

#ifndef __HW_WTD_H__
#define __HW_WTD_H__

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>

#include "reg_Dev.h"
#include "error_def.h"

typedef enum
{
    EN_WDT_RST_MODE_NONE        = 0,
    EN_WDT_RST_MODE_RST         = 1,
}EN_WDT_RST_MODE_T;

typedef enum
{
    EN_WDT_FEED_MODE_FEED          = 0,
    EN_WDT_FEED_MODE_CLR_INT       = 1,
}EN_WDT_FEED_MODE_T;

typedef enum
{
    EN_WDT_INT_MATCH            = WDT_IER_INT_MATCH_EN_MASK << WDT_IER_INT_MATCH_EN_SHIFT,
    EN_WDT_INT_WRITE_PROTECTION = WDT_IER_INT_WP_EN_MASK << WDT_IER_INT_WP_EN_SHIFT,
}EN_WDT_INT_T;

typedef struct
{
    EN_WDT_RST_MODE_T enRstMode;
    EN_WDT_FEED_MODE_T enFeedMode;
    uint32_t u32Counter;
    uint32_t u32IwdtDiv;
}stWdtInit_t;

EN_ERR_STA_T rom_hw_wdt_enable_write(bool bEnable);
EN_ERR_STA_T rom_hw_wdt_set_feed_mode(EN_WDT_FEED_MODE_T enMode);
EN_ERR_STA_T rom_hw_wdt_set_reset_mode(EN_WDT_RST_MODE_T enMode);
EN_ERR_STA_T rom_hw_wdt_set_counter(uint32_t u32Counter);
EN_ERR_STA_T rom_hw_wdt_get_counter(uint32_t *pu32Counter);
EN_ERR_STA_T rom_hw_wdt_set_interrupt_mask(uint8_t u8Mask);
EN_ERR_STA_T rom_hw_wdt_enable_interrupt(uint8_t u8Int);
EN_ERR_STA_T rom_hw_wdt_get_interrup_flag(uint8_t *pu8Flag);
EN_ERR_STA_T rom_hw_wdt_clear_interrupt_flag(uint8_t u8Flag);
EN_ERR_STA_T rom_hw_wdt_feed_dog(void);
EN_ERR_STA_T rom_hw_iwdt_enable_write(bool bEnable);
EN_ERR_STA_T rom_hw_iwdt_set_feed_mode(EN_WDT_FEED_MODE_T enMode);
EN_ERR_STA_T rom_hw_iwdt_set_reset_mode(EN_WDT_RST_MODE_T enMode);
EN_ERR_STA_T rom_hw_iwdt_set_counter(uint32_t u32Counter);
EN_ERR_STA_T rom_hw_iwdt_get_counter(uint32_t *pu32Counter);
EN_ERR_STA_T rom_hw_iwdt_set_interrupt_mask(uint8_t u8Mask);
EN_ERR_STA_T rom_hw_iwdt_enable_interrupt(uint8_t u8Int);
EN_ERR_STA_T rom_hw_iwdt_get_interrup_flag(uint8_t *pu8Flag);
EN_ERR_STA_T rom_hw_iwdt_clear_interrupt_flag(uint8_t u8Flag);
EN_ERR_STA_T rom_hw_iwdt_feed_dog(void);
EN_ERR_STA_T rom_hw_iwdt_set_clock_division(uint32_t u32Div);
EN_ERR_STA_T rom_hw_iwdt_timer_enable(bool bEnable);
EN_ERR_STA_T rom_hw_wdt_init(stWdtHandle_t *pstHandle, stWdtInit_t *pstInit);
EN_ERR_STA_T rom_hw_wdt_deinit(stWdtHandle_t *pstHandle);

#endif


