/**
 * ****************************************************************************
 * @file hw_rtc.c
 * @author SLC(SC) software team.
 * @brief
 * @version  1.0
 * ****************************************************************************
 *
 * THIS CODE IS LIMITED TO GUIDE THE APPLICATION AND IMPROVE THE EFFICIENCY OF RESEARCH AND DEVELOPMENT.
 * AS A RESULT, SILICONDUCTOR IS NOT RESPONSIBLE FOR ANY CLAIMS ARISING DIRECTLY OR INDIRECTLY FROM SUCH FIRMWARE AND/OR CONTENT.
 *
 * Copyright (c) 2025 Siliconductor Semiconductor all rights reserved.
 *
 * ****************************************************************************
 */

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "reg_Dev.h"
#include "error_def.h"
#include "hw_rtc.h"
#include "utility.h"
#include "hw_sysctrl.h"

EN_ERR_STA_T rom_hw_rtc_start_counter(bool bEnable)
{
    if (bEnable)
    {
        RTC->RTC_RCR = RTC_RCR_CMD_START;
    }
    else
    {
        RTC->RTC_RCR = RTC_RCR_CMD_STOP;
        // 至少等待2个32K时钟周期确保RTC完全停止
        rom_utility_delay_us(LP_RW_DELAY);
    }

    return EN_ERROR_STA_OK;
}

EN_ERR_STA_T rom_hw_rtc_trig_sync_setting(void)
{
    RTC->RTC_SER = RTC_TIRG_SYNC_REG_MASK;

    // 至少等待1个32K时钟周期确保RTC数值加载成功，此处为方便和上面一样延时2个32K时钟
    rom_utility_delay_us(LP_RW_DELAY);
    return EN_ERROR_STA_OK;
}

EN_ERR_STA_T rom_hw_rtc_set_time(stRtcTime_t *pstTime)
{
    if (NULL == pstTime || pstTime->u8Week > RTC_WEEK_SAT)
    {
        return EN_ERROR_STA_INVALID;
    }

    uint32_t u32Cfg = (((pstTime->u8Second & RTC_STR_SECOND_MASK) << RTC_STR_SECOND_SHIFT) |
                       ((pstTime->u8Minute & RTC_STR_MINUTE_MASK) << RTC_STR_MINUTE_SHIFT) |
                       ((pstTime->u8Hour & RTC_STR_HOUR_MASK) << RTC_STR_HOUR_SHIFT) |
                       ((pstTime->u8Week & RTC_STR_WEEK_MASK) << RTC_STR_WEEK_SHIFT));

    RTC->RTC_STR = u32Cfg & RTC_STR_REG_MASK;

    u32Cfg = (((pstTime->u8Day & RTC_SDR_DAY_MASK) << RTC_SDR_DAY_SHIFT) |
              ((pstTime->u8Month & RTC_SDR_MONTH_MASK) << RTC_SDR_MONTH_SHIFT) |
              ((pstTime->u16Year & RTC_SDR_YEAR_MASK) << RTC_SDR_YEAR_SHIFT));
    RTC->RTC_SDR = u32Cfg & RTC_SDR_REG_MASK;

    rom_hw_rtc_trig_sync_setting();

    return EN_ERROR_STA_OK;
}

EN_ERR_STA_T rom_hw_rtc_set_alarm_a(stRtcTime_t *pstTime)
{
    if (NULL == pstTime)
    {
        return EN_ERROR_STA_INVALID;
    }
    RTC->RTC_ASR = pstTime->u16SubSecond & RTC_ASR_REG_MASK;
    RTC->RTC_ATR = (((pstTime->u8Second & RTC_STR_SECOND_MASK) << RTC_STR_SECOND_SHIFT) |
                    ((pstTime->u8Minute & RTC_STR_MINUTE_MASK) << RTC_STR_MINUTE_SHIFT) |
                    ((pstTime->u8Hour & RTC_STR_HOUR_MASK) << RTC_STR_HOUR_SHIFT) |
                    ((pstTime->u8Week & RTC_STR_WEEK_MASK) << RTC_STR_WEEK_SHIFT));

    RTC->RTC_ADR = (((pstTime->u8Day & RTC_SDR_DAY_MASK) << RTC_SDR_DAY_SHIFT) |
                    ((pstTime->u8Month & RTC_SDR_MONTH_MASK) << RTC_SDR_MONTH_SHIFT) |
                    ((pstTime->u16Year & RTC_SDR_YEAR_MASK) << RTC_SDR_YEAR_SHIFT));

    return EN_ERROR_STA_OK;
}

