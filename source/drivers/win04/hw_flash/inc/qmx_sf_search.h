/**
 * ****************************************************************************
 * @file qmx_sf_search.h
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

#ifndef __QMX_SF_SEARCH_H__
#define __QMX_SF_SEARCH_H__

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include "reg_Dev.h"
#include "error_def.h"
#include "utility.h"

#include "qmx_phy.h"

void qmx_sf_search_enable(bool enable);
void qmx_sf_search_cfg(uint16_t sfsearch_num, float multi0, float multi1, float multi2);
void qmx_sf_search_training(uint16_t sf);

#endif
