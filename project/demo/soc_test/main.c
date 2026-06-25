#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <math.h>

#include "reg_Dev.h"
#include "error_def.h"
#include "utility.h"
#include "debug.h"
#include "slc_hal_uart.h"
#include "slc_hal_sysctrl.h"
#include "app_cfg.h"
#include "slc_hal_intc.h"
#include "slc_hal_delay.h"
#include "slc_cali.h"
#include "slc_lpwr_ctrl.h"
#include "slc_hal_pmu.h"
#include "slc_phy_test.h"
#include "slc_soc_test.h"
#include "compiler.h"
#include "slc_intc.h"
#include "slc_test_slave.h"
#include "slc_private_spi_frame.h"

#ifdef SLC_SOCTEST
#include "slc_hal_iwdt.h"

static void slc_iwdt_dump_regs(const char *tag)
{
    PRINTF("\r\n[IWDT regs %s]\r\n", tag);
    PRINTF("  WDT_WPR  = 0x%08X\r\n", IWDT->WDT_WPR);
    PRINTF("  WDT_SCR  = 0x%08X\r\n", IWDT->WDT_SCR);
    PRINTF("  WDT_LDR  = 0x%08X\r\n", IWDT->WDT_LDR);
    PRINTF("  WDT_CDR  = 0x%08X\r\n", IWDT->WDT_CDR);
    PRINTF("  WDT_IER  = 0x%08X\r\n", IWDT->WDT_IER);
    PRINTF("  WDT_IMR  = 0x%08X\r\n", IWDT->WDT_IMR);
    PRINTF("  WDT_ISR  = 0x%08X\r\n", IWDT->WDT_ISR);
    PRINTF("  WDT_TSR  = 0x%08X\r\n", IWDT->WDT_TSR);
    PRINTF("  WDT_FDR  = 0x%08X\r\n", IWDT->WDT_FDR);
    PRINTF("  IWDT_DLL = 0x%08X\r\n", IWDT->IWDT_DLL);
    PRINTF("  IWDT_TER = 0x%08X\r\n", IWDT->IWDT_TER);
}
#endif

#ifdef SLC_AUTOTEST
#include "slc_uart_cmd_process.h"
#endif

#ifdef NT_SHELL
#include "nttop.h"

extern uint8_t g_lp_fail_flag;
uint32_t g_test_nmi_continue_flag __RETENTION_DATA = 0;

void NMI_Handler_Proc(uint32_t u32ExcReturn, uint32_t u32Msp, uint32_t u32Psp)
{
    if(g_test_nmi_continue_flag) {
        g_lp_fail_flag = slc_hal_pmu_get_lp_fail_flag();
    } else {
        PRINTF("@ NMI handler......\n");
        volatile uint32_t *u32Sp = NULL;
        if (u32ExcReturn & 0x04) {
            PRINTF("use PSP, ");
            u32Sp = (volatile uint32_t *)u32Psp;
        } else {
            PRINTF("use MSP, ");
            u32Sp = (volatile uint32_t *)u32Msp;
        }
        PRINTF("and stack frame:\n");
        PRINTF("R0  : 0x%08X\n", u32Sp[0]);
        PRINTF("R1  : 0x%08X\n", u32Sp[1]);
        PRINTF("R2  : 0x%08X\n", u32Sp[2]);
        PRINTF("R3  : 0x%08X\n", u32Sp[3]);
        PRINTF("R12 : 0x%08X\n", u32Sp[4]);
        PRINTF("LR  : 0x%08X\n", u32Sp[5]);
        PRINTF("PC  : 0x%08X\n", u32Sp[6]);
        PRINTF("xPSR: 0x%08X\n", u32Sp[7]);
        
        PRINTF("An unexpected exception, manual reset needed.\n");
        while(1);
    }
}

__RETENTION_FUNC __attribute__((interrupt)) void NMI_Handler_in_RAM(void)
{
    if(g_test_nmi_continue_flag) {
        g_lp_fail_flag = slc_hal_pmu_get_lp_fail_flag();
        slc_hal_nop_delay_us(10);
    } else {
        PRINTF("An unexpected exception, manual reset needed.\n");
        while(1);
    }
}

void slc_debug_uart_irq_handler(void)
{
    uint32_t sta __MAYBE_UNUSED = 0;
    sta = slc_hal_uart_get_irq_status(DEBUG_UART_HANDLE);
    ntshell_top_run_once();
}
#endif

