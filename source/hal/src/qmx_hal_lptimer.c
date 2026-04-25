/**
 * ****************************************************************************
 * @file qmx_hal_lptimer.c
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
#include "qmx_hal_lptimer.h"
#include "reg_Dev.h"
#include "error_def.h"
#include "utility.h"
#include "hw_timer.h"

static stTimerHandle_t *get_lptimer_handle(hal_lptimer_id_e lptimer_id)
{
    switch (lptimer_id) {
        case HAL_LPTIMER0:
            return LPTIM0;


        case HAL_LPTIMER1:
            return LPTIM1;

        default:
            break;
    }

    return NULL;
}

void qmx_hal_lptimer_stop(hal_lptimer_id_e lptimer_id)
{
    stTimerHandle_t *timer = get_lptimer_handle(lptimer_id);

    if (timer == NULL) {
        return;
    }

    rom_hw_timer_stop_counter(timer);
}

void qmx_hal_lptimer_start(hal_lptimer_id_e lptimer_id)
{
    stTimerHandle_t *timer = get_lptimer_handle(lptimer_id);

    if (timer == NULL) {
        return;
    }

    rom_hw_timer_start_counter(timer);
}

uint32_t qmx_hal_lptimer_get_count(hal_lptimer_id_e lptimer_id)
{
    uint32_t cnt = 0;
    stTimerHandle_t *timer = get_lptimer_handle(lptimer_id);

    if (timer == NULL) {
        return 0;
    }

    rom_hw_timer_get_counter(timer, &cnt);
    return cnt;
}

void qmx_hal_lptimer_init(hal_lptimer_id_e lptimer_id, uint32_t count, bool enable_irq)
{
    stTimerHandle_t *timer = get_lptimer_handle(lptimer_id);

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

void qmx_hal_lptimer_enable_irq(hal_lptimer_id_e lptimer_id)
{
    stTimerHandle_t *timer = get_lptimer_handle(lptimer_id);

    if (timer == NULL) {
        return;
    }

    rom_hw_timer_set_interrupt_mask(timer, false);
    rom_hw_timer_enable_interrupt(timer);
}

void qmx_hal_lptimer_disable_irq(hal_lptimer_id_e lptimer_id)
{
    stTimerHandle_t *timer = get_lptimer_handle(lptimer_id);

    if (timer == NULL) {
        return;
    }

    rom_hw_timer_set_interrupt_mask(timer, true);
    rom_hw_timer_disable_interrupt(timer);
}

uint32_t qmx_hal_lptimer_get_irq_status(hal_lptimer_id_e lptimer_id)
{
    stTimerHandle_t *timer = get_lptimer_handle(lptimer_id);
    uint8_t sta = 0;

    if (timer == NULL) {
        return 0;
    }

    rom_hw_timer_get_interrupt_flag(timer, &sta);

    return sta;
}

void qmx_hal_lptimer_clear_irq(hal_lptimer_id_e lptimer_id, uint32_t irq_status)
{
    stTimerHandle_t *timer = get_lptimer_handle(lptimer_id);

    if (timer == NULL) {
        return;
    }

    rom_hw_timer_clear_interrupt_flag(timer);
}
