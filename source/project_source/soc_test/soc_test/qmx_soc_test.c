#include "qmx_soc_test.h"

#include "qmx_nvic_test.h"
#include "qmx_systick_test.h"
#include "qmx_mem_test.h"
#include "qmx_dma_test.h"
#include "qmx_uart_test.h"
#include "qmx_iic_test.h"
#include "qmx_wdt_test.h"
#include "qmx_iwdt_test.h"
#include "qmx_rng_test.h"
#include "qmx_crc_test.h"
#include "qmx_aes_test.h"
#include "qmx_rtc_test.h"
#include "qmx_low_power_test.h"
#include "qmx_flash_test.h"
#include "qmx_timer_test.h"
#include "qmx_pwm_test.h"
#include "qmx_gpio_test.h"
#include "qmx_lpio_test.h"
#include "qmx_lpuart_test.h"
#include "qmx_spi_test.h"

#define SOCTEST_START_LINE    ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>"
#define SOCTEST_END_LINE      "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<"

soctest_record_t g_soctest_record = {0};

static const soctest_case_t qmx_nvic_test_case[] = {
    {"NVIC Interrupt Test",     qmx_nvic_intc_test,         SMOKE_TEST | AUTO_TEST | PRESSURE_TEST},
    {"NVIC nest&priority Test", qmx_nvic_nest_and_priority_test,    SMOKE_TEST | AUTO_TEST | PRESSURE_TEST},
    {"NVIC Primask Test",       qmx_nvic_primask_test,      MANUAL_TEST | EXCEPTION_TEST},
};

static const soctest_case_t qmx_systick_test_case[] = {
    {"Systick Accuracy Test",   qmx_systick_accuracy_test,  SMOKE_TEST | AUTO_TEST | PRESSURE_TEST},
    {"Systick Interrupt Test",  qmx_systick_intc_test,      SMOKE_TEST | AUTO_TEST | PRESSURE_TEST},
    {"Systick Maxtime Test",    qmx_systick_maxtime_test,   AUTO_TEST | PRESSURE_TEST},
};

static const soctest_case_t qmx_mem_test_case[] = {
    {"MEM Align 1B Test",   qmx_mem_align_1byte_test,   SMOKE_TEST | AUTO_TEST | PRESSURE_TEST},
    {"MEM Align 2B Test",   qmx_mem_align_2byte_test,   SMOKE_TEST | AUTO_TEST | PRESSURE_TEST},
    {"MEM Align 4B Test",   qmx_mem_align_4byte_test,   SMOKE_TEST | AUTO_TEST | PRESSURE_TEST},
    {"MEM Align 8B Test",   qmx_mem_align_8byte_test,   SMOKE_TEST | AUTO_TEST | PRESSURE_TEST},
    {"MEM Unalign Test",    qmx_mem_unalign_test,       MANUAL_TEST | EXCEPTION_TEST},
    {"MEM Reserved Test",   qmx_mem_reserved_test,      AUTO_TEST | PRESSURE_TEST},
};

static const soctest_case_t qmx_wdt_test_case[] = {
    {"WDT int feed reset Test",     qmx_wdt_int_feed_reset_test,    MANUAL_TEST},
    {"WDT feed reset Test",         qmx_wdt_feed_reset_test,        MANUAL_TEST},
    {"WDT int feed noreset Test",   qmx_wdt_int_feed_noreset_test,  MANUAL_TEST},
    {"WDT feed noreset Test",       qmx_wdt_feed_noreset_test,      SMOKE_TEST | AUTO_TEST | PRESSURE_TEST},
    {"WDT deinit Test",             qmx_wdt_deinit_test,            SMOKE_TEST | AUTO_TEST | PRESSURE_TEST},
    {"WDT write protect Test",      qmx_wdt_write_protect_test,     SMOKE_TEST | AUTO_TEST | PRESSURE_TEST},
};

