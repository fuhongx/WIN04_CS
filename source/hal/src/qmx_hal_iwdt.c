/**
 * ****************************************************************************
 * @file qmx_hal_iwdt.c
 * @author QMX software team.
 * @brief
 * @version  1.0
 * ****************************************************************************
 *
 * THIS CODE IS LIMITED TO GUIDE THE APPLICATION AND IMPROVE THE EFFICIENCY OF RESEARCH AND DEVELOPMENT.
 * AS A RESULT, QMX IS NOT RESPONSIBLE FOR ANY CLAIMS ARISING DIRECTLY OR INDIRECTLY FROM SUCH FIRMWARE AND/OR CONTENT.
 *
 * Copyright (c) 2026 QMX Semiconductor all rights reserved.
 *
 * ****************************************************************************
 */
#include "qmx_hal_iwdt.h"
#include "reg_Dev.h"
#include "error_def.h"
#include "hw_wdt.h"

void qmx_hal_iwdt_init(hal_iwdt_cfg_t *cfg)
{
    stWdtInit_t stInit;
    stInit.enFeedMode = (EN_WDT_FEED_MODE_T)cfg->feed_mode;
    stInit.enRstMode = (EN_WDT_RST_MODE_T)cfg->rst_mode;
    stInit.u32Counter = cfg->timeout_cnt;
    stInit.u32IwdtDiv = cfg->clk_div;
    rom_hw_wdt_init(IWDT, &stInit);
}

void qmx_hal_iwdt_deinit(void)
{
    rom_hw_wdt_deinit(IWDT);
}

void qmx_hal_iwdt_feed(void)
{
    rom_hw_iwdt_feed_dog();
}

void qmx_hal_iwdt_clear_interrupt(uint32_t int_mask)
{
    rom_hw_iwdt_clear_interrupt_flag(int_mask);
}

uint32_t qmx_hal_iwdt_get_interrupt_status(void)
{
    uint8_t sta;
    rom_hw_iwdt_get_interrup_flag(&sta);
    return sta;
}
