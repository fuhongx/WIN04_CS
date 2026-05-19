#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "reg_Dev.h"
#include "error_def.h"
#include "slc_hal_sysctrl.h"
#include "app_cfg.h"
#include "utility.h"
#include "debug.h"
#include "hw_timer.h"
#include "slc_hal_intc.h"
#include "slc_hal_rtc.h"
#include "slc_hal_delay.h"
#include "slc_rtc_test.h"

// BCD format, expect subsecond
#define TEST_RTC_YEAR       (0x2024)
#define TEST_RTC_MONTH      (0x2)
#define TEST_RTC_DAY        (0x28)
#define TEST_RTC_WEEK       (HAL_RTC_WEEK_WED)
#define TEST_RTC_HOUR       (0x23)
#define TEST_RTC_MINUTE     (0x59)
#define TEST_RTC_SECOND     (0x30)
#define TEST_RTC_SUBSECOND  (0)
static const char* slc_week_days[] = {
    "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"
};

uint8_t g_test_rtc_alarm_flag = 0;

void slc_rtc_alarm_test_irq(void)
{
    uint32_t status = 0;

    status = slc_hal_rtc_get_alarm_int_sta();
    slc_hal_rtc_clear_alarm_int_sta(status);

    g_test_rtc_alarm_flag++;

    hal_rtc_calendar_t stTime;
    slc_hal_rtc_get_calendar(&stTime);
    PRINTF("RTC status 0x%X, now at %04X/%02X/%02X(%s) %02X:%02X:%02X.%d--\n",
           status, stTime.year, stTime.month, stTime.day, slc_week_days[stTime.week],
           stTime.hour, stTime.minute, stTime.second, stTime.subsecond);
}

int slc_rtc_accuracy_test(void)
{
    hal_rtc_calendar_t rtc_time;
    hal_rtc_calendar_t cur_time = {0};
    int ret = 0;

    // BCD format: year/month/day(week) hour:minute:second.subsecond
    rtc_time.year = TEST_RTC_YEAR;
    rtc_time.month = TEST_RTC_MONTH;
    rtc_time.day = TEST_RTC_DAY;
    rtc_time.week = TEST_RTC_WEEK;
    rtc_time.hour = TEST_RTC_HOUR;
    rtc_time.minute = TEST_RTC_MINUTE;
    rtc_time.second = TEST_RTC_SECOND;
    rtc_time.subsecond = TEST_RTC_SUBSECOND;

    g_test_rtc_alarm_flag = 0;

    slc_hal_rtc_stop();
    slc_hal_rtc_set_calendar(&rtc_time);

    PRINTF("RTC init done, start at %04X/%02X/%02X(%s) %02X:%02X:%02X.%d\n",
           rtc_time.year, rtc_time.month, rtc_time.day, slc_week_days[rtc_time.week],
           rtc_time.hour, rtc_time.minute, rtc_time.second, rtc_time.subsecond);

    slc_hal_rtc_start();

    slc_hal_nop_delay_s(5);

    slc_hal_rtc_get_calendar(&cur_time);
    if (cur_time.year != rtc_time.year || cur_time.month != rtc_time.month ||
        cur_time.day != rtc_time.day || cur_time.week != rtc_time.week || cur_time.hour != rtc_time.hour ||
        cur_time.minute != rtc_time.minute) {
        PRINTF("RTC accuracy test fail!\n");
        ret = -1;
    }
    if (cur_time.second - rtc_time.second != 5) {
        PRINTF("RTC accuracy test fail!\n");
        ret = -1;
    }

    PRINTF("RTC now at %04X/%02X/%02X(%s) %02X:%02X:%02X.%d--\n",
           cur_time.year, cur_time.month, cur_time.day, slc_week_days[cur_time.week],
           cur_time.hour, cur_time.minute, cur_time.second, cur_time.subsecond);

    slc_hal_rtc_stop();

    return ret;
}