static const soctest_case_t qmx_iwdt_test_case[] = {
    {"IWDT int feed reset Test",    qmx_iwdt_int_feed_reset_test,   MANUAL_TEST},
    {"IWDT feed reset Test",        qmx_iwdt_feed_reset_test,       MANUAL_TEST},
    {"IWDT int feed noreset Test",  qmx_iwdt_int_feed_noreset_test, MANUAL_TEST},
    {"IWDT feed noreset Test",      qmx_iwdt_feed_noreset_test,     SMOKE_TEST | AUTO_TEST | PRESSURE_TEST},
    {"IWDT deinit Test",            qmx_iwdt_deinit_test,           SMOKE_TEST | AUTO_TEST | PRESSURE_TEST},
    {"IWDT write protect Test",     qmx_iwdt_write_protect_test,    SMOKE_TEST | AUTO_TEST | PRESSURE_TEST},
};

static const soctest_case_t qmx_dma_test_case[] = {
    {"DMA RAM<->RAM Test",      qmx_dma_ram_to_ram_test,        SMOKE_TEST | AUTO_TEST | PRESSURE_TEST},
    {"DMA RAM<->FLASH Test",    qmx_dma_ram_to_flash_test,      AUTO_TEST},
    {"DMA FLASH<->FLASH Test",  qmx_dma_flash_to_flash_test,    AUTO_TEST},
    {"DMA multi trigger Test",  qmx_dma_muti_trig_test,         SMOKE_TEST | AUTO_TEST | PRESSURE_TEST},
    {"DMA diff width test",     qmx_dma_diff_width_test,        SMOKE_TEST | AUTO_TEST | PRESSURE_TEST},
    {"DMA timeout Test",        qmx_dma_timeout_test,           SMOKE_TEST | AUTO_TEST | PRESSURE_TEST},
    {"DMA Performance Test",    qmx_dma_performance_test,       AUTO_TEST | PRESSURE_TEST},
    {"DMA UART Test",           qmx_dma_uart_trx_test,          MANUAL_TEST | EXCEPTION_TEST},
    {"DMA LPUART Test",         qmx_dma_lpuart_trx_test,        MANUAL_TEST | EXCEPTION_TEST},
    {"DMA SPI-M Test",          qmx_dma_spi_master_test,        MANUAL_TEST | EXCEPTION_TEST},
    {"DMA SPI-S Test",          qmx_dma_spi_slave_test,         MANUAL_TEST | EXCEPTION_TEST},
    {"DMA IIC-M Test",          qmx_dma_iic_master_test,        MANUAL_TEST | EXCEPTION_TEST},
    {"DMA IIC-S Test",          qmx_dma_iic_slave_test,         MANUAL_TEST | EXCEPTION_TEST},
};

static const soctest_case_t qmx_iic_test_case[] = {
    {"I2C master speed Test",   qmx_iic_master_speed_test,      SMOKE_TEST | AUTO_TEST | PRESSURE_TEST},
    {"I2C master addrbits Test", qmx_iic_master_addrbits_test,  SMOKE_TEST | AUTO_TEST | PRESSURE_TEST},
    {"I2C slv addrbits Test",   qmx_iic_slv_addrbits_test,      SMOKE_TEST | AUTO_TEST | PRESSURE_TEST},
};

static const soctest_case_t qmx_spi_test_case[] = {
    {"SPI master clk div Test", qmx_spi_master_div_test,        SMOKE_TEST | AUTO_TEST | PRESSURE_TEST},
    {"SPI master polarity_phase Test", qmx_spi_master_polarity_phase_test,  SMOKE_TEST | AUTO_TEST | PRESSURE_TEST},
    {"SPI master data_mode Test", qmx_spi_master_data_mode_test,    SMOKE_TEST | AUTO_TEST | PRESSURE_TEST},
    {"SPI master data_len Test", qmx_spi_master_data_len_test,  SMOKE_TEST | AUTO_TEST | PRESSURE_TEST},
    {"SPI slv polarity_phase Test", qmx_spi_salve_polarity_phase_test,  SMOKE_TEST | AUTO_TEST | PRESSURE_TEST},
    {"SPI slv data_mode Test", qmx_spi_slave_data_mode_test,    SMOKE_TEST | AUTO_TEST | PRESSURE_TEST},
    {"SPI slv data_len Test", qmx_spi_slave_data_len_test,      SMOKE_TEST | AUTO_TEST | PRESSURE_TEST},
};

