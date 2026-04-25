/**
 * ****************************************************************************
 * @file qmx_hal_rtc.h
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

#ifndef __QMX_HAL_RTC_H__
#define __QMX_HAL_RTC_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

typedef enum
{
    HAL_RTC_WEEK_SUN = 0,
    HAL_RTC_WEEK_MON = 1,
    HAL_RTC_WEEK_TUE = 2,
    HAL_RTC_WEEK_WED = 3,
    HAL_RTC_WEEK_THU = 4,
    HAL_RTC_WEEK_FRI = 5,
    HAL_RTC_WEEK_SAT = 6
} hal_rtc_week_e;

typedef struct
{
    uint16_t year;          /*!< 年 */
    uint8_t month;          /*!< 月 */
    uint8_t day;            /*!< 日 */
    uint8_t week;           /*!< 星期 */
    uint8_t hour;           /*!< 时 */
    uint8_t minute;         /*!< 分 */
    uint8_t second;         /*!< 秒 */
    uint8_t subsecond;      /*!< 亚秒 */
} hal_rtc_calendar_t;

typedef enum
{
    HAL_RTC_ALARM0,
    HAL_RTC_ALARM1,

    HAL_RTC_ALARM_MAX,
} hal_rtc_alarm_id_e;

typedef struct
{
    uint8_t year_mask;          /*!< 年份掩码 */
    uint8_t month_mask;         /*!< 月份掩码 */
    uint8_t day_mask;           /*!< 天掩码 */
    uint8_t week_mask;          /*!< 周掩码 */
    uint8_t hour_mask;          /*!< 小时掩码 */
    uint8_t minute_mask;        /*!< 分钟掩码 */
    uint8_t second_mask;        /*!< 秒掩码 */
    uint8_t subsecond_mask;     /*!< 亚秒掩码 */
} hal_rtc_alarm_msk_t;

#define HAL_RTC_ALARM_YEAR_SHIFT        (7U)
#define HAL_RTC_ALARM_YEAR_MASK         (0x1 << HAL_RTC_ALARM_YEAR_SHIFT)
#define HAL_RTC_ALARM_YEAR_VAL(x)       (((x) << HAL_RTC_ALARM_YEAR_SHIFT) & HAL_RTC_ALARM_YEAR_MASK)
#define HAL_RTC_ALARM_MONTH_SHIFT       (6U)
#define HAL_RTC_ALARM_MONTH_MASK        (0x1 << HAL_RTC_ALARM_MONTH_SHIFT)
#define HAL_RTC_ALARM_MONTH_VAL(x)      (((x) << HAL_RTC_ALARM_MONTH_SHIFT) & HAL_RTC_ALARM_MONTH_MASK)
#define HAL_RTC_ALARM_DAY_SHIFT         (5U)
#define HAL_RTC_ALARM_DAY_MASK          (0x1 << HAL_RTC_ALARM_DAY_SHIFT)
#define HAL_RTC_ALARM_DAY_VAL(x)        (((x) << HAL_RTC_ALARM_DAY_SHIFT) & HAL_RTC_ALARM_DAY_MASK)
#define HAL_RTC_ALARM_WEEK_SHIFT        (4U)
#define HAL_RTC_ALARM_WEEK_MASK         (0x1 << HAL_RTC_ALARM_WEEK_SHIFT)
#define HAL_RTC_ALARM_WEEK_VAL(x)       (((x) << HAL_RTC_ALARM_WEEK_SHIFT) & HAL_RTC_ALARM_WEEK_MASK)
#define HAL_RTC_ALARM_HOUR_SHIFT        (3U)
#define HAL_RTC_ALARM_HOUR_MASK         (0x1 << HAL_RTC_ALARM_HOUR_SHIFT)
#define HAL_RTC_ALARM_HOUR_VAL(x)       (((x) << HAL_RTC_ALARM_HOUR_SHIFT) & HAL_RTC_ALARM_HOUR_MASK)
#define HAL_RTC_ALARM_MINUTE_SHIFT      (2U)
#define HAL_RTC_ALARM_MINUTE_MASK       (0x1 << HAL_RTC_ALARM_MINUTE_SHIFT)
#define HAL_RTC_ALARM_MINUTE_VAL(x)     (((x) << HAL_RTC_ALARM_MINUTE_SHIFT) & HAL_RTC_ALARM_MINUTE_MASK)
#define HAL_RTC_ALARM_SECOND_SHIFT      (1U)
#define HAL_RTC_ALARM_SECOND_MASK       (0x1 << HAL_RTC_ALARM_SECOND_SHIFT)
#define HAL_RTC_ALARM_SECOND_VAL(x)     (((x) << HAL_RTC_ALARM_SECOND_SHIFT) & HAL_RTC_ALARM_SECOND_MASK)
#define HAL_RTC_ALARM_SUBSECOND_SHIFT   (0U)
#define HAL_RTC_ALARM_SUBSECOND_MASK    (0x1 << HAL_RTC_ALARM_SUBSECOND_SHIFT)
#define HAL_RTC_ALARM_SUBSECOND_VAL(x)  (((x) << HAL_RTC_ALARM_SUBSECOND_SHIFT) & HAL_RTC_ALARM_SUBSECOND_MASK)

/* 中断状态 */
#define HAL_RTC_STA_ALARM0      (1 << 0)
#define HAL_RTC_STA_ALARM1      (1 << 1)

/**
 * @brief 设置RTC的日期时间, 设置时间前需要先暂停RTC
 * 
 * @param calendar 日期时间的结构体，具体见hal_rtc_calendar_t
 */
void qmx_hal_rtc_set_calendar(hal_rtc_calendar_t *calendar);

/**
 * @brief 获取RTC的日期时间
 * 
 * @param calendar 日期时间的结构体，具体见hal_rtc_calendar_t
 */
void qmx_hal_rtc_get_calendar(hal_rtc_calendar_t *calendar);

/**
 * @brief 暂停RTC
 */
void qmx_hal_rtc_stop(void);

/**
 * @brief 启动RTC
 */
void qmx_hal_rtc_start(void);

/**
 * @brief 设置RTC的闹钟
 * 
 * @param alarm_index 闹钟序号，具体见hal_rtc_alarm_id_e
 * @param alarm 周期性闹钟的掩码位，具体见hal_rtc_alarm_msk_t
 * @param calendar 闹钟时间的结构体，具体见hal_rtc_calendar_t
 */
void qmx_hal_rtc_set_alarm(hal_rtc_alarm_id_e alarm_index, hal_rtc_alarm_msk_t *alarm, hal_rtc_calendar_t *calendar);

/**
 * @brief 使能RTC的闹钟, 达到设定时间产生中断
 * 
 * @param alarm_index 闹钟序号
 */
void qmx_hal_rtc_enable_alarm(hal_rtc_alarm_id_e alarm_index);

/**
 * @brief 关闭RTC的闹钟
 * 
 * @param alarm_index 闹钟序号
 */
void qmx_hal_rtc_disable_alarm(hal_rtc_alarm_id_e alarm_index);

/**
 * @brief 获取RTC的闹钟中断状态
 * 
 * @return uint32_t 中断状态见 HAL_RTC_STA_ALARMx
 */
uint32_t qmx_hal_rtc_get_alarm_int_sta(void);

/**
 * @brief 清除RTC的闹钟中断状态
 * 
 * @param sta 中断源状态见 HAL_RTC_STA_ALARMx
 */
void qmx_hal_rtc_clear_alarm_int_sta(uint32_t sta);

#ifdef __cplusplus
}
#endif

#endif  /* __QMX_HAL_RTC_H__ */
