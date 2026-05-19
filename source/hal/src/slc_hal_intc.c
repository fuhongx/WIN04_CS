/**
 * ****************************************************************************
 * @file slc_hal_intc.c
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
#include "slc_hal_intc.h"
#include "reg_Dev.h"
#include "slc_intc.h"

void slc_hal_intc_init(void)
{
    slc_intc_init();
}

void slc_hal_register_irq_handler(IRQn_Type irq_num, hal_irq_handler_t handler)
{
    slc_register_irq_handler(irq_num, handler);
}

void slc_hal_unregister_irq_handler(IRQn_Type irq_num)
{
    slc_unregister_irq_handler(irq_num);
}