static const soctest_case_t qmx_rng_test_case[] = {
    {"RNG Generate Test",       qmx_rng_generate_test,      SMOKE_TEST | AUTO_TEST | PRESSURE_TEST},
    {"RNG Performance Test",    qmx_rng_perf_test,          AUTO_TEST | PRESSURE_TEST},
    {"RNG Poker Test",          qmx_rng_poker_test,         AUTO_TEST | PRESSURE_TEST},
};

static const soctest_case_t qmx_crc_test_case[] = {
    {"CRC16 Accuracy Test",     qmx_crc16_accuracy_test,    SMOKE_TEST | AUTO_TEST | PRESSURE_TEST},
    {"CRC24 Accuracy Test",     qmx_crc24_accuracy_test,    SMOKE_TEST | AUTO_TEST | PRESSURE_TEST},
    {"CRC32 Accuracy Test",     qmx_crc32_accuracy_test,    SMOKE_TEST | AUTO_TEST | PRESSURE_TEST},
    {"CRC Performance Test",    qmx_crc_performance_test,   AUTO_TEST | PRESSURE_TEST},
    {"CRC Random test",         qmx_crc_random_test,        MANUAL_TEST},
};

static const soctest_case_t qmx_aes_test_case[] = {
    {"AES128 Accuracy Test",    qmx_aes128_accuracy_test,    SMOKE_TEST | AUTO_TEST | PRESSURE_TEST},
    {"AES128 Polling Test",     qmx_aes128_polling_test,     AUTO_TEST | PRESSURE_TEST},
    {"AES128 Performance Test", qmx_aes128_performance_test, AUTO_TEST | PRESSURE_TEST},
    {"AES128 Random Test",      qmx_aes128_random_test,      MANUAL_TEST},
};

static const soctest_case_t  qmx_flash_test_case[] = {
    {"Flash dev id uid Test",           qmx_flash_read_id_test,             SMOKE_TEST | MANUAL_TEST},
    {"Flash Erase Write Read Test",     qmx_flash_erase_write_read_test,    SMOKE_TEST | AUTO_TEST},
    {"Flash QSPI Div Test",             qmx_flash_qspi_div_test,            SMOKE_TEST | AUTO_TEST},
    {"Flash Read Write Mode Test",      qmx_flash_read_write_mode_test,     SMOKE_TEST | AUTO_TEST},
    {"Flash QSPI Mode Test",            qmx_flash_qspi_mode_test,           SMOKE_TEST},
    {"Flash Read Data Capture Test",    qmx_flash_read_data_capture_test,   SMOKE_TEST | AUTO_TEST},
    {"Flash QSPI Delay Test",           qmx_flash_qspi_delay_test,          SMOKE_TEST | AUTO_TEST},
    {"Flash Security Register Test",    qmx_flash_security_register_test,   SMOKE_TEST | AUTO_TEST},
    {"Flash write protect Test",        qmx_flash_protect_test,             MANUAL_TEST | EXCEPTION_TEST},
};

static const soctest_case_t qmx_rtc_test_case[] = {
    {"RTC Accuracy Test",       qmx_rtc_accuracy_test,    SMOKE_TEST | AUTO_TEST | PRESSURE_TEST},
    {"RTC stop Test",           qmx_rtc_stop_test,        SMOKE_TEST | AUTO_TEST | PRESSURE_TEST},
    {"RTC Leap Month Test",     qmx_rtc_leap_month_test,  SMOKE_TEST | AUTO_TEST | PRESSURE_TEST},
    {"RTC ERR BCD Test",        qmx_rtc_err_bcd_test,     SMOKE_TEST | AUTO_TEST | PRESSURE_TEST},
    {"RTC Alarm A Test",        qmx_rtc_alarm_a_test,     SMOKE_TEST | AUTO_TEST | PRESSURE_TEST},
    {"RTC Alarm B Test",        qmx_rtc_alarm_b_test,     SMOKE_TEST | AUTO_TEST | PRESSURE_TEST},
    {"RTC Alarm AB Test",       qmx_rtc_alarm_ab_test,    SMOKE_TEST | AUTO_TEST | PRESSURE_TEST},
};

