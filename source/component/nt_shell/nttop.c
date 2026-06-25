#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#include "ntshell.h"
#include "ntlibc.h"
#include "nttop.h"
#include "ntopt.h"

#include "reg_Dev.h"
#include "error_def.h"
#include "utility.h"
#include "slc_hal_sysctrl.h"
#include "slc_hal_uart.h"
#include "app_cfg.h"
#include "slc_hal_intc.h"
#include "slc_hal_delay.h"

#include "slc_hal_pmu.h"
#include "slc_phy.h"
#include "slc_rf.h"
#include "slc_phy_trx.h"
#include "slc_subg.h"
#include "slc_cad.h"
#include "slc_afc.h"
#include "slc_sf_search.h"
#include "slc_debug_ila.h"

#include "slc_private_spi_frame.h"
#include "slc_hal_rng.h"
#include "slc_cali.h"

#include "slc_lpwr_ctrl.h"

#ifdef SLC_SOCTEST
#include "slc_soc_test.h"
#endif

#ifdef SLC_PHYTEST
#include "slc_phy_test.h"
#endif

#define NT_SHELL_UART_ID        DEBUG_UART_HANDLE
#define NT_SHELL_UART_TIMEOUT   5000

typedef struct {
    char cmd[NT_SHELL_CMD_WIDTH];
    char desc[NT_SHELL_CMD_DESC_WIDTH];
    USRCMDFUNC func;
} cmd_table_t;

static cmd_table_t cmdlist[NT_SHELL_CMDLIST_NUMBER] = {0};
ntshell_t ntshell;

#define UNUSED_VARIABLE(N)  do { (void)(N); } while (0)

static void nt_shell_printf(char *str)
{
    uint16_t len = ntlibc_strlen(str);
    slc_hal_uart_send_data(NT_SHELL_UART_ID, (uint8_t *)str, len, NT_SHELL_UART_TIMEOUT);
}

static int usrcmd_help(int argc, char **argv)
{
    const cmd_table_t *p = cmdlist;

    for (int i = 0; i < NT_SHELL_CMDLIST_NUMBER; i++, p++) {
        if (p->cmd[0] == '\0')
            continue;

        nt_shell_printf((char *)p->cmd);
        nt_shell_printf("\r\n");
        nt_shell_printf("    --");
        nt_shell_printf((char *)p->desc);
        nt_shell_printf("\r\n");
    }

    return 0;
}

static int usrcmd_write(int argc, char **argv)
{
    uint32_t addr=0;
    uint32_t data=0;
    uint8_t start_bit=0;
    uint8_t end_bit=0;

    if(argc <3)
        return -1;

    addr = ntlibc_atoi(argv[1], 16);
    data = ntlibc_atoi(argv[2], 16);

    if (argc == 3) {
        write32(addr, data);
        PRINTF("WR: 0x%x 0x%x\r\n", addr, data);
        return 0;
    }

    if (argc != 5) {
        PRINTF("Usage: wr addr data start_bit end_bit\r\n");
        PRINTF("Example: wr 0x40010000 0x12 16 31\r\n");
        return -1;
    }

    start_bit = (uint8_t)ntlibc_atoi(argv[3], 10);
    end_bit = (uint8_t)ntlibc_atoi(argv[4], 10);
    set_bits((uint32_t *)addr, start_bit, end_bit, data);
    PRINTF("WR bits: 0x%X[%d:%d]=0x%X\r\n", addr, end_bit, start_bit, data);

    return 0;
}
static int usrcmd_read(int argc, char **argv)
{
    uint32_t addr=0;
    uint32_t data=0;
    uint8_t start_bit=0;
    uint8_t end_bit=0;

    if(argc < 2)
        return -1;

    addr = ntlibc_atoi(argv[1], 16);

    if (argc == 2) {
        data = read32(addr);
        PRINTF("RD: 0x%X=0x%X\r\n", addr, data);
        return 0;
    }

    if (argc != 4) {
        PRINTF("Usage: rd addr start_bit end_bit\r\n");
        PRINTF("Example: rd 0x40010000 16 31\r\n");
        return -1;
    }

    start_bit = (uint8_t)ntlibc_atoi(argv[2], 10);
    end_bit = (uint8_t)ntlibc_atoi(argv[3], 10);
    data = get_bits((uint32_t *)addr, start_bit, end_bit);
    PRINTF("RD bits: 0x%X[%d:%d]=0x%X\r\n", addr, end_bit, start_bit, data);

    return 0;
}

