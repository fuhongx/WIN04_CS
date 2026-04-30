/**
 * ****************************************************************************
 * @file qmx_hal_delay.h
 * @author QMX software team.
 * @brief
 * @version  1.0
 * ****************************************************************************
 *
 * THIS CODE IS LIMITED TO GUIDE APPLICATION DEVELOPMENT AND INCREASE DEVELOPMENT SPEED.
 * AS A RESULT, QMX IS NOT RESPONSIBLE FOR ANY CLAIMS ARISING DIRECTLY OR INDIRECTLY FROM SUCH FIRMWARE AND/OR CONTENT.
 *
 * Copyright (c) 2026 QMX Semiconductor all rights reserved.
 *
 * ****************************************************************************
 */

#ifndef __QMX_HAL_DELAY_H__
#define __QMX_HAL_DELAY_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "utility.h"
#include "hw_systick.h"

/* 以下延时函数都为阻塞式，如需非阻塞的延时函数可自己实现 */
/* delay by nop */
#define qmx_hal_nop_delay_us(us)        (rom_utility_delay_us(us))
#define qmx_hal_nop_delay_ms(ms)        (rom_utility_delay_ms(ms))
#define qmx_hal_nop_delay_s(s)          (rom_utility_delay_s(s))

/* delay by systick */
#define qmx_hal_systick_delay_us(us)    (rom_hw_systick_delay_us(us))
#define qmx_hal_systick_delay_ms(ms)    (rom_hw_systick_delay_ms(ms))
#define qmx_hal_systick_delay_s(s)      (rom_hw_systick_delay_s(s))
#define qmx_hal_systick_enable_irq()    (rom_hw_systick_enable_interrupt())
#define qmx_hal_systick_disable_irq()   (rom_hw_systick_disable_interrupt())

#ifdef __cplusplus
}
#endif

#endif  /* __QMX_HAL_DELAY_H__ */