static void slc_rf_spi_rw_test(void)
{
    const uint16_t spi_addr = SLC_RF_SPI_ADDR_PMU(0x20); /* RF_PMU->DCDC_CTRL */
    const uint32_t ahb_addr = ADDR_RF_PMU_BASE + 0x20U;
    uint32_t orig_spi;
    uint32_t orig_bus;
    uint32_t test_val;
    uint32_t rb_spi;
    uint32_t rb_bus;

    orig_spi = slc_rf_spi_read32_cmd(spi_addr);
    orig_bus = read32(ahb_addr);
    test_val = (orig_spi & ~SLC_DCDC_OSC_CAPTRIM_MASK) | SLC_DCDC_OSC_CAPTRIM_VAL(0x1A);

    PRINTF("\r\n[RF SPI RW test] DCDC_CTRL\r\n");
    PRINTF("  spi_addr=0x%04X, ahb=0x%08X\r\n", spi_addr, ahb_addr);
    PRINTF("  before : spi=0x%08X, bus=0x%08X\r\n", orig_spi, orig_bus);

    slc_rf_spi_write32_cmd(spi_addr, test_val);
    rb_spi = slc_rf_spi_read32_cmd(spi_addr);
    rb_bus = read32(ahb_addr);
    PRINTF("  write  : 0x%08X\r\n", test_val);
    PRINTF("  after  : spi=0x%08X, bus=0x%08X\r\n", rb_spi, rb_bus);
    PRINTF("  spi rw : %s\r\n", (rb_spi == test_val) ? "PASS" : "FAIL");
    PRINTF("  spi=bus: %s\r\n", (rb_spi == rb_bus) ? "PASS" : "FAIL");

    slc_rf_spi_write32_cmd(spi_addr, orig_spi);
    PRINTF("  restore: spi=0x%08X\r\n", slc_rf_spi_read32_cmd(spi_addr));
}

void slc_platform_init(void)
{
    slc_hal_sysctrl_cache_mode_set(HAL_CACHE_ENABLE);

#ifndef SLC_FPGA
    // TODO: MPW规避standby起来后RF未复位的异常，待CS优化后删除
    slc_hal_sysctrl_reset_phy();

    slc_hal_pmu_phy_power_enable(true);
    slc_private_spi_init();
    // vtcxo bypass，使用vBAT供电，切25M之前配置，防止TCXO未工作，切换后CPU卡死
    slc_rf_tcxo_bypass(true);

    // 校准时切换到TCXO25M，可正常使用外设等，否则时钟波动可能导致打印等异常
    slc_hal_sysctrl_system_clock_init(HAL_SYSCLK_TCXO25M, HAL_SYSCLK_DIV_NONE);

#if APP_DEBUG_ENABLED
    debug_printf_init();
#endif

    PRINTF("FW start to CALI\n");
    if (slc_cali_init(SLC_CALI_DC_IQ) != 0) {
        PRINTF("FW CALI fail\n");
    }
#endif

#ifdef SLC_FPGA
    slc_hal_pmu_phy_power_enable(true);
    slc_private_spi_init();
#endif

    // 校准结束切换回RC50M，需重新进行UART初始化
    slc_hal_sysctrl_system_clock_init(HAL_SYSCLK_FDB50M, HAL_SYSCLK_DIV_NONE);

#if APP_DEBUG_ENABLED
    debug_printf_init();
    PRINTF("reset_src: %x\n",slc_hal_sysctrl_sw_get_reset_src());
#ifdef NT_SHELL

#ifdef SLC_SOCTEST
    slc_iwdt_dump_regs("before deinit");
    // IWDT survives system reset; always deinit before any test or shell. 
    //slc_hal_iwdt_deinit();
#endif

    slc_hal_register_irq_handler(DEBUG_UART_IRQ, slc_debug_uart_irq_handler);
    slc_hal_uart_enable_irq(DEBUG_UART_HANDLE, HAL_UART_INT_EN_RX_NOT_EMPTY);
    SLC_HAL_ENABLE_PERIPHERAL_IRQ(DEBUG_UART_IRQ, 0x3);

    ntshell_top_init();
#endif
    slc_rf_spi_rw_test();
#endif

    PRINTF("FW start to work(freq: %dMHz). Build Time:[%s T %s].\n",
            slc_hal_sysctrl_get_system_clock() / 1000000, __DATE__, __TIME__);
}

void reset_verification(void)
{
    uint8_t rst_cause = slc_hal_sysctrl_get_reset_src();
    switch(rst_cause) {
        case HAL_MORMAL_POWERUP:
            PRINTF("Reset from PowerOn\n");
            break;
        case HAL_RST_FROM_WDT:
            PRINTF("Reset from WDT timeout\n");
            break;
        case HAL_RST_FROM_IWDT:
            PRINTF("Reset from IWDT timeout\n");
            break;
        case HAL_RST_FROM_SOFT:
            PRINTF("Reset from NVIC reset\n");
            break;
        default:
            PRINTF("Reset from unknown reason: 0x%02X\n", rst_cause);
            break;
    }
    slc_register_nmi_handler(NMI_Handler_in_RAM);
}


int main(void)
{


    slc_check_boot_status();

#ifdef SLC_AUTOTEST
    slc_uart_cmd_process_main();
#endif // SLC_AUTOTEST

    slc_platform_init();
    reset_verification();

#ifdef SLC_PHYTEST
    slc_phy_test();
#endif // SLC_PHYTEST

#ifdef SLC_SOCTEST
#ifndef SLC_TEST_SLAVE
    soctest_main();
#else
    slc_test_slave_main();
#endif // SLC_TEST_SLAVE
#endif // SLC_SOCTEST

    while (1);
#ifdef __CC_ARM
#pragma diag_suppress 111
#endif
    return 0;
}
