/**
 * ****************************************************************************
 * @file qmx_hal_intc.c
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
#include "qmx_hal_intc.h"
#include "reg_Dev.h"
#include "qmx_intc.h"

void qmx_hal_intc_init(void)
{
    qmx_intc_init();
}

void qmx_hal_register_irq_handler(IRQn_Type irq_num, hal_irq_handler_t handler)
{
    qmx_register_irq_handler(irq_num, handler);
}

void qmx_hal_unregister_irq_handler(IRQn_Type irq_num)
{
    qmx_unregister_irq_handler(irq_num);
}
