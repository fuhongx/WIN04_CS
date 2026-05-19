#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>

#include "slc_phy_test.h"
#include "slc_hal_sysctrl.h"
#include "app_cfg.h"
#include "utility.h"
#include "debug.h"
#include "slc_hal_pmu.h"
#include "slc_sf_search.h"
#include "slc_afc.h"
#include "slc_private_spi_frame.h"
#include "slc_hal_rng.h"
#include "slc_hal_intc.h"

uint8_t phy_test_rx_buffer[PHY_PAYLOAD_MAX_LEN+1];

uint8_t sf_train_val[256] = {
    7, 6, 5, 5, 5, 9, 5, 5, 6, 5, 8, 6, 5, 6, 8, 7,
    6, 7, 7, 6, 9, 5, 5, 7, 5, 8, 6, 5, 6, 5, 5, 5,
    7, 7, 5, 6, 6, 5, 6, 7, 7, 7, 5, 5, 8, 5, 6, 9,
    6, 5, 7, 6, 6, 7, 5, 7, 5, 5, 5, 6, 7, 6, 8, 5,
    5, 6, 5, 9, 5, 8, 12, 6, 6, 5, 5, 7, 6, 5, 5, 8,
    9, 5, 5, 5, 5, 6, 6, 5, 5, 5, 7, 6, 5, 6, 6, 7,
    5, 5, 5, 8, 5, 5, 7, 5, 8, 6, 6, 6, 5, 8, 6, 5,
    5, 7, 5, 7, 6, 5, 6, 7, 5, 5, 6, 5, 5, 5, 5, 5,
    6, 5, 8, 5, 5, 6, 7, 5, 5, 5, 6, 6, 10, 6, 5, 9,
    6, 6, 5, 5, 5, 6, 5, 5, 5, 5, 5, 5, 5, 6, 5, 6,
    5, 6, 5, 6, 7, 5, 7, 7, 6, 5, 5, 10, 8, 6, 5, 7,
    10, 5, 7, 5, 10, 6, 5, 5, 5, 6, 7, 5, 5, 5, 6, 5,
    5, 6, 5, 6, 5, 7, 5, 5, 5, 6, 7, 6, 5, 5, 8, 5,
    5, 5, 9, 5, 5, 5, 5, 5, 6, 11, 5, 5, 5, 5, 7, 5,
    5, 5, 7, 6, 6, 6, 11, 5, 6, 5, 9, 5, 5, 6, 5, 5,
    5, 5, 5, 8, 5, 6, 5, 5, 6, 8, 5, 6, 5, 8, 5, 6
};

void slc_phy_display_rx_data(void)
{
    uint32_t rx_sta = slc_phy_monitor_get_rx_det_sta();
    PRINTF("PHY RX_DET_HEADER: 0x%X\n", rx_sta);
    slc_phy_read_rx_buffer(phy_test_rx_buffer, (rx_sta & PHY_RX_PAYLOAD_LEN_MASK) >> PHY_RX_PAYLOAD_LEN_SHIFT);
    dump_u8buf("PHY RX DATA", phy_test_rx_buffer, ((rx_sta & PHY_RX_PAYLOAD_LEN_MASK) >> PHY_RX_PAYLOAD_LEN_SHIFT));
}

static volatile uint8_t g_phy_test_irq_flag = 0;

uint8_t slc_phy_test_get_flag(void)
{
    return g_phy_test_irq_flag;
}

void slc_phy_test_set_flag(uint8_t flag)
{
    g_phy_test_irq_flag = flag;
}

typedef struct
{
    uint32_t tx_done;
    uint32_t rx_done;
    uint32_t payload_crc_ok;
    uint32_t payload_crc_err;
    uint32_t header_crc_ok;
    uint32_t header_crc_err;
    uint32_t rx_miss_sync;
    uint32_t timeout;
    uint32_t rx_frame_detect;
    uint32_t rx_sym_sync;
    uint32_t css_demod_done;
    uint32_t cad_syncap;
    uint32_t cad_timeout;
    uint32_t cad_demod_err;
    uint32_t sfsearch_fail;
    uint32_t sfsearch_done;
    uint32_t netid_error;
    uint32_t sto_estimate_done;
} slc_phy_test_pkg_t;

static slc_phy_test_pkg_t g_phy_test_pkg = {0};

