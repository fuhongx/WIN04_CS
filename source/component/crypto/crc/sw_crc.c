#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include "sw_crc.h"

#include "app_cfg.h"
#include "utility.h"
#include "debug.h"

/******************************************************************************
 * Name:    CRC-4/ITU           x4+x+1
 * Poly:    0x03
 * Init:    0x00
 * Refin:   True
 * Refout:  True
 * Xorout:  0x00
 * Note:
 *****************************************************************************/
uint8_t crc4_itu(uint8_t *data, uint16_t length)
{
    uint8_t i;
    uint8_t crc = 0; // Initial value
    while (length--)
    {
        crc ^= *data++; // crc ^= *data; data++;
        for (i = 0; i < 8; ++i)
        {
            if (crc & 1)
                crc = (crc >> 1) ^ 0x0C; // 0x0C = (reverse 0x03)>>(8-4)
            else
                crc = (crc >> 1);
        }
    }
    return crc;
}

/******************************************************************************
 * Name:    CRC-5/EPC           x5+x3+1
 * Poly:    0x09
 * Init:    0x09
 * Refin:   False
 * Refout:  False
 * Xorout:  0x00
 * Note:
 *****************************************************************************/
uint8_t crc5_epc(uint8_t *data, uint16_t length)
{
    uint8_t i;
    uint8_t crc = 0x48; // Initial value: 0x48 = 0x09<<(8-5)
    while (length--)
    {
        crc ^= *data++; // crc ^= *data; data++;
        for (i = 0; i < 8; i++)
        {
            if (crc & 0x80)
                crc = (crc << 1) ^ 0x48; // 0x48 = 0x09<<(8-5)
            else
                crc <<= 1;
        }
    }
    return crc >> 3;
}

/******************************************************************************
 * Name:    CRC-5/ITU           x5+x4+x2+1
 * Poly:    0x15
 * Init:    0x00
 * Refin:   True
 * Refout:  True
 * Xorout:  0x00
 * Note:
 *****************************************************************************/
uint8_t crc5_itu(uint8_t *data, uint16_t length)
{
    uint8_t i;
    uint8_t crc = 0; // Initial value
    while (length--)
    {
        crc ^= *data++; // crc ^= *data; data++;
        for (i = 0; i < 8; ++i)
        {
            if (crc & 1)
                crc = (crc >> 1) ^ 0x15; // 0x15 = (reverse 0x15)>>(8-5)
            else
                crc = (crc >> 1);
        }
    }
    return crc;
}

/******************************************************************************
 * Name:    CRC-5/USB           x5+x2+1
 * Poly:    0x05
 * Init:    0x1F
 * Refin:   True
 * Refout:  True
 * Xorout:  0x1F
 * Note:
 *****************************************************************************/
uint8_t crc5_usb(uint8_t *data, uint16_t length)
{
    uint8_t i;
    uint8_t crc = 0x1F; // Initial value
    while (length--)
    {
        crc ^= *data++; // crc ^= *data; data++;
        for (i = 0; i < 8; ++i)
        {
            if (crc & 1)
                crc = (crc >> 1) ^ 0x14; // 0x14 = (reverse 0x05)>>(8-5)
            else
                crc = (crc >> 1);
        }
    }
    return crc ^ 0x1F;
}

/******************************************************************************
 * Name:    CRC-6/ITU           x6+x+1
 * Poly:    0x03
 * Init:    0x00
 * Refin:   True
 * Refout:  True
 * Xorout:  0x00
 * Note:
 *****************************************************************************/
uint8_t crc6_itu(uint8_t *data, uint16_t length)
{
    uint8_t i;
    uint8_t crc = 0; // Initial value
    while (length--)
    {
        crc ^= *data++; // crc ^= *data; data++;
        for (i = 0; i < 8; ++i)
        {
            if (crc & 1)
                crc = (crc >> 1) ^ 0x30; // 0x30 = (reverse 0x03)>>(8-6)
            else
                crc = (crc >> 1);
        }
    }
    return crc;
}

