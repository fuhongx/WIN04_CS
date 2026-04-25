
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "reg_Dev.h"
#include "error_def.h"
#include "qmx_hal_sysctrl.h"
#include "hw_flash.h"
#include "app_cfg.h"
#include "debug.h"
#include "utility.h"
#include "qmx_flash_test.h"
#include "qmx_hal_rng.h"

int qmx_flash_read_id_test(void)
{
    uint8_t dev_id[3] = {0};
    uint8_t uid[8] = {0};

    rom_hw_flash_read_dev_id(dev_id, 3);
    rom_hw_flash_read_uid(uid, 8);
    dump_u8buf("flash dev id", dev_id, sizeof(dev_id));
    dump_u8buf("flash uid", uid, sizeof(uid));

    return 0;
}

__RAM_FUNC int qmx_flash_erase_write_read_test(void)
{
    uint32_t erase_len = UNIT_SECTOR;
    uint32_t single_write_len = UNIT_PAGE;
    uint32_t start_addr = FLASH_USERER_DATA_ADDR;
    uint8_t write_data[UNIT_PAGE] = {0};
    uint8_t *buffer = malloc(single_write_len);
    uint32_t i = 0;
    int ret = 0;
    hal_rng_init_t rng_init = {0};

    if (buffer == NULL) {
        PRINTF("malloc buffer error\n");
        return -1;
    }

    qmx_hal_sysctrl_peripheral_clk_enable(HAL_CLK_RAND, true);
    qmx_hal_sysctrl_peripheral_mod_reset(HAL_CLK_RAND);
    rng_init.seed = 0x12345678;
    qmx_hal_rng_init(&rng_init);

    ret = qmx_hal_get_random_u8(write_data, sizeof(write_data), 1000);
    if (ret != 0) {
        PRINTF("get random write_data error, ret=%d\n", ret);
        free(buffer);
        return -1;
    }

    rom_hw_flash_erase_by_length(FLASH_USERER_DATA_ADDR, erase_len);

    while (start_addr < (FLASH_USERER_DATA_ADDR + erase_len)) {
        memset(buffer, 0, single_write_len);
        rom_hw_flash_read_data(start_addr, buffer, single_write_len);
        for (i = 0; i < single_write_len; i++) {
            if (buffer[i] != 0xFF) {
                PRINTF("address 0x%X value: 0x%X is not oxFF after erase!\n", start_addr + i, buffer[i]);
                free(buffer);
                return -1;
            }
        }

        rom_hw_flash_write_data(start_addr, write_data, single_write_len);
        rom_hw_flash_read_data(start_addr, buffer, single_write_len);
        for (i = 0; i < single_write_len; i++) {
            if (buffer[i] != write_data[i]) {
                PRINTF("address 0x%X value 0x%X is not equal to write value: 0x%X!\n", start_addr + i, buffer[i], write_data[i]);
                free(buffer);
                return -1;
            }
        }

        start_addr += single_write_len;
    }
    PRINTF("erase and write test success!\n");

    start_addr = FLASH_USERER_DATA_ADDR;
    rom_hw_flash_erase_by_length(FLASH_USERER_DATA_ADDR, erase_len);
    // 如果不flush cache，读出来的数据是擦除前的数据
    qmx_hal_sysctrl_cache_mode_set(HAL_CACHE_FLUSH);

    while (start_addr < (FLASH_USERER_DATA_ADDR + erase_len)) {
        memset(buffer, 0, single_write_len);
        rom_hw_flash_read_data(start_addr, buffer, single_write_len);
        for (i = 0; i < single_write_len; i++) {
            if (buffer[i] != 0xFF) {
                PRINTF("after test, address 0x%X value is not oxFF after erase! value: 0x%X\n", start_addr + i, buffer[i]);
                free(buffer);
                return -1;
            }
        }
        start_addr += single_write_len;
    }

    free(buffer);
    return 0;
}