int slc_rtc_stop_test(void)
{
    hal_rtc_calendar_t rtc_time;
    hal_rtc_calendar_t cur_time0 = {0};
    hal_rtc_calendar_t cur_time1 = {0};
    int ret = 0;

    // BCD format: year/month/day(week) hour:minute:second.subsecond
    rtc_time.year = TEST_RTC_YEAR;
    rtc_time.month = TEST_RTC_MONTH;
    rtc_time.day = TEST_RTC_DAY;
    rtc_time.week = TEST_RTC_WEEK;
    rtc_time.hour = TEST_RTC_HOUR;
    rtc_time.minute = TEST_RTC_MINUTE;
    rtc_time.second = TEST_RTC_SECOND;
    rtc_time.subsecond = TEST_RTC_SUBSECOND;

    g_test_rtc_alarm_flag = 0;

    slc_hal_rtc_stop();
    slc_hal_rtc_set_calendar(&rtc_time);

    PRINTF("RTC init done, start at %04X/%02X/%02X(%s) %02X:%02X:%02X.%d\n",
           rtc_time.year, rtc_time.month, rtc_time.day, slc_week_days[rtc_time.week],
           rtc_time.hour, rtc_time.minute, rtc_time.second, rtc_time.subsecond);

    slc_hal_rtc_start();

    slc_hal_nop_delay_s(5);

    slc_hal_rtc_stop();
    slc_hal_rtc_get_calendar(&cur_time0);
    if (cur_time0.year != rtc_time.year || cur_time0.month != rtc_time.month ||
        cur_time0.day != rtc_time.day || cur_time0.week != rtc_time.week || cur_time0.hour != rtc_time.hour ||
        cur_time0.minute != rtc_time.minute) {
        PRINTF("RTC stop test fail! run fail\n");
        ret = -1;
    }
    if (cur_time0.second - rtc_time.second != 5) {
        PRINTF("RTC stop test fail! second != 5\n");
        ret = -1;
    }

    slc_hal_rtc_get_calendar(&cur_time1);
    if (memcmp(&cur_time0, &cur_time1, sizeof(hal_rtc_calendar_t)) != 0) {
        PRINTF("RTC stop test fail! stop fail\n");
        PRINTF("RTC time0 at %04X/%02X/%02X(%s) %02X:%02X:%02X.%d--\n",
            cur_time0.year, cur_time0.month, cur_time0.day, slc_week_days[cur_time0.week],
            cur_time0.hour, cur_time0.minute, cur_time0.second, cur_time0.subsecond);
        PRINTF("RTC time1 at %04X/%02X/%02X(%s) %02X:%02X:%02X.%d--\n",
            cur_time1.year, cur_time1.month, cur_time1.day, slc_week_days[cur_time1.week],
            cur_time1.hour, cur_time1.minute, cur_time1.second, cur_time1.subsecond);
        ret = -1;
    }

    return ret;
}

int slc_rtc_leap_month_test(void)
{
    hal_rtc_calendar_t rtc_time;
    hal_rtc_calendar_t cur_time = {0};
    int ret = 0;

    // BCD format: year/month/day(week) hour:minute:second.subsecond
    rtc_time.year = TEST_RTC_YEAR;
    rtc_time.month = TEST_RTC_MONTH;
    rtc_time.day = TEST_RTC_DAY;
    rtc_time.week = TEST_RTC_WEEK;
    rtc_time.hour = TEST_RTC_HOUR;
    rtc_time.minute = TEST_RTC_MINUTE;
    rtc_time.second = TEST_RTC_SECOND;
    rtc_time.subsecond = TEST_RTC_SUBSECOND;

    g_test_rtc_alarm_flag = 0;

    slc_hal_rtc_stop();
    slc_hal_rtc_set_calendar(&rtc_time);

    PRINTF("RTC init done, start at %04X/%02X/%02X(%s) %02X:%02X:%02X.%d\n",
           rtc_time.year, rtc_time.month, rtc_time.day, slc_week_days[rtc_time.week],
           rtc_time.hour, rtc_time.minute, rtc_time.second, rtc_time.subsecond);

    slc_hal_rtc_start();

    slc_hal_nop_delay_s(35);

    slc_hal_rtc_get_calendar(&cur_time);
    if ((cur_time.month != TEST_RTC_MONTH) || (cur_time.day != (TEST_RTC_DAY + 1))) {
        PRINTF("RTC leap month test fail!\n");
        ret = -1;
    }

    PRINTF("RTC now at %04X/%02X/%02X(%s) %02X:%02X:%02X.%d--\n",
           cur_time.year, cur_time.month, cur_time.day, slc_week_days[cur_time.week],
           cur_time.hour, cur_time.minute, cur_time.second, cur_time.subsecond);

    slc_hal_rtc_stop();

    return ret;
}

