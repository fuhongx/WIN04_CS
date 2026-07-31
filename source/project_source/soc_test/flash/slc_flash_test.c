
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "reg_Dev.h"
#include "error_def.h"
#include "slc_hal_sysctrl.h"
#include "hw_flash.h"
#include "hw_sysctrl.h"
#include "app_cfg.h"
#include "debug.h"
#include "utility.h"
#include "slc_flash_test.h"
#include "slc_hal_rng.h"
#include "slc_hal_delay.h"
#include "gt25q20d.h"
#include "p25q21l.h"
#include "win04_mem_cfg.h"

/*
 * GT25Q40 为 512KB，WIN04 仅使用前 256KB (0x08000000~0x0803FFFF)。
 * SR1=0x44 保护 512KB 整片最后 4KB (0x0807F000)，超出可用范围，不适用。
 * SR1=0x64 (SEC+TB+BP0, CMP=0) 保护最低 4KB (0x08000000~0x08000FFF)，Table1。
 */
#define FLASH_PROTECT_SR1_CFG       (EN_FLASH_STA1_BP0 | EN_FLASH_STA1_SEC | EN_FLASH_STA1_TB)
#define FLASH_USABLE_MAX_ADDR       (FLASH_BASE_ADDR + FLASH_SIZE_MAX)
#define FLASH_PROTECTED_TEST_ADDR   (FLASH_BASE_ADDR + 0x800U)
#define FLASH_UNPROTECTED_TEST_ADDR (FLASH_USERER_DATA_ADDR)

static __RAM_FUNC int flash_protect_wait_ready(void)
{
    uint8_t sta = 0;
    uint32_t retry = 100000U;

    while (retry--) {
        rom_hw_flash_read_status_reg(EN_FLASH_READ_STA_REG1, &sta);
        if (!(sta & EN_FLASH_STA1_BUSY)) {
            return 0;
        }
    }

    return -1;
}

static __RAM_FUNC int flash_protect_restore_sr(uint8_t sr1, uint8_t sr2)
{
    uint8_t sta = 0;

    if (rom_hw_flash_write_status_reg(EN_FLASH_READ_STA_REG2, sr2) != EN_ERROR_STA_OK) {
        return -1;
    }
    if (flash_protect_wait_ready() != 0) {
        return -1;
    }

    if (rom_hw_flash_write_status_reg(EN_FLASH_READ_STA_REG1, sr1) != EN_ERROR_STA_OK) {
        return -1;
    }
    if (flash_protect_wait_ready() != 0) {
        return -1;
    }

    rom_hw_flash_read_status_reg(EN_FLASH_READ_STA_REG1, &sta);
    PRINTF("Flash status register 1 restored: 0x%X\n", sta & (uint8_t) ~(EN_FLASH_STA1_WEL | EN_FLASH_STA1_BUSY));

    return 0;
}

static __RAM_FUNC int flash_protect_clear_bp(uint8_t sr2_keep)
{
    uint8_t sr2 = sr2_keep & (uint8_t) ~EN_FLASH_STA2_CMP;

    return flash_protect_restore_sr(0, sr2);
}

static __RAM_FUNC int flash_protect_prog_probe(uint32_t addr, int expect_program_ok, int do_erase)
{
    uint8_t pattern[4] = {0x5A, 0xA5, 0x55, 0xAA};
    uint8_t before[4] = {0};
    uint8_t after[4] = {0};
    uint32_t sector = addr & ~(UNIT_SECTOR - 1U);
    int ret;

    PRINTF("flash protect test: probe 0x%08X, expect %s%s\n",
           addr, expect_program_ok ? "program ok" : "protected",
           do_erase ? " (erase+program)" : " (program only)");

    slc_hal_sysctrl_cache_mode_set(HAL_CACHE_FLUSH);
    rom_hw_flash_read_data(addr, before, sizeof(before));

    if (do_erase) {
        rom_hw_flash_erase_by_length(sector, UNIT_SECTOR);
        if (flash_protect_wait_ready() != 0) {
            PRINTF("flash protect test: erase timeout, addr=0x%08X\n", addr);
            return -1;
        }

        slc_hal_sysctrl_cache_mode_set(HAL_CACHE_FLUSH);
        rom_hw_flash_read_data(addr, after, sizeof(after));
        if (!expect_program_ok && memcmp(after, before, sizeof(before)) != 0) {
            PRINTF("flash protect test: addr 0x%08X expect protected, but erase changed data\n", addr);
            dump_u8buf("before", before, sizeof(before));
            dump_u8buf("after erase", after, sizeof(after));
            return -1;
        }
    }

    rom_hw_flash_write_data(addr, pattern, sizeof(pattern));
    if (flash_protect_wait_ready() != 0) {
        PRINTF("flash protect test: program timeout, addr=0x%08X\n", addr);
        return -1;
    }

    slc_hal_sysctrl_cache_mode_set(HAL_CACHE_FLUSH);
    rom_hw_flash_read_data(addr, after, sizeof(after));

    if (expect_program_ok) {
        if (memcmp(after, pattern, sizeof(pattern)) != 0) {
            PRINTF("flash protect test: addr 0x%08X expect program ok, rd mismatch\n", addr);
            dump_u8buf("expect", pattern, sizeof(pattern));
            dump_u8buf("read", after, sizeof(after));
            ret = -1;
        } else {
            PRINTF("flash protect test: addr 0x%08X program ok\n", addr);
            ret = 0;
        }
    } else if (memcmp(after, pattern, sizeof(pattern)) == 0) {
        PRINTF("flash protect test: addr 0x%08X expect protected, but programmed\n", addr);
        ret = -1;
    } else if (memcmp(after, before, sizeof(before)) != 0) {
        PRINTF("flash protect test: addr 0x%08X expect protected, but content changed\n", addr);
        dump_u8buf("before", before, sizeof(before));
        dump_u8buf("after", after, sizeof(after));
        ret = -1;
    } else {
        PRINTF("flash protect test: addr 0x%08X protected (%s blocked)\n", addr,
               do_erase ? "erase/program" : "program");
        ret = 0;
    }

    return ret;
}

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

/* SEC_MEM0 = fw_security_info_t: cap[32] + sta[32] + key[192] */
/* TODO(PY): 写状态寄存器后续改为 rom_hw_flash_write_status_reg1(u16Sta)，
 * 一次性写入 SR1+SR2（低字节=SR1，高字节=SR2）；GT 仍用 rom_hw_flash_write_status_reg。
 * 涉及 flash_fuse_lock_sec_mem0_lb、flash_protect_restore_sr、flash protect test 等。
 * 本次先不改。 */
