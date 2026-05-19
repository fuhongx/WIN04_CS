/**
 * ****************************************************************************
 * @file slc_cali.h
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

#ifndef __SLC_CALI_H__
#define __SLC_CALI_H__

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

#include "reg_Dev.h"
#include "error_def.h"
#include "utility.h"

#define SLC_CALI_CLK_RC32K_TIMEOUT_US   (34000)
#define SLC_CALI_CLK_RC50M_TIMEOUT_US   (8000)
#define SLC_CALI_CLK_DCDC1M_TIMEOUT_US  (1000)
#define SLC_CALI_RC_TIMEOUT_US          (200)
#define SLC_CALI_R_TIMEOUT_US           (500)
#define SLC_CALI_AFC_TIMEOUT_US         (400)
#define SLC_CALI_KADC_TIMEOUT_US        (40000)
#define SLC_CALI_DCIQ_TIMEOUT_US        (1000)
#define SLC_CALI_FFTIQ_TIMEOUT_US       (1000)
#define SLC_CALI_TXIQ_TIMEOUT_US        (200)

#define SLC_MAX_RXABB_CBPF_CTRIM_IDX    (8)
#define SLC_MAX_LPF_CTRIM_IDX           (11)

#define SLC_PEAKDET_VREF_1P1_TX_CODE    (3)
#define SLC_PEAKDET_VREF_1P1_RX_CODE    (1)

typedef enum {
    SLC_CALI_RC32K,
    SLC_CALI_RC50M,
    SLC_CALI_DCDC1M,
} slc_cali_clk_e;

typedef enum {
    SLC_GPADC_VSENSOR,
    SLC_GPADC_TSENSOR,
    SLC_GPADC_TXPDT,
    SLC_GPADC_IO,
} slc_cali_gpadc_buf_sel_e;

typedef enum {
    SLC_CALI_SINE,
    SLC_CALI_DC,
} slc_cali_sel_e;

typedef enum {
    SLC_CALI_DC_IQ,
    SLC_CALI_FFT_IQ,
} slc_cali_iq_e;

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
} slc_cali_data_t;

typedef struct {
    uint16_t decision_threshold;
    uint8_t rxabb_cbpf_ctrim;
    uint8_t lpf_ctrim;
    uint8_t tia;
    uint8_t freq_doubler;
    uint8_t lo_ppf;
    uint8_t pll_dac_lpf;
} slc_rccali_t;

int slc_clk_cali(slc_cali_clk_e clk);
int slc_r_cali(void);
int slc_rc_cali(void);
int slc_afc_cali(uint32_t freq_hz, int32_t inter_freq_hz, bool tx, bool polar);
int slc_kdac_cali(void);

int slc_cali_init(slc_cali_iq_e iq_type);
void slc_restore_cali_data(slc_cali_iq_e iq_type);
slc_cali_data_t *slc_get_cali_data_ptr(void);
uint32_t slc_get_cali_data_size(void);

#endif
