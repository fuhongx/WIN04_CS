
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "reg_Dev.h"
#include "error_def.h"
#include "slc_hal_sysctrl.h"
#include "hw_flash.h"
#include "app_cfg.h"
#include "debug.h"
#include "utility.h"
#include "slc_flash_test.h"
#include "slc_hal_rng.h"
#include "slc_hal_delay.h"

int slc_flash_read_id_test(void)
{
    uint8_t dev_id[3] = {0};
    uint8_t uid[8] = {0};

    rom_hw_flash_read_dev_id(dev_id, 3);
    rom_hw_flash_read_uid(uid, 8);
    dump_u8buf("flash dev id", dev_id, sizeof(dev_id));
    dump_u8buf("flash uid", uid, sizeof(uid));

    return 0;
}

__RAM_FUNC int slc_flash_erase_write_read_test(void)
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

    slc_hal_sysctrl_peripheral_clk_enable(HAL_CLK_RAND, true);
    slc_hal_sysctrl_peripheral_mod_reset(HAL_CLK_RAND);
    rng_init.seed = 0x12345678;
    slc_hal_rng_init(&rng_init);

    ret = slc_hal_get_random_u8(write_data, sizeof(write_data), 1000);
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
    slc_hal_sysctrl_cache_mode_set(HAL_CACHE_FLUSH);

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

/**
 * @brief QSPI时钟分频测试，遍历clk/4~clk/64(步长clk/4)共16档分频，
 *        每档分频下执行flash擦写读测试，结束后恢复原始分频配置。
 * @return 0: 全部通过; -1: 某档分频测试失败
 */
int slc_flash_qspi_div_test(void)
{
    int i;
    int ret = 0;
    uint32_t clk_div_save = (FLASH_CTRL->QSPI_CFG >> FLASH_CTRL_QSPI_CFG_CLK_DIV_SHIFT) & FLASH_CTRL_QSPI_CFG_CLK_DIV_MASK;

    for (i = EN_FLASH_CLK_DIV2; i <= EN_FLASH_CLK_DIV32; i++) {
        rom_hw_flash_ctrl_set_clk_div((EN_FLASH_CLK_DIV_T)i);
        PRINTF("set clk div %d\n", (i + 1) * 2);

        if (slc_flash_erase_write_read_test() != 0) {
            PRINTF("flash clk div %d erase rw test fail!\n", (i + 1) * 2);
            ret = -1;
            break;
        }
    }

    rom_hw_flash_ctrl_set_clk_div((EN_FLASH_CLK_DIV_T)clk_div_save);
    return ret;
}

int slc_flash_read_write_mode_test(void)
{
    uint8_t read_write_mode[4] = {EN_FLASH_RW_MODE_SPI, EN_FLASH_RW_MODE_DPI, EN_FLASH_RW_MODE_QSPI, EN_FLASH_RW_MODE_FQSPI};

    for (int i = 0; i < (sizeof(read_write_mode)/sizeof(read_write_mode[0])); i++) {
        rom_hw_flash_config_read_write_mode((EN_FLASH_RW_MODE_T)read_write_mode[i]);
        PRINTF("config read write mode: %d\n", read_write_mode[i]);
        if (slc_flash_erase_write_read_test() != 0) {
            PRINTF("read write mode %d erase rw test fail!\n", read_write_mode[i]);
            rom_hw_flash_config_read_write_mode(EN_FLASH_RW_MODE_QSPI);
            return -1;
        }
    }

    rom_hw_flash_config_read_write_mode(EN_FLASH_RW_MODE_QSPI);
    return 0;
}

int slc_flash_qspi_mode_test(void)
{
    for (int i = EN_FLASH_QSPI_MODE_0; i < EN_FLASH_QSPI_MODE_MAX; i++) {
        rom_hw_flash_ctrl_set_qspi_mode((EN_FLASH_CTRL_SPI_MODE_T)i);
        PRINTF("set qspi mode: %d\n", i);
        if (slc_flash_erase_write_read_test() != 0) {
            PRINTF("qspi mode %d erase rw test fail!\n", i);
            rom_hw_flash_ctrl_set_qspi_mode(EN_FLASH_QSPI_MODE_0);
            return -1;
        }
    }

    rom_hw_flash_ctrl_set_qspi_mode(EN_FLASH_QSPI_MODE_0);
    return 0;
}