/******************************************************************************
 * Name:    CRC-7/MMC           x7+x3+1
 * Poly:    0x09
 * Init:    0x00
 * Refin:   False
 * Refout:  False
 * Xorout:  0x00
 * Use:     MultiMediaCard,SD,ect.
 *****************************************************************************/
uint8_t crc7_mmc(uint8_t *data, uint16_t length)
{
    uint8_t i;
    uint8_t crc = 0; // Initial value
    while (length--)
    {
        crc ^= *data++; // crc ^= *data; data++;
        for (i = 0; i < 8; i++)
        {
            if (crc & 0x80)
                crc = (crc << 1) ^ 0x12; // 0x12 = 0x09<<(8-7)
            else
                crc <<= 1;
        }
    }
    return crc >> 1;
}

/******************************************************************************
 * Name:    CRC-8               x8+x2+x+1
 * Poly:    0x07
 * Init:    0x00
 * Refin:   False
 * Refout:  False
 * Xorout:  0x00
 * Note:
 *****************************************************************************/
uint8_t crc8(uint8_t *data, uint16_t length)
{
    uint8_t i;
    uint8_t crc = 0; // Initial value
    while (length--)
    {
        crc ^= *data++; // crc ^= *data; data++;
        for (i = 0; i < 8; i++)
        {
            if (crc & 0x80)
                crc = (crc << 1) ^ 0x07;
            else
                crc <<= 1;
        }
    }
    return crc;
}

/******************************************************************************
 * Name:    CRC-8/ITU           x8+x2+x+1
 * Poly:    0x07
 * Init:    0x00
 * Refin:   False
 * Refout:  False
 * Xorout:  0x55
 * Alias:   CRC-8/ATM
 *****************************************************************************/
uint8_t crc8_itu(uint8_t *data, uint16_t length)
{
    uint8_t i;
    uint8_t crc = 0; // Initial value
    while (length--)
    {
        crc ^= *data++; // crc ^= *data; data++;
        for (i = 0; i < 8; i++)
        {
            if (crc & 0x80)
                crc = (crc << 1) ^ 0x07;
            else
                crc <<= 1;
        }
    }
    return crc ^ 0x55;
}

/******************************************************************************
 * Name:    CRC-8/ROHC          x8+x2+x+1
 * Poly:    0x07
 * Init:    0xFF
 * Refin:   True
 * Refout:  True
 * Xorout:  0x00
 * Note:
 *****************************************************************************/
uint8_t crc8_rohc(uint8_t *data, uint16_t length)
{
    uint8_t i;
    uint8_t crc = 0xFF; // Initial value
    while (length--)
    {
        crc ^= *data++; // crc ^= *data; data++;
        for (i = 0; i < 8; ++i)
        {
            if (crc & 1)
                crc = (crc >> 1) ^ 0xE0; // 0xE0 = reverse 0x07
            else
                crc = (crc >> 1);
        }
    }
    return crc;
}

/******************************************************************************
 * Name:    CRC-8/MAXIM         x8+x5+x4+1
 * Poly:    0x31
 * Init:    0x00
 * Refin:   True
 * Refout:  True
 * Xorout:  0x00
 * Alias:   DOW-CRC,CRC-8/IBUTTON
 * Use:     Maxim(Dallas)'s some devices,e.g. DS18B20
 *****************************************************************************/
uint8_t crc8_maxim(uint8_t *data, uint16_t length)
{
    uint8_t i;
    uint8_t crc = 0; // Initial value
    while (length--)
    {
        crc ^= *data++; // crc ^= *data; data++;
        for (i = 0; i < 8; i++)
        {
            if (crc & 1)
                crc = (crc >> 1) ^ 0x8C; // 0x8C = reverse 0x31
            else
                crc >>= 1;
        }
    }
    return crc;
}

/******************************************************************************
 * Name:    CRC-16/IBM          x16+x15+x2+1
 * Poly:    0x8005
 * Init:    0x0000
 * Refin:   True
 * Refout:  True
 * Xorout:  0x0000
 * Alias:   CRC-16,CRC-16/ARC,CRC-16/LHA
 *****************************************************************************/