static const soctest_case_t qmx_timer_test_case[] = {
    {"Timer Accuracy Test",     qmx_timer_accuracy_test,    SMOKE_TEST | AUTO_TEST | PRESSURE_TEST},
    {"Timer Pause Test",        qmx_timer_pause_test,       SMOKE_TEST | AUTO_TEST | PRESSURE_TEST},
};

static const soctest_case_t qmx_lptimer_test_case[] = {
    {"LpTimer Accuracy Test",   qmx_lptimer_accuracy_test,  SMOKE_TEST | AUTO_TEST | PRESSURE_TEST},
    {"LpTimer Pause Test",      qmx_lptimer_pause_test,     SMOKE_TEST | AUTO_TEST | PRESSURE_TEST},
};

static const soctest_case_t qmx_pwm_test_case[] = {
    {"PWM Accuracy Test",       qmx_pwm_timer_accuracy_test, SMOKE_TEST | AUTO_TEST | PRESSURE_TEST},
    {"PWM Pause Test",          qmx_pwm_timer_pause_test,    SMOKE_TEST | AUTO_TEST | PRESSURE_TEST},
    {"PWM output Test",         qmx_pwm_timer_output_test,   MANUAL_TEST | EXCEPTION_TEST},
};

static const soctest_case_t qmx_lowpower_test_case[] = {
    {"sleep phy enable test",   low_power_sleep_phy_enable_test,    SMOKE_TEST | AUTO_TEST | PRESSURE_TEST},
    {"sleep phy disable test",  low_power_sleep_phy_disable_test,   SMOKE_TEST | AUTO_TEST | PRESSURE_TEST},
    {"stop phy enable Test",    low_power_stop_phy_enable_test,     MANUAL_TEST | EXCEPTION_TEST},
    {"stop phy disable Test",   low_power_stop_phy_disable_test,    MANUAL_TEST | EXCEPTION_TEST},
    {"standby Test",            low_power_standby_test,             MANUAL_TEST | EXCEPTION_TEST},
    {"standby flash force on Test", low_power_standby_flash_force_on_test,  MANUAL_TEST | EXCEPTION_TEST},
    {"standby fast on Test",    low_power_faston_test,  MANUAL_TEST | EXCEPTION_TEST},
    {"standby faston forceon Test", low_power_fourceon_faston_test, MANUAL_TEST | EXCEPTION_TEST},
    {"shutdown Test",           low_power_shutdown_test,            MANUAL_TEST | EXCEPTION_TEST},
    {"sleep multiple wakeup Test",   low_power_sleep_multiple_wakeup_test,      MANUAL_TEST | EXCEPTION_TEST},
    {"stop multiple wakeup Test",    low_power_stop_multiple_wakeup_test,       MANUAL_TEST | EXCEPTION_TEST},
    {"standby multiple wakeup Test", low_power_standby_multiple_wakeup_test,    MANUAL_TEST | EXCEPTION_TEST},
    {"stop not wakeup Test",    low_power_stop_not_wakeup_test,     MANUAL_TEST | EXCEPTION_TEST},
    {"standby not wakeup Test", low_power_standby_not_wakeup_test,  MANUAL_TEST | EXCEPTION_TEST},
    {"stop lpio wakeup Test",    low_power_stop_wakeup_by_lpio_test,     MANUAL_TEST | EXCEPTION_TEST},
    {"standby lpio wakeup Test", low_power_standby_wakeup_by_lpio_test,  MANUAL_TEST | EXCEPTION_TEST},
};

