/**
 * ****************************************************************************
 * @file utility.c
 * @author SLC(SC) software team.
 * @brief
 * @version  1.0
 * ****************************************************************************
 *
 * Copyright (c) 2024 SLC Semiconductor all rights reserved.
 *
 * ****************************************************************************
 */

#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "reg_Dev.h"
#include "utility.h"
#include "hw_sysctrl.h"

void rom_utility_delay_us(uint32_t u32TimeUs)
{
    uint32_t u32Cycles;

    u32Cycles = SystemCoreClock / 1000000U * u32TimeUs;

    rom_utility_delay_cycles(u32Cycles);
}

void rom_utility_run_callback_noparam(uint32_t u32Addr)
{
    CallbackNoParam Callback = (CallbackNoParam)(u32Addr | 0x1);
    Callback();
}

void rom_utility_delay_ms(uint32_t u32Ms)
{
    for (int i = 0; i < u32Ms; i++)
        rom_utility_delay_us(1000);
}

void rom_utility_delay_s(uint32_t u32s)
{
    for (int i = 0; i < u32s; i++)
        rom_utility_delay_ms(1000);
}

// All chip
uint32_t rom_utility_little_endian_read_32(const uint8_t *pu8Buf, uint32_t uin32Pos)
{
    return (((uint32_t)pu8Buf[uin32Pos]) | (((uint32_t)pu8Buf[uin32Pos + 1]) << 8) | (((uint32_t)pu8Buf[uin32Pos + 2]) << 16) | (((uint32_t)pu8Buf[uin32Pos + 3]) << 24));
}

uint16_t rom_utility_little_endian_read_16(const uint8_t *pu8Buf, uint32_t uin32Pos)
{
    return (((uint16_t)pu8Buf[uin32Pos]) | (((uint16_t)pu8Buf[uin32Pos + 1]) << 8));
}

void rom_utility_little_endian_store_32(uint8_t *pu8Buf, uint32_t u32Pos, uint32_t u32Value)
{
    pu8Buf[u32Pos++] = u32Value;
    pu8Buf[u32Pos++] = u32Value >> 8;
    pu8Buf[u32Pos++] = u32Value >> 16;
    pu8Buf[u32Pos++] = u32Value >> 24;
}