uint16_t crc16_ibm(uint8_t *data, uint16_t length)
{
    uint8_t i;
    uint16_t crc = 0; // Initial value
    while (length--)
    {
        crc ^= *data++; // crc ^= *data; data++;
        for (i = 0; i < 8; ++i)
        {
            if (crc & 1)
                crc = (crc >> 1) ^ 0xA001; // 0xA001 = reverse 0x8005
            else
                crc = (crc >> 1);
        }
    }
    return crc;
}

/******************************************************************************
 * Name:    CRC-16/MAXIM        x16+x15+x2+1
 * Poly:    0x8005
 * Init:    0x0000
 * Refin:   True
 * Refout:  True
 * Xorout:  0xFFFF
 * Note:
 *****************************************************************************/
uint16_t crc16_maxim(uint8_t *data, uint16_t length)
{
    uint8_t i;
    uint16_t crc = 0; // Initial value
    while (length--)
    {
        crc ^= *data++; // crc ^= *data; data++;
        for (i = 0; i < 8; ++i)
        {
            if (crc & 1)
                crc = (crc >> 1) ^ 0xA001; // 0xA001 = reverse 0x8005
            else
                crc = (crc >> 1);
        }
    }
    return ~crc; // crc^0xffff
}

/******************************************************************************
 * Name:    CRC-16/USB          x16+x15+x2+1
 * Poly:    0x8005
 * Init:    0xFFFF
 * Refin:   True
 * Refout:  True
 * Xorout:  0xFFFF
 * Note:
 *****************************************************************************/
uint16_t crc16_usb(uint8_t *data, uint16_t length)
{
    uint8_t i;
    uint16_t crc = 0xffff; // Initial value
    while (length--)
    {
        crc ^= *data++; // crc ^= *data; data++;
        for (i = 0; i < 8; ++i)
        {
            if (crc & 1)
                crc = (crc >> 1) ^ 0xA001; // 0xA001 = reverse 0x8005
            else
                crc = (crc >> 1);
        }
    }
    return ~crc; // crc^0xffff
}

/******************************************************************************
 * Name:    CRC-16/MODBUS       x16+x15+x2+1
 * Poly:    0x8005
 * Init:    0xFFFF
 * Refin:   True
 * Refout:  True
 * Xorout:  0x0000
 * Note:
 *****************************************************************************/
uint16_t crc16_modbus(uint8_t *data, uint16_t length)
{
    uint8_t i;
    uint16_t crc = 0xffff; // Initial value
    while (length--)
    {
        crc ^= *data++; // crc ^= *data; data++;
        for (i = 0; i < 8; ++i)
        {
            if (crc & 1)
                crc = (crc >> 1) ^ 0xA001; // 0xA001 = reverse 0x8005
            else
                crc = (crc >> 1);
        }
    }
    return crc;
}

/******************************************************************************
 * Name:    CRC-16/CCITT        x16+x12+x5+1
 * Poly:    0x1021
 * Init:    0x0000
 * Refin:   True
 * Refout:  True
 * Xorout:  0x0000
 * Alias:   CRC-CCITT,CRC-16/CCITT-TRUE,CRC-16/KERMIT
 *****************************************************************************/
uint16_t crc16_ccitt(uint8_t *data, uint16_t length)
{
    uint8_t i;
    uint16_t crc = 0; // Initial value
    while (length--)
    {
        crc ^= *data++; // crc ^= *data; data++;
        for (i = 0; i < 8; ++i)
        {
            if (crc & 1)
                crc = (crc >> 1) ^ 0x8408; // 0x8408 = reverse 0x1021
            else
                crc = (crc >> 1);
        }
    }
    return crc;
}

/******************************************************************************
 * Name:    CRC-16/CCITT-FALSE   x16+x12+x5+1
 * Poly:    0x1021
 * Init:    0xFFFF
 * Refin:   False
 * Refout:  False
 * Xorout:  0x0000
 * Note:
 *****************************************************************************/