/**
 * @brief Flash读数据采样参数测试，遍历16种采样延迟(0x00~0xF0，步长0x10)
 *        和2种采样边沿(下降沿/上升沿)组合，每组合下执行擦写读测试，
 *        结束后恢复原始采样配置。
 * @return 0: 全部通过; -1: 某组参数测试失败
 */
int slc_flash_read_data_capture_test(void)
{
    uint32_t read_cap_save = FLASH_CTRL->READ_DATA_CAP;
    int ret = 0;

    for (int i = 0; i < 0xff; i += 0x10) {
        for (int j = 0; j < 2; j++) {
            PRINTF("set read data capture sample delay: 0x%X, edge: %d\n", i, j);
            rom_hw_flash_ctrl_set_read_data_capture(i, j);
            if (slc_flash_erase_write_read_test() != 0) {
                PRINTF("sample delay: 0x%X, edge: %d erase rw test fail!\n", i, j);
                ret = -1;
                goto restore;
            }
        }
    }

restore:
    FLASH_CTRL->READ_DATA_CAP = read_cap_save;
    return ret;
}

/**
 * @brief QSPI各阶段延迟参数测试，遍历CSSOT/CSEOT/CSDADS/CSDA延迟从0x01到0x05
 *       （步长0x01），每档延迟下执行擦写读测试，结束后恢复原始延迟配置。
 * @return 0: 全部通过; -1: 某档延迟测试失败
 */
int slc_flash_qspi_delay_test(void)
{
    unFlashQspiDelay_t delay;
    uint32_t delay_save = FLASH_CTRL->QSPI_DELAY;
    int ret = 0;
    delay.u32Cfg = 0x01010101;
    uint32_t delay_stop = 0x05050505;
    uint32_t delay_step = 0x01010101;

    while (delay.u32Cfg <= delay_stop) {
        rom_hw_flash_ctrl_set_qspi_delay(delay);
        PRINTF("set qspi delay 0x%X\n", delay.u32Cfg);
        if (slc_flash_erase_write_read_test() != 0) {
            PRINTF("qspi delay 0x%X erase rw test fail!\n", delay.u32Cfg);
            ret = -1;
            break;
        }
        delay.u32Cfg += delay_step;
    }

    FLASH_CTRL->QSPI_DELAY = delay_save;
    return ret;
}

__RAM_FUNC int slc_flash_security_register_test(void)
{
    uint8_t write_data[256] = {0x0};
    uint32_t erase_len = 256;
    uint8_t buffer[256] = {0x0};
    int i;
    int ret = 0;
    hal_rng_init_t rng_init = {0};

    slc_hal_sysctrl_peripheral_clk_enable(HAL_CLK_RAND, true);
    slc_hal_sysctrl_peripheral_mod_reset(HAL_CLK_RAND);
    rng_init.seed = 0x12345678;
    slc_hal_rng_init(&rng_init);

    ret = slc_hal_get_random_u8(write_data, sizeof(write_data), 1000);
    if (ret != 0) {
        PRINTF("get random write_data error, ret=0x%X\n", ret);
        return -1;
    }

    rom_hw_flash_erase_security_mem(EN_FLASH_SEC_MEM2, FLASH_ERASE_SECURITY_MEM_KEY);
    rom_hw_flash_read_security_mem(EN_FLASH_SEC_MEM2, 0, buffer, erase_len);
    for (i = 0; i < erase_len; i++) {
        if (buffer[i] != 0xFF) {
            PRINTF("EN_FLASH_SEC_MEM2 0x%X value: 0x%X is not oxFF after erase!\n", i, buffer[i]);
            dump_u8buf("EN_FLASH_SEC_MEM2", buffer, erase_len);
            return -1;
        }
    }

    rom_hw_flash_write_security_mem(EN_FLASH_SEC_MEM2, 0, write_data, erase_len);
    rom_hw_flash_read_security_mem(EN_FLASH_SEC_MEM2, 0, buffer, erase_len);
    for (i = 0; i < erase_len; i++) {
        if (buffer[i] != write_data[i]) {
            PRINTF("EN_FLASH_SEC_MEM2 0x%X value: 0x%X is not equal to write data 0x%X!\n", i, buffer[i], write_data[i]);
            dump_u8buf("EN_FLASH_SEC_MEM2", buffer, erase_len);
            dump_u8buf("write_data", write_data, erase_len);
            return -1;
        }
    }

    return 0;
}