#define FLASH_OTP_FUSE_STATUS_OFFSET        (33U)
#define FLASH_OTP_FUSE_STATUS_FUSED        (1U)
#define FLASH_SEC_MEM0_SNAPSHOT_SIZE       (256U)
#define FLASH_SEC_MEM0_CAP_FLAG_END        (2U)    /* cap.fw_enc/otp_fuse_capable @0~1 */
#define FLASH_SEC_MEM0_STA_OFF             (32U)
#define FLASH_SEC_MEM0_STA_FLAG_END        (38U)   /* sta fields @32~37 */
#define FLASH_SEC_MEM0_KEY_OFF             (64U)
#define FLASH_SEC_MEM0_KEY_PUBKEY_END      (128U)  /* key.pub_key @64~127 */

static const uint16_t flash_otp_tamper_probe_offsets[] = {
    0,   /* cap */
    32,  /* sta.fw_enc_status */
    34,  /* sta.hash_type (skip fuse byte @33) */
    64,  /* key start */
    128, /* key mid */
};

static int flash_fuse_read_sec_mem0(uint8_t *buf, uint16_t len)
{
    EN_ERR_STA_T ret;

    ret = rom_hw_flash_read_security_mem(EN_FLASH_SEC_MEM0, 0, buf, len);
    if (ret != EN_ERROR_STA_OK) {
        PRINTF("read SEC_MEM0 failed, ret=0x%X\n", ret);
        return -1;
    }

    return 0;
}

static int flash_fuse_is_py_flash(void)
{
    return rom_hw_flash_get_type() == EN_FLASH_TYPE_PY;
}

static uint8_t flash_fuse_sec_mem0_lb_mask(void)
{
    return flash_fuse_is_py_flash() ? PY_FLASH_STA_LB1 : (uint8_t)EN_FLASH_STA2_LB;
}

static int flash_fuse_sec_mem0_lb_locked(uint8_t sr_high)
{
    return (sr_high & flash_fuse_sec_mem0_lb_mask()) != 0U;
}

/* Scheme B: blow Flash LB to permanently lock SEC_MEM0 (Security Register #1). */
static int flash_fuse_lock_sec_mem0_lb(void)
{
    uint8_t sr_high = 0;
    uint8_t sr_high_after = 0;
    uint8_t lb_mask = flash_fuse_sec_mem0_lb_mask();
    EN_ERR_STA_T ret;

    ret = rom_hw_flash_read_status_reg(EN_FLASH_READ_STA_REG2, &sr_high);
    if (ret != EN_ERROR_STA_OK) {
        PRINTF("read status high byte failed, ret=0x%X\n", ret);
        return -1;
    }

    if (flash_fuse_sec_mem0_lb_locked(sr_high)) {
        PRINTF("flash fuse: SEC_MEM0 already locked, SR-high=0x%02X\n", sr_high);
        return 0;
    }

    sr_high |= lb_mask;
    PRINTF("flash fuse: lock SEC_MEM0 via LB, target SR-high=0x%02X (irreversible)\n", sr_high);

    ret = rom_hw_flash_write_status_reg(EN_FLASH_READ_STA_REG2, sr_high);
    if (ret != EN_ERROR_STA_OK) {
        PRINTF("write status LB failed, ret=0x%X\n", ret);
        return -1;
    }

    ret = rom_hw_flash_read_status_reg(EN_FLASH_READ_STA_REG2, &sr_high_after);
    if (ret != EN_ERROR_STA_OK) {
        PRINTF("readback status high byte failed, ret=0x%X\n", ret);
        return -1;
    }

    if (!flash_fuse_sec_mem0_lb_locked(sr_high_after)) {
        PRINTF("flash fuse fail: SEC_MEM0 LB not set, SR-high=0x%02X\n", sr_high_after);
        return -1;
    }

    PRINTF("flash fuse: SEC_MEM0 LB locked, SR-high=0x%02X\n", sr_high_after);
    return 0;
}

/* 1) 尝试写入 OTP 区，预期内容不被篡改 */
static int flash_fuse_verify_otp_write_protected(const uint8_t *snap)
{
    uint8_t tamper = 0;
    uint8_t readback[FLASH_SEC_MEM0_SNAPSHOT_SIZE];
    EN_ERR_STA_T ret;
    uint32_t i;

    PRINTF("fuse lock test 1: tamper OTP region by program write\n");

    for (i = 0; i < (sizeof(flash_otp_tamper_probe_offsets) / sizeof(flash_otp_tamper_probe_offsets[0])); i++) {
        uint16_t offset = flash_otp_tamper_probe_offsets[i];

        tamper = snap[offset] ^ 0xA5U;
        if (tamper == snap[offset]) {
            tamper = 0x00U;
        }

        ret = rom_hw_flash_write_security_mem(EN_FLASH_SEC_MEM0, offset, &tamper, 1);
        PRINTF("  write probe offset %u with 0x%02X, ret=0x%X\n", offset, tamper, ret);
    }

    if (flash_fuse_read_sec_mem0(readback, sizeof(readback)) != 0) {
        return -1;
    }

    if (memcmp(snap, readback, sizeof(readback)) != 0) {
        PRINTF("fuse lock test 1 fail: OTP region changed after tamper write\n");
        dump_u8buf("snap", (uint8_t *)snap, sizeof(readback));
        dump_u8buf("readback", readback, sizeof(readback));
        return -1;
    }

    PRINTF("fuse lock test 1 pass: OTP region not tampered\n");
    return 0;
}

/* 2) 尝试擦除 OTP 区，预期擦不掉 */
/* 仅 resv 区可写测试图案；cap/sta/key 设计标志位见 bootloader.h fw_security_info_t */
static int flash_fuse_sec_mem0_is_test_byte(uint16_t offset)
{
    if ((offset >= FLASH_SEC_MEM0_CAP_FLAG_END) && (offset < FLASH_SEC_MEM0_STA_OFF)) {
        return 1;
    }

    if ((offset >= FLASH_SEC_MEM0_STA_FLAG_END) && (offset < FLASH_SEC_MEM0_KEY_OFF)) {
        return 1;
    }

    if (offset >= FLASH_SEC_MEM0_KEY_PUBKEY_END) {
        return 1;
    }

    return 0;
}

static uint8_t flash_fuse_pattern_byte(uint16_t offset)
{
    uint8_t val = 0x5AU ^ (uint8_t)(offset + 1U);

    if (val == 0xFFU) {
        val = 0x5AU;
    }

    return val;
}

static void flash_fuse_dump_sr_high(uint8_t sr_high)
{
    if (flash_fuse_is_py_flash()) {
        PRINTF("  PY SR-high(35H)=0x%02X: QE=%u LB1=%u LB2=%u LB3=%u (SEC_MEM0 need LB1=1)\n",
               sr_high,
               (sr_high & PY_FLASH_STA_QE) ? 1U : 0U,
               (sr_high & PY_FLASH_STA_LB1) ? 1U : 0U,
               (sr_high & PY_FLASH_STA_LB2) ? 1U : 0U,
               (sr_high & PY_FLASH_STA_LB3) ? 1U : 0U);
    } else {
        PRINTF("  GT SR2=0x%02X: SRP1=%u QE=%u LB=%u CMP=%u SUS=%u (SEC_MEM0 need LB=1)\n",
               sr_high,
               (sr_high & EN_FLASH_STA2_SRP1) ? 1U : 0U,
               (sr_high & EN_FLASH_STA2_QE) ? 1U : 0U,
               (sr_high & EN_FLASH_STA2_LB) ? 1U : 0U,
               (sr_high & EN_FLASH_STA2_CMP) ? 1U : 0U,
               (sr_high & EN_FLASH_STA2_SUS) ? 1U : 0U);
    }
}