uint16_t crc16_ccitt_false(uint8_t *data, uint16_t length)
{
    uint8_t i;
    uint16_t crc = 0xffff; // Initial value
    while (length--)
    {
        crc ^= (uint16_t)(*data++) << 8; // crc ^= (uint6_t)(*data)<<8; data++;
        for (i = 0; i < 8; ++i)
        {
            if (crc & 0x8000)
                crc = (crc << 1) ^ 0x1021;
            else
                crc <<= 1;
        }
    }
    return crc;
}

/******************************************************************************
 * Name:    CRC-16/X25          x16+x12+x5+1
 * Poly:    0x1021
 * Init:    0xFFFF
 * Refin:   True
 * Refout:  True
 * Xorout:  0XFFFF
 * Note:
 *****************************************************************************/
uint16_t crc16_x25(uint8_t *data, uint16_t length)
{
    uint8_t i;
    uint16_t crc = 0xffff; // Initial value
    while (length--)
    {
        crc ^= *data++; // crc ^= *data; data++;
        for (i = 0; i < 8; ++i)
        {
            if (crc & 1)
                crc = (crc >> 1) ^ 0x8408; // 0x8408 = reverse 0x1021
            else
                crc = (crc >> 1);
        }
    }
    return ~crc; // crc^Xorout
}

/******************************************************************************
 * Name:    CRC-16/XMODEM       x16+x12+x5+1
 * Poly:    0x1021
 * Init:    0x0000
 * Refin:   False
 * Refout:  False
 * Xorout:  0x0000
 * Alias:   CRC-16/ZMODEM,CRC-16/ACORN
 *****************************************************************************/
uint16_t crc16_xmodem(uint8_t *data, uint16_t length)
{
    uint8_t i;
    uint16_t crc = 0; // Initial value
    while (length--)
    {
        crc ^= (uint16_t)(*data++) << 8; // crc ^= (uint16_t)(*data)<<8; data++;
        for (i = 0; i < 8; ++i)
        {
            if (crc & 0x8000)
                crc = (crc << 1) ^ 0x1021;
            else
                crc <<= 1;
        }
    }
    return crc;
}

/******************************************************************************
 * Name:    CRC-16/DNP          x16+x13+x12+x11+x10+x8+x6+x5+x2+1
 * Poly:    0x3D65
 * Init:    0x0000
 * Refin:   True
 * Refout:  True
 * Xorout:  0xFFFF
 * Use:     M-Bus,ect.
 *****************************************************************************/
uint16_t crc16_dnp(uint8_t *data, uint16_t length)
{
    uint8_t i;
    uint16_t crc = 0; // Initial value
    while (length--)
    {
        crc ^= *data++; // crc ^= *data; data++;
        for (i = 0; i < 8; ++i)
        {
            if (crc & 1)
                crc = (crc >> 1) ^ 0xA6BC; // 0xA6BC = reverse 0x3D65
            else
                crc = (crc >> 1);
        }
    }
    return ~crc; // crc^Xorout
}

/******************************************************************************
 * Name:    CRC-32  x32+x26+x23+x22+x16+x12+x11+x10+x8+x7+x5+x4+x2+x+1
 * Poly:    0x04C11DB7
 * Init:    0xFFFFFFFF
 * Refin:   True
 * Refout:  True
 * Xorout:  0xFFFFFFF
 * Alias:   CRC_32/ADCCP
 * Use:     WinRAR,ect.
 *****************************************************************************/
uint32_t crc32(uint8_t *data, uint16_t length)
{
    uint8_t i;
    uint32_t crc = 0xffffffff; // Initial value
    while (length--)
    {
        crc ^= *data++; // crc ^= *data; data++;
        for (i = 0; i < 8; ++i)
        {
            if (crc & 1)
                crc = (crc >> 1) ^ 0xEDB88320; // 0xEDB88320= reverse 0x04C11DB7
            else
                crc = (crc >> 1);
        }
    }
    return ~crc;
}

/******************************************************************************
 * Name:    CRC-32/MPEG-2  x32+x26+x23+x22+x16+x12+x11+x10+x8+x7+x5+x4+x2+x+1
 * Poly:    0x4C11DB7
 * Init:    0xFFFFFFF
 * Refin:   False
 * Refout:  False
 * Xorout:  0x0000000
 * Note:
 *****************************************************************************/
