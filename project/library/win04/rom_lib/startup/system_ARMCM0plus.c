/******************************************************************************
 * @file     system_ARMCM0plus.c
 * @brief    CMSIS Device System Source File for
 *           ARMCM0plus Device
 * @version  V1.0.0
 * @date     09. July 2018
 *****************************************************************************/
/*
 * Copyright (c) 2009-2018 Arm Limited. All rights reserved.
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

// this file has been modified

#include "ARMCM0plus.h"
#include "hw_sysctrl.h"

/**
 * @brief  System Core Clock update function.
 */
void SystemCoreClockUpdate(void)
{

}

/**
 * @brief  System initialization function.
 */
void SystemInit(void)
{
    rom_hw_sysctrl_system_clock_init(EN_SYS_CLK_RC50M, EN_SYS_CLK_MCLK_DIV_NONE);
}
