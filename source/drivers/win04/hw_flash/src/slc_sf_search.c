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

void slc_sf_search_enable(bool enable)
{
    if (enable)
        PHY_CTRL->SF_SEARCH |= PHY_SF_SEARCH_EN_MASK;
    else
        PHY_CTRL->SF_SEARCH &= ~PHY_SF_SEARCH_EN_MASK;
}

void slc_sf_search_cfg(uint16_t sfsearch_num, float multi0, float multi1, float multi2)
{
    uint32_t val = PHY_CTRL->SF_SEARCH;
    val &= ~(PHY_SF_SEARCH_NUM_MASK | PHY_SF_SEARCH_MULTI3_MASK | PHY_SF_SEARCH_MULTI1_MASK | PHY_SF_SEARCH_MULTI2_MASK);

    val |= PHY_SF_SEARCH_NUM_VAL(sfsearch_num);
    val |= PHY_SF_SEARCH_MULTI1_VAL(multi0 * 8);
    val |= PHY_SF_SEARCH_MULTI2_VAL(multi1 * 8);
    val |= PHY_SF_SEARCH_MULTI3_VAL(multi2 * 8);

    PHY_CTRL->SF_SEARCH = val;
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
