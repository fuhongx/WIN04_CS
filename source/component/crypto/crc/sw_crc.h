#ifndef __SW_CRC_H__
#define __SW_CRC_H__

#include "stdint.h"
#include "stdbool.h"

uint8_t crc4_itu(uint8_t *data, uint16_t length);
uint8_t crc5_epc(uint8_t *data, uint16_t length);
uint8_t crc5_itu(uint8_t *data, uint16_t length);
uint8_t crc5_usb(uint8_t *data, uint16_t length);
uint8_t crc6_itu(uint8_t *data, uint16_t length);
uint8_t crc7_mmc(uint8_t *data, uint16_t length);
uint8_t crc8(uint8_t *data, uint16_t length);
uint8_t crc8_itu(uint8_t *data, uint16_t length);
uint8_t crc8_rohc(uint8_t *data, uint16_t length);
uint8_t crc8_maxim(uint8_t *data, uint16_t length);//DS18B20
uint16_t crc16_ibm(uint8_t *data, uint16_t length);
uint16_t crc16_maxim(uint8_t *data, uint16_t length);
uint16_t crc16_usb(uint8_t *data, uint16_t length);
uint16_t crc16_modbus(uint8_t *data, uint16_t length);
uint16_t crc16_ccitt(uint8_t *data, uint16_t length);
uint16_t crc16_ccitt_false(uint8_t *data, uint16_t length);
uint16_t crc16_x25(uint8_t *data, uint16_t length);
uint16_t crc16_xmodem(uint8_t *data, uint16_t length);
uint16_t crc16_dnp(uint8_t *data, uint16_t length);
uint32_t crc32(uint8_t *data, uint16_t length);
uint32_t crc32_mpeg_2(uint8_t *data, uint16_t length);

// CRC32 配置结构体
typedef struct {
    uint32_t poly;      // 多项式
    uint32_t init;      // 初始值
    uint32_t xorout;    // 异或输出值
    uint8_t  refin;     // 输入是否反转（按位反转，LSB first）
    uint8_t  refout;    // 输出是否反转
} crc_config_t;

void crc16_init_table(const crc_config_t *config);
uint16_t crc16_compute(const uint8_t *data, size_t len, const crc_config_t *config);
void crc24_init_table(const crc_config_t *config);
uint32_t crc24_compute(const uint8_t *data, size_t len, const crc_config_t *config);
void crc32_init_table(const crc_config_t *config);
uint32_t crc32_compute(const uint8_t *data, size_t len, const crc_config_t *config);

#endif // __SW_CRC_H__