/**
 * @brief 熔丝前准备：不擦除 SEC_MEM0，仅向 resv 区写测试图案，保留现有标志/控制位。
 *        用法：本用例 -> 手动熔丝 -> Flash fuse lock probe Test
 */
int slc_flash_fuse_sec_mem0_write_test(void)
{
    uint8_t sr_high = 0;
    uint8_t baseline[FLASH_SEC_MEM0_SNAPSHOT_SIZE];
    uint8_t expected[FLASH_SEC_MEM0_SNAPSHOT_SIZE];
    uint8_t readback[FLASH_SEC_MEM0_SNAPSHOT_SIZE];
    EN_ERR_STA_T ret;
    uint16_t offset;
    int pattern_count = 0;

    PRINTF("flash fuse sec mem0 write test: write resv only, no erase, keep flag bytes\n");
    PRINTF("flash type: %s\n", flash_fuse_is_py_flash() ? "PY" : "GT");

    ret = rom_hw_flash_read_status_reg(EN_FLASH_READ_STA_REG2, &sr_high);
    if (ret != EN_ERROR_STA_OK) {
        PRINTF("read SR-high failed, ret=0x%X\n", ret);
        return -1;
    }

    flash_fuse_dump_sr_high(sr_high);

    if (flash_fuse_sec_mem0_lb_locked(sr_high)) {
        PRINTF("flash fuse sec mem0 write test fail: SEC_MEM0 already locked\n");
        return -1;
    }

    if (flash_fuse_read_sec_mem0(baseline, sizeof(baseline)) != 0) {
        return -1;
    }

    memcpy(expected, baseline, sizeof(expected));

    for (offset = 0; offset < FLASH_SEC_MEM0_SNAPSHOT_SIZE; offset++) {
        uint8_t pattern;

        if (!flash_fuse_sec_mem0_is_test_byte(offset)) {
            continue;
        }

        pattern = flash_fuse_pattern_byte(offset);
        ret = rom_hw_flash_write_security_mem(EN_FLASH_SEC_MEM0, offset, &pattern, 1);
        if (ret != EN_ERROR_STA_OK) {
            PRINTF("write offset %u with 0x%02X failed, ret=0x%X\n", offset, pattern, ret);
            return -1;
        }

        expected[offset] &= pattern;
        pattern_count++;
    }

    if (pattern_count == 0) {
        PRINTF("flash fuse sec mem0 write test fail: no test pattern region\n");
        return -1;
    }

    if (flash_fuse_read_sec_mem0(readback, sizeof(readback)) != 0) {
        return -1;
    }

    for (offset = 0; offset < FLASH_SEC_MEM0_SNAPSHOT_SIZE; offset++) {
        if (readback[offset] != expected[offset]) {
            PRINTF("flash fuse sec mem0 write test fail: offset %u expect 0x%02X got 0x%02X\n",
                   offset, expected[offset], readback[offset]);
            return -1;
        }
    }

    PRINTF("flash fuse sec mem0 write test pass: %d resv bytes written\n", pattern_count);
    PRINTF("write: cap.resv@2~31, sta.resv@38~63, key.resv@128~255\n");
    PRINTF("keep: cap@0~1, sta@32~37, key.pub_key@64~127 (unchanged)\n");
    PRINTF("next: manual fuse, then run Flash fuse lock probe Test\n");
    return 0;
}

static int flash_fuse_verify_otp_erase_protected(const uint8_t *snap)
{
    uint8_t readback[FLASH_SEC_MEM0_SNAPSHOT_SIZE];
    EN_ERR_STA_T ret;

    PRINTF("fuse lock test 2: erase OTP region\n");

    ret = rom_hw_flash_erase_security_mem(EN_FLASH_SEC_MEM0, FLASH_ERASE_SECURITY_MEM_KEY);
    PRINTF("  erase SEC_MEM0 ret=0x%X\n", ret);

    if (flash_fuse_read_sec_mem0(readback, sizeof(readback)) != 0) {
        return -1;
    }

    if (memcmp(snap, readback, sizeof(readback)) != 0) {
        PRINTF("fuse lock test 2 fail: OTP region changed after erase attempt\n");
        dump_u8buf("snap", (uint8_t *)snap, sizeof(readback));
        dump_u8buf("readback", readback, sizeof(readback));
        return -1;
    }

    PRINTF("fuse lock test 2 pass: OTP region not erased\n");
    return 0;
}

/* 3) 尝试改写指定 OTP 字节，预期读回与快照一致 */
static int flash_fuse_verify_otp_byte_locked(const uint8_t *snap, uint16_t offset)
{
    static const uint8_t try_vals[] = {0x00U, 0xFFU, 0xFEU, 0x01U, 0x5AU};
    uint8_t readback = 0;
    EN_ERR_STA_T ret;
    uint32_t i;

    PRINTF("fuse lock test 3: tamper OTP byte@%u (snap=0x%02X)\n", offset, snap[offset]);

    for (i = 0; i < (sizeof(try_vals) / sizeof(try_vals[0])); i++) {
        ret = rom_hw_flash_write_security_mem(EN_FLASH_SEC_MEM0,
                                              offset,
                                              (uint8_t *)&try_vals[i], 1);
        if (ret != EN_ERROR_STA_OK) {
            PRINTF("  write byte@%u with 0x%02X rejected, ret=0x%X\n",
                   offset, try_vals[i], ret);
        }

        ret = rom_hw_flash_read_security_mem(EN_FLASH_SEC_MEM0,
                                             offset,
                                             &readback, 1);
        if (ret != EN_ERROR_STA_OK) {
            PRINTF("  readback byte@%u failed, ret=0x%X\n", offset, ret);
            return -1;
        }

        PRINTF("  write byte@%u with 0x%02X, readback 0x%02X\n",
               offset, try_vals[i], readback);
        if (readback != snap[offset]) {
            PRINTF("fuse lock test 3 fail: byte@%u changed 0x%02X->0x%02X\n",
                   offset, snap[offset], readback);
            return -1;
        }
    }

    PRINTF("fuse lock test 3 pass: OTP byte@%u unchanged\n", offset);
    return 0;
}

