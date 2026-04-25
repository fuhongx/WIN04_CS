/**
 * ****************************************************************************
 * @file qmx_cali.h
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

#ifndef __QMX_CALI_H__
#define __QMX_CALI_H__

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

#include "reg_Dev.h"
#include "error_def.h"
#include "utility.h"

#define QMX_CALI_CLK_RC32K_TIMEOUT_US   (34000)
#define QMX_CALI_CLK_RC50M_TIMEOUT_US   (8000)
#define QMX_CALI_CLK_DCDC1M_TIMEOUT_US  (1000)
#define QMX_CALI_RC_TIMEOUT_US          (200)
#define QMX_CALI_R_TIMEOUT_US           (500)
#define QMX_CALI_AFC_TIMEOUT_US         (400)
#define QMX_CALI_KADC_TIMEOUT_US        (40000)
#define QMX_CALI_DCIQ_TIMEOUT_US        (1000)
#define QMX_CALI_FFTIQ_TIMEOUT_US       (1000)
#define QMX_CALI_TXIQ_TIMEOUT_US        (200)

#define QMX_MAX_RXABB_CBPF_CTRIM_IDX    (8)
#define QMX_MAX_LPF_CTRIM_IDX           (11)

#define QMX_PEAKDET_VREF_1P1_TX_CODE    (3)
#define QMX_PEAKDET_VREF_1P1_RX_CODE    (1)

typedef enum {
    QMX_CALI_RC32K,
    QMX_CALI_RC50M,
    QMX_CALI_DCDC1M,
} qmx_cali_clk_e;

typedef enum {
    QMX_GPADC_VSENSOR,
    QMX_GPADC_TSENSOR,
    QMX_GPADC_TXPDT,
    QMX_GPADC_IO,
} qmx_cali_gpadc_buf_sel_e;

typedef enum {
    QMX_CALI_SINE,
    QMX_CALI_DC,
} qmx_cali_sel_e;

typedef enum {
    QMX_CALI_DC_IQ,
    QMX_CALI_FFT_IQ,
} qmx_cali_iq_e;

typedef struct {
    uint8_t rc32k_ctrim;
    uint8_t rc32k_rtrim;
    uint8_t rc50m_ctune;
    uint8_t rc50m_rtune;
    uint8_t dcdc1m_captrim;
    uint8_t rc_cali_idx;
    uint8_t pmu_iout_tune_trim;
    uint8_t pmu_iout_ptat_trim;
    uint8_t kdac_gain;
    /* DC IQ Cali Data */
    int16_t dciq_iopt;
    int16_t dciq_qopt;
    int dciq_amp_compi;
    int dciq_amp_compq;
    int16_t dciq_phase_idx;
    /* FFT IQ Cali Data */
    int16_t fftiq_dc_i;
    int16_t fftiq_dc_q;
    int fftiq_amp_compi;
    int fftiq_amp_compq;
    int16_t fftiq_phase_idx;
} qmx_cali_data_t;

typedef struct {
    uint16_t decision_threshold;
    uint8_t rxabb_cbpf_ctrim;
    uint8_t lpf_ctrim;
    uint8_t tia;
    uint8_t freq_doubler;
    uint8_t lo_ppf;
    uint8_t pll_dac_lpf;
} qmx_rccali_t;

int qmx_clk_cali(qmx_cali_clk_e clk);
int qmx_r_cali(void);
int qmx_rc_cali(void);
int qmx_afc_cali(uint32_t freq_hz, int32_t inter_freq_hz, bool tx, bool polar);
int qmx_kdac_cali(void);

int qmx_cali_init(qmx_cali_iq_e iq_type);
void qmx_restore_cali_data(qmx_cali_iq_e iq_type);
qmx_cali_data_t *qmx_get_cali_data_ptr(void);
uint32_t qmx_get_cali_data_size(void);

#endif