static const soctest_case_t qmx_uart_test_case[] = {
    {"UART baudrate Test",  qmx_uart_baudrate_test, SMOKE_TEST | AUTO_TEST | PRESSURE_TEST},
    {"UART stopbit Test",   qmx_uart_stopbit_test,  AUTO_TEST | PRESSURE_TEST},
    {"UART parity Test",    qmx_uart_parity_test,   AUTO_TEST | PRESSURE_TEST},
    {"UART databits Test",  qmx_uart_databits_test, MANUAL_TEST | EXCEPTION_TEST},
    {"UART complex test",   qmx_uart_complex_test,  AUTO_TEST | PRESSURE_TEST},
    {"UART rx_fifo Test",   qmx_uart_rx_fifo_test,  AUTO_TEST | PRESSURE_TEST},
    {"UART rts_cts Test",   qmx_uart_rts_cts_test,  MANUAL_TEST | EXCEPTION_TEST},
    {"UART rs485 Test",     qmx_uart_rs485_test,    MANUAL_TEST | EXCEPTION_TEST},
};

static const soctest_case_t qmx_lpuart_test_case[] = {
    {"LPUART baudrate Test",  qmx_lpuart_baudrate_test, SMOKE_TEST | AUTO_TEST | PRESSURE_TEST},
    {"LPUART stopbit Test",   qmx_lpuart_stopbit_test,  AUTO_TEST | PRESSURE_TEST},
    {"LPUART parity Test",    qmx_lpuart_parity_test,   AUTO_TEST | PRESSURE_TEST},
    {"LPUART complex test",   qmx_lpuart_complex_test,  AUTO_TEST | PRESSURE_TEST},
};

static const soctest_case_t qmx_gpio_test_case[] = {
    {"GPIO output Test",        qmx_gpio_output_test,               SMOKE_TEST | AUTO_TEST | PRESSURE_TEST},
    {"GPIO input Test",         qmx_gpio_input_test,                SMOKE_TEST | AUTO_TEST | PRESSURE_TEST},
    {"GPIO iomux Test",         qmx_gpio_iomux_test,                AUTO_TEST | PRESSURE_TEST},
    {"GPIO levl intc Test",     qmx_gpio_level_interrupt_test,      AUTO_TEST | PRESSURE_TEST},
    {"GPIO riging intc test",   qmx_gpio_rising_interrupt_test,     AUTO_TEST | PRESSURE_TEST},
    {"GPIO falling intc Test",  qmx_gpio_falling_interrupt_test,    AUTO_TEST | PRESSURE_TEST},
    {"GPIO OD Test",            qmx_gpio_od_test,                   MANUAL_TEST},
    {"GPIO filter Test",        qmx_gpio_filter_test,               MANUAL_TEST},
};

static const soctest_case_t qmx_lpio_test_case[] = {
    {"LPIO output Test",        qmx_lpio_output_test,               MANUAL_TEST},
    {"LPIO irq high Test",      qmx_lpio_high_interrupt_test,       MANUAL_TEST},
    {"LPIO irq low Test",       qmx_lpio_low_interrupt_test,        MANUAL_TEST},
};

#define TESTCASE_TABLE_SIZE(case)   (sizeof(case) / sizeof(case[0]))