static void flash_fuse_print_status_info(void)
{
    uint8_t fuse_sta = 0;
    uint8_t sr_high = 0;

    if (rom_hw_flash_read_security_mem(EN_FLASH_SEC_MEM0,
                                       FLASH_OTP_FUSE_STATUS_OFFSET,
                                       &fuse_sta, 1) != EN_ERROR_STA_OK) {
        PRINTF("info: read otp_fuse_status failed\n");
        return;
    }

    if (rom_hw_flash_read_status_reg(EN_FLASH_READ_STA_REG2, &sr_high) != EN_ERROR_STA_OK) {
        PRINTF("info: read status high byte failed\n");
        return;
    }

    PRINTF("info: otp_fuse_status=0x%02X, SR-high=0x%02X (reference only)\n", fuse_sta, sr_high);
    flash_fuse_dump_sr_high(sr_high);
}

static int flash_fuse_run_lock_probe(const uint8_t *snap, int probe_fuse_byte)
{
    if (flash_fuse_verify_otp_write_protected(snap) != 0) {
        return -1;
    }

    if (flash_fuse_verify_otp_erase_protected(snap) != 0) {
        return -1;
    }

    if (probe_fuse_byte) {
        if (flash_fuse_verify_otp_byte_locked(snap, FLASH_OTP_FUSE_STATUS_OFFSET) != 0) {
            return -1;
        }
    }

    return 0;
}

static int flash_fuse_check_precondition(void)
{
    uint8_t fuse_sta = 0;
    uint8_t sr_high = 0;
    uint8_t lb_mask = flash_fuse_sec_mem0_lb_mask();
    EN_ERR_STA_T ret;

    ret = rom_hw_flash_read_security_mem(EN_FLASH_SEC_MEM0,
                                         FLASH_OTP_FUSE_STATUS_OFFSET,
                                         &fuse_sta, 1);
    if (ret != EN_ERROR_STA_OK) {
        PRINTF("read otp_fuse_status failed, ret=0x%X\n", ret);
        return -1;
    }

    ret = rom_hw_flash_read_status_reg(EN_FLASH_READ_STA_REG2, &sr_high);
    if (ret != EN_ERROR_STA_OK) {
        PRINTF("read status high byte failed, ret=0x%X\n", ret);
        return -1;
    }

    PRINTF("otp_fuse_status=0x%02X (expect 0x01), SR-high=0x%02X\n", fuse_sta, sr_high);
    flash_fuse_dump_sr_high(sr_high);

    if (fuse_sta != FLASH_OTP_FUSE_STATUS_FUSED) {
        PRINTF("flash fuse verify fail: otp not fused");
        if (fuse_sta == 0xFFU) {
            PRINTF(" (0xFF=default/erased, SEC_MEM0 offset %u not blown)",
                   FLASH_OTP_FUSE_STATUS_OFFSET);
        }
        if (flash_fuse_sec_mem0_lb_locked(sr_high)) {
            PRINTF(" [LB already set: program fuse byte before lock next time]");
        }
        PRINTF("\n");
        return -1;
    }

    if (!flash_fuse_sec_mem0_lb_locked(sr_high)) {
        PRINTF("flash fuse verify fail: SEC_MEM0 LB not locked");
        PRINTF(" (need SR-high|0x%02X, e.g. 0x%02X if other bits unchanged)\n",
               lb_mask, (uint8_t)(sr_high | lb_mask));
        return -1;
    }

    return 0;
}

/**
 * @brief 外部已熔丝/已锁 OTP 后的只读验证用例（不吹熔丝、不写 SR2 LB）。
 *
 * 设计逻辑
 * 1、确认 otp_fuse_status 已熔、SEC_MEM0 LB 已锁（PY: LB1, GT: SR2.LB）
 * 2、快照 SEC_MEM0，尝试篡改 OTP 区多个偏移
 * 3、尝试擦除 SEC_MEM0
 * 4、尝试写 0/FF/FE 清熔丝位
 *
 * check逻辑
 * 1、篡改写后 SEC_MEM0 与快照一致
 * 2、擦除后 SEC_MEM0 与快照一致
 * 3、清熔丝写后 otp_fuse_status 仍为 1
 */
int slc_flash_fuse_lock_verify_test(void)
{
    uint8_t sec_mem0_snap[FLASH_SEC_MEM0_SNAPSHOT_SIZE];

    PRINTF("flash fuse lock verify test: external fused OTP only\n");

    if (flash_fuse_check_precondition() != 0) {
        return -1;
    }

    if (flash_fuse_read_sec_mem0(sec_mem0_snap, sizeof(sec_mem0_snap)) != 0) {
        return -1;
    }

    if (flash_fuse_run_lock_probe(sec_mem0_snap, 1) != 0) {
        return -1;
    }

    PRINTF("flash fuse lock verify test pass\n");
    return 0;
}

/**
 * @brief 不检查熔丝/LB 前置，直接探测 SEC_MEM0 是否已被硬件锁住（PY/GT 通用）。
 */
int slc_flash_fuse_lock_probe_test(void)
{
    uint8_t sec_mem0_snap[FLASH_SEC_MEM0_SNAPSHOT_SIZE];

    PRINTF("flash fuse lock probe test: tamper/erase without precondition\n");
    PRINTF("flash type: %s\n", flash_fuse_is_py_flash() ? "PY" : "GT");

    flash_fuse_print_status_info();

    if (flash_fuse_read_sec_mem0(sec_mem0_snap, sizeof(sec_mem0_snap)) != 0) {
        return -1;
    }

    if (flash_fuse_run_lock_probe(sec_mem0_snap, 1) != 0) {
        PRINTF("flash fuse lock probe test fail: SEC_MEM0 not locked\n");
        return -1;
    }

    PRINTF("flash fuse lock probe test pass: SEC_MEM0 write/erase protected\n");
    return 0;
}

/**
 * @brief GT Flash 专用：仅写/擦探测，不检查 offset33 软件熔丝标志。
 *        
 */
int slc_flash_fuse_lock_probe_gt_test(void)
{
    uint8_t sec_mem0_snap[FLASH_SEC_MEM0_SNAPSHOT_SIZE];

    if (flash_fuse_is_py_flash()) {
        PRINTF("flash fuse lock probe gt test fail: GT flash required\n");
        return -1;
    }

    PRINTF("flash fuse lock probe gt test: tamper/erase only, skip offset33\n");

    flash_fuse_print_status_info();

    if (flash_fuse_read_sec_mem0(sec_mem0_snap, sizeof(sec_mem0_snap)) != 0) {
        return -1;
    }

    if (flash_fuse_run_lock_probe(sec_mem0_snap, 0) != 0) {
        PRINTF("flash fuse lock probe gt test fail: SEC_MEM0 not locked\n");
        return -1;
    }

    PRINTF("flash fuse lock probe gt test pass: SEC_MEM0 write/erase protected\n");
    return 0;
}

