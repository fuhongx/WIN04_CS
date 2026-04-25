/**
 * ****************************************************************************
 * @file qmx_subg.c
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
#include <math.h>

#include "qmx_hal_delay.h"
#include "qmx_private_spi_frame.h"
#include "qmx_subg.h"

/* SUBG2 config func, only for FPGA */
void qmx_rf_subg_set_gpio(void)
{
    // set IO driver strength
    qmx_rf_spi_set_bits(0x00CC, 24, 25, 0x0);
}

void qmx_rf_subg_disable_trx(void)
{
    qmx_rf_spi_set_bits(0x00C8, 2, 5, 0xA);
}

void qmx_rf_subg_set_tx_en(void)
{
    qmx_rf_spi_set_bits(0x00C8, 2, 3, 0x3);
}

void qmx_rf_subg_set_rx_en(void)
{
    qmx_rf_spi_set_bits(0x00C8, 4, 5, 0x3);
}

EN_ERR_STA_T qmx_rf_subg_afc_cali(void)
{
    uint32_t timeout = 5000;

    qmx_rf_spi_write32_cmd(0x00D0, 0x00003FFF);
    qmx_rf_spi_set_bits(0x0004, 16, 16, 1);
    qmx_rf_spi_set_bits(0x0004, 0, 0, 0);
    qmx_rf_spi_set_bits(0x0030, 0, 0, 1);
    do {
        if (qmx_rf_spi_get_bits(0x0030, 0, 0) == 0)
            break;

        timeout--;
        qmx_hal_nop_delay_ms(1);
    } while (timeout);

    if (timeout == 0)
        return EN_ERROR_STA_TIMEOUT;

    return EN_ERROR_STA_OK;
}

void qmx_rf_subg_fc_cali(uint32_t freq)
{
    uint32_t val = floor(freq / 1000000.0 - 460);
    uint32_t afc_code = 0;

    qmx_rf_spi_set_bits(0x00D4, 16, 21, val);
    qmx_rf_spi_set_bits(0x00D4, 28, 28, 1);

    afc_code = qmx_rf_spi_get_bits(0x00D4, 0, 13);
    qmx_rf_spi_set_bits(0x00D4, 28, 28, 0);
    qmx_rf_spi_set_bits(0x00D4, 24, 24, 1);
    qmx_rf_spi_set_bits(0x0004, 16, 16, 0);
    qmx_rf_spi_set_bits(0x0004, 0, 0, 1);

    qmx_rf_spi_set_bits(0x00D4, 24, 24, 1);
    qmx_rf_spi_set_bits(0x00D4, 0, 13, afc_code);
    qmx_rf_spi_set_bits(0x00D4, 24, 24, 0);
}

void qmx_rf_subg_set_fc(uint32_t freq)
{
    // floor( 载波 / 0.238418579101562)
    uint32_t fc = floor(freq * ((1 << 22) / 1000000.0));
    qmx_rf_spi_write32_cmd(0x0024, fc);
}

void qmx_rf_subg_set_sync_opt_agc_opt(void)
{
    qmx_rf_spi_set_bits(0x006C, 1, 1, 0x0);
    qmx_rf_spi_set_bits(0x006C, 31, 31, 0x1);
}