static const soctest_mod_t g_soctest_mod_list[SOCTEST_MOD_TYPE_MAX] =
{
    [SOCTEST_MOD_NVIC]      = {"NVIC", NULL, NULL, qmx_nvic_test_case, TESTCASE_TABLE_SIZE(qmx_nvic_test_case)},
    [SOCTEST_MOD_CACHE]     = {"CACHE", NULL, NULL, NULL, 0},
    [SOCTEST_MOD_SYSTICK]   = {"SYSTICK", NULL, NULL, qmx_systick_test_case, TESTCASE_TABLE_SIZE(qmx_systick_test_case)},
    [SOCTEST_MOD_EXCPTION]  = {"EXCPTION", NULL, NULL, NULL, 0},
    [SOCTEST_MOD_FLASH]     = {"FLASH", NULL, NULL, qmx_flash_test_case, TESTCASE_TABLE_SIZE(qmx_flash_test_case)},
    [SOCTEST_MOD_MEM]       = {"MEM", NULL, NULL, qmx_mem_test_case, TESTCASE_TABLE_SIZE(qmx_mem_test_case)},
    [SOCTEST_MOD_UART]      = {"UART", qmx_uart_test_init, qmx_uart_test_deinit, qmx_uart_test_case, TESTCASE_TABLE_SIZE(qmx_uart_test_case)},
    [SOCTEST_MOD_IIC]       = {"I2C", qmx_iic_test_init, qmx_iic_test_deinit, qmx_iic_test_case, TESTCASE_TABLE_SIZE(qmx_iic_test_case)},
    [SOCTEST_MOD_SPI]       = {"SPI", qmx_spi_test_init, qmx_spi_test_deinit, qmx_spi_test_case, TESTCASE_TABLE_SIZE(qmx_spi_test_case)},
    [SOCTEST_MOD_GPIO]      = {"GPIO", qmx_gpio_test_init, qmx_gpio_test_deinit, qmx_gpio_test_case, TESTCASE_TABLE_SIZE(qmx_gpio_test_case)},
    [SOCTEST_MOD_WDT]       = {"WDT", NULL, NULL, qmx_wdt_test_case, TESTCASE_TABLE_SIZE(qmx_wdt_test_case)},
    [SOCTEST_MOD_DMA]       = {"DMA", NULL, NULL, qmx_dma_test_case, TESTCASE_TABLE_SIZE(qmx_dma_test_case)},
    [SOCTEST_MOD_RTC]       = {"RTC", NULL, NULL, qmx_rtc_test_case, TESTCASE_TABLE_SIZE(qmx_rtc_test_case)},
    [SOCTEST_MOD_TIMER]     = {"TIMER", NULL, NULL, qmx_timer_test_case, TESTCASE_TABLE_SIZE(qmx_timer_test_case)},
    [SOCTEST_MOD_PWM]       = {"PWM", NULL, NULL, qmx_pwm_test_case, TESTCASE_TABLE_SIZE(qmx_pwm_test_case)},
    [SOCTEST_MOD_RNG]       = {"RNG", NULL, NULL, qmx_rng_test_case, TESTCASE_TABLE_SIZE(qmx_rng_test_case)},
    [SOCTEST_MOD_CRC]       = {"CRC", NULL, NULL, qmx_crc_test_case, TESTCASE_TABLE_SIZE(qmx_crc_test_case)},
    [SOCTEST_MOD_AES]       = {"AES", NULL, NULL, qmx_aes_test_case, TESTCASE_TABLE_SIZE(qmx_aes_test_case)},
    [SOCTEST_MOD_IWDT]      = {"IWDT", NULL, NULL, qmx_iwdt_test_case, TESTCASE_TABLE_SIZE(qmx_iwdt_test_case)},
    [SOCTEST_MOD_LPUART]    = {"LPUART", qmx_lpuart_test_init, qmx_lpuart_test_deinit, qmx_lpuart_test_case, TESTCASE_TABLE_SIZE(qmx_lpuart_test_case)},
    [SOCTEST_MOD_LPTIMER]   = {"LPTIMER", NULL, NULL, qmx_lptimer_test_case, TESTCASE_TABLE_SIZE(qmx_lptimer_test_case)},
    [SOCTEST_MOD_LPIO]      = {"LPIO", qmx_lpio_test_init, qmx_lpio_test_deinit, qmx_lpio_test_case, TESTCASE_TABLE_SIZE(qmx_lpio_test_case)},
    [SOCTEST_MOD_LPWR]      = {"LPWR", NULL, NULL, qmx_lowpower_test_case, TESTCASE_TABLE_SIZE(qmx_lowpower_test_case)},
    [SOCTEST_MOD_RESET]     = {"RESET", NULL, NULL, NULL, 0},
};

static const char* soctest_case_type_str[SOCTEST_CASE_TYPE_ID_MAX] =
{
    [SOCTEST_SMOKE]     = "SMOKE_TEST",
    [SOCTEST_AUTO]      = "AUTO_TEST",
    [SOCTEST_MANUAL]    = "MANUAL_TEST",
    [SOCTEST_PRESSURE]  = "PRESSURE_TEST",
    [SOCTEST_EXCEPTION] = "EXCEPTION_TEST",
};

