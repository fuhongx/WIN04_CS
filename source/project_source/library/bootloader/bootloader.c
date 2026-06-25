#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "error_def.h"
#include "utility.h"

#include "reg_Dev.h"
#include "hw_flash.h"
#include "hw_sysctrl.h"
#include "hw_pmu.h"

#include "version.h"
#include "app_cfg.h"
#include "debug.h"
#include "hw_wdt.h"
#include "hw_crc.h"
#include "hw_rng.h"
#include "bootloader.h"
#include "boot_interface.h"
#include "firmware_upgrade.h"
#include "compiler.h"
#include "sha256.h"
#include "uECC.h"

stFlashSelfCali_t gstCaliInfo;
fw_security_info_t g_security_info = {0};
chip_cap_t g_chip_cap = {0};

uint32_t rom_hw_crc32(uint8_t *pu8Buffer, uint16_t u16Len)
{
    stCrcInit_t stInit;

    stInit.bDmaEn = false;
    stInit.bInputReverse =true;
    stInit.bOutputXor = true;
    stInit.enOutputMode = EN_CRC_OUTPUT_REVERSE;

    rom_hw_crc_init(CRC32, &stInit);

    rom_hw_crc_set_init_value(CRC32, 0xFFFFFFFF);
    rom_hw_crc_set_xor_bytes(CRC32, 0xFFFFFFFF);

    return rom_hw_crc_calculate_data(CRC32, pu8Buffer, u16Len);
}

void boot_load_code(uint32_t u32FlashAddr, uint32_t u32LoadLen, uint32_t u32LoadAddr)
{
    uint32_t *puRam = (uint32_t *)u32LoadAddr;
    rom_hw_flash_read_data_u32(u32FlashAddr, puRam, u32LoadLen/4);
}

void boot_flash_self_cali_init(void)
{
    gstCaliInfo.enMode = EN_FLASH_RW_MODE_QSPI;
    
    gstCaliInfo.u32FlashQspiDelay.u32Cfg = FLASH_SELF_CALI_INIT_QSPI_DELAY;
    gstCaliInfo.u8FlashClkDiv = 0;
    gstCaliInfo.u8SampleDelay = 0x1;
    gstCaliInfo.u8SampleEdge = 0x1; //0x1 Default falling edge.
}

bool boot_reconfig_flash_mode(void)
{
    if((gstCaliInfo.u8Index & 0xF) < FLASH_SELF_CALI_CLK_DIV_MAX)
    {
        gstCaliInfo.u8FlashClkDiv = gstCaliInfo.u8Index & 0xF;
    }

    if((gstCaliInfo.u8Index & 0xF) >= FLASH_SELF_CALI_CLK_DIV_MAX)
    {
        gstCaliInfo.u32FlashQspiDelay.u32Cfg = FLASH_SELF_CALI_INIT_QSPI_DELAY * ((gstCaliInfo.u8Index & 0xF) - FLASH_SELF_CALI_CLK_DIV_MAX);
    }

    if((gstCaliInfo.u8Index == 0x10))
    {
        boot_flash_self_cali_init();
        
        gstCaliInfo.enMode = EN_FLASH_RW_MODE_DPI;
    }

    if((gstCaliInfo.u8Index == 0x20))
    {
        boot_flash_self_cali_init();
        
        gstCaliInfo.enMode = EN_FLASH_RW_MODE_SPI;
    }

    if((gstCaliInfo.u8Index == 0x30))
    {
        gstCaliInfo.u32FlashQspiDelay.u32Cfg = FLASH_SELF_CALI_INIT_QSPI_DELAY;
        gstCaliInfo.u8SampleEdge = 0x0; //Falling edge
    }

    rom_hw_flash_ctrl_set_clk_div(gstCaliInfo.u8FlashClkDiv);
    rom_hw_flash_ctrl_set_qspi_delay(gstCaliInfo.u32FlashQspiDelay);
    rom_hw_flash_ctrl_set_read_data_capture(gstCaliInfo.u8SampleDelay, gstCaliInfo.u8SampleEdge);
    rom_hw_flash_config_read_write_mode(gstCaliInfo.enMode);
    gstCaliInfo.u8Index++;

    if(gstCaliInfo.u8Index == 0x40)
    {
        return false;
    }

    return true;
}