static int usrcmd_subg_write(int argc, char **argv)
{
    uint32_t addr=0;
    uint32_t data=0;
    uint8_t start_bit=0;
    uint8_t end_bit=0;

    if (argc < 3)
        return -1;

    addr = ntlibc_atoi(argv[1], 16);
    data = ntlibc_atoi(argv[2], 16);

    if (argc == 3) {
        slc_rf_spi_write32_cmd(addr, data);
        PRINTF("SUBG WR: 0x%x 0x%x\r\n", addr, data);
        return 0;
    }

    if (argc != 5) {
        PRINTF("Usage: subg_wr addr data start_bit end_bit\r\n");
        PRINTF("Example: subg_wr 0x40010000 0x12 16 31\r\n");
        return -1;
    }

    start_bit = (uint8_t)ntlibc_atoi(argv[3], 10);
    end_bit = (uint8_t)ntlibc_atoi(argv[4], 10);
    slc_rf_spi_set_bits(addr, start_bit, end_bit, data);
    PRINTF("SUBG WR bits: 0x%X[%d:%d]=0x%X\r\n", addr, end_bit, start_bit, data);

    return 0;

}
static int usrcmd_subg_read(int argc, char **argv)
{
    uint32_t addr=0;
    uint32_t data=0;
    uint8_t start_bit=0;
    uint8_t end_bit=0;

    if(argc < 2)
        return -1;

    addr = ntlibc_atoi(argv[1], 16);

    if (argc == 2) {
        data = slc_rf_spi_read32_cmd(addr);
        PRINTF("SUBG RD: 0x%X=0x%X\r\n", addr, data);
        return 0;
    }

    if (argc != 4) {
        PRINTF("Usage: subg_rd addr start_bit end_bit\r\n");
        PRINTF("Example: subg_rd 0x40010000 16 31\r\n");
        return -1;
    }

    start_bit = (uint8_t)ntlibc_atoi(argv[2], 10);
    end_bit = (uint8_t)ntlibc_atoi(argv[3], 10);
    data = slc_rf_spi_get_bits(addr, start_bit, end_bit);
    PRINTF("SUBG RD bits: 0x%X[%d:%d]=0x%X\r\n", addr, end_bit, start_bit, data);

    return 0;
}

