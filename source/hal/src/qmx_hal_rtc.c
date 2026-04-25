/**
 * ****************************************************************************
 * @file qmx_hal_rtc.c
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
#include "qmx_hal_rtc.h"
#include "reg_Dev.h"
#include "error_def.h"
#include "utility.h"
#include "hw_rtc.h"

static bool is_valid_bcd(uint8_t val)
{
    // 高4位和低4位都必须小于等于 9
    return ((val & 0xF0) <= 0x90) && ((val & 0x0F) <= 0x09);
}

static bool is_valid_bcd_16(uint16_t val)
{
    // 检查高字节和低字节是否都符合 BCD
    return is_valid_bcd((uint8_t)(val >> 8)) && is_valid_bcd((uint8_t)(val & 0xFF));
}

static int qmx_check_calendar_bcd(hal_rtc_calendar_t *calendar)
{
    if (calendar == NULL) {
        return -1;
    }

    // 检查秒 (00-59)
    if (!is_valid_bcd(calendar->second) || (calendar->second > 0x59)) {
        return -2;
    }

    // 检查分 (00-59)
    if (!is_valid_bcd(calendar->minute) || (calendar->minute > 0x59)) {
        return -2;
    }

    // 检查时 (00-23)
    if (!is_valid_bcd(calendar->hour) || (calendar->hour > 0x23)) {
        return -2;
    }

    // 检查周 (通常 1-7 或 0-6，这里为 0~6)
    if (calendar->week >= 0x07) {
        return -2;
    }

    // 检查日 (01-31)
    if (!is_valid_bcd(calendar->day) || (calendar->day > 0x31) || (calendar->day == 0x00)) {
        return -2;
    }

    // 检查月 (01-12)
    if (!is_valid_bcd(calendar->month) || (calendar->month > 0x12) || (calendar->month == 0x00)) {
        return -2;
    }

    // 检查年 (0000-9999)
    if (!is_valid_bcd_16(calendar->year)) {
        return -2;
    }

    return 0;
}

void qmx_hal_rtc_set_calendar(hal_rtc_calendar_t *calendar)
{
    if (qmx_check_calendar_bcd(calendar) != 0) {
        return;
    }

    stRtcTime_t rtc_time;
    rtc_time.u16Year = calendar->year;
    rtc_time.u8Month = calendar->month;
    rtc_time.u8Day = calendar->day;
    rtc_time.u8Week = calendar->week;
    rtc_time.u8Hour = calendar->hour;
    rtc_time.u8Minute = calendar->minute;
    rtc_time.u8Second = calendar->second;
    rtc_time.u16SubSecond = calendar->subsecond;

    rom_hw_rtc_set_time(&rtc_time);
}

void qmx_hal_rtc_get_calendar(hal_rtc_calendar_t *calendar)
{
    stRtcTime_t rtc_time;
    rom_hw_rtc_get_time(&rtc_time, EN_RTC_TIME_NOW);
    calendar->year = rtc_time.u16Year;
    calendar->month = rtc_time.u8Month;
    calendar->day = rtc_time.u8Day;
    calendar->week = rtc_time.u8Week;
    calendar->hour = rtc_time.u8Hour;
    calendar->minute = rtc_time.u8Minute;
    calendar->second = rtc_time.u8Second;
    calendar->subsecond = rtc_time.u16SubSecond;
}

void qmx_hal_rtc_stop(void)
{
    rom_hw_rtc_start_counter(false);
}

void qmx_hal_rtc_start(void)
{
    rom_hw_rtc_start_counter(true);
}

void qmx_hal_rtc_set_alarm(hal_rtc_alarm_id_e alarm_index, hal_rtc_alarm_msk_t *alarm, hal_rtc_calendar_t *calendar)
{
    if (qmx_check_calendar_bcd(calendar) != 0) {
        return;
    }

    if (alarm_index >= HAL_RTC_ALARM_MAX) {
        return;
    }

    uint8_t alarm_mask = 0;
    stRtcTime_t rtc_time;

    rtc_time.u16Year = calendar->year;
    rtc_time.u8Month = calendar->month;
    rtc_time.u8Day = calendar->day;
    rtc_time.u8Week = calendar->week;
    rtc_time.u8Hour = calendar->hour;
    rtc_time.u8Minute = calendar->minute;
    rtc_time.u8Second = calendar->second;
    rtc_time.u16SubSecond = calendar->subsecond;

    alarm_mask = (HAL_RTC_ALARM_YEAR_VAL(alarm->year_mask) | HAL_RTC_ALARM_MONTH_VAL(alarm->month_mask) | \
                  HAL_RTC_ALARM_DAY_VAL(alarm->day_mask) | HAL_RTC_ALARM_WEEK_VAL(alarm->week_mask) | \
                  HAL_RTC_ALARM_HOUR_VAL(alarm->hour_mask) | HAL_RTC_ALARM_MINUTE_VAL(alarm->minute_mask) | \
                  HAL_RTC_ALARM_SECOND_VAL(alarm->second_mask) | HAL_RTC_ALARM_SUBSECOND_VAL(alarm->subsecond_mask));

    if (alarm_index == HAL_RTC_ALARM0) {
        rom_hw_rtc_set_alarm_a(&rtc_time);
    } else {
        rom_hw_rtc_set_alarm_b(&rtc_time);
        
    }
    rom_hw_rtc_set_alarm_mask(alarm_index, alarm_mask);
}

void qmx_hal_rtc_enable_alarm(hal_rtc_alarm_id_e alarm_index)
{
    rom_hw_rtc_enable_alarm_interrupt(alarm_index);
}

void qmx_hal_rtc_disable_alarm(hal_rtc_alarm_id_e alarm_index)
{
    rom_hw_rtc_disable_alarm_interrupt(alarm_index);
}

uint32_t qmx_hal_rtc_get_alarm_int_sta(void)
{
    uint8_t sta;
    rom_hw_rtc_get_interrupt_flag(&sta);
    return sta;
}

void qmx_hal_rtc_clear_alarm_int_sta(uint32_t sta)
{
    rom_hw_rtc_clear_interrupt_flag(sta);
}
