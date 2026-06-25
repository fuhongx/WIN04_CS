/**
 * ****************************************************************************
 * @file slc_sf_search.c
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

#include "slc_sf_search.h"
#include "app_cfg.h"


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

uint8_t sf_train_val1[256] = {
    5, 5, 5, 5, 5, 5, 5, 5, 6, 5, 8, 6, 5, 6, 8, 7,
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


void slc_sf_search_enable(bool enable)
{
    if (enable)
        PHY_CTRL->SF_SEARCH1 |= PHY_SF_SEARCH_EN_MASK;
    else
        PHY_CTRL->SF_SEARCH1 &= ~PHY_SF_SEARCH_EN_MASK;
}

void slc_sf_search_cfg(uint16_t sfsearch_num, float multi0, float multi1, float multi2, uint8_t sfseeearch_len)
{
    uint32_t val = PHY_CTRL->SF_SEARCH1;
    val &= ~(PHY_SF_SEARCH_NUM_MASK | PHY_SF_SEARCH_MULTI3_MASK | PHY_SF_SEARCH_MULTI1_MASK 
            | PHY_SF_SEARCH_MULTI2_MASK | PHY_SF_SEARCH_LEN_MASK);

    val |= PHY_SF_SEARCH_NUM_VAL(sfsearch_num);
    val |= PHY_SF_SEARCH_MULTI1_VAL(multi0 * 8);
    val |= PHY_SF_SEARCH_MULTI2_VAL(multi1 * 8);
    val |= PHY_SF_SEARCH_MULTI3_VAL(multi2 * 8);
    val |= PHY_SF_SEARCH_LEN_VAL(sfseeearch_len);

    PHY_CTRL->SF_SEARCH1 = val;

    PHY_CTRL->SF_SEARCH2 &= ~PHY_SF_SEARCH_ADDR_MASK;
    PHY_CTRL->SF_SEARCH2 &= ~(PHY_SF_SEARCH1_MASK | PHY_SF_SEARCH2_MASK | PHY_SF_SEARCH3_MASK | PHY_SF_SEARCH4_MASK);
    for(uint8_t sf_num=0; sf_num<64; sf_num++){
        PHY_CTRL->SF_SEARCH2 = (PHY_SF_SEARCH_ADDR_VAL(sf_num * 4) | PHY_SF_SEARCH1_VAL(sf_train_val[(sf_num*4)]) | PHY_SF_SEARCH2_VAL(sf_train_val[(sf_num*4)+1]) 
                                | PHY_SF_SEARCH3_VAL(sf_train_val[(sf_num*4)+2]) | PHY_SF_SEARCH4_VAL(sf_train_val[(sf_num*4)+3]) );
    uint32_t reg_data = PHY_CTRL->SF_SEARCH2;
    PRINTF("SF_SEARCH2 = %08x\n",reg_data);
    }

}


void slc_sf_search_training(uint16_t sf)
{
    uint32_t val = 0;

    slc_sf_search_enable(false);

    val = PHY_CTRL->CTRL_REG1;
    val &= ~PHY_SF_MASK;
    val |= PHY_SF_VAL(sf);
    PHY_CTRL->CTRL_REG1 = val;

    slc_phy_set_rx_param_default(sf);

    slc_phy_sw_reset();

    slc_sf_search_enable(true);
}