#ifdef SLC_PHYTEST
uint8_t phy_tx_buf[256];
static int usrcmd_phy_trx_cfg(int argc, char **argv)
{
    phy_cfg_trx_e trx_mode;
    phy_cfg_tx_mode_e tx_mode;
    slc_rf_flo_e flo;
    phy_cfg_t cfg;
    uint16_t netid;
    uint32_t freq;

    if (argc < 11) {
        nt_shell_printf("param num error, at least 11!\r\n");
        nt_shell_printf("phy_cfg rx/tx_iq/tx_polar freq netid sf bw header crc coderate payload_len preamble_num\r\n");
        return -1;
    }

    // FPGA上CAD休眠后会复位PHY寄存器，需重新使能PHY中断
    slc_phy0_irq_enable(PHY_IRQ_ALL_MASK);

    for (uint16_t i = 0; i < sizeof(phy_tx_buf); i++) {
        phy_tx_buf[i] = (uint8_t)((i & 0xFF) + 0x55);
    }

    if (ntlibc_strcmp((const char *)argv[1], "rx") == 0) {
        trx_mode = PHY_RX_EN;
        tx_mode = PHY_TX_IQ;
    } else if (ntlibc_strcmp((const char *)argv[1], "tx_iq") == 0) {
        trx_mode = PHY_TX_EN;
        tx_mode = PHY_TX_IQ;
    } else {
        trx_mode = PHY_TX_EN;
        tx_mode = PHY_TX_POLAR;
    }

    flo = (slc_rf_flo_e)ntlibc_atoi(argv[2], 10);
    freq = ntlibc_atoi(argv[3], 10);
    netid = (uint16_t)ntlibc_atoi(argv[4], 10);
    cfg.sf = (uint16_t)ntlibc_atoi(argv[5], 10);
    cfg.bw = (uint16_t)ntlibc_atoi(argv[6], 10);
    cfg.header = (uint16_t)ntlibc_atoi(argv[7], 10);
    cfg.crc = (uint16_t)ntlibc_atoi(argv[8], 10);
    cfg.coderate = (uint16_t)ntlibc_atoi(argv[9], 10);
    cfg.payload_len = (uint16_t)ntlibc_atoi(argv[10], 10);
    cfg.preamble_num = (uint16_t)ntlibc_atoi(argv[11], 10);

    PRINTF("phy %s config: flo=%d, freq=%d, netid=%d, sf=%d, bw=%d, header=%d, crc=%d, cr=%d payload_len=%d, preamble_num=%d\r\n",
           trx_mode == PHY_RX_EN ? "rx" : "tx", flo, freq, netid,
           cfg.sf, cfg.bw, cfg.header, cfg.crc, cfg.coderate, cfg.payload_len, cfg.preamble_num);

    slc_phy_trx_config(&cfg, netid, trx_mode, flo, freq, tx_mode);

    slc_phy_write_tx_buffer(phy_tx_buf, cfg.payload_len);

    return 0;
}

__RETENTION_FUNC void phy_test_wake_up(void)
{
    // 睡前拉低，醒后拉高，统计唤醒时间
    slc_hal_sysctrl_peripheral_clk_enable(HAL_CLK_GPIO, true);

#if APP_DEBUG_ENABLED
    debug_printf_init();
#endif

#ifdef NT_SHELL
    extern void slc_debug_uart_irq_handler(void);
    slc_hal_register_irq_handler(DEBUG_UART_IRQ, slc_debug_uart_irq_handler);
    slc_hal_uart_enable_irq(DEBUG_UART_HANDLE, HAL_UART_INT_EN_RX_NOT_EMPTY);
    SLC_HAL_ENABLE_PERIPHERAL_IRQ(DEBUG_UART_IRQ, 0x3);

    ntshell_top_init();

    //QMX_HAL_ENABLE_GLOBAL_IRQ();
    
#endif
}