int qmx_flash_qspi_div_test(void)
{
    int i;
    for (i = EN_FLASH_CLK_DIV2; i <= EN_FLASH_CLK_DIV32; i++) {
        rom_hw_flash_ctrl_set_clk_div((EN_FLASH_CLK_DIV_T)i);
        PRINTF("set clk div %d\n", (i + 1) * 2);

        if (qmx_flash_erase_write_read_test() != 0) {
            PRINTF("flash clk div %d erase rw test fail!\n", (i + 1) * 2);
            return -1;
        }
    }

    rom_hw_flash_ctrl_set_clk_div(EN_FLASH_CLK_DIV2);
    return 0;
}

int qmx_flash_read_write_mode_test(void)
{
    uint8_t read_write_mode[4] = {EN_FLASH_RW_MODE_SPI, EN_FLASH_RW_MODE_DPI, EN_FLASH_RW_MODE_QSPI, EN_FLASH_RW_MODE_FQSPI};

    for (int i = 0; i < (sizeof(read_write_mode)/sizeof(read_write_mode[0])); i++) {
        rom_hw_flash_config_read_write_mode((EN_FLASH_RW_MODE_T)read_write_mode[i]);
        PRINTF("config read write mode: %d\n", read_write_mode[i]);
        if (qmx_flash_erase_write_read_test() != 0) {
            PRINTF("read write mode %d erase rw test fail!\n", read_write_mode[i]);
            rom_hw_flash_config_read_write_mode(EN_FLASH_RW_MODE_QSPI);
            return -1;
        }
    }

    rom_hw_flash_config_read_write_mode(EN_FLASH_RW_MODE_QSPI);
    return 0;
}

int qmx_flash_qspi_mode_test(void)
{
    for (int i = EN_FLASH_QSPI_MODE_0; i < EN_FLASH_QSPI_MODE_MAX; i++) {
        rom_hw_flash_ctrl_set_qspi_mode((EN_FLASH_CTRL_SPI_MODE_T)i);
        PRINTF("set qspi mode: %d\n", i);
        if (qmx_flash_erase_write_read_test() != 0) {
            PRINTF("qspi mode %d erase rw test fail!\n", i);
            rom_hw_flash_ctrl_set_qspi_mode(EN_FLASH_QSPI_MODE_0);
            return -1;
        }
    }

    rom_hw_flash_ctrl_set_qspi_mode(EN_FLASH_QSPI_MODE_0);
    return 0;
}

int qmx_flash_read_data_capture_test(void)
{
    for (int i = 0; i < 0xff; i += 0x10) {
        for (int j = 0; j < 2; j++) {
            PRINTF("set read data capture sample delay: 0x%X, edge: %d\n", i, j);
            rom_hw_flash_ctrl_set_read_data_capture(i, j);
            if (qmx_flash_erase_write_read_test() != 0) {
                PRINTF("sample delay: 0x%X, edge: %d erase rw test fail!\n", i, j);
                return -1;
            }
        }
    }

    return 0;
}

int qmx_flash_qspi_delay_test(void)
{
    unFlashQspiDelay_t delay;
    delay.u32Cfg = 0x01010101;
    uint32_t delay_stop = 0x05050505;
    uint32_t delay_step = 0x01010101;

    while (delay.u32Cfg <= delay_stop) {
        rom_hw_flash_ctrl_set_qspi_delay(delay);
        PRINTF("set qspi delay 0x%X\n", delay.u32Cfg);
        if (qmx_flash_erase_write_read_test() != 0) {
            PRINTF("qspi delay 0x%X erase rw test fail!\n", delay.u32Cfg);
            return -1;
        }
        delay.u32Cfg += delay_step;
    }
    return 0;
}