static void soctest_select_module(void)
{
    uint32_t i;

    if (BIT(g_soctest_record.test_mode) & (SMOKE_TEST | AUTO_TEST | PRESSURE_TEST))
        return;

    g_soctest_record.test_module = SOCTEST_MOD_TYPE_MAX;

    PRINTF("%s\n", SOCTEST_START_LINE);
    PRINTF("QMX SoC Test Module Menu, select module to test:\n");
    PRINTF(" ID \tModule%-10s\tCASE_NUM\n", "");
    for (i = 0; i < SOCTEST_MOD_TYPE_MAX; i++) {
        PRINTF("[%02d]\t%-16s\t%d\n", i, g_soctest_mod_list[i].mod_name, g_soctest_mod_list[i].case_num);
    }

    while (g_soctest_record.test_module >= SOCTEST_MOD_TYPE_MAX);

    PRINTF("you select test module: [%s]\n", g_soctest_mod_list[g_soctest_record.test_module].mod_name);

    PRINTF("%s\n", SOCTEST_END_LINE);
}

static void print_case_types(uint32_t case_type)
{
    bool first = true;
    for (int j = 0; j < SOCTEST_CASE_TYPE_ID_MAX; j++) {
        if (case_type & BIT(j)) {
            if (!first) {
                PRINTF(" | ");
            }
            PRINTF("%s", soctest_case_type_str[j]);
            first = false;
        }
    }
}

static void soctest_select_case(soctest_mod_type_e mod_type)
{
    if (BIT(g_soctest_record.test_mode) & (SMOKE_TEST | AUTO_TEST | PRESSURE_TEST)) {
        g_soctest_record.test_case = 0xFFFFFFFF;
        return;
    }

    g_soctest_record.test_case = 0;

    if (mod_type >= SOCTEST_MOD_TYPE_MAX) {
        PRINTF("Invalid module type %d\n", mod_type);
        return;
    }

    if (g_soctest_mod_list[mod_type].case_num == 0 || g_soctest_mod_list[mod_type].case_list == NULL) {
        PRINTF("No case for module %s\n", g_soctest_mod_list[mod_type].mod_name);
        return;
    }

    PRINTF("%s\n", SOCTEST_START_LINE);

    PRINTF("QMX SoC Test CASE Menu, select case to test:\n");
    PRINTF(" ID\tCase%-44s\tType\n", "");
    for (int i = 0; i < g_soctest_mod_list[mod_type].case_num; i++) {
        PRINTF("BIT[%02d]\t%-48s\t", i, g_soctest_mod_list[mod_type].case_list[i].case_name);
        print_case_types(g_soctest_mod_list[mod_type].case_list[i].case_type);
        PRINTF("\n");
    }

    while (g_soctest_record.test_case == 0);

    PRINTF("%s\n", SOCTEST_END_LINE);
}

void soctest_set_test_mode(uint32_t test_mode)
{
    g_soctest_record.test_mode = test_mode;
}

void soctest_set_test_case(uint32_t case_mask)
{
    g_soctest_record.test_case = case_mask;
}

void soctest_set_test_num(uint32_t test_num)
{
    g_soctest_record.test_num = test_num;
}

void soctest_set_test_module(uint32_t test_module)
{
    g_soctest_record.test_module = test_module;
}

uint32_t soctest_get_test_result(void)
{
    return g_soctest_record.test_result;
}

void soctest_clear_test_result(void)
{
    g_soctest_record.test_result = 0xFFFFFFFF;
}

void soctest_display_test_case(uint32_t case_type)
{
    uint32_t i;

    if (case_type >= SOCTEST_CASE_TYPE_ID_MAX) {
        PRINTF("Invalid case type %d\n", case_type);
        return;
    }

    PRINTF("%s\n", SOCTEST_START_LINE);

    PRINTF("QMX %s case list\n", soctest_case_type_str[case_type]);
    for (i = 0; i < SOCTEST_MOD_TYPE_MAX; i++) {
        if (g_soctest_mod_list[i].case_num == 0 || g_soctest_mod_list[i].case_list == NULL) {
            continue;
        }

        PRINTF("Module: %s\n", g_soctest_mod_list[i].mod_name);
        for (int j = 0; j < g_soctest_mod_list[i].case_num; j++) {
            if (g_soctest_mod_list[i].case_list[j].case_type & case_type)
                PRINTF("    %s\n", g_soctest_mod_list[i].case_list[j].case_name);
        }
    }

    PRINTF("%s\n", SOCTEST_END_LINE);
}