static int usrcmd_phy_trigger_trx(int argc, char **argv)
{
    phy_cfg_trx_e trx_mode = PHY_RX_EN;
    phy_cfg_tx_mode_e tx_mode = PHY_TX_POLAR;
    uint8_t phy_test_mode = PHY_MODE_NORMAL;
    uint8_t afc_en = 0;
    uint32_t tx_pkg_num = 1;
    uint32_t tx_delay = 0;
    char *phy_mode_str[3] = {"normal", "cad", "duty_cycle"};
    rf_power_e power = RF_PA_GAIN_P20DB;

    if (argc < 4) {
        nt_shell_printf("param num error, at least 4!\r\n");
        nt_shell_printf("phy_trigger rx/tx_iq/tx_polar 0/1/2 0/1 num dBm\r\n");
        return -1;
    }

    if (ntlibc_strcmp((const char *)argv[1], "rx") == 0) {
        trx_mode = PHY_RX_EN;
        tx_mode = PHY_TX_IQ;
    } else if (ntlibc_strcmp((const char *)argv[1], "tx_iq") == 0) {
        trx_mode = PHY_TX_EN;
        tx_mode = PHY_TX_IQ;
    } else {
        trx_mode = PHY_TX_EN;
        tx_mode = PHY_TX_POLAR;
    }

    phy_test_mode = (uint8_t)ntlibc_atoi(argv[2], 10);
    if (phy_test_mode > PHY_MODE_DUTY_CYCLE) {
        PRINTF("param error, mode=%d need less than 3\r\n", phy_test_mode);
        return -1;
    }

    afc_en = (uint8_t)ntlibc_atoi(argv[3], 10);
    if (afc_en > 1) {
        PRINTF("param error, afc_en=%d only support 0/1\r\n", afc_en);
        return -1;
    }

    if (argc > 4) {
        tx_pkg_num = (uint32_t)ntlibc_atoi(argv[4], 10);
        PRINTF("tx pkg num=%d\r\n", tx_pkg_num);
    }
    if (argc > 5) {
        power = (uint32_t)ntlibc_atoi(argv[5], 10);
        PRINTF("tx power = %d\r\n", power);
    }

    PRINTF("phy %s %s(%s) start(afc %s)...\r\n", phy_mode_str[phy_test_mode],
            trx_mode == PHY_RX_EN ? "rx" : "tx", tx_mode == PHY_TX_IQ ? "iq" : "polar",
            afc_en ? "enable" : "disable");

    if (trx_mode == PHY_TX_EN)
        slc_phy_start_trans_tx_data(tx_mode, power);

start:
    slc_phy_test_set_flag(0);
    if (trx_mode == PHY_TX_EN) {
        if (afc_en)
            slc_afc_adjust(PHY_TX_EN, tx_mode);

        if (phy_test_mode == PHY_MODE_CAD)
            slc_cad_mode_enable(CAD_NORMAL_MODE);
        else if (phy_test_mode == PHY_MODE_DUTY_CYCLE)
            slc_cad_mode_enable(CAD_DUTY_CYCLE_MODE);

        slc_phy_tx_trigger_single();
    } else {
        if (afc_en)
            slc_afc_adjust(PHY_RX_EN, tx_mode);

        slc_phy_start_get_rx_data();

        if (phy_test_mode == PHY_MODE_CAD)
            slc_cad_mode_enable(CAD_NORMAL_MODE);
        else if (phy_test_mode == PHY_MODE_DUTY_CYCLE)
            slc_cad_mode_enable(CAD_DUTY_CYCLE_MODE);
    }

    tx_pkg_num--;
    // SF搜索的测试需求，确保phy中断优先级高于uart
    if (tx_pkg_num > 0) {
        slc_hal_get_random_u32(&tx_delay, 4, 1000);
        tx_delay = (tx_delay < 100) ? (tx_delay+100) : (tx_delay % 1000);
        if (trx_mode == PHY_RX_EN)
            tx_delay = 10;  // 确保小于发送的间隔，大于收包的时长
        while (slc_phy_test_get_flag() == 0);
        slc_hal_nop_delay_ms(tx_delay);
        goto start;
    }

    if (argc > 4)
        PRINTF("all pkg trx trigger done\r\n");

    return 0;
}

