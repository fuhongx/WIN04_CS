/**************************************************************************//**
 * @file     system_ARMCM0plus.c
 * @brief    CMSIS Device System Source File for
 *           ARMCM0plus Device
 * @version  V2.0.0
 * @date     06. April 2023
 ******************************************************************************/
/*
 * Copyright (c) 2009-2023 Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

//------------------------------------------------------------------------------
//This file only can use in example project. Don't use this file in Rom lib.
//------------------------------------------------------------------------------

#include "ARMCM0plus.h"

#if defined(__VTOR_PRESENT) && (__VTOR_PRESENT == 1U)
extern uint32_t __VECTOR_TABLE;
#endif

/**
 * Local Variables
 */
#define MP_REMAP_ENABLE  1
#define PERI_IRQ_NUM_MAX 32

uint32_t __NEW_VECTOR_TABLE[PERI_IRQ_NUM_MAX + 16] __attribute__((aligned(512), section(".bss.new_vector")));

#include "reg_Dev.h"
#include "error_def.h"
#include "app_cfg.h"

/**
 * @brief  System initialization function.
 */
void SystemInit(void)
{

    uint32_t           u32VectorIndex  = 0;
    volatile uint32_t *pu32VectorTable = 0;

    pu32VectorTable = (volatile uint32_t *)&__Vectors;

    for (u32VectorIndex = 0; u32VectorIndex < PERI_IRQ_NUM_MAX + 16; u32VectorIndex++) //+16:internal irq
    {
        __NEW_VECTOR_TABLE[u32VectorIndex] = pu32VectorTable[u32VectorIndex];
    }

    SCB->VTOR = (uint32_t)__NEW_VECTOR_TABLE;

}