void boot_check_header(void)
{
    //Read Boot2 Header
    uint32_t au32ReadBuffer[BOOT2HEADER_LEN/4] = {0};
    bool bPassFlag = false;

    boot_flash_self_cali_init();
    gstCaliInfo.u8Index = 0;

    while(!bPassFlag) {
        rom_hw_flash_read_data_u32(FLASH_BOOT2_HEADER_ADDR, au32ReadBuffer, BOOT2HEADER_LEN/4);
        
        boot2_header_t *pstBoot2Ver = (boot2_header_t *)au32ReadBuffer;

        uint32_t u32HeaderCrcValue = rom_hw_crc32((uint8_t *)&au32ReadBuffer[0], BOOT2HEADER_LEN - BOOT2HEADER_CRC_LEN);

        if(u32HeaderCrcValue != pstBoot2Ver->u32Boot2HeaderCrc) {
           bool bSeflCheckFlag = boot_reconfig_flash_mode();
           while(!bSeflCheckFlag) {
                //Waiting for download.
           }
        } else {
            bPassFlag = true;
        }
    }
}

void boot_set_chip_capability(void)
{
    uint32_t chip_cap_val = 0;

    // GT由于不支持分块熔丝，故不支持基带限制功能
    if (rom_hw_flash_get_type() == EN_FLASH_TYPE_GT) {
        return;
    }

    rom_hw_flash_read_security_mem(EN_FLASH_SEC_MEM1, 0, (uint8_t *)&g_chip_cap, sizeof(g_chip_cap));
    dump_u8buf("g_chip_cap", (uint8_t *)&g_chip_cap, sizeof(g_chip_cap));

    // OTP不烧录的话，OTP区域默认值是全F，基带功能也默认全功能
    g_chip_cap.hsf_mod = (g_chip_cap.hsf_mod == 0xFF) ? 0 : (g_chip_cap.hsf_mod & 0x1);
    g_chip_cap.frame_mode = (g_chip_cap.frame_mode == 0xFF) ? 0 : (g_chip_cap.frame_mode & 0x1);
    g_chip_cap.tof_rang_limit_en = (g_chip_cap.tof_rang_limit_en == 0xFF) ?
        0 : ((g_chip_cap.tof_rang_limit_en & 0x1) ? 1 : 0);
    g_chip_cap.cad_limit_en = (g_chip_cap.cad_limit_en == 0xFF) ?
        0 : ((g_chip_cap.cad_limit_en & 0x1) ? 1 : 0);
    g_chip_cap.buf_limit_en = (g_chip_cap.buf_limit_en == 0xFF) ?
        0 : ((g_chip_cap.buf_limit_en & 0x1) ? 1 : 0);
    g_chip_cap.sfbw_limit_en = (g_chip_cap.sfbw_limit_en == 0xFF) ?
        0 : ((g_chip_cap.sfbw_limit_en & 0x1) ? 1 : 0);

    // 配置受限功能的范围区间
    g_chip_cap.buf_limit_val = (g_chip_cap.buf_limit_val < 0x1) ? 0x1 : g_chip_cap.buf_limit_val;
    g_chip_cap.buf_limit_val = (g_chip_cap.buf_limit_val > 0x180) ? 0x180 : g_chip_cap.buf_limit_val;
    g_chip_cap.bw_high_limit_val = (g_chip_cap.bw_high_limit_val > 0x6) ? 0x0 : g_chip_cap.bw_high_limit_val;
    g_chip_cap.bw_low_limit_val = (g_chip_cap.bw_low_limit_val > 0x6) ? 0x6 : g_chip_cap.bw_low_limit_val;
    g_chip_cap.sf_high_limit_val = (g_chip_cap.sf_high_limit_val < 0x5) ? 0x5 : g_chip_cap.sf_high_limit_val;
    g_chip_cap.sf_high_limit_val = (g_chip_cap.sf_high_limit_val > 0xC) ? 0xC : g_chip_cap.sf_high_limit_val;
    g_chip_cap.sf_low_limit_val = (g_chip_cap.sf_low_limit_val < 0x5) ? 0x5 : g_chip_cap.sf_low_limit_val;
    g_chip_cap.sf_low_limit_val = (g_chip_cap.sf_low_limit_val > 0xC) ? 0x5 : g_chip_cap.sf_low_limit_val;
    chip_cap_val = ((g_chip_cap.hsf_mod << 31) | (g_chip_cap.frame_mode << 30) |
                    (g_chip_cap.tof_rang_limit_en << 29) | (g_chip_cap.cad_limit_en << 28) |
                    (g_chip_cap.buf_limit_en << 27) | (g_chip_cap.sfbw_limit_en << 26) |
                    (g_chip_cap.buf_limit_val << 16) | (g_chip_cap.bw_high_limit_val << 12) |
                    (g_chip_cap.bw_low_limit_val << 8) | (g_chip_cap.sf_high_limit_val << 4) |
                    (g_chip_cap.sf_low_limit_val << 0)) & 0xFDFFFFFF;

    write32(ADDR_SYS_CTRL_BASE + 0x60, chip_cap_val);
}