static int usrcmd_phy_cad_cfg(int argc, char **argv)
{
    uint8_t cad_type = 0;
    pre_cad_cfg_t cfg;
    uint16_t check_symbol_num;
    uint16_t duty_cycle_period;
    slc_lpwr_ctrl_cfg lpcfg;
    uint8_t sleep_mode = HAL_PMU_LP_MODE_NORMAL;
    uint32_t phy_test_irq_mask;
    char *argv1[] = {"phy_trigger", "rx", "1", "0"};
    int argc1 = 4;
    uint8_t no_demod_data;

    if (argc < 4) {
        nt_shell_printf("param num error, at least 4!\r\n");
        nt_shell_printf("cad_cfg type(0: CAD; 1: PRE CAD) check_symbol_num duty_cycle_period\r\n");
        return -1;
    }

    lpcfg.lp_wakeup_src_msk = HAL_PMU_LP_WAKEUP_SRC_MSK_PHY0;
    lpcfg.phy_power_enable = true;
    lpcfg.rf_power_enable = true;
    lpcfg.flash_force_on = false;
    lpcfg.lp_before_cb_func = NULL;
    lpcfg.lp_after_cb_func = phy_test_wake_up;
    slc_lpwr_ctrl_init(&lpcfg);

    cad_type = (uint8_t)ntlibc_atoi(argv[1], 10);
    check_symbol_num = ntlibc_atoi(argv[2], 10);
    duty_cycle_period = (uint16_t)ntlibc_atoi(argv[3], 10);
    no_demod_data = (uint8_t)ntlibc_atoi(argv[4], 10);
    if((argc > 5) && (cad_type == 0)){
        sleep_mode = (uint8_t)ntlibc_atoi(argv[5], 10);
        phy_test_irq_mask = (uint32_t)ntlibc_atoi(argv[6], 10);
    }

    PRINTF("cad config: type=%s, check_symbol_num=%d, duty_cycle_period=%d\r\n",
            (cad_type == 0) ? "CAD" : "PRE_CAD", check_symbol_num, duty_cycle_period);

    if (cad_type == 0) {
        slc_cad_cfg(check_symbol_num, duty_cycle_period, NULL ,no_demod_data);
        if(sleep_mode){
            slc_phy0_irq_disable(PHY_IRQ_ALL_MASK);
            slc_phy0_irq_enable(phy_test_irq_mask);
            usrcmd_phy_trigger_trx(argc1, argv1);
            slc_lpwr_ctrl_sleep(sleep_mode);
        }
        return 0;
    }

    if ((cad_type == 1) && argc < 10) {
        nt_shell_printf("pre cad mode param need at least 10!\r\n");
        nt_shell_printf("eg: cad_cfg 1(precad) check_symbol_num duty_cycle_period");
        nt_shell_printf(" pre_addr pre_num cycle matchaddr_wide marker nosleep_cycle\r\n");
        return -1;
    }

    cfg.pre_addr = (uint16_t)ntlibc_atoi(argv[5], 16);
    cfg.pre_num = (uint16_t)ntlibc_atoi(argv[6], 10);
    cfg.cycle_num = (uint16_t)ntlibc_atoi(argv[7], 10);
    cfg.matchaddr_wide = (uint16_t)ntlibc_atoi(argv[8], 10);
    cfg.marker = (uint16_t)ntlibc_atoi(argv[9], 10);
    cfg.nosleep_cycle = (uint16_t)ntlibc_atoi(argv[10], 10);
    if(argc > 11){
        sleep_mode = (uint8_t)ntlibc_atoi(argv[11], 10);
        phy_test_irq_mask = (uint32_t)ntlibc_atoi(argv[12], 10);
    }

    PRINTF("pre cad config: pre_addr=0x%X, pre_num=%d, cycle_num=%d, matchaddr_wide=%d, marker=%d, nosleep_cycle=%d\r\n",
           cfg.pre_addr, cfg.pre_num, cfg.cycle_num, cfg.matchaddr_wide, cfg.marker, cfg.nosleep_cycle);

    slc_cad_cfg(check_symbol_num, duty_cycle_period, &cfg ,no_demod_data);
    if(sleep_mode){
        slc_phy0_irq_disable(PHY_IRQ_ALL_MASK);
        slc_phy0_irq_enable(phy_test_irq_mask);
        usrcmd_phy_trigger_trx(argc1, argv1);
        slc_lpwr_ctrl_sleep(sleep_mode);
    }

    return 0;
}

static int usrcmd_phy_total_cnt(int argc, char **argv)
{
    slc_phy_test_display_cnt();

    return 0;
}

static int usrcmd_phy_single_tone(int argc, char **argv)
{
    slc_phy_single_tone();
    return 0;
}

static int usrcmd_phy_cad_exit(int argc, char **argv)
{
    PRINTF("cad exit duty cycle\r\n");

    slc_cad_duty_cycle_disable();

    return 0;
}