uint32_t crc32_mpeg_2(uint8_t *data, uint16_t length)
{
    uint8_t i;
    uint32_t crc = 0xffffffff; // Initial value
    while (length--)
    {
        crc ^= (uint32_t)(*data++) << 24; // crc ^=(uint32_t)(*data)<<24; data++;
        for (i = 0; i < 8; ++i)
        {
            if (crc & 0x80000000)
                crc = (crc << 1) ^ 0x04C11DB7;
            else
                crc <<= 1;
        }
    }
    return crc;
}

// 预计算 CRC32 查表法使用的表（可选：运行时生成或静态定义）
static uint32_t crc32_table[256];
static uint16_t crc16_table[256];
static uint32_t crc24_table[256];

#if 0
// 辅助函数：反转8位数据的位序
static uint8_t reverse_bits8(uint8_t value)
{
    value = ((value & 0xAA) >> 1) | ((value & 0x55) << 1);
    value = ((value & 0xCC) >> 2) | ((value & 0x33) << 2);
    value = ((value & 0xF0) >> 4) | ((value & 0x0F) << 4);
    return value;
}
#endif

static uint16_t reverse_bits16(uint16_t x)
{
    x = ((x & 0xAAAA) >> 1) | ((x & 0x5555) << 1);
    x = ((x & 0xCCCC) >> 2) | ((x & 0x3333) << 2);
    x = ((x & 0xF0F0) >> 4) | ((x & 0x0F0F) << 4);
    x = ((x & 0xFF00) >> 8) | ((x & 0x00FF) << 8);
    return x;
}

static uint32_t reverse_bits24(uint32_t x)
{
    uint32_t r = 0;
    for (int i = 0; i < 24; ++i) {
        if (x & (1u << i))
            r |= 1u << (23 - i);
    }
    return r;
}

// 辅助函数：反转32位数据的位序
static uint32_t reverse_bits32(uint32_t value)
{
    value = ((value & 0xAAAAAAAA) >> 1) | ((value & 0x55555555) << 1);
    value = ((value & 0xCCCCCCCC) >> 2) | ((value & 0x33333333) << 2);
    value = ((value & 0xF0F0F0F0) >> 4) | ((value & 0x0F0F0F0F) << 4);
    value = ((value & 0xFF00FF00) >> 8) | ((value & 0x00FF00FF) << 8);
    value = ((value & 0xFFFF0000) >> 16) | ((value & 0x0000FFFF) << 16);
    return value;
}

/* 初始化 CRC16 查表 */
void crc16_init_table(const crc_config_t *config)
{
    uint16_t poly = config->poly & 0xFFFF;
    if (config->refin)
        poly = reverse_bits16(poly);

    for (int i = 0; i < 256; ++i) {
        uint16_t crc;
        if (config->refin) {
            crc = (uint16_t)i;
            for (int j = 0; j < 8; ++j) {
                if (crc & 1)
                    crc = (crc >> 1) ^ poly;
                else
                    crc >>= 1;
            }
        } else {
            crc = (uint16_t)i << 8;
            for (int j = 0; j < 8; ++j) {
                if (crc & 0x8000)
                    crc = (crc << 1) ^ poly;
                else
                    crc <<= 1;
            }
        }
        crc16_table[i] = crc;
    }
}

/* 计算 CRC16 */
uint16_t crc16_compute(const uint8_t *data, size_t len, const crc_config_t *config)
{
    const uint16_t mask = 0xFFFFu;
    uint16_t crc = config->init & mask;

    if (config->refin) {
        crc = reverse_bits16(crc);
    }

    if (config->refin) {
        for (size_t i = 0; i < len; ++i)
            crc = crc16_table[(crc ^ data[i]) & 0xFFu] ^ (crc >> 8);
    } else {
        for (size_t i = 0; i < len; ++i)
            crc = crc16_table[((crc >> 8) ^ data[i]) & 0xFFu] ^ (crc << 8);
    }

    if (config->refout != config->refin) {
        crc = reverse_bits16(crc);
    }

    crc ^= (config->xorout & mask);
    return crc & mask;
}