uint8_t rom_get_safe_level(void)
{
    return g_security_info.sta.safe_level;
}

void boot_identify_from_lowpower(void)
{
    uint8_t lpio_en = 0;
    uint8_t flash_force_on = 0;

    rom_hw_pmu_set_boot_lpwr_flag(0);

    rom_hw_pmu_get_lpio_status(&lpio_en);

    if (lpio_en == 0)
        return;

    // set boot lpwr flag for app
    rom_hw_pmu_set_boot_lpwr_flag(1);

    // disable lpio
    rom_hw_pmu_enable_lpio(false);

    // release flash from deep power down
    // rom_hw_pmu_get_flash_pd_status(&flash_force_on);
    // if (flash_force_on)
    //     rom_hw_flash_release_deep_power_down();
}

void boot_identify_from_rst(void)
{
    uint8_t rst_cause = 0;
    rst_cause = rom_hw_sysctrl_get_reset_src();
    rom_hw_sysctrl_set_boot_rst_flag(rst_cause);
    // 如果是低功耗看门狗复位，需关闭看门狗
    if ((rst_cause == EN_RST_FROM_IWDT) && (g_security_info.sta.iwdt_disable)) {
        rom_hw_iwdt_feed_dog();
        rom_hw_iwdt_clear_interrupt_flag(0x3);
        rom_hw_wdt_deinit(IWDT);
        NVIC_ClearPendingIRQ(IWTD_IRQ);
    }
}

void boot_set_flash_enc_by_security_info(void)
{
    uint32_t fw_key = 0;

    rom_hw_flash_read_security_mem(EN_FLASH_SEC_MEM0, 0, (uint8_t *)&g_security_info, BOOT_SECURITY_INFO_BUF_SIZE);
    dump_u8buf("security_info", (uint8_t *)&g_security_info, BOOT_SECURITY_INFO_BUF_SIZE);
    if (g_security_info.sta.safe_level == 0xFF) {
        // 安全信息未初始化，默认安全等级为0
        g_security_info.sta.safe_level = FW_SAFE_LEVEL0;
    }
    if ((g_security_info.sta.fw_enc_status == 1) && (g_security_info.sta.safe_level != FW_SAFE_LEVEL0)) {
        fw_key = rom_hw_crc32(g_security_info.key.pub_key, 64);
        // 写入flash加密密钥
        SYS_CTRL->Res3 = fw_key;
        // 关闭SWD并打开flash加密
        SYS_CTRL->Res2 = 0x3;

        // flash加密依赖cache的使能
        rom_hw_sysctrl_set_cache_mode(EN_CACHE_ENABLE);
        rom_hw_sysctrl_set_cache_mode(EN_CACHE_FLUSH);

        rom_hw_update_rng_seed(fw_key);
    }
}

int rom_check_firmware(uint32_t u32Addr, uint32_t u32Len, uint8_t *pu8Hash, uint8_t *pu8Sign)
{
    uint8_t hash[HASH_MAX_LEN] = {0};
    const struct uECC_Curve_t *curves;
    uint8_t is_standby = 0;
    bool check_hash = false;
    bool check_ecc = false;

    rom_hw_pmu_get_boot_lpwr_flag(&is_standby);
    curves = uECC_secp256k1();
    uECC_set_rng(uECC_user_get_bytes);

    switch (g_security_info.sta.safe_level) {
        case FW_SAFE_LEVEL0:
            check_hash = false;
            check_ecc = false;
            break;

        case FW_SAFE_LEVEL1:
            // Level 1: 仅在非低功耗模式下校验 Hash
            check_hash = (is_standby == 0) ? true : false;
            check_ecc = false;
            break;

        case FW_SAFE_LEVEL2:
            // Level 2: 仅在非低功耗模式下校验 Hash 和 ECC
            check_hash = (is_standby == 0) ? true : false;
            check_ecc = (is_standby == 0) ? true : false;
            break;

        case FW_SAFE_LEVEL3:
            // Level 3: 始终校验 Hash，仅在非低功耗模式下校验 ECC
            check_hash = true;
            check_ecc = (is_standby == 0) ? true : false;
            break;

        case FW_SAFE_LEVEL4:
            // Level 4: 始终校验 Hash 和 ECC
            check_hash = true;
            check_ecc = true;
            break;

        default:
            check_hash = true;
            check_ecc = true;
            break;
    }

    // 调试时bypass校验
    // check_hash = false;
    // check_ecc = false;

    // 执行 Hash 校验
    if (check_hash) {
        sha256((uint8_t*)u32Addr, u32Len, hash);

        if (memcmp(hash, pu8Hash, HASH_MAX_LEN) != 0) {
            PRINTF("hash check fail!\n");
            goto err;
        }
    }

    // 执行 ECC 签名校验
    if (check_ecc) {
        if (!uECC_verify(g_security_info.key.pub_key, hash, HASH_MAX_LEN, pu8Sign, curves)) {
            PRINTF("ecc verify failed\n");
            goto err;
        }
    }

    return 0;

err:
    dump_u8buf("read fw hash", pu8Hash, HASH_MAX_LEN);
    dump_u8buf("calculated hash", hash, HASH_MAX_LEN);
    dump_u8buf("read fw sign", pu8Sign, SIGNATURE_MAX_LEN);

    return -1;
}

