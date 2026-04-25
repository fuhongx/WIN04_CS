/**
 * ****************************************************************************
 * @file qmx_debug_ila.h
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

#ifndef __QMX_DEBUG_ILA_H__
#define __QMX_DEBUG_ILA_H__

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include "reg_Dev.h"
#include "error_def.h"
#include "utility.h"

#define QMX_ILA_CLK_GATE_MASK   (0x1 << 6)
#define QMX_ILA_CLK_RST_MASK    (0x1 << 6)

enum ila_data_format
{
    ILA_DATA_32BIT = 0,
    ILA_DATA_16BIT,
};

/**
 * 编号         采样源            时钟           速率    位宽    写入格式
 *  0   ILA_RF_ADC_DATA_9BIT    clk25m          25M     9x2     32b
 *  1   ILA_RF_ADC_DATA_8BIT    clk25m	        25M     8x2     16b
 *  2   ILA_RF_DAC_DATA         clk25m          25M     4x2     16b
 *  3   ILA_PHY_DBG_DATA        clk25m/12.5m     ?      14x2    32b
 *  4   ILA_DFE_DATA            clk25m          5*BW    5x2     16b
 */
enum ila_data_src
{
    ILA_RF_ADC_DATA_9BIT = 0,
    ILA_RF_ADC_DATA_8BIT,
    ILA_RF_DAC_DATA,
    ILA_PHY_DBG_DATA,
    ILA_DFE_DATA,
    ILA_MAX_DATA_SRC,
};

enum ila_trigger_src
{
    ILA_TRIG_PHY_BUSY = 0,
    ILA_TRIG_PHY_IRQ0,
    ILA_TRIG_PHY_IRQ1,
    ILA_TRIG_PHY,
    ILA_MAX_TRIG_SRC,
};

enum ila_clk_src
{
    ILA_CLK_XTAL25M = 0,
    ILA_CLK_PHY_FCLK,
};

enum ila_trig_type
{
    ILA_TRIG_EXTERNAL = 0,
    ILA_TRIG_MANUAL,
};

typedef struct
{
    uint32_t addr;
    uint16_t size;
    uint8_t clk_src;
    uint8_t trigger_src;
    uint8_t data_src;
    uint8_t trigger_delay;
    uint8_t trigger_type;
} ila_cfg_t;

void qmx_debug_ila_enable_clk_gate(bool enable);
void qmx_debug_ila_clk_reset(void);
void qmx_debug_ila_interrupt_enable(bool enable);
uint32_t qmx_debug_ila_get_status(void);
void qmx_debug_ila_clear_status(void);
void qmx_debug_ila_set_mem(uint32_t addr, uint8_t data_format, uint16_t depth);
void qmx_debug_ila_set_trigger_src(uint8_t clk_src, uint8_t trigger_src, uint8_t data_src);
void qmx_debug_ila_set_trigger_mode(uint8_t trigger_delay, uint8_t trigger_type);
void qmx_debug_ila_start(bool enable);

void qmx_debug_ila_init(ila_cfg_t *ila_cfg);

#endif