void slc_phy_test_display_cnt(void)
{
    PRINTF("\r\n============PHY TEST CNT:============\r\n");
    PRINTF("tx_done: %d, rx_done: %d\r\n", g_phy_test_pkg.tx_done, g_phy_test_pkg.rx_done);
    PRINTF("payload_crc_ok: %d, header_crc_ok: %d\r\n", g_phy_test_pkg.payload_crc_ok, g_phy_test_pkg.header_crc_ok);
    PRINTF("netid_error: %d, timeout: %d\r\n", g_phy_test_pkg.netid_error, g_phy_test_pkg.timeout);
    PRINTF("payload_crc_err: %d, header_crc_err: %d, rx_miss_sync: %d\r\n",
            g_phy_test_pkg.payload_crc_err, g_phy_test_pkg.header_crc_err, g_phy_test_pkg.rx_miss_sync);
    PRINTF("rx_frame_detect: %d, rx_sym_sync: %d\r\n", g_phy_test_pkg.rx_frame_detect, g_phy_test_pkg.rx_sym_sync);
    PRINTF("css_demod_done: %d\r\n", g_phy_test_pkg.css_demod_done);
    PRINTF("cad_syncap: %d, cad_timeout: %d, cad_demod_err: %d\r\n",
            g_phy_test_pkg.cad_syncap, g_phy_test_pkg.cad_timeout, g_phy_test_pkg.cad_demod_err);
    PRINTF("sfsearch_done: %d, sfsearch_fail: %d\r\n", g_phy_test_pkg.sfsearch_done, g_phy_test_pkg.sfsearch_fail);
    PRINTF("sto_estimate_done: %d\r\n", g_phy_test_pkg.sto_estimate_done);
    PRINTF("\r\n");
}

void PHY0_IRQ_Handler(void)
{
    uint32_t irq_flag = slc_phy_irq_get_status();
    static uint32_t sf_fail_cnt = 0;

    // clear irq flag
    slc_phy_irq_clr_status(irq_flag);

    // PRINTF("PHY0 IRQ: 0x%08X\r\n", irq_flag);

    if (irq_flag & PHY_IRQ_TX_DONE_MASK) {
        g_phy_test_pkg.tx_done++;
        PRINTF("PHY0 TX DONE, num=%d\r\n", g_phy_test_pkg.tx_done);
        slc_phy_test_set_flag(1);
    }
    if (irq_flag & PHY_IRQ_RX_DONE_MASK) {
        g_phy_test_pkg.rx_done++;
        PRINTF("PHY0 RX DONE, num=%d\r\n", g_phy_test_pkg.rx_done);
        slc_phy_display_rx_data();
    }
    if (irq_flag & PHY_IRQ_CRC_OK_MASK) {
        g_phy_test_pkg.payload_crc_ok++;
        PRINTF("PHY0 payload CRC OK, num=%d\r\n", g_phy_test_pkg.payload_crc_ok);
        slc_afc_set_sw_flag(true);
    }
    if (irq_flag & PHY_IRQ_CRC_ERR_MASK) {
        g_phy_test_pkg.payload_crc_err++;
        PRINTF("PHY0 payload CRC ERR, num=%d\r\n", g_phy_test_pkg.payload_crc_err);
        slc_afc_set_sw_flag(true);
    }
    if (irq_flag & PHY_IRQ_HD_OK_MASK) {
        g_phy_test_pkg.header_crc_ok++;
        PRINTF("PHY0 header CRC OK, num=%d\r\n", g_phy_test_pkg.header_crc_ok);
        slc_afc_set_sw_flag(true);
    }
    if (irq_flag & PHY_IRQ_HD_ERR_MASK) {
        g_phy_test_pkg.header_crc_err++;
        PRINTF("PHY0 header CRC ERR, num=%d\r\n", g_phy_test_pkg.header_crc_err);
    }
    if (irq_flag & PHY_IRQ_RX_MISS_SYNC_MASK) {
        g_phy_test_pkg.rx_miss_sync++;
        PRINTF("PHY0 RX miss sync, num=%d\r\n", g_phy_test_pkg.rx_miss_sync);
    }
    if (irq_flag & PHY_IRQ_TIMEOUT_MASK) {
        g_phy_test_pkg.timeout++;
        PRINTF("PHY0 timeout, num=%d\r\n", g_phy_test_pkg.timeout);
    }
    if (irq_flag & PHY_IRQ_RX_FRAME_DETECT_MASK) {
        g_phy_test_pkg.rx_frame_detect++;
        PRINTF("PHY0 RX frame detect, num=%d\r\n", g_phy_test_pkg.rx_frame_detect);
    }
    if (irq_flag & PHY_IRQ_RX_SYM_SYNC_MASK) {
        g_phy_test_pkg.rx_sym_sync++;
        PRINTF("PHY0 RX symbol sync, num=%d\r\n", g_phy_test_pkg.rx_sym_sync);
    }
    if (irq_flag & PHY_IRQ_CSS_DEMOD_DONE_MASK) {
        g_phy_test_pkg.css_demod_done++;
        PRINTF("PHY0 CSS demod done, num=%d\r\n", g_phy_test_pkg.css_demod_done);
        slc_phy_test_set_flag(1);
    }
    if (irq_flag & PHY_IRQ_CAD_SYNCAP_MASK) {
        g_phy_test_pkg.cad_syncap++;
        PRINTF("PHY0 CAD capture, num=%d\r\n", g_phy_test_pkg.cad_syncap);
    }
    if (irq_flag & PHY_IRQ_CAD_TIMEOUT_MASK) {
        g_phy_test_pkg.cad_timeout++;
        PRINTF("PHY0 CAD timeout, num=%d\r\n", g_phy_test_pkg.cad_timeout);
    }
    if (irq_flag & PHY_IRQ_CAD_DEMOD_ERR_MASK) {
        g_phy_test_pkg.cad_demod_err++;
        PRINTF("PHY0 CAD demod error, num=%d\r\n", g_phy_test_pkg.cad_demod_err);
    }
    if (irq_flag & PHY_IRQ_SFSEARCH_FAIL_MASK) {
        g_phy_test_pkg.sfsearch_fail++;
        slc_sf_search_training(sf_train_val[sf_fail_cnt % 256]);
        // if ((sf_fail_cnt % 20) == 0)
        //     PRINTF("PHY0 SF search fail, num=%d\r\n", g_phy_test_pkg.sfsearch_fail);
        sf_fail_cnt++;
    }
    if (irq_flag & PHY_IRQ_SFSEARCH_DONE_MASK) {
        g_phy_test_pkg.sfsearch_done++;
        // slc_sf_search_enable(false);
        sf_fail_cnt = 0;
        PRINTF("PHY0 SF search done, num=%d\r\n", g_phy_test_pkg.sfsearch_done);
    }
    if (irq_flag & PHY_IRQ_NETID_ERR_MASK) {
        g_phy_test_pkg.netid_error++;
        PRINTF("PHY0 netid error, num=%d\r\n", g_phy_test_pkg.netid_error);
    }
    if (irq_flag & PHY_IRQ_STO_EST_DONE_MASK) {
        g_phy_test_pkg.sto_estimate_done++;
        PRINTF("PHY0 STO estimate done, num=%d\r\n", g_phy_test_pkg.sto_estimate_done);
    }

    // slc_phy_test_display_cnt();
}

