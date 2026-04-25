/**
 * ****************************************************************************
 * @file debug.h
 * @author SLC(SC) software team. 
 * @brief 
 * @version  1.0
 * ****************************************************************************
 * 
 * Copyright (c) 2024 SLC Semiconductor all rights reserved.
 * 
 * ****************************************************************************
 */

#ifndef __DEBUG_H__
#define __DEBUG_H__

#include "app_cfg.h"

extern void debug_printf_init(void);

static inline void dump_u8buf(char *info, uint8_t *buf, uint32_t len)
{
#if APP_DEBUG_ENABLED
    PRINTF("%s: len = %d", info, len);
    for (int i = 0; i < len; i++)
    {
        PRINTF("%s%02X%s", i % 16 == 0 ? "\n     " : " ",
               buf[i], i == len - 1 ? "\n" : "");
    }
#endif
}

static inline void dump_u32buf(char *info, uint32_t *buf, uint32_t len)
{
#if APP_DEBUG_ENABLED
    PRINTF("%s: len = %d", info, len);
    for (int i = 0; i < len; i+=4)
    {
        PRINTF("%s%08X%s", i % 16 == 0 ? "\n     " : " ",
               buf[i/4], (i+4) >= len ? "\n" : "");
    }
#endif
}

#endif