EN_ERR_STA_T rom_hw_rtc_set_alarm_b(stRtcTime_t *pstTime)
{
    if (NULL == pstTime)
    {
        return EN_ERROR_STA_INVALID;
    }
    RTC->RTC_BSR = pstTime->u16SubSecond & RTC_BSR_REG_MASK;
    RTC->RTC_BTR = (((pstTime->u8Second & RTC_STR_SECOND_MASK) << RTC_STR_SECOND_SHIFT) |
                    ((pstTime->u8Minute & RTC_STR_MINUTE_MASK) << RTC_STR_MINUTE_SHIFT) |
                    ((pstTime->u8Hour & RTC_STR_HOUR_MASK) << RTC_STR_HOUR_SHIFT) |
                    ((pstTime->u8Week & RTC_STR_WEEK_MASK) << RTC_STR_WEEK_SHIFT));

    RTC->RTC_BDR = (((pstTime->u8Day & RTC_SDR_DAY_MASK) << RTC_SDR_DAY_SHIFT) |
                    ((pstTime->u8Month & RTC_SDR_MONTH_MASK) << RTC_SDR_MONTH_SHIFT) |
                    ((pstTime->u16Year & RTC_SDR_YEAR_MASK) << RTC_SDR_YEAR_SHIFT));

    return EN_ERROR_STA_OK;
}

EN_ERR_STA_T rom_hw_rtc_get_time(stRtcTime_t *pstTime, EN_RTC_TIME_T enMode)
{
    if ((NULL == pstTime) | (EN_RTC_TIME_MAX < enMode))
    {
        return EN_ERROR_STA_INVALID;
    }

    // Init Time now
    uint32_t u32SubSecond = RTC->RTC_CSR;
    uint32_t u32LoDate = RTC->RTC_CTR;
    uint32_t u32HiDate = RTC->RTC_CDR;

    switch (enMode)
    {
        case EN_RTC_TIME_ALARM_A:
        {
            u32SubSecond = RTC->RTC_ASR;
            u32LoDate = RTC->RTC_ATR;
            u32HiDate = RTC->RTC_ADR;
            break;
        }

        case EN_RTC_TIME_ALARM_B:
        {
            u32SubSecond = RTC->RTC_BSR;
            u32LoDate = RTC->RTC_BTR;
            u32HiDate = RTC->RTC_BDR;
            break;
        }

        default:
            break;
    }

    pstTime->u16SubSecond = u32SubSecond & RTC_CSR_REG_MASK;

    pstTime->u8Second = (u32LoDate >> RTC_STR_SECOND_SHIFT) & RTC_STR_SECOND_MASK;
    pstTime->u8Minute = (u32LoDate >> RTC_STR_MINUTE_SHIFT) & RTC_STR_MINUTE_MASK;
    pstTime->u8Hour = (u32LoDate >> RTC_STR_HOUR_SHIFT) & RTC_STR_HOUR_MASK;
    pstTime->u8Week = (u32LoDate >> RTC_STR_WEEK_SHIFT) & RTC_STR_WEEK_MASK;

    pstTime->u8Day = (u32HiDate >> RTC_SDR_DAY_SHIFT) & RTC_SDR_DAY_MASK;
    pstTime->u8Month = (u32HiDate >> RTC_SDR_MONTH_SHIFT) & RTC_SDR_MONTH_MASK;
    pstTime->u16Year = (u32HiDate >> RTC_SDR_YEAR_SHIFT) & RTC_SDR_YEAR_MASK;

    return EN_ERROR_STA_OK;
}

EN_ERR_STA_T rom_hw_rtc_get_interrupt_flag(uint8_t *pu8Flag)
{
    if (NULL == pu8Flag)
    {
        return EN_ERROR_STA_INVALID;
    }

    *pu8Flag = RTC->RTC_ISR;

    return EN_ERROR_STA_OK;
}

EN_ERR_STA_T rom_hw_rtc_clear_interrupt_flag(uint8_t u8Flag)
{
    RTC->RTC_ISR = u8Flag;
    rom_utility_delay_us(LP_RW_DELAY);
    return EN_ERROR_STA_OK;
}

void rom_hw_rtc_set_alarm_mask(uint8_t alarm_id, uint8_t u8BMask)
{
    if (alarm_id == 0) {
        RTC->RTC_ACR &= ~(RTC_ACR_A_MASK << RTC_ACR_A_SHIFT);
        RTC->RTC_ACR |= ((u8BMask & RTC_ACR_A_MASK) << RTC_ACR_A_SHIFT);
    } else {
        RTC->RTC_ACR &= ~(RTC_ACR_B_MASK << RTC_ACR_B_SHIFT);
        RTC->RTC_ACR |= ((u8BMask & RTC_ACR_B_MASK) << RTC_ACR_B_SHIFT);
    }
}

void rom_hw_rtc_enable_alarm_interrupt(uint8_t alarm_id)
{
    if (alarm_id == 0) {
        RTC->RTC_IER |= EN_RTC_INT_ALARM_A;
    } else {
        RTC->RTC_IER |= EN_RTC_INT_ALARM_B;
    }
}

void rom_hw_rtc_disable_alarm_interrupt(uint8_t alarm_id)
{
    if (alarm_id == 0) {
        RTC->RTC_IER &= ~EN_RTC_INT_ALARM_A;
    } else {
        RTC->RTC_IER &= ~EN_RTC_INT_ALARM_B;
    }
}