/**
 * @brief GT Flash：测试前先置 SR2.LB 熔丝，再做写/擦探测（跳过 offset33）。
 *
 * 设计逻辑
 * 1、确认当前为 GT Flash
 * 2、调用 flash_fuse_lock_sec_mem0_lb 置位 SR2.LB（已锁则跳过）
 * 3、快照 SEC_MEM0，执行写/擦保护探测（不测 offset33）
 *
 * check逻辑
 * 1、LB 置位成功（或原本已锁）
 * 2、篡改写后 SEC_MEM0 与快照一致
 * 3、擦除后 SEC_MEM0 与快照一致
 */
int slc_flash_fuse_lock_probe_gt_blow_test(void)
{
    uint8_t sec_mem0_snap[FLASH_SEC_MEM0_SNAPSHOT_SIZE];

    if (flash_fuse_is_py_flash()) {
        PRINTF("flash fuse lock probe gt blow test fail: GT flash required\n");
        return -1;
    }

    PRINTF("flash fuse lock probe gt blow test: blow LB then probe (skip offset33)\n");
    PRINTF("warning: SR2.LB lock is irreversible\n");

    flash_fuse_print_status_info();

    if (flash_fuse_lock_sec_mem0_lb() != 0) {
        PRINTF("flash fuse lock probe gt blow test fail: LB lock failed\n");
        return -1;
    }

    flash_fuse_print_status_info();

    if (flash_fuse_read_sec_mem0(sec_mem0_snap, sizeof(sec_mem0_snap)) != 0) {
        return -1;
    }

    if (flash_fuse_run_lock_probe(sec_mem0_snap, 0) != 0) {
        PRINTF("flash fuse lock probe gt blow test fail: SEC_MEM0 not locked\n");
        return -1;
    }

    PRINTF("flash fuse lock probe gt blow test pass: LB blown and OTP protected\n");
    return 0;
}

int slc_flash_fuse_test(void)
{
    uint8_t fuse_sta = 0;
    uint8_t fuse_val = FLASH_OTP_FUSE_STATUS_FUSED;
    uint8_t byte_prev = 0;
    uint8_t byte_next = 0;
    uint8_t byte_prev_after = 0;
    uint8_t byte_next_after = 0;
    uint8_t sec_mem0_snap[FLASH_SEC_MEM0_SNAPSHOT_SIZE];
    EN_ERR_STA_T ret;
    int already_fused = 0;

    PRINTF("flash fuse test: blow SEC_MEM0 otp_fuse_status (irreversible)\n");

    ret = rom_hw_flash_read_security_mem(EN_FLASH_SEC_MEM0,
                                         FLASH_OTP_FUSE_STATUS_OFFSET - 1U,
                                         &byte_prev, 1);
    if (ret != EN_ERROR_STA_OK) {
        PRINTF("read byte before fuse offset failed, ret=0x%X\n", ret);
        return -1;
    }

    ret = rom_hw_flash_read_security_mem(EN_FLASH_SEC_MEM0,
                                         FLASH_OTP_FUSE_STATUS_OFFSET,
                                         &fuse_sta, 1);
    if (ret != EN_ERROR_STA_OK) {
        PRINTF("read otp_fuse_status failed, ret=0x%X\n", ret);
        return -1;
    }

    ret = rom_hw_flash_read_security_mem(EN_FLASH_SEC_MEM0,
                                         FLASH_OTP_FUSE_STATUS_OFFSET + 1U,
                                         &byte_next, 1);
    if (ret != EN_ERROR_STA_OK) {
        PRINTF("read byte after fuse offset failed, ret=0x%X\n", ret);
        return -1;
    }

    PRINTF("otp_fuse_status before: 0x%02X, neighbor: 0x%02X / 0x%02X\n",
           fuse_sta, byte_prev, byte_next);

    if (fuse_sta == FLASH_OTP_FUSE_STATUS_FUSED) {
        PRINTF("flash fuse test: already fused, verify OTP lock only\n");
        already_fused = 1;
    } else {
        ret = rom_hw_flash_write_security_mem(EN_FLASH_SEC_MEM0,
                                              FLASH_OTP_FUSE_STATUS_OFFSET,
                                              &fuse_val, 1);
        if (ret != EN_ERROR_STA_OK) {
            PRINTF("write otp_fuse_status failed, ret=0x%X\n", ret);
            return -1;
        }

        fuse_sta = 0;
        ret = rom_hw_flash_read_security_mem(EN_FLASH_SEC_MEM0,
                                             FLASH_OTP_FUSE_STATUS_OFFSET,
                                             &fuse_sta, 1);
        if (ret != EN_ERROR_STA_OK) {
            PRINTF("readback otp_fuse_status failed, ret=0x%X\n", ret);
            return -1;
        }

        PRINTF("otp_fuse_status after blow: 0x%02X\n", fuse_sta);

        if (fuse_sta != FLASH_OTP_FUSE_STATUS_FUSED) {
            PRINTF("flash fuse test fail: blow result mismatch\n");
            return -1;
        }

        ret = rom_hw_flash_read_security_mem(EN_FLASH_SEC_MEM0,
                                             FLASH_OTP_FUSE_STATUS_OFFSET - 1U,
                                             &byte_prev_after, 1);
        if (ret != EN_ERROR_STA_OK) {
            PRINTF("read neighbor before fuse offset failed, ret=0x%X\n", ret);
            return -1;
        }

        ret = rom_hw_flash_read_security_mem(EN_FLASH_SEC_MEM0,
                                             FLASH_OTP_FUSE_STATUS_OFFSET + 1U,
                                             &byte_next_after, 1);
        if (ret != EN_ERROR_STA_OK) {
            PRINTF("read neighbor after fuse offset failed, ret=0x%X\n", ret);
            return -1;
        }

        if ((byte_prev_after != byte_prev) || (byte_next_after != byte_next)) {
            PRINTF("flash fuse test fail: neighbor changed 0x%02X->0x%02X, 0x%02X->0x%02X\n",
                   byte_prev, byte_prev_after, byte_next, byte_next_after);
            return -1;
        }

        PRINTF("neighbor verify pass: only fuse byte changed\n");
    }

    if (flash_fuse_lock_sec_mem0_lb() != 0) {
        return -1;
    }

    if (flash_fuse_read_sec_mem0(sec_mem0_snap, sizeof(sec_mem0_snap)) != 0) {
        return -1;
    }

    if (sec_mem0_snap[FLASH_OTP_FUSE_STATUS_OFFSET] != FLASH_OTP_FUSE_STATUS_FUSED) {
        PRINTF("flash fuse test fail: fuse not set before lock verify\n");
        return -1;
    }

    if (flash_fuse_run_lock_probe(sec_mem0_snap, 1) != 0) {
        return -1;
    }

    if (already_fused) {
        PRINTF("flash fuse test pass: already fused and OTP locked\n");
    } else {
        PRINTF("flash fuse test pass: blow success and OTP locked\n");
    }

    return 0;
}

