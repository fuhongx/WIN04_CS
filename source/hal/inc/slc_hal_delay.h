/**
 * ****************************************************************************
 * @file slc_hal_delay.h
 * @author SLC software team.
 * @brief
 * @version  1.0
 * ****************************************************************************
 *
 * THIS CODE IS LIMITED TO GUIDE APPLICATION DEVELOPMENT AND INCREASE DEVELOPMENT SPEED.
 * AS A RESULT, SLC IS NOT RESPONSIBLE FOR ANY CLAIMS ARISING DIRECTLY OR INDIRECTLY FROM SUCH FIRMWARE AND/OR CONTENT.
 *
 * Copyright (c) 2026 SLC Semiconductor all rights reserved.
 *
 * ****************************************************************************
 */

#ifndef __SLC_HAL_DELAY_H__
#define __SLC_HAL_DELAY_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "utility.h"
#include "hw_systick.h"

/* 以下延时函数都为阻塞式，如需非阻塞的延时函数可自己实现 */
/* delay by nop */
#define slc_hal_nop_delay_us(us)        (rom_utility_delay_us(us))
#define slc_hal_nop_delay_ms(ms)        (rom_utility_delay_ms(ms))
#define slc_hal_nop_delay_s(s)          (rom_utility_delay_s(s))

/* delay by systick */
#define slc_hal_systick_delay_us(us)    (rom_hw_systick_delay_us(us))
#define slc_hal_systick_delay_ms(ms)    (rom_hw_systick_delay_ms(ms))
#define slc_hal_systick_delay_s(s)      (rom_hw_systick_delay_s(s))
#define slc_hal_systick_enable_irq()    (rom_hw_systick_enable_interrupt())
#define slc_hal_systick_disable_irq()   (rom_hw_systick_disable_interrupt())

#ifdef __cplusplus
}
#endif

#endif  /* __SLC_HAL_DELAY_H__ */