__RAM_FUNC int qmx_flash_security_register_test(void)
{
    uint8_t write_data[256] = {0x0};
    uint32_t erase_len = 256;
    uint8_t buffer[256] = {0x0};
    int i;
    int ret = 0;
    hal_rng_init_t rng_init = {0};

    qmx_hal_sysctrl_peripheral_clk_enable(HAL_CLK_RAND, true);
    qmx_hal_sysctrl_peripheral_mod_reset(HAL_CLK_RAND);
    rng_init.seed = 0x12345678;
    qmx_hal_rng_init(&rng_init);

    ret = qmx_hal_get_random_u8(write_data, sizeof(write_data), 1000);
    if (ret != 0) {
        PRINTF("get random write_data error, ret=0x%X\n", ret);
        return -1;
    }

    rom_hw_flash_erase_security_mem(EN_FLASH_SEC_MEM1, FLASH_ERASE_SECURITY_MEM_KEY);
    rom_hw_flash_read_security_mem(EN_FLASH_SEC_MEM1, 0, buffer, erase_len);
    for (i = 0; i < erase_len; i++) {
        if (buffer[i] != 0xFF) {
            PRINTF("EN_FLASH_SEC_MEM1 0x%X value: 0x%X is not oxFF after erase!\n", i, buffer[i]);
            return -1;
        }
    }

    rom_hw_flash_write_security_mem(EN_FLASH_SEC_MEM1, 0, write_data, erase_len);
    rom_hw_flash_read_security_mem(EN_FLASH_SEC_MEM1, 0, buffer, erase_len);
    for (i = 0; i < erase_len; i++) {
        if (buffer[i] != write_data[i]) {
            PRINTF("EN_FLASH_SEC_MEM1 0x%X value: 0x%X is not equal to write data 0x%X!\n", i, buffer[i], write_data[i]);
            return -1;
        }
    }

    return 0;
}

__RAM_FUNC int qmx_flash_protect_test(void)
{
    uint8_t sta = 0;
    uint32_t val = 0;
    rom_hw_flash_read_status_reg(EN_FLASH_READ_STA_REG1, &sta);
    PRINTF("Flash status register 1: 0x%X\n", sta);
    rom_hw_flash_read_status_reg(EN_FLASH_READ_STA_REG2, &sta);
    PRINTF("Flash status register 2: 0x%X\n", sta);
    rom_hw_flash_read_status_reg(EN_FLASH_READ_STA_REG3, &sta);
    PRINTF("Flash status register 3: 0x%X\n", sta);

    rom_hw_flash_write_status_reg(EN_FLASH_READ_STA_REG1, 0x44);
    rom_hw_flash_read_status_reg(EN_FLASH_READ_STA_REG1, &sta);
    PRINTF("Flash status register 1: 0x%X\n", sta);
    rom_hw_flash_read_status_reg(EN_FLASH_READ_STA_REG2, &sta);
    PRINTF("Flash status register 2: 0x%X\n", sta);

    val = read32(FLASH_MAX_ADDR - UNIT_1K);
    PRINTF("test flash 0x%X\n", val);
    write32(FLASH_MAX_ADDR - UNIT_1K, val + 0x12345678);
    PRINTF("test flash 0x%X\n", read32(FLASH_MAX_ADDR - UNIT_1K));

    val = read32(0x8000FF8);
    PRINTF("test flash 0x%X\n", val);
    write32(0x8000FF8, val + 0x12345678);
    PRINTF("test flash 0x%X\n", read32(0x8000FF8));

    val = read32(FLASH_FIRM_UP_BOOT2_CODE_ADDR);
    PRINTF("addr 0x%X test flash 0x%X\n", FLASH_FIRM_UP_BOOT2_CODE_ADDR, val);
    write32(FLASH_FIRM_UP_BOOT2_CODE_ADDR, val + 0x12345678);
    PRINTF("test flash 0x%X\n", read32(FLASH_FIRM_UP_BOOT2_CODE_ADDR));

    val = read32(FLASH_FIRM_UP_BOOT2_CODE_ADDR + UNIT_1K * 20);
    PRINTF("addr 0x%X test flash 0x%X\n", FLASH_FIRM_UP_BOOT2_CODE_ADDR + UNIT_1K * 20, val);
    write32(FLASH_FIRM_UP_BOOT2_CODE_ADDR + UNIT_1K * 20, val + 0x12345678);
    PRINTF("test flash 0x%X\n", read32(FLASH_FIRM_UP_BOOT2_CODE_ADDR + UNIT_1K * 20));

    return 0;
}