void boot_peripheral_deinit(void)
{
    rom_hw_sysctrl_reset_peripheral(EN_SYSCTRL_GPIO);
    rom_hw_sysctrl_enable_clock_gate(EN_SYSCTRL_CRC32, false);
    rom_hw_sysctrl_reset_peripheral(EN_SYSCTRL_CRC32);
    rom_hw_sysctrl_enable_clock_gate(EN_SYSCTRL_RAND, false);
    rom_hw_sysctrl_reset_peripheral(EN_SYSCTRL_RAND);
    rom_hw_sysctrl_enable_clock_gate(EN_SYSCTRL_UART0, false);
    rom_hw_sysctrl_reset_peripheral(EN_SYSCTRL_UART0);
    rom_hw_sysctrl_enable_clock_gate(EN_SYSCTRL_UART1, false);
    rom_hw_sysctrl_reset_peripheral(EN_SYSCTRL_UART1);
}

void boot_from_boot2(void)
{
    uint32_t au32ReadBuffer[BOOT2HEADER_LEN/4] = {0};
    uint32_t flash_reset_addr = 0;
    int ret = 0;

    // 0. check header crc, if fail, try to reconfig flash mode, if all mode tried, wait for download.
    boot_check_header();

    //Read Boot2 Header
    rom_hw_flash_read_data_u32(FLASH_BOOT2_HEADER_ADDR, au32ReadBuffer, BOOT2HEADER_LEN/4);

    boot2_header_t *pstBoot2Ver = (boot2_header_t *)au32ReadBuffer;
    // 若boot2在ram，则加载至ram后执行，否则直接执行flash中的boot2代码
    if ((pstBoot2Ver->u32Boot2LoadAddr >= RAM_BASE_ADDR) && (pstBoot2Ver->u32Boot2LoadAddr < RAM_ADDR_MAX)) {
        PRINTF("[BL] Boot2 in RAM\n");
        boot_load_code(FLASH_BOOT2_CODE_ADDR, pstBoot2Ver->u32Boot2Len, pstBoot2Ver->u32Boot2LoadAddr);
        flash_reset_addr = pstBoot2Ver->u32Boot2LoadAddr;
    } else {
        flash_reset_addr = *(volatile uint32_t*)(pstBoot2Ver->u32Boot2LoadAddr);
        PRINTF("[BL] Boot2 in FLASH\n");
    }

    PRINTF("[BL] Boot2 Flag = 0x%x\n", pstBoot2Ver->u32Boot2Flag);
    PRINTF("[BL] Boot2 Len = 0x%x\n", pstBoot2Ver->u32Boot2Len);
    PRINTF("[BL] Boot2 Ver = 0x%x\n", pstBoot2Ver->u32Boot2Ver);
    PRINTF("[BL] Boot2 Crc = 0x%x\n", pstBoot2Ver->u32Boot2Crc);
    PRINTF("[BL] Boot2 LoadAddr = 0x%x\n", pstBoot2Ver->u32Boot2LoadAddr);

    ret = rom_check_firmware(pstBoot2Ver->u32Boot2LoadAddr, pstBoot2Ver->u32Boot2Len,
                                pstBoot2Ver->hash, pstBoot2Ver->signiture);

    if (ret == 0) {
        PRINTF("[BL] Boot2 Check Success\n");
        boot_peripheral_deinit();
        rom_utility_run_callback_noparam(flash_reset_addr);
    }

    while(1);
}


void boot_selection(void)
{
    uint8_t is_standby = 0;
    rom_hw_pmu_get_boot_lpwr_flag(&is_standby);
    // 如果是从低功耗唤醒，直接进入boot2，避免进入升级流程
    if (is_standby == 1) {
        boot_from_boot2();
    }

    boot_interface_gpio_init();

    bool bBootSel = boot_interface_need_down_bin();

    boot_set_chip_capability();

    if(bBootSel) {
        boot_interface_uart_init();
        firmware_upgrade_main();
    } else {
        boot_from_boot2();
    }
}