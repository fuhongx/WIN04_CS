/**
 * ****************************************************************************
 * @file qmx_debug_ila.c
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

#include "qmx_debug_ila.h"

void qmx_debug_ila_enable_clk_gate(bool enable)
{
    if (enable)
        SYS_CTRL->AHB_CLK_CFG |= QMX_ILA_CLK_GATE_MASK;
    else
        SYS_CTRL->AHB_CLK_CFG &= ~QMX_ILA_CLK_GATE_MASK;
}

void qmx_debug_ila_clk_reset(void)
{
    SYS_CTRL->AHB_SW_RST |= QMX_ILA_CLK_RST_MASK;
    SYS_CTRL->AHB_SW_RST &= ~QMX_ILA_CLK_RST_MASK;
}

void qmx_debug_ila_interrupt_enable(bool enable)
{
    if (enable)
        ILA->ILA_IER |= QMX_ILA_INT_EN_MASK;
    else
        ILA->ILA_IER &= ~QMX_ILA_INT_EN_MASK;
}

uint32_t qmx_debug_ila_get_status(void)
{
    return ILA->ILA_ISR;
}

void qmx_debug_ila_clear_status(void)
{
    ILA->ILA_ISR = 0xFFFFFFFF;
}

void qmx_debug_ila_set_mem(uint32_t addr, uint8_t data_format, uint16_t depth)
{
    ILA->ILA_MCR = QMX_ILA_MEM_ADDR_VAL(addr >> 2) | QMX_ILA_MEM_FORMAT_VAL(data_format);

    ILA->ILA_SCR = QMX_ILA_SAMPLE_NUM_VAL(depth-1);
}

void qmx_debug_ila_set_trigger_src(uint8_t clk_src, uint8_t trigger_src, uint8_t data_src)
{
    ILA->ILA_SSR = QMX_ILA_CLK_SEL_VAL(clk_src) |
                   QMX_ILA_TRIG_SEL_VAL(trigger_src) |
                   QMX_ILA_DATA_SEL_VAL(data_src);
}

void qmx_debug_ila_set_trigger_mode(uint8_t trigger_delay, uint8_t trigger_type)
{
    ILA->ILA_TCR = QMX_ILA_TRIG_DELAY_VAL(trigger_delay) |
                   QMX_ILA_TRIG_TYPE_VAL(trigger_type);
}

void qmx_debug_ila_start(bool enable)
{
    if (enable)
        ILA->ILA_RCR |= QMX_ILA_START_RUN_MASK;
    else
        ILA->ILA_RCR &= ~QMX_ILA_START_RUN_MASK;
}

void qmx_debug_ila_init(ila_cfg_t *ila_cfg)
{
    uint8_t data_format[ILA_MAX_DATA_SRC] =
        {ILA_DATA_32BIT, ILA_DATA_16BIT, ILA_DATA_16BIT, ILA_DATA_32BIT, ILA_DATA_16BIT};
    uint32_t mem_depth = 0;

    if (ila_cfg->data_src >= ILA_MAX_DATA_SRC || ila_cfg->clk_src > ILA_CLK_PHY_FCLK ||
        ila_cfg->trigger_type > ILA_TRIG_MANUAL || ila_cfg->trigger_type >= ILA_MAX_TRIG_SRC)
        return;

    if (data_format[ila_cfg->data_src] == ILA_DATA_32BIT)
        mem_depth = ila_cfg->size / 4;
    else
        mem_depth = ila_cfg->size / 2;

    qmx_debug_ila_enable_clk_gate(true);
    qmx_debug_ila_clk_reset();

    qmx_debug_ila_set_trigger_src(ila_cfg->clk_src, ila_cfg->trigger_src, ila_cfg->data_src);
    qmx_debug_ila_set_trigger_mode(ila_cfg->trigger_delay, ila_cfg->trigger_type);
    qmx_debug_ila_set_mem(ila_cfg->addr, data_format[ila_cfg->data_src], mem_depth);
}