/**
 * @brief Flash写保护功能测试：先读取并保存状态寄存器原始值，然后写入0x44
 *        设置块保护，验证受保护地址区域的写入行为，最后恢复原始写保护状态。
 *        注意：该测试会尝试写BOOT2_CODE_ADDR等关键地址区域，需人工判定结果。
 * @return 0: 测试完成
 */
__RAM_FUNC int slc_flash_protect_test(void)
{
    uint8_t sta = 0;
    uint8_t sta_orig1 = 0;
    uint32_t val = 0;
    rom_hw_flash_read_status_reg(EN_FLASH_READ_STA_REG1, &sta_orig1);
    PRINTF("Flash status register 1: 0x%X\n", sta_orig1);
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

    /* 恢复原始写保护状态 */
    rom_hw_flash_write_status_reg(EN_FLASH_READ_STA_REG1, sta_orig1);
    rom_hw_flash_read_status_reg(EN_FLASH_READ_STA_REG1, &sta);
    PRINTF("Flash status register 1 restored: 0x%X\n", sta);

    return 0;
}

/**
 * @brief Flash 耐久性测试：对同一地址反复执行擦除→写入随机数→回读比对，
 *        循环10000次，每1000次打印进度，验证flash存储单元无损坏。
 * @return 0: 全部通过; -1: 某次擦写读比对失败
 */
__RAM_FUNC int slc_flash_endurance_test(void)
{
#define ENDURANCE_CYCLES       10000
#define ENDURANCE_ERASE_LEN     UNIT_SECTOR
#define ENDURANCE_WRITE_LEN     UNIT_PAGE
#define ENDURANCE_ADDR          FLASH_USERER_DATA_ADDR
#define ENDURANCE_REPORT_STEP   1000

    uint8_t write_data[UNIT_PAGE] = {0};
    uint8_t *buffer = malloc(UNIT_PAGE);
    uint32_t i;
    int ret = 0;
    hal_rng_init_t rng_init = {0};

    if (buffer == NULL) {
        PRINTF("endurance test: malloc buffer error\n");
        return -1;
    }

    slc_hal_sysctrl_peripheral_clk_enable(HAL_CLK_RAND, true);
    slc_hal_sysctrl_peripheral_mod_reset(HAL_CLK_RAND);
    rng_init.seed = 0x12345678;
    slc_hal_rng_init(&rng_init);

    for (i = 0; i < ENDURANCE_CYCLES; i++) {
        ret = slc_hal_get_random_u8(write_data, sizeof(write_data), 1000);
        if (ret != 0) {
            PRINTF("endurance test[%d]: get random data fail, ret=%d\n", i, ret);
            free(buffer);
            return -1;
        }

        rom_hw_flash_erase_by_length(ENDURANCE_ADDR, ENDURANCE_ERASE_LEN);
        rom_hw_flash_write_data(ENDURANCE_ADDR, write_data, ENDURANCE_WRITE_LEN);

        slc_hal_sysctrl_cache_mode_set(HAL_CACHE_FLUSH);
        memset(buffer, 0, ENDURANCE_WRITE_LEN);
        rom_hw_flash_read_data(ENDURANCE_ADDR, buffer, ENDURANCE_WRITE_LEN);

        for (int j = 0; j < ENDURANCE_WRITE_LEN; j++) {
            if (buffer[j] != write_data[j]) {
                PRINTF("endurance test[%d]: addr 0x%X fail! wr=0x%X rd=0x%X\n",
                       i, ENDURANCE_ADDR + j, write_data[j], buffer[j]);
                free(buffer);
                return -1;
            }
        }

        if ((i + 1) % ENDURANCE_REPORT_STEP == 0) {
            PRINTF("endurance test: %d / %d PASS\n", i + 1, ENDURANCE_CYCLES);
        }
    }

    PRINTF("endurance test: %d cycles all PASS\n", ENDURANCE_CYCLES);
    free(buffer);
    return 0;
}
