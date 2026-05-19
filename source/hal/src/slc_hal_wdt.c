/**
 * ****************************************************************************
 * @file slc_hal_wdt.c
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
#include "slc_hal_wdt.h"
#include "reg_Dev.h"
#include "hw_wdt.h"

void slc_hal_wdt_init(hal_wdt_cfg_t *cfg)
{
    stWdtInit_t stInit;
    stInit.enFeedMode = (EN_WDT_FEED_MODE_T)cfg->feed_mode;
    stInit.enRstMode = (EN_WDT_RST_MODE_T)cfg->rst_mode;
    stInit.u32Counter = cfg->timeout_cnt;
    stInit.u32IwdtDiv = 0;
    rom_hw_wdt_init(WDT, &stInit);
}

void slc_hal_wdt_deinit(void)
{
    rom_hw_wdt_deinit(WDT);
}

void slc_hal_wdt_feed(void)
{
    rom_hw_wdt_feed_dog();
}

void slc_hal_wdt_clear_interrupt(uint32_t int_mask)
{
    rom_hw_wdt_clear_interrupt_flag(int_mask);
}

uint32_t slc_hal_wdt_get_interrupt_status(void)
{
    uint8_t sta;
    rom_hw_wdt_get_interrup_flag(&sta);
    return sta;
}