int slc_rtc_alarm_ab_test(void)
{
    hal_rtc_calendar_t rtc_time;
    hal_rtc_alarm_msk_t alarm_mask = {0};
    int ret = 0;

    // BCD format: year/month/day(week) hour:minute:second.subsecond
    rtc_time.year = TEST_RTC_YEAR;
    rtc_time.month = TEST_RTC_MONTH;
    rtc_time.day = TEST_RTC_DAY;
    rtc_time.week = TEST_RTC_WEEK;
    rtc_time.hour = TEST_RTC_HOUR;
    rtc_time.minute = TEST_RTC_MINUTE;
    rtc_time.second = TEST_RTC_SECOND;
    rtc_time.subsecond = TEST_RTC_SUBSECOND;

    g_test_rtc_alarm_flag = 0;

    slc_hal_rtc_stop();
    slc_hal_rtc_set_calendar(&rtc_time);

    PRINTF("RTC init done, start at %04X/%02X/%02X(%s) %02X:%02X:%02X.%d\n",
           rtc_time.year, rtc_time.month, rtc_time.day, slc_week_days[rtc_time.week],
           rtc_time.hour, rtc_time.minute, rtc_time.second, rtc_time.subsecond);

    alarm_mask.year_mask = 0;
    alarm_mask.month_mask = 0;
    alarm_mask.day_mask = 0;
    alarm_mask.week_mask = 0;
    alarm_mask.hour_mask = 0;
    alarm_mask.minute_mask = 0;
    alarm_mask.second_mask = 1;
    alarm_mask.subsecond_mask = 1;

    rtc_time.second += 0x10;
    slc_hal_rtc_set_alarm(HAL_RTC_ALARM0, &alarm_mask, &rtc_time);
    slc_hal_rtc_enable_alarm(HAL_RTC_ALARM0);
    PRINTF("alarm A init done, trigger at XXXX/XX/XX XX:XX:%02X.%d\n",
           rtc_time.second, rtc_time.subsecond);

    rtc_time.second += 0x10;
    slc_hal_rtc_set_alarm(HAL_RTC_ALARM1, &alarm_mask, &rtc_time);
    slc_hal_rtc_enable_alarm(HAL_RTC_ALARM1);
    PRINTF("alarm B init done, trigger at XXXX/XX/XX XX:XX:%02X.%d\n",
           rtc_time.second, rtc_time.subsecond);

    slc_hal_register_irq_handler(RTC_IRQ, slc_rtc_alarm_test_irq);
    SLC_HAL_ENABLE_PERIPHERAL_IRQ(RTC_IRQ, 0x3);

    slc_hal_rtc_start();

    slc_hal_nop_delay_s(12);
    if (g_test_rtc_alarm_flag != 1) {
        PRINTF("RTC-A alarm test fail!\n");
        ret = -1;
    }

    slc_hal_nop_delay_s(12);
    if (g_test_rtc_alarm_flag != 2) {
        PRINTF("RTC-B alarm test fail!\n");
        ret = -1;
    }

    slc_hal_nop_delay_s(60);
    if (g_test_rtc_alarm_flag != 4) {
        PRINTF("RTC-AB alarm test fail!\n");
        ret = -1;
    }

    slc_hal_rtc_stop();
    slc_hal_rtc_disable_alarm(HAL_RTC_ALARM0);
    slc_hal_rtc_disable_alarm(HAL_RTC_ALARM1);
    NVIC_DisableIRQ(RTC_IRQ);
    slc_hal_unregister_irq_handler(RTC_IRQ);

    return ret;
}