void soctest_select_test_mode(void)
{
    g_soctest_record.test_mode = SOCTEST_CASE_TYPE_ID_MAX;

    PRINTF("%s\n", SOCTEST_START_LINE);

    PRINTF("Select test mode(soctest mode 0/1/...):\n");
    for (int i = 0; i < SOCTEST_CASE_TYPE_ID_MAX; i++) {
        PRINTF("[%02d] %s\n", i, soctest_case_type_str[i]);
    }

    while (g_soctest_record.test_mode >= SOCTEST_CASE_TYPE_ID_MAX);

    PRINTF("you select test mode: [%s]\n", soctest_case_type_str[g_soctest_record.test_mode]);

    PRINTF("%s\n", SOCTEST_END_LINE);
}

void soctest_select_test_num(void)
{
    g_soctest_record.test_num = 0;

    if (g_soctest_record.test_mode != SOCTEST_PRESSURE) {
        g_soctest_record.test_num = 1;
        return;
    }

    PRINTF("%s\n", SOCTEST_START_LINE);

    PRINTF("set test num(soctest num 0x...):\n");

    while (g_soctest_record.test_num == 0);

    PRINTF("you set test num: [0x%X]\n", g_soctest_record.test_num);

    PRINTF("%s\n", SOCTEST_END_LINE);
}

void soctest_run_case(void)
{
    uint32_t i;
    uint32_t test_module_num = 0;
    uint32_t module = g_soctest_record.test_module;
    uint32_t test_mode = BIT(g_soctest_record.test_mode);
    int ret = 0;

    soctest_clear_test_result();

    if (test_mode & (SMOKE_TEST | AUTO_TEST | PRESSURE_TEST)) {
        module = 0;
        test_module_num = SOCTEST_MOD_TYPE_MAX;
    } else {
        module = g_soctest_record.test_module;
        test_module_num = 1;
    }

once:
    for (i = 0; i < g_soctest_mod_list[module].case_num; i++) {
        if ((test_mode == MANUAL_TEST) ||
            (g_soctest_mod_list[module].case_list[i].case_type & test_mode)) {
            if (g_soctest_mod_list[module].mod_init != NULL) {
                g_soctest_mod_list[module].mod_init();
            }

            if (g_soctest_record.test_case & BIT(i)) {
                PRINTF("Start run test case: %s\n", g_soctest_mod_list[module].case_list[i].case_name);
                ret = g_soctest_mod_list[module].case_list[i].case_func();
                if (ret == 0) {
                    g_soctest_record.test_result &= ~BIT(i);
                    PRINTF("Test case [%s] PASS\n", g_soctest_mod_list[module].case_list[i].case_name);
                } else {
                    g_soctest_record.test_result |= BIT(i);
                    PRINTF("Test case [%s] FAIL, ret = %d\n", g_soctest_mod_list[module].case_list[i].case_name, ret);
                }
            }

            if (g_soctest_mod_list[module].mod_deinit != NULL) {
                g_soctest_mod_list[module].mod_deinit();
            }
        }
    }

    module++;
    test_module_num--;
    if (test_module_num != 0)
        goto once;

    if (test_mode & (AUTO_TEST | PRESSURE_TEST)) {
        module = 0;
        test_module_num = SOCTEST_MOD_TYPE_MAX;
    } else {
        module = g_soctest_record.test_module;
        test_module_num = 1;
    }

    if (g_soctest_record.test_num > 1) {
        g_soctest_record.test_num--;
        goto once;
    }
}

void soctest_main(void)
{
    memset((void *)&g_soctest_record, 0, sizeof(soctest_record_t));

start:
    soctest_select_test_mode();

    soctest_select_test_num();

    soctest_select_module();

    soctest_select_case((soctest_mod_type_e)g_soctest_record.test_module);

    soctest_run_case();

    goto start;
}