void PHY1_IRQ_Handler(void)
{
    uint32_t irq_flag = slc_phy_irq_get_status();
    static uint32_t sf_fail_cnt = 0;

    // clear irq flag
    slc_phy_irq_clr_status(irq_flag);

    // PRINTF("PHY1 IRQ: 0x%08X\r\n", irq_flag);

    if (irq_flag & PHY_IRQ_TX_DONE_MASK) {
        PRINTF("PHY1 TX DONE\r\n");
    }
    if (irq_flag & PHY_IRQ_RX_DONE_MASK) {
        PRINTF("PHY1 RX DONE\r\n");
        slc_phy_display_rx_data();
        // slc_phy_disable_rx();
    }
    if (irq_flag & PHY_IRQ_CRC_OK_MASK) {
        PRINTF("PHY1 payload CRC OK\r\n");
        slc_afc_set_sw_flag(true);
    }
    if (irq_flag & PHY_IRQ_CRC_ERR_MASK) {
        PRINTF("PHY1 payload CRC ERR\r\n");
        slc_afc_set_sw_flag(true);
    }
    if (irq_flag & PHY_IRQ_HD_OK_MASK) {
        PRINTF("PHY1 header CRC OK\r\n");
        slc_afc_set_sw_flag(true);
    }
    if (irq_flag & PHY_IRQ_HD_ERR_MASK) {
        PRINTF("PHY1 header CRC ERR\r\n");
    }
    if (irq_flag & PHY_IRQ_RX_MISS_SYNC_MASK) {
        PRINTF("PHY1 RX miss sync\r\n");
    }
    if (irq_flag & PHY_IRQ_TIMEOUT_MASK) {
        PRINTF("PHY1 timeout\r\n");
    }
    if (irq_flag & PHY_IRQ_RX_FRAME_DETECT_MASK) {
        PRINTF("PHY1 RX frame detect\r\n");
    }
    if (irq_flag & PHY_IRQ_RX_SYM_SYNC_MASK) {
        PRINTF("PHY1 RX symbol sync\r\n");
    }
    if (irq_flag & PHY_IRQ_CSS_DEMOD_DONE_MASK) {
        PRINTF("PHY1 CSS demod done\r\n");
    }
    if (irq_flag & PHY_IRQ_CAD_SYNCAP_MASK) {
        PRINTF("PHY1 CAD capture\r\n");
    }
    if (irq_flag & PHY_IRQ_CAD_TIMEOUT_MASK) {
        PRINTF("PHY1 CAD timeout\r\n");
    }
    if (irq_flag & PHY_IRQ_CAD_DEMOD_ERR_MASK) {
        PRINTF("PHY1 CAD demod error\r\n");
    }
    if (irq_flag & PHY_IRQ_SFSEARCH_FAIL_MASK) {
        slc_sf_search_training(sf_train_val[sf_fail_cnt % 256]);
        // if ((sf_fail_cnt % 20) == 0)
        //     PRINTF("PHY1 SF search fail\r\n");
        sf_fail_cnt++;
    }
    if (irq_flag & PHY_IRQ_SFSEARCH_DONE_MASK) {
        // slc_sf_search_enable(false);
        sf_fail_cnt = 0;
        PRINTF("PHY1 SF search done\r\n");
    }
    if (irq_flag & PHY_IRQ_NETID_ERR_MASK) {
        PRINTF("PHY1 netid error\r\n");
    }
    if (irq_flag & PHY_IRQ_STO_EST_DONE_MASK) {
        PRINTF("PHY1 STO estimate done\r\n");
    }
}

