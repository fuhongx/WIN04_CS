/**
 * ****************************************************************************
 * @file utility.h
 * @author SLC(SC) software team. 
 * @brief 
 * @version  1.0
 * ****************************************************************************
 * 
 * Copyright (c) 2024 SLC Semiconductor all rights reserved.
 * 
 * ****************************************************************************
 */

#ifndef __UTILITY_H__
#define __UTILITY_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

// 定义常量
#define PI 3.14159265358979323846f

typedef void (*CallbackNoParam)(void);

typedef void (*Callback1Param)(uint32_t u32Param);

extern void rom_utility_run_callback_noparam(uint32_t u32Addr);

extern uint16_t rom_utility_little_endian_read_16(const uint8_t *pu8Buf, uint32_t uin32Pos);

extern uint32_t rom_utility_little_endian_read_32(const uint8_t *pu8Buf, uint32_t uin32Pos);
extern void rom_utility_little_endian_store_32(uint8_t *pu8Buf, uint32_t u32Pos, uint32_t u32Value);

extern void rom_utility_delay_cycles(uint32_t cycles);

extern void rom_utility_delay_us(uint32_t u32TimeUs);

extern void rom_utility_delay_ms(uint32_t u32Ms);

extern void rom_utility_delay_s(uint32_t u32s);

#define BIT(pos)                (1UL << (pos))

#define write8(addr, value)     ((*(volatile uint8_t *)(addr)) = (value))
#define read8(addr)             ((*(volatile uint8_t *)(addr)))
#define write16(addr, value)    ((*(volatile uint16_t *)(addr)) = (value))
#define read16(addr)            ((*(volatile uint16_t *)(addr)))
#define write32(addr, value)    ((*(volatile uint32_t *)(addr)) = (value))
#define read32(addr)            ((*(volatile uint32_t *)(addr)))

#define get_bit(addr, bit)      ((read32(addr) >> bit) & 0x1)
#define set_bit(addr, bit)      (write32((addr), (read32(addr) | (1U << bit))))
#define clear_bit(addr, bit)    (write32((addr), (read32(addr) & ~(1U << bit))))

static inline void set_bits(uint32_t *addr, uint8_t start_bit, uint8_t end_bit, uint32_t value)
{
    uint32_t mask = ((1U << (end_bit - start_bit + 1U)) - 1U) << start_bit;
    uint32_t reg_value = read32(addr);
    reg_value = (reg_value & ~mask) | ((value << start_bit) & mask);
    write32(addr, reg_value);
}

// 获取指定位域的值
static inline uint32_t get_bits(uint32_t *addr, uint8_t start_bit, uint8_t end_bit)
{
    uint32_t mask = (1U << (end_bit - start_bit + 1U)) - 1U;
    return (read32(addr) >> start_bit) & mask;
}

#ifdef __cplusplus
}
#endif

#endif
