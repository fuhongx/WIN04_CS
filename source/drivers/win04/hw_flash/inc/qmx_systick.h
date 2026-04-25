/**
 * ****************************************************************************
 * @file qmx_systick.h
 * @author QMX software team.
 * @brief
 * @version  1.0
 * ****************************************************************************
 *
 * THIS CODE IS LIMITED TO GUIDE THE APPLICATION AND IMPROVE THE EFFICIENCY OF RESEARCH AND DEVELOPMENT.
 * AS A RESULT, QMX IS NOT RESPONSIBLE FOR ANY CLAIMS ARISING DIRECTLY OR INDIRECTLY FROM SUCH FIRMWARE AND/OR CONTENT.
 *
 * Copyright (c) 2025 QMX Semiconductor all rights reserved.
 *
 * ****************************************************************************
 */

#ifndef __QMX_SYSTICK_H__
#define __QMX_SYSTICK_H__

void qmx_systick_delay_us(uint32_t us);
void qmx_systick_delay_ms(uint32_t ms);
void qmx_systick_delay_s(uint32_t s);

void qmx_systick_enable_interrupt(void);
void qmx_systick_disable_interrupt(void);

#endif