static int usrcmd_phy_wakeup(int argc, char **argv)
{
    int ret = 0;

    // 上升沿唤醒
    slc_hal_pmu_phy_power_enable(false);

    ret = slc_hal_pmu_phy_power_enable(true);
    if (ret != 0) {
        PRINTF("phy wakeup failed\r\n");
        return -1;
    }
    ret += slc_clk_cali(SLC_CALI_DCDC1M);
    ret += slc_clk_cali(SLC_CALI_RC32K);
    ret += slc_clk_cali(SLC_CALI_RC50M);
    if (ret != 0) {
        PRINTF("clk cali failed\r\n");
        return -1;
    }

    PRINTF("phy wakeup success\r\n");
    return 0;
}

static int usrcmd_cpu_reset(int argc, char **argv)
{
    NVIC_SystemReset();
    return 0;
}

static int usrcmd_sfsearch_cfg(int argc, char **argv)
{
    int ret = 0;
    uint32_t sf_en, sf_num;
    float sf1 = 0.0f;
    float sf2 = 0.0f;
    float sf3 = 0.0f;
    uint8_t sfsearch_len;

    if (argc < 2) {
        nt_shell_printf("param num error, at least 2!\r\n");
        nt_shell_printf("sfsearch 0/1(0:disable, 1:enable)\r\n");
        return -1;
    }

    sf_en = (uint32_t)ntlibc_atoi(argv[1], 10);

    if ((sf_en !=0) && (argc < 6)) {
        nt_shell_printf("param num error, at least 6!\r\n");
        nt_shell_printf("sfsearch 1 num 1.5 1.5 1.5\r\n");
        return -1;
    }

    if (sf_en != 0) {
        sf_num = (uint32_t)ntlibc_atoi(argv[2], 10);
        sf1 = (float)atof(argv[3]);
        sf2 = (float)atof(argv[4]);
        sf3 = (float)atof(argv[5]);
        sfsearch_len = (uint8_t)ntlibc_atoi(argv[6], 10);
        slc_sf_search_cfg(sf_num, sf1, sf2, sf3, sfsearch_len);
        PRINTF("sfsearch config: sf_num=%d, beishu1=%.03f, beishu2=%.03f, beishu3=%.03f\r\n",
                sf_num, sf1, sf2, sf3);
    }

    PRINTF("sfsearch status: %s\r\n",
           (sf_en==0) ? "disable" : "enable");

    slc_sf_search_enable(sf_en);

    return 0;
}
#endif

#ifdef SLC_SOCTEST
static int usrcmd_soctest_cfg(int argc, char **argv)
{
    uint32_t val;

    if (argc < 3) {
        nt_shell_printf("param num error, at least 3!\r\n");
        nt_shell_printf("soctest mode/num/module/case 1\r\n");
        return -1;
    }

    if (ntlibc_strcmp((const char *)argv[1], "mode") == 0) {
        val = (uint32_t)ntlibc_atoi(argv[2], 10);
        soctest_set_test_mode(val);
    } else if (ntlibc_strcmp((const char *)argv[1], "num") == 0) {
        val = (uint32_t)ntlibc_atoi(argv[2], 16);
        soctest_set_test_num(val);
    } else if (ntlibc_strcmp((const char *)argv[1], "module") == 0) {
        val = (uint32_t)ntlibc_atoi(argv[2], 10);
        soctest_set_test_module(val);
    } else if (ntlibc_strcmp((const char *)argv[1], "case") == 0) {
        val = (uint32_t)ntlibc_atoi(argv[2], 16);
        soctest_set_test_case(val);
    } else {
        nt_shell_printf("param error!\r\n");
        nt_shell_printf("soctest mode/num/module/case 1\r\n");
        return -1;
    }

    return 0;
}
#endif

#ifdef SLC_ILATEST
uint32_t g_ila_data __attribute__((aligned(4), section(".ram.ila")));
#define RAM_ILA_START_ADDR  ((uint32_t)&g_ila_data)
#define RAM_ILA_SIZE        (RAM_APP_DATA_MAX_ADDR - RAM_ILA_START_ADDR)

void slc_debug_ila_test_irq(void)
{
    slc_debug_ila_clear_status();
    PRINTF("ILA irq\n");
}