/* 初始化 CRC24 查表 */
void crc24_init_table(const crc_config_t *config)
{
    uint32_t poly = config->poly & 0xFFFFFF;
    if (config->refin)
        poly = reverse_bits24(poly);

    for (int i = 0; i < 256; ++i) {
        uint32_t crc;
        if (config->refin) {
            crc = (uint32_t)i;
            for (int j = 0; j < 8; ++j) {
                if (crc & 1)
                    crc = (crc >> 1) ^ poly;
                else
                    crc >>= 1;
            }
        } else {
            crc = (uint32_t)i << 16;
            for (int j = 0; j < 8; ++j) {
                if (crc & 0x800000)
                    crc = ((crc << 1) ^ poly) & 0xFFFFFF;
                else
                    crc = (crc << 1) & 0xFFFFFF;
            }
        }
        crc24_table[i] = crc & 0xFFFFFF;
    }
}

uint32_t crc24_compute(const uint8_t *data, size_t len, const crc_config_t *config)
{
    const uint32_t mask = 0xFFFFFFu;
    uint32_t crc = config->init & mask;

    if (config->refin) {
        crc = reverse_bits24(crc);
    }

    if (config->refin) {
        for (size_t i = 0; i < len; ++i)
            crc = crc24_table[(crc ^ data[i]) & 0xFFu] ^ (crc >> 8);
    } else {
        for (size_t i = 0; i < len; ++i)
            crc = crc24_table[((crc >> 16) ^ data[i]) & 0xFFu] ^ ((crc << 8) & mask);
    }

    if (config->refout != config->refin) {
        crc = reverse_bits24(crc);
    }

    crc ^= (config->xorout & mask);
    return crc & mask;
}

// 初始化查表：注意，如果 refin 为 true，需要用 "反射后的多项式" 生成表（右移、LSB-first）
// 若 config->poly 是常规 MSB 形式 (例如 0x04C11DB7)，函数内部会自动在 refin 时转换为反射形式。
void crc32_init_table(const crc_config_t *config)
{
    uint32_t poly = config->poly;
    if (config->refin) {
        // 反射 poly 得到 LSB-first 形式（例如 0x04C11DB7 -> 0xEDB88320）
        poly = reverse_bits32(poly);
    }

    for (int i = 0; i < 256; ++i) {
        uint32_t crc;
        if (config->refin) {
            // LSB-first table 生成（右移）
            crc = (uint32_t)i;
            for (int j = 0; j < 8; ++j) {
                if (crc & 1)
                    crc = (crc >> 1) ^ poly;
                else
                    crc >>= 1;
            }
        } else {
            // MSB-first table 生成（左移）
            crc = (uint32_t)i << 24;
            for (int j = 0; j < 8; ++j) {
                if (crc & 0x80000000U)
                    crc = (crc << 1) ^ poly;
                else
                    crc <<= 1;
            }
        }
        crc32_table[i] = crc & 0xFFFFFFFFU;
    }
}

// 通用 CRC32（查表法）
uint32_t crc32_compute(const uint8_t *data, size_t len, const crc_config_t *config)
{
    const uint32_t mask = 0xFFFFFFFFu;
    uint32_t crc = config->init & mask;

    /* 如果使用 LSB-first（refin），则把 init 反射为内部位序 */
    if (config->refin) {
        crc = reverse_bits32(crc);
    }

    if (config->refin) {
        for (size_t i = 0; i < len; ++i)
            crc = crc32_table[(crc ^ data[i]) & 0xFFu] ^ (crc >> 8);
    } else {
        for (size_t i = 0; i < len; ++i)
            crc = crc32_table[((crc >> 24) ^ data[i]) & 0xFFu] ^ (crc << 8);
    }

    /* 如果输出和输入的反射状态不同，需要反射一次（按位宽） */
    if (config->refout != config->refin) {
        crc = reverse_bits32(crc);
    }

    crc ^= (config->xorout & mask);
    return crc & mask;
}