/**
 * @brief GT25Q40 写保护测试 (Status Register BP, CMP=0, 仅使用前 256KB):
 *        先清 BP，再擦用户数据扇区；设 SR1=0x64 保护最低 4KB，用户数据区应可擦写。
 *        不做整片擦除（运行中擦全会破坏 Flash 内代码/字符串导致跑飞）。
 * @return 0: 通过; -1: 失败
 */
__RAM_FUNC int slc_flash_protect_test(void)
{
    uint8_t sta = 0;
    uint8_t sta_orig1 = 0;
    uint8_t sta_orig2 = 0;
    uint8_t sr2_cfg = 0;
    int ret = 0;

    PRINTF("flash usable range: 0x%08X ~ 0x%08X\n", FLASH_BASE_ADDR, FLASH_USABLE_MAX_ADDR - 1U);

    rom_hw_flash_read_status_reg(EN_FLASH_READ_STA_REG1, &sta_orig1);
    PRINTF("Flash status register 1: 0x%X\n", sta_orig1 & (uint8_t) ~(EN_FLASH_STA1_WEL | EN_FLASH_STA1_BUSY));
    rom_hw_flash_read_status_reg(EN_FLASH_READ_STA_REG2, &sta_orig2);
    PRINTF("Flash status register 2: 0x%X\n", sta_orig2);
    rom_hw_flash_read_status_reg(EN_FLASH_READ_STA_REG3, &sta);
    PRINTF("Flash status register 3: 0x%X\n", sta);

    sr2_cfg = sta_orig2 & (uint8_t) ~EN_FLASH_STA2_CMP;

    PRINTF("flash protect test: clear BP and erase user data sector\n");
    if (flash_protect_clear_bp(sr2_cfg) != 0) {
        PRINTF("flash protect test: clear BP failed\n");
        return -1;
    }
    rom_hw_flash_erase_by_length(FLASH_UNPROTECTED_TEST_ADDR, UNIT_SECTOR);
    if (flash_protect_wait_ready() != 0) {
        PRINTF("flash protect test: user data sector erase timeout\n");
        return -1;
    }
    slc_hal_sysctrl_cache_mode_set(HAL_CACHE_FLUSH);
    PRINTF("flash protect test: prep done (BP cleared, user sector erased)\n");

    if (rom_hw_flash_write_status_reg(EN_FLASH_READ_STA_REG2, sr2_cfg) != EN_ERROR_STA_OK) {
        PRINTF("flash protect test: write SR2 failed\n");
        return -1;
    }
    if (flash_protect_wait_ready() != 0) {
        return -1;
    }

    if (rom_hw_flash_write_status_reg(EN_FLASH_READ_STA_REG1, FLASH_PROTECT_SR1_CFG) != EN_ERROR_STA_OK) {
        PRINTF("flash protect test: write SR1 failed\n");
        flash_protect_clear_bp(sta_orig2);
        return -1;
    }
    if (flash_protect_wait_ready() != 0) {
        flash_protect_clear_bp(sta_orig2);
        return -1;
    }

    rom_hw_flash_read_status_reg(EN_FLASH_READ_STA_REG1, &sta);
    PRINTF("Flash status register 1: 0x%X (expect 0x%X)\n",
           sta & (uint8_t) ~(EN_FLASH_STA1_WEL | EN_FLASH_STA1_BUSY), FLASH_PROTECT_SR1_CFG);
    rom_hw_flash_read_status_reg(EN_FLASH_READ_STA_REG2, &sta);
    PRINTF("Flash status register 2: 0x%X\n", sta);

    PRINTF("protected probe addr 0x%08X (low 4KB, erase+program expect blocked)\n",
           FLASH_PROTECTED_TEST_ADDR);
    if (flash_protect_prog_probe(FLASH_PROTECTED_TEST_ADDR, 0, 1) != 0) {
        ret = -1;
    }

    PRINTF("unprotected probe addr 0x%08X (user data)\n", FLASH_UNPROTECTED_TEST_ADDR);
    if (flash_protect_prog_probe(FLASH_UNPROTECTED_TEST_ADDR, 1, 1) != 0) {
        ret = -1;
    }

    if (flash_protect_clear_bp(sta_orig2) != 0) {
        ret = -1;
    } else {
        PRINTF("Flash BP cleared, SR1=0x0 (ready for reprogram)\n");
    }

    if (ret == 0) {
        PRINTF("flash protect test passed.\n");
    }

    return ret;
}

/**
 * @brief Flash 耐久性测试：对同一地址反复执行擦除→写入随机数→回读比对，
 *        循环10000次，每1000次打印进度，验证flash存储单元无损坏。
 * @return 0: 全部通过; -1: 某次擦写读比对失败
 */