static int usrcmd_ila_cfg(int argc, char **argv)
{
    int ret = 0;
    ila_cfg_t ila_cfg = {0};
    hal_uart_init_t config = {0};

    if (argc < 2) {
        PRINTF("param num error, at least 3!\r\n");
        PRINTF("ila_cfg start/end ...\r\n");
        return -1;
    }

    if (ntlibc_strcmp((const char *)argv[1], "end") == 0) {
        slc_hal_gpio_set_iomux(HAL_GPIO_PIN2, HAL_IOMUX_MODE0);
        slc_hal_gpio_set_iomux(HAL_GPIO_PIN3, HAL_IOMUX_MODE0);
        slc_hal_sysctrl_peripheral_clk_enable(HAL_CLK_UART1, true);
        slc_hal_sysctrl_peripheral_mod_reset(HAL_CLK_UART1);

        config.baudrate = 921600;
        config.parity = HAL_UART_PARITY_NONE;
        config.stopbit = HAL_UART_STOPBIT_1;
        config.databits = HAL_UART_DATA_8BIT;

        config.tx_fifo_thld = HAL_UART_TXFIFO_THLD_EMPTY;
        config.rx_fifo_thld = HAL_UART_RXFIFO_THLD_1BYTE;
        config.fifo_en = true;
        config.flow_ctrl_en = false;

        slc_hal_uart_init(HAL_UART1, &config);

        slc_debug_ila_start(false);
        slc_hal_uart_send_data(HAL_UART1, (uint8_t *)RAM_ILA_START_ADDR, RAM_ILA_SIZE, HAL_UART_TIMEOUT_US);
        return 0;
    }

    if (ntlibc_strcmp((const char *)argv[1], "start") != 0 || argc < 7) {
        PRINTF("param error! ila_cfg start clk_src trigger_src data_src delay trigger_type\r\n");
        return -1;
    }

    ila_cfg.addr = RAM_ILA_START_ADDR;
    ila_cfg.size = RAM_ILA_SIZE;
    ila_cfg.clk_src = (uint8_t)ntlibc_atoi(argv[2], 10);
    ila_cfg.trigger_src = (uint8_t)ntlibc_atoi(argv[3], 10);
    ila_cfg.data_src = (uint8_t)ntlibc_atoi(argv[4], 10);
    ila_cfg.trigger_delay = (uint8_t)ntlibc_atoi(argv[5], 10);
    ila_cfg.trigger_type = (uint8_t)ntlibc_atoi(argv[6], 10);
    PRINTF("addr 0x%X, size %d, clk_src %d, trigger_src %d, data_src %d, delay %d, trigger_type = %d\r\n",
            ila_cfg.addr, ila_cfg.size, ila_cfg.clk_src, ila_cfg.trigger_src, ila_cfg.data_src,
            ila_cfg.trigger_delay, ila_cfg.trigger_type);

    memset((void *)RAM_ILA_START_ADDR, 0, ila_cfg.size);

    slc_debug_ila_init(&ila_cfg);

    slc_hal_register_irq_handler(ILA_IRQ, slc_debug_ila_test_irq);
    slc_debug_ila_interrupt_enable(true);
    SLC_HAL_ENABLE_PERIPHERAL_IRQ(ILA_IRQ, 0x3);

    slc_debug_ila_start(true);

    return 0;
}
#endif

int ntshell_cmd_register(char *cmd, char *descriptor, USRCMDFUNC func)
{
    static char index = 0;
    if(index>=(sizeof(cmdlist)/sizeof(cmdlist[0])))
        return -1;
    if(ntlibc_strlen(cmd) > sizeof(cmdlist[0].cmd))
        return -1;
    if(ntlibc_strlen(descriptor) > sizeof(cmdlist[0].desc))
        return -1;
    ntlibc_strcpy((char *)cmdlist[index].cmd, cmd);
    ntlibc_strcpy((char *)cmdlist[index].desc, descriptor);
    cmdlist[index].func = func;
    index++;
    return 0;
}

