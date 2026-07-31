/**
 * ****************************************************************************
 * @file slc_systick.h
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

#ifndef __HW_SYSTICK_H__
#define __HW_SYSTICK_H__

void rom_hw_systick_delay_us(uint32_t us);
void rom_hw_systick_delay_ms(uint32_t ms);
void rom_hw_systick_delay_s(uint32_t s);

void rom_hw_systick_enable_interrupt(void);
void rom_hw_systick_disable_interrupt(void);

#endif
