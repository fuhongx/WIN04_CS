/**
 * ****************************************************************************
 * @file reg_Dev.h
 * @author SLC(SC) software team. 
 * @brief 
 * @version  1.0
 * ****************************************************************************
 * 
 * Copyright (c) 2024 SLC Semiconductor all rights reserved.
 * 
 * ****************************************************************************
 */

#ifndef __REG_DEV_H__
#define __REG_DEV_H__

#ifdef WIN04
#include "reg_win04.h"
#define DEV_SELECTED
#endif

#ifndef DEV_SELECTED
 #error "Chip select error - no such chip!!!!"
#endif

#endif