static int ntshell_func_read(char *buf, int cnt, void *extobj)
{
    uint32_t len = cnt;
    UNUSED_VARIABLE(extobj);

    slc_hal_uart_receive_data(NT_SHELL_UART_ID, (uint8_t *)buf, &len, NT_SHELL_UART_TIMEOUT);

    return len;
}

static int ntshell_func_write(const char *buf, int cnt, void *extobj)
{
    uint16_t len = cnt;
    UNUSED_VARIABLE(extobj);

    slc_hal_uart_send_data(NT_SHELL_UART_ID, (uint8_t *)buf, len, NT_SHELL_UART_TIMEOUT);

    return len;
}

static int nt_shell_usrcmd_ntopt_callback(int argc, char **argv, void *extobj)
{
    int ret = -1;
    const cmd_table_t *p = &cmdlist[0];

    if(argc < 1)
        return ret;

    nt_shell_printf(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\r\n");
    for (int i = 0; i < sizeof(cmdlist) / sizeof(cmdlist[0]); i++) {
        if (p->func != NULL && p->cmd[0] != '\0' && ntlibc_strcmp((const char *)argv[0], p->cmd) == 0) {
            ret = p->func(argc, argv);
            goto end;
        }
        p++;
    }
    nt_shell_printf("Unknown command found.\r\n");

end:
    nt_shell_printf("<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<\r\n");
    return ret;
}

static int ntshell_func_callback(const char *text, void *extobj)
{
    ntshell_t *ntshell = (ntshell_t *)extobj;
    UNUSED_VARIABLE(ntshell);
    UNUSED_VARIABLE(extobj);
    if (ntlibc_strlen(text) > 0) 
        ntopt_parse(text, nt_shell_usrcmd_ntopt_callback, NULL);

    return 0;
}

int ntshell_top_init(void)
{
    ntshell_init(
        &ntshell,
        ntshell_func_read,
        ntshell_func_write,
        ntshell_func_callback,
        (void *)&ntshell
    );

    // 命令注册
    ntshell_cmd_register("help", "help.", usrcmd_help);
    ntshell_cmd_register("wr", "write", usrcmd_write);
    ntshell_cmd_register("rd", "read", usrcmd_read);
#ifdef SLC_FPGA
    ntshell_cmd_register("subg_wr", "subg write", usrcmd_subg_write);
    ntshell_cmd_register("subg_rd", "subg read", usrcmd_subg_read);
#endif

#ifdef SLC_PHYTEST
    ntshell_cmd_register("phy_cfg", "phy_cfg", usrcmd_phy_trx_cfg);
    ntshell_cmd_register("cad_cfg", "cad_cfg", usrcmd_phy_cad_cfg);
    ntshell_cmd_register("phy_trigger", "phy_trigger tx/rx", usrcmd_phy_trigger_trx);
    ntshell_cmd_register("cad_exit", "cad_exit", usrcmd_phy_cad_exit);
    ntshell_cmd_register("phy_wakeup", "phy_wakeup", usrcmd_phy_wakeup);
    ntshell_cmd_register("sfsearch", "sfsearch cfg", usrcmd_sfsearch_cfg);
    ntshell_cmd_register("single_tone", "single_tone", usrcmd_phy_single_tone);
    ntshell_cmd_register("phy_num", "get phy num", usrcmd_phy_total_cnt);
    ntshell_cmd_register("cpu_reset", "cpu_reset", usrcmd_cpu_reset);
#endif

#ifdef SLC_ILATEST
    ntshell_cmd_register("ila_cfg", "ila cfg", usrcmd_ila_cfg);
#endif

#ifdef SLC_SOCTEST
    ntshell_cmd_register("soctest", "soctest cfg", usrcmd_soctest_cfg);
#endif

    ntshell_set_prompt(&ntshell, "nsh>");
    ntshell_execute(&ntshell);

    nt_shell_printf("\r\n");
    return 0;
}

void ntshell_top_run_once(void)
{
    ntshell_execute_once(&ntshell);
}
