/**
 * ****************************************************************************
 * @file hw_rtc.h
 * @author SLC(SC) software team.
 * @brief
 * @version  1.0
 * ****************************************************************************
 *
 * THIS CODE IS LIMITED TO GUIDE APPLICATION DEVELOPMENT AND INCREASE DEVELOPMENT SPEED.
 * AS A RESULT, SILICONDUCTOR IS NOT RESPONSIBLE FOR ANY CLAIMS ARISING DIRECTLY OR INDIRECTLY FROM SUCH FIRMWARE AND/OR CONTENT.
 *
 * Copyright (c) 2025 Siliconductor Semiconductor all rights reserved.
 *
 * ****************************************************************************
 */

#ifndef __HW_RTC_H__
#define __HW_RTC_H__

typedef struct
{
    uint16_t u16SubSecond;
    uint8_t u8Second;
    uint8_t u8Minute;
    uint8_t u8Hour;
    uint8_t u8Week; // 0-6, 0:Sunday
    uint8_t u8Day;
    uint8_t u8Month;
    uint16_t u16Year;
} stRtcTime_t;

typedef enum
{
    RTC_WEEK_SUN = 0,
    RTC_WEEK_MON = 1,
    RTC_WEEK_TUE = 2,
    RTC_WEEK_WED = 3,
    RTC_WEEK_THU = 4,
    RTC_WEEK_FRI = 5,
    RTC_WEEK_SAT = 6
} EN_RTC_WEEK_T;

typedef enum
{
    EN_RTC_TIME_NOW = 0,
    EN_RTC_TIME_ALARM_A = 1,
    EN_RTC_TIME_ALARM_B = 2,
    EN_RTC_TIME_MAX,
} EN_RTC_TIME_T;

typedef enum
{
    EN_RTC_INT_MASK_SUB_SECOND = 0x1 << RTC_ACR_SUB_SECOND_MASK_SHIT,
    EN_RTC_INT_MASK_SECOND = (0x1 << RTC_ACR_SECOND_MASK_SHIT) | EN_RTC_INT_MASK_SUB_SECOND,
    EN_RTC_INT_MASK_MINUTE = (0x1 << RTC_ACR_MINUTE_MASK_SHIT) | EN_RTC_INT_MASK_SECOND,
    EN_RTC_INT_MASK_HOUR = (0x1 << RTC_ACR_HOUR_MASK_SHIT) | EN_RTC_INT_MASK_MINUTE,
    EN_RTC_INT_MASK_WEEK = (0x1 << RTC_ACR_WEEK_MASK_SHIT) | EN_RTC_INT_MASK_HOUR,
    EN_RTC_INT_MASK_DAY = (0x1 << RTC_ACR_DAY_MASK_SHIT) | EN_RTC_INT_MASK_HOUR,
    EN_RTC_INT_MASK_MONTH = (0x1 << RTC_ACR_MONTH_MASK_SHIT) | EN_RTC_INT_MASK_DAY,
    EN_RTC_INT_MASK_YEAR = (0x1 << RTC_ACR_YEAR_MASK_SHIT) | EN_RTC_INT_MASK_MONTH,
} EN_RTC_INT_MASK_T;

typedef enum
{
    EN_RTC_INT_ALARM_A = RTC_IER_A_MASK << RTC_IER_A_SHIFT,
    EN_RTC_INT_ALARM_B = RTC_IER_B_MASK << RTC_IER_B_SHIFT,
    EN_RTC_INT_ALARM_BOTH_AB = EN_RTC_INT_ALARM_A | EN_RTC_INT_ALARM_B,
} EN_RTC_INT_T;

/**
 * @brief Enable/Disable the RTC counter.
 *
 * @param bEnable true : enable, false : disable.
 * @return EN_ERR_STA_T
 */
EN_ERR_STA_T rom_hw_rtc_start_counter(bool bEnable);

/**
 * @brief The RTC is triggered to load the configuration value into the counter.
 *        This function is not required for API calls.
 *
 * @return EN_ERR_STA_T
 */
EN_ERR_STA_T rom_hw_rtc_trig_sync_setting(void);

/**
 * @brief Config the current time of the RTC.
 *
 * @param pstTime @ref stRtcTime_t.
 * @return EN_ERR_STA_T
 */
EN_ERR_STA_T rom_hw_rtc_set_time(stRtcTime_t *pstTime);

/**
 * @brief Configure the set value of alarm clock A.
 *
 * @param pstTime @ref stRtcTime_t.
 * @return EN_ERR_STA_T
 */
EN_ERR_STA_T rom_hw_rtc_set_alarm_a(stRtcTime_t *pstTime);

/**
 * @brief Configure the set value of alarm clock B.
 *
 * @param pstTime @ref stRtcTime_t.
 * @return EN_ERR_STA_T
 */
EN_ERR_STA_T rom_hw_rtc_set_alarm_b(stRtcTime_t *pstTime);

/**
 * @brief Gets the current RTC count.
 *
 * @param pstTime @ref stRtcTime_t.
 * @param enMode @ref EN_RTC_TIME_T.
 * @return EN_ERR_STA_T
 */
EN_ERR_STA_T rom_hw_rtc_get_time(stRtcTime_t *pstTime, EN_RTC_TIME_T enMode);

/**
 * @brief Get the RTC interrupt flag.
 *
 * @param pu8Flag @ref EN_RTC_INT_T.
 * @return EN_ERR_STA_T
 */
EN_ERR_STA_T rom_hw_rtc_get_interrupt_flag(uint8_t *pu8Flag);

/**
 * @brief Clean RTC interrupt flags.
 *
 * @param u8Flag @ref EN_RTC_INT_T.
 * @return EN_ERR_STA_T
 */
EN_ERR_STA_T rom_hw_rtc_clear_interrupt_flag(uint8_t u8Flag);

void rom_hw_rtc_set_alarm_mask(uint8_t alarm_id, uint8_t u8BMask);
void rom_hw_rtc_enable_alarm_interrupt(uint8_t alarm_id);
void rom_hw_rtc_disable_alarm_interrupt(uint8_t alarm_id);

#endif