int slc_rtc_alarm_a_test(void)
{
    hal_rtc_calendar_t rtc_time;
    hal_rtc_alarm_msk_t alarm_mask = {0};
    int ret = 0;

    // BCD format: year/month/day(week) hour:minute:second.subsecond
    rtc_time.year = TEST_RTC_YEAR;
    rtc_time.month = TEST_RTC_MONTH;
    rtc_time.day = TEST_RTC_DAY;
    rtc_time.week = TEST_RTC_WEEK;
    rtc_time.hour = TEST_RTC_HOUR;
    rtc_time.minute = TEST_RTC_MINUTE;
    rtc_time.second = TEST_RTC_SECOND;
    rtc_time.subsecond = TEST_RTC_SUBSECOND;

    g_test_rtc_alarm_flag = 0;

    slc_hal_rtc_stop();
    slc_hal_rtc_set_calendar(&rtc_time);

    PRINTF("RTC init done, start at %04X/%02X/%02X(%s) %02X:%02X:%02X.%d\n",
           rtc_time.year, rtc_time.month, rtc_time.day, slc_week_days[rtc_time.week],
           rtc_time.hour, rtc_time.minute, rtc_time.second, rtc_time.subsecond);

    alarm_mask.year_mask = 0;
    alarm_mask.month_mask = 0;
    alarm_mask.day_mask = 0;
    alarm_mask.week_mask = 0;
    alarm_mask.hour_mask = 0;
    alarm_mask.minute_mask = 0;
    alarm_mask.second_mask = 1;
    alarm_mask.subsecond_mask = 1;

    rtc_time.second += 0x10;
    slc_hal_rtc_set_alarm(HAL_RTC_ALARM0, &alarm_mask, &rtc_time);
    slc_hal_rtc_enable_alarm(HAL_RTC_ALARM0);
    PRINTF("alarm A init done, trigger at XXXX/XX/XX XX:XX:%02X.%d\n",
           rtc_time.second, rtc_time.subsecond);

    slc_hal_register_irq_handler(RTC_IRQ, slc_rtc_alarm_test_irq);
    SLC_HAL_ENABLE_PERIPHERAL_IRQ(RTC_IRQ, 0x3);

    slc_hal_rtc_start();

    slc_hal_nop_delay_s(12);
    if (g_test_rtc_alarm_flag != 1) {
        PRINTF("RTC-A alarm test fail!\n");
        ret = -1;
    }

    slc_hal_nop_delay_s(60);
    if (g_test_rtc_alarm_flag != 2) {
        PRINTF("RTC-A alarm second test fail!\n");
        ret = -1;
    }

    slc_hal_rtc_stop();
    slc_hal_rtc_disable_alarm(HAL_RTC_ALARM0);
    NVIC_DisableIRQ(RTC_IRQ);
    slc_hal_unregister_irq_handler(RTC_IRQ);

    return ret;
}

