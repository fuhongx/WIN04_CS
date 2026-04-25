/**
 * ****************************************************************************
 * @file qmx_hal_timer.c
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
#include "utility.h"
#include "error_def.h"
#include "reg_Dev.h"
#include "qmx_hal_timer.h"
#include "hw_timer.h"

static stTimerHandle_t *get_timer_handle(hal_timer_id_e timer_id)
{
    switch (timer_id) {
        case HAL_TIMER0:
            return TIM0;


        case HAL_TIMER1:
            return TIM1;

        case HAL_TIMER2:
            return TIM2;

        default:
            break;
    }

    return NULL;
}

void qmx_hal_timer_stop(hal_timer_id_e timer_id)
{
    stTimerHandle_t *timer = get_timer_handle(timer_id);

    if (timer == NULL) {
        return;
    }

    rom_hw_timer_stop_counter(timer);
}

void qmx_hal_timer_start(hal_timer_id_e timer_id)
{
    stTimerHandle_t *timer = get_timer_handle(timer_id);

    if (timer == NULL) {
        return;
    }

    rom_hw_timer_start_counter(timer);
}

uint32_t qmx_hal_timer_get_count(hal_timer_id_e timer_id)
{
    uint32_t cnt = 0;
    stTimerHandle_t *timer = get_timer_handle(timer_id);

    if (timer == NULL) {
        return 0;
    }

    rom_hw_timer_get_counter(timer, &cnt);
    return cnt;
}

void qmx_hal_timer_init(hal_timer_id_e timer_id, uint32_t count, bool enable_irq)
{
    stTimerHandle_t *timer = get_timer_handle(timer_id);

    if (timer == NULL) {
        return;
    }

    rom_hw_timer_stop_counter(timer);
    rom_hw_timer_set_conter_reload_value(timer, count);

    if (enable_irq) {
        rom_hw_timer_set_interrupt_mask(timer, false);
        rom_hw_timer_enable_interrupt(timer);
    } else {
        rom_hw_timer_set_interrupt_mask(timer, true);
        rom_hw_timer_disable_interrupt(timer);
    }
}

void qmx_hal_timer_enable_irq(hal_timer_id_e timer_id)
{
    stTimerHandle_t *timer = get_timer_handle(timer_id);

    if (timer == NULL) {
        return;
    }

    rom_hw_timer_set_interrupt_mask(timer, false);
    rom_hw_timer_enable_interrupt(timer);
}

void qmx_hal_timer_disable_irq(hal_timer_id_e timer_id)
{
    stTimerHandle_t *timer = get_timer_handle(timer_id);

    if (timer == NULL) {
        return;
    }

    rom_hw_timer_set_interrupt_mask(timer, true);
    rom_hw_timer_disable_interrupt(timer);
}

uint32_t qmx_hal_timer_get_irq_status(hal_timer_id_e timer_id)
{
    stTimerHandle_t *timer = get_timer_handle(timer_id);
    uint8_t sta = 0;

    if (timer == NULL) {
        return 0;
    }

    rom_hw_timer_get_interrupt_flag(timer, &sta);

    return sta;
}

void qmx_hal_timer_clear_irq(hal_timer_id_e timer_id, uint32_t irq_status)
{
    stTimerHandle_t *timer = get_timer_handle(timer_id);

    if (timer == NULL) {
        return;
    }

    rom_hw_timer_clear_interrupt_flag(timer);
}
