/**
 * ****************************************************************************
 * @file slc_intc.c
 * @author SLC software team.
 * @brief
 * @version  1.0
 * ****************************************************************************
 *
 * THIS CODE IS LIMITED TO GUIDE THE APPLICATION AND IMPROVE THE EFFICIENCY OF RESEARCH AND DEVELOPMENT.
 * AS A RESULT, SLC IS NOT RESPONSIBLE FOR ANY CLAIMS ARISING DIRECTLY OR INDIRECTLY FROM SUCH FIRMWARE AND/OR CONTENT.
 *
 * Copyright (c) 2025 SLC Semiconductor all rights reserved.
 *
 * ****************************************************************************
 */

#include <stdio.h>
#include <stdint.h>

#include "reg_Dev.h"
#include "slc_intc.h"

#define INTERNAL_IRQ_MAX    16
#define PERI_IRQ_NUM_MAX    32
#define MAX_IRQ_NUM         (INTERNAL_IRQ_MAX + PERI_IRQ_NUM_MAX)

#if defined(__VTOR_PRESENT) && (__VTOR_PRESENT == 1U)
extern uint32_t __VECTOR_TABLE;
#endif

uint32_t g_intc_irq_vector[MAX_IRQ_NUM] __attribute__((aligned(512), section(".bss.new_vector")));

void slc_intc_init(void)
{
    uint32_t           u32VectorIndex  = 0;
    volatile uint32_t *pu32VectorTable = 0;

    pu32VectorTable = (volatile uint32_t *)&__VECTOR_TABLE;
    // copy vector table to RAM, and remap to RAM
    for (u32VectorIndex = 0; u32VectorIndex < MAX_IRQ_NUM; u32VectorIndex++)
    {
        g_intc_irq_vector[u32VectorIndex] = pu32VectorTable[u32VectorIndex];
    }

    SCB->VTOR = (uint32_t)g_intc_irq_vector;
}

void slc_register_irq_handler(IRQn_Type irq_num, irq_handler_t handler)
{
    g_intc_irq_vector[irq_num + INTERNAL_IRQ_MAX] = (uint32_t)handler;
}

void slc_unregister_irq_handler(IRQn_Type irq_num)
{
    g_intc_irq_vector[irq_num + INTERNAL_IRQ_MAX] = (uint32_t)NULL;
}