__RAM_FUNC int slc_flash_endurance_test(void)
{
    const uint32_t ENDURANCE_CYCLES = 10000;
    const uint32_t ENDURANCE_ERASE_LEN = UNIT_SECTOR;
    const uint32_t ENDURANCE_WRITE_LEN = UNIT_PAGE;
    const uint32_t ENDURANCE_ADDR = FLASH_USERER_DATA_ADDR;
    const uint32_t ENDURANCE_REPORT_STEP = 1000;

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

#define FIXED_KEY  0x52109735u
#define ADDR_CONST 0x1a397u

__RAM_FUNC static uint32_t byteswap(uint32_t data)
{
    uint8_t b0 = (uint8_t)(data);
    uint8_t b1 = (uint8_t)(data >> 8);
    uint8_t b2 = (uint8_t)(data >> 16);
    uint8_t b3 = (uint8_t)(data >> 24);

    return ((uint32_t)b1 << 24) |
           ((uint32_t)b3 << 16) |
           ((uint32_t)b0 << 8)  |
           ((uint32_t)b2);
}

__RAM_FUNC static uint32_t inv_byteswap(uint32_t data)
{
    uint8_t b0 = (uint8_t)(data);
    uint8_t b1 = (uint8_t)(data >> 8);
    uint8_t b2 = (uint8_t)(data >> 16);
    uint8_t b3 = (uint8_t)(data >> 24);

    return ((uint32_t)b2 << 24) |
           ((uint32_t)b0 << 16) |
           ((uint32_t)b3 << 8)  |
           ((uint32_t)b1);
}

__RAM_FUNC static inline uint32_t bit_of(uint32_t A, int n)
{
    return (A >> n) & 1u;
}

__RAM_FUNC static uint32_t scramble_addr(uint32_t word_addr)
{
    /* 18 位 word_addr 与常数相加，利用进位特性 */
    uint32_t A = word_addr + ADDR_CONST;

    /* 按位映射表逐位构造 32 位结果 */
    uint32_t result = 0;

    /* 地址[31:24] */
    result |= (bit_of(A,  1) << 31);
    result |= (bit_of(A, 16) << 30);
    result |= (bit_of(A,  7) << 29);
    result |= (bit_of(A,  8) << 28);
    result |= (bit_of(A,  0) << 27);
    result |= (bit_of(A, 11) << 26);
    result |= (bit_of(A, 12) << 25);
    result |= (bit_of(A,  5) << 24);

    /* 地址[23:16] */
    result |= (bit_of(A, 13) << 23);
    result |= (bit_of(A,  1) << 22);
    result |= (bit_of(A,  6) << 21);
    result |= (bit_of(A,  4) << 20);
    result |= (bit_of(A, 10) << 19);
    result |= (bit_of(A,  3) << 18);
    result |= (bit_of(A,  8) << 17);
    result |= (bit_of(A, 17) << 16);

    /* 地址[15:8] */
    result |= (bit_of(A,  7) << 15);
    result |= (bit_of(A,  6) << 14);
    result |= (bit_of(A, 14) << 13);
    result |= (bit_of(A,  4) << 12);
    result |= (bit_of(A, 16) << 11);
    result |= (bit_of(A, 15) << 10);
    result |= (bit_of(A, 12) << 9);
    result |= (bit_of(A,  9) << 8);

    /* 地址[7:0] */
    result |= (bit_of(A, 17) << 7);
    result |= (bit_of(A, 11) << 6);
    result |= (bit_of(A,  6) << 5);
    result |= (bit_of(A, 14) << 4);
    result |= (bit_of(A,  0) << 3);
    result |= (bit_of(A,  2) << 2);
    result |= (bit_of(A, 12) << 1);
    result |= (bit_of(A, 15) << 0);

    return result;
}

__RAM_FUNC uint32_t encrypt(uint8_t data[4], uint32_t word_addr, uint32_t userkey)
{
    uint32_t data32 = (uint32_t)data[0] | ((uint32_t)data[1] << 8) |
           ((uint32_t)data[2] << 16) | ((uint32_t)data[3] << 24);
    /* 第一级：userkey 异或 */
    uint32_t tmp = data32 ^ userkey;
    /* 第二级：字节交换后再与固定 KEY 异或 */
    tmp = byteswap(tmp) ^ FIXED_KEY;
    /* 第三级：与地址混淆值异或 */
    tmp ^= scramble_addr(word_addr);
    return tmp;
}

__RAM_FUNC uint32_t decrypt(uint8_t cipher[4], uint32_t word_addr, uint32_t userkey)
{
    uint32_t data32 = (uint32_t)cipher[0] | ((uint32_t)cipher[1] << 8) |
           ((uint32_t)cipher[2] << 16) | ((uint32_t)cipher[3] << 24);
    /* 逆第三级：异或地址混淆值 */
    uint32_t tmp = data32 ^ scramble_addr(word_addr);
    /* 逆第二级：异或固定 KEY 后再进行逆字节交换 */
    tmp = inv_byteswap(tmp ^ FIXED_KEY);
    /* 逆第一级：异或 userkey */
    tmp ^= userkey;
    return tmp;
}

/**
 * @brief Flash 加解密测试
 *   1、禁用时，软件加密32字节RAW data，写入Flash User Data区域
 *   2、启用，读取该区域数据，验证数据准确
 *   3、擦除该区域后，写入32字节新数据
 *   4、再次禁用，读取该区域，软件解密，验证数据准确性
 * @return 0: 全部通过; -1: 某次擦写读比对失败
 */
__RAM_FUNC int slc_flash_encrypt_test(void)
{
    const uint32_t FW_TEST_ADDRESS = FLASH_USERER_DATA_ADDR;
    const uint32_t FWKEY_TEST = 0x23456781;
    const uint32_t FW_ENCRYPT_ENABLE = 0x1;
    const uint32_t FW_ENCRYPT_DISABLE = 0x0;
    
    slc_hal_sysctrl_cache_mode_set(HAL_CACHE_ENABLE);
    uint8_t write_data[128] = {0};
    uint8_t read_data[128] = {0};
    uint8_t temporary_data[128] = {0};
    
    // 1. flash read decrypt test 
    for(int i=0;i<128;i++)
        write_data[i] = i+111;
    
    // software encrypt data and write data
    for(int i=0;i<128;i+=4){
        uint32_t d = encrypt(write_data+i, (FW_TEST_ADDRESS+i)>>2, FWKEY_TEST);
        temporary_data[i+0] = (uint8_t)(d);
        temporary_data[i+1] = (uint8_t)(d >> 8);
        temporary_data[i+2] = (uint8_t)(d >> 16);
        temporary_data[i+3] = (uint8_t)(d >> 24);
    }
    SYS_CTRL->Res2 = FW_ENCRYPT_DISABLE;
    rom_hw_sysctrl_set_cache_mode(EN_CACHE_FLUSH);
    
    slc_hal_nop_delay_ms(1);
    rom_hw_flash_erase_by_length(FW_TEST_ADDRESS, UNIT_SECTOR);
    rom_hw_flash_write_data(FW_TEST_ADDRESS, temporary_data, 128);
    
    // read decrypt data
    SYS_CTRL->Res3 = FWKEY_TEST;
    SYS_CTRL->Res2 = FW_ENCRYPT_ENABLE;
    rom_hw_sysctrl_set_cache_mode(EN_CACHE_FLUSH);
    
    slc_hal_nop_delay_ms(1);
    rom_hw_flash_read_data(FW_TEST_ADDRESS, read_data, 128);
    for(int i=0; i<128; i++) {
        if(write_data[i] != read_data[i]) {
            // PRINTF无法使用 
            //PRINTF("read decrypt test failed @ 0x%08X: R0x%02X != W0x%02X\n",
            //    FW_TEST_ADDRESS+i, read_data[i], write_data[i]);
            
            SYS_CTRL->Res2 = FW_ENCRYPT_DISABLE;
            rom_hw_sysctrl_set_cache_mode(EN_CACHE_FLUSH);
            return -1;
        }
    }
    
    // 2. flash write encrypt test 
    for(int i=0;i<128;i++) {
        write_data[i] = i+1;
    }
    // write encrypt data
    rom_hw_flash_erase_by_length(FW_TEST_ADDRESS, UNIT_SECTOR);
    rom_hw_flash_write_data(FW_TEST_ADDRESS, write_data, 128);
    
    // read data and software decrypt data
    SYS_CTRL->Res2 = FW_ENCRYPT_DISABLE;
    rom_hw_sysctrl_set_cache_mode(EN_CACHE_FLUSH);
    
    slc_hal_nop_delay_ms(1);
    rom_hw_flash_read_data(FW_TEST_ADDRESS, temporary_data, 128);
    for(int i=0;i<128;i+=4){
        uint32_t d = decrypt(temporary_data+i, (FW_TEST_ADDRESS+i)>>2, FWKEY_TEST);
        read_data[i+0] = (uint8_t)(d);
        read_data[i+1] = (uint8_t)(d >> 8);
        read_data[i+2] = (uint8_t)(d >> 16);
        read_data[i+3] = (uint8_t)(d >> 24);
    }
    
    for(int i=0; i<128; i++) {
        if(write_data[i] != read_data[i]) {
            //PRINTF("write encrypt test failed @ 0x%08X: R0x%02X != W0x%02X\n",
            //    FW_TEST_ADDRESS+i, read_data[i], write_data[i]);
            
            SYS_CTRL->Res2 = FW_ENCRYPT_DISABLE;
            rom_hw_sysctrl_set_cache_mode(EN_CACHE_FLUSH);
            return -1;
        }
    }
    
    SYS_CTRL->Res2 = FW_ENCRYPT_DISABLE;
    rom_hw_sysctrl_set_cache_mode(EN_CACHE_FLUSH);
    slc_hal_nop_delay_ms(100);
    
    PRINTF("flash encrypt & decrypt test finished.\n");
    return 0;
}

/**
 * @brief Flash 加解密测试（Cache禁用），流程与Cache使能时一致
 *   由于Flash加解密模块需要用到的当前读写地址来自cache，因此关闭cache无法使能加解密功能
 * @return 0: 全部通过; -1: 某次擦写读比对失败
 */
__RAM_FUNC int slc_flash_encrypt_without_cache_test(void)
{
    const uint32_t FW_TEST_ADDRESS = FLASH_USERER_DATA_ADDR;
    const uint32_t FWKEY_TEST = 0x23456781;
    const uint32_t FW_ENCRYPT_ENABLE = 0x1;
    const uint32_t FW_ENCRYPT_DISABLE = 0x0;
    uint8_t write_data[128] = {0};
    uint8_t read_data[128] = {0};
    uint8_t temporary_data[128] = {0};
    
    rom_hw_sysctrl_set_cache_mode(EN_CACHE_FLUSH);
    __DSB(); __ISB();
    slc_hal_sysctrl_cache_mode_set(HAL_CACHE_DISABLE);
    // 1. flash read decrypt test 
    for(int i=0;i<128;i++)
        write_data[i] = i+101;
    
    // software encrypt data and write data
    for(int i=0;i<128;i+=4){
        uint32_t d = encrypt(write_data+i, (FW_TEST_ADDRESS+i)>>2, FWKEY_TEST);
        temporary_data[i+0] = (uint8_t)(d);
        temporary_data[i+1] = (uint8_t)(d >> 8);
        temporary_data[i+2] = (uint8_t)(d >> 16);
        temporary_data[i+3] = (uint8_t)(d >> 24);
    }
    SYS_CTRL->Res2 = FW_ENCRYPT_DISABLE;
    
    slc_hal_nop_delay_ms(1);
    rom_hw_flash_erase_by_length(FW_TEST_ADDRESS, UNIT_SECTOR);
    rom_hw_flash_write_data(FW_TEST_ADDRESS, temporary_data, 128);
    
    // read decrypt data
    SYS_CTRL->Res3 = FWKEY_TEST;
    SYS_CTRL->Res2 = FW_ENCRYPT_ENABLE;
    
    slc_hal_nop_delay_ms(1);
    rom_hw_flash_read_data(FW_TEST_ADDRESS, read_data, 128);
    for(int i=0; i<128; i++) {
        if(write_data[i] != read_data[i]) {
            // PRINTF无法使用 
            //PRINTF("read decrypt test failed @ 0x%08X: R0x%02X != W0x%02X\n",
            //    FW_TEST_ADDRESS+i, read_data[i], write_data[i]);
            SYS_CTRL->Res2 = FW_ENCRYPT_DISABLE;
            return -1;
        }
    }
    
    // 2. flash write encrypt test 
    for(int i=0;i<128;i++) {
        write_data[i] = i+2;
    }
    // write encrypt data
    rom_hw_flash_erase_by_length(FW_TEST_ADDRESS, UNIT_SECTOR);
    rom_hw_flash_write_data(FW_TEST_ADDRESS, write_data, 128);
    
    // read data and software decrypt data
    SYS_CTRL->Res2 = FW_ENCRYPT_DISABLE;
    
    slc_hal_nop_delay_ms(1);
    rom_hw_flash_read_data(FW_TEST_ADDRESS, temporary_data, 128);
    for(int i=0;i<128;i+=4){
        uint32_t d = decrypt(temporary_data+i, (FW_TEST_ADDRESS+i)>>2, FWKEY_TEST);
        read_data[i+0] = (uint8_t)(d);
        read_data[i+1] = (uint8_t)(d >> 8);
        read_data[i+2] = (uint8_t)(d >> 16);
        read_data[i+3] = (uint8_t)(d >> 24);
    }
    
    for(int i=0; i<128; i++) {
        if(write_data[i] != read_data[i]) {
            //PRINTF("write encrypt test failed @ 0x%08X: R0x%02X != W0x%02X\n",
            //    FW_TEST_ADDRESS+i, read_data[i], write_data[i]);
            
            SYS_CTRL->Res2 = FW_ENCRYPT_DISABLE;
            return -2;
        }
    }
    
    SYS_CTRL->Res2 = FW_ENCRYPT_DISABLE;
    
    slc_hal_sysctrl_cache_mode_set(HAL_CACHE_ENABLE);
    slc_hal_nop_delay_ms(100);
    PRINTF("flash encrypt & decrypt test finished.\n");
    return 0;
}

uint8_t dev_id[2][3] = {0};
int slc_flash_pmu_set(void)
{
    rom_hw_sysctrl_set_cache_mode(EN_CACHE_DISABLE);
    //PMU->LP_MAN |= (1<<3);
    for(int i=0; i<1; i++){

        if(i == 0){
            //PMU->LP_MAN |= (1<<4);
            PMU->LP_MAN = 0X18;
        }
        else if(i == 1){
            //PMU->LP_MAN &= ~(1<<4);
            PMU->LP_MAN = 0X08;
        }
        else {}
        
        slc_hal_nop_delay_ms(100);
        rom_hw_flash_read_dev_id(dev_id[i], 3);
        }
    PMU->LP_MAN = 0;
    rom_hw_sysctrl_set_cache_mode(EN_CACHE_ENABLE);
    return 0;
}

int slc_flash_io_ctrl_test(void)
{
    ///uint8_t dev_id[3] = {0};
    uint8_t uid[8] = {0};
    uint8_t dev_id_test[3] = {0xC4, 0x40, 0x12};

    slc_flash_pmu_set();
    for(uint8_t i=0; i<2; i++){
        dump_u8buf("dev_id", dev_id[i], 3);
        if(memcmp(dev_id[i], dev_id_test, 3) == 0){
            if(i == 0){
                PRINTF("flash io open %d PASS\n",i);
            }
            else{
                PRINTF("flash io close %d FAIL\n",i);
            }
        }
        else{
            if(i != 0){
                PRINTF("flash io close %d PASS\n",i);
            }
            else{
                PRINTF("flash io open %d FAIL\n",i);
            }
        }
    }
    
    return 0;
}