void slc_rffe_irq_handler(void)
{
    uint32_t irq_flag = slc_rf_irq_get_sta();

    // clear irq flag
    slc_rf_irq_clr_sta(irq_flag);

    // PRINTF("RFFE IRQ: 0x%08X\r\n", irq_flag);

    if (irq_flag & RF_IRQ_LVD_MASK) {
        PRINTF("RFFE LVD trigger\r\n");
    }
    if (irq_flag & RF_IRQ_DCDC_1M_MASK) {
        PRINTF("RFFE DCDC 1M Tune done\r\n");
    }
    if (irq_flag & RF_IRQ_RC32K_MASK) {
        PRINTF("RFFE RC32K Tune done\r\n");
    }
    if (irq_flag & RF_IRQ_RC50M_MASK) {
        PRINTF("RFFE RC50M Tune done\r\n");
    }
    if (irq_flag & RF_IRQ_RXABB_RCCAL_MASK) {
        PRINTF("RFFE RXABB RCCAL done\r\n");
    }
    if (irq_flag & RF_IRQ_PMU_RCAL_MASK) {
        PRINTF("RFFE PMU RCAL done\r\n");
    }
    if (irq_flag & RF_IRQ_AGC_MAX_ENTRY_MASK) {
        PRINTF("RFFE AGC max entry\r\n");
    }
    if (irq_flag & RF_IRQ_AGC_MAX_EXIT_MASK) {
        PRINTF("RFFE AGC max exit\r\n");
    }
    if (irq_flag & RF_IRQ_RFPLL_AFC_MASK) {
        PRINTF("RFFE RFPLL AFC done\r\n");
    }
    if (irq_flag & RF_IRQ_RFPLL_DAC_MASK) {
        PRINTF("RFFE RFPLL DAC done\r\n");
    }
}

void slc_phy_test(void)
{
#ifdef SLC_FPGA
    slc_private_spi_init();
#endif
    slc_hal_pmu_phy_power_enable(true);

    slc_hal_sysctrl_peripheral_clk_enable(HAL_CLK_RAND, true);
    slc_hal_sysctrl_peripheral_mod_reset(HAL_CLK_RAND);

    memset(&g_phy_test_pkg, 0, sizeof(slc_phy_test_pkg_t));

    hal_rng_init_t rng_init;
    rng_init.seed = 0x12345678;
    slc_hal_rng_init(&rng_init);

    SLC_HAL_ENABLE_PERIPHERAL_IRQ(PHY0_IRQ, 0x1);
    slc_hal_register_irq_handler(PHY0_IRQ, PHY0_IRQ_Handler);
    slc_phy0_irq_enable(PHY_IRQ_ALL_MASK);

    // 配置GPIO17为TRX切换控制口
    slc_hal_gpio_set_iomux(HAL_GPIO_PIN17, HAL_IOMUX_MODE7);

#if 0
    SLC_HAL_ENABLE_PERIPHERAL_IRQ(PHY1_IRQ, 0x1);
    slc_hal_register_irq_handler(PHY1_IRQ, PHY1_IRQ_Handler);
    slc_phy1_irq_enable(PHY_IRQ_ALL_MASK);
#endif
    // slc_rf_set_pmu_lvd_crvt(RF_LVD_2P0);
    // slc_rf_set_pmu_lvd_enable(true);
    // slc_rf_irq_clr_sta(RF_IRQ_ALL_MASK);

    // slc_hal_register_irq_handler(RFFE_IRQ, slc_rffe_irq_handler);
    // slc_rf_irq_enable(RF_IRQ_LVD_MASK);
    // SLC_HAL_ENABLE_PERIPHERAL_IRQ(RFFE_IRQ, 0x3);

    SLC_HAL_ENABLE_GLOBAL_IRQ();
}