int slc_rtc_alarm_b_test(void)
{
    hal_rtc_calendar_t rtc_time;
    hal_rtc_alarm_msk_t alarm_mask = {0};
    int ret = 0;

    // BCD format: year/month/day(week) hour:minute:second.subsecond
    rtc_time.year = TEST_RTC_YEAR;
    rtc_time.month = TEST_RTC_MONTH;
    rtc_time.day = TEST_RTC_DAY;
    rtc_time.week = TEST_RTC_WEEK;
    rtc_time.hour = TEST_RTC_HOUR;
    rtc_time.minute = TEST_RTC_MINUTE;
    rtc_time.second = TEST_RTC_SECOND;
    rtc_time.subsecond = TEST_RTC_SUBSECOND;

    g_test_rtc_alarm_flag = 0;

    slc_hal_rtc_stop();
    slc_hal_rtc_set_calendar(&rtc_time);

    PRINTF("RTC init done, start at %04X/%02X/%02X(%s) %02X:%02X:%02X.%d\n",
           rtc_time.year, rtc_time.month, rtc_time.day, slc_week_days[rtc_time.week],
           rtc_time.hour, rtc_time.minute, rtc_time.second, rtc_time.subsecond);

    alarm_mask.year_mask = 0;
    alarm_mask.month_mask = 0;
    alarm_mask.day_mask = 0;
    alarm_mask.week_mask = 0;
    alarm_mask.hour_mask = 0;
    alarm_mask.minute_mask = 0;
    alarm_mask.second_mask = 1;
    alarm_mask.subsecond_mask = 1;

    rtc_time.second += 0x10;
    slc_hal_rtc_set_alarm(HAL_RTC_ALARM1, &alarm_mask, &rtc_time);
    slc_hal_rtc_enable_alarm(HAL_RTC_ALARM1);
    PRINTF("alarm B init done, trigger at XXXX/XX/XX XX:XX:%02X.%d\n",
           rtc_time.second, rtc_time.subsecond);

    slc_hal_register_irq_handler(RTC_IRQ, slc_rtc_alarm_test_irq);
    SLC_HAL_ENABLE_PERIPHERAL_IRQ(RTC_IRQ, 0x3);

    slc_hal_rtc_start();

    slc_hal_nop_delay_s(12);
    if (g_test_rtc_alarm_flag != 1) {
        PRINTF("RTC-B alarm test fail!\n");
        ret = -1;
    }

    slc_hal_nop_delay_s(60);
    if (g_test_rtc_alarm_flag != 2) {
        PRINTF("RTC-B alarm second test fail!\n");
        ret = -1;
    }

    slc_hal_rtc_stop();
    slc_hal_rtc_disable_alarm(HAL_RTC_ALARM1);
    NVIC_DisableIRQ(RTC_IRQ);
    slc_hal_unregister_irq_handler(RTC_IRQ);

    return ret;
}

/* 此处调用底层接口验证芯片功能 */
#include "hw_rtc.h"
int slc_rtc_err_bcd_test(void)
{
    stRtcTime_t stTime;
    stRtcTime_t cur_time = {0};
    int ret = 0;

    // BCD format: year/month/day(week) hour:minute:second.subsecond
    stTime.u16Year = TEST_RTC_YEAR;
    stTime.u8Month = TEST_RTC_MONTH;
    stTime.u8Day = TEST_RTC_DAY;
    stTime.u8Week = TEST_RTC_WEEK;
    stTime.u8Hour = TEST_RTC_HOUR;
    stTime.u8Minute = 0x66;
    stTime.u8Second = 0x50;
    stTime.u16SubSecond = TEST_RTC_SUBSECOND;

    g_test_rtc_alarm_flag = 0;

    slc_hal_rtc_stop();
    rom_hw_rtc_set_time(&stTime);

    PRINTF("RTC init done, start at %04X/%02X/%02X(%s) %02X:%02X:%02X.%d\n",
           stTime.u16Year, stTime.u8Month, stTime.u8Day, slc_week_days[stTime.u8Week],
           stTime.u8Hour, stTime.u8Minute, stTime.u8Second, stTime.u16SubSecond);

    slc_hal_rtc_start();

    slc_hal_nop_delay_s(10);

    rom_hw_rtc_get_time(&cur_time, EN_RTC_TIME_NOW);
    if (cur_time.u8Hour != 0x0 || cur_time.u8Minute != 0x0 || cur_time.u8Second != 0x10) {
        PRINTF("RTC err_bcd test fail!\n");
        ret = -1;
    }

    PRINTF("RTC now at %04X/%02X/%02X(%s) %02X:%02X:%02X.%d--\n",
           cur_time.u16Year, cur_time.u8Month, cur_time.u8Day, slc_week_days[cur_time.u8Week],
           cur_time.u8Hour, cur_time.u8Minute, cur_time.u8Second, cur_time.u16SubSecond);

    slc_hal_rtc_stop();

    return ret;
}
