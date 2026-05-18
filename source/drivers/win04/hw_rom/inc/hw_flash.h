/**
 * ****************************************************************************
 * @file hw_flash.h
 * @author SLC(SC) software team. 
 * @brief 
 * @version  1.0
 * ****************************************************************************
 * 
 * Copyright (c) 2024 SLC Semiconductor all rights reserved.
 * 
 * ****************************************************************************
 */

#ifndef __HW_FLASH_H__
#define __HW_FLASH_H__

#ifdef __cplusplus
extern "C"
{
#endif

#define FLASH_ERASE_SECURITY_MEM_KEY            (0x70617373)

/**
 * @brief Flash type define.
 * 
 */
typedef enum 
{
    EN_FLASH_TYPE_GT    = 0xC4,
    EN_FLASH_TYPE_PY    = 0x85,
} EN_FLASH_TYPE_T;

/**
 * @brief Flash ctrl qspi mode enumeration
 * 
 */
typedef enum
{
    EN_FLASH_QSPI_MODE_0    = 0x0,
    EN_FLASH_QSPI_MODE_1    = 0x1,
    EN_FLASH_QSPI_MODE_2    = 0x2,
    EN_FLASH_QSPI_MODE_3    = 0x3,
    EN_FLASH_QSPI_MODE_MAX,
}EN_FLASH_CTRL_SPI_MODE_T;

/**
 * @brief Security mem defined in GT25Qxx,
 * if using puya flash, only support EN_FLASH_SEC_MEM0~2.
 * 
 */
typedef enum
{
    EN_FLASH_SEC_MEM0       = 0x0,
    EN_FLASH_SEC_MEM1       = 0x1,
    EN_FLASH_SEC_MEM2       = 0x2,
    EN_FLASH_SEC_MEM3       = 0x3,
}EN_FLASH_SEC_MEM_T;

/**
 * @brief XIP mode selection enumeration.
 * 
 */
typedef enum
{
    EN_FLASH_OUT_XIP_MODE                           = 0x0,
    EN_FLASH_IN_XIP_MODE_NEXT_READ                  = 0x1,
    EN_FLASH_IN_XIP_MODE_IMMEDIATE                  = 0x2,
    EN_FLASH_XIP_MODE_MAX,
}EN_FLASH_XIP_MODE_T;

/**
 * @brief Flash ctrl clk div enumeration.
 * 
 */
typedef enum
{
    EN_FLASH_CLK_DIV2           = 0x0,
    EN_FLASH_CLK_DIV4           = 0x1,
    EN_FLASH_CLK_DIV6           = 0x2,
    EN_FLASH_CLK_DIV8           = 0x3,
    EN_FLASH_CLK_DIV10          = 0x4,
    EN_FLASH_CLK_DIV12          = 0x5,
    EN_FLASH_CLK_DIV14          = 0x6,
    EN_FLASH_CLK_DIV16          = 0x7,
    EN_FLASH_CLK_DIV18          = 0x8,
    EN_FLASH_CLK_DIV20          = 0x9,
    EN_FLASH_CLK_DIV22          = 0xA,
    EN_FLASH_CLK_DIV24          = 0xB,
    EN_FLASH_CLK_DIV26          = 0xC,
    EN_FLASH_CLK_DIV28          = 0xD,
    EN_FLASH_CLK_DIV30          = 0xE,
    EN_FLASH_CLK_DIV32          = 0xF,
}EN_FLASH_CLK_DIV_T;

/**
 * @brief Flash erase enumeration.
 * 
 */
typedef enum
{
    EN_FLASH_ERASE_SECTOR       = 0x0,
    EN_FLASH_ERASE_32K          = 0x1,
    EN_FLASH_ERASE_64K          = 0x2,
    EN_FLASH_ERASE_CHIP         = 0x3,
    EN_FLASH_ERASE_PAGE         = 0x4, //Only in puya flash.
}EN_FLASH_ERASE_T;

/**
 * @brief Read flash status reg enumeration.
 * 
 */
typedef enum
{
    EN_FLASH_READ_STA_REG1      = 0x0,
    EN_FLASH_READ_STA_REG2      = 0x1,
    EN_FLASH_READ_STA_REG3      = 0x2,
}EN_FLASH_READ_STA_REG_T;


typedef enum
{
    EN_FLASH_RW_MODE_SPI        =   0x0,
    EN_FLASH_RW_MODE_DPI        =   0x1,
    EN_FLASH_RW_MODE_QSPI       =   0x2,
    EN_FLASH_RW_MODE_FQSPI      =   0x3,
}EN_FLASH_RW_MODE_T;


/**
 * @brief Flash ctrl interrupt status enumeration.
 * 
 */
typedef enum
{
    EN_FLASH_CTRL_UNDERFLOW_DECT_INT        = (FLASH_CTRL_INT_STA_UNDERFLOW_DETECT_MASK << FLASH_CTRL_INT_STA_UNDERFLOW_DETECT_SHIFT),
    EN_FLASH_CTRL_INDAC_FINISH_INT          = (FLASH_CTRL_INT_STA_INDAC_FINISH_MASK << FLASH_CTRL_INT_STA_INDAC_FINISH_SHIFT),
    EN_FLASH_CTRL_INDAC_REJECT_INT          = (FLASH_CTRL_INT_STA_INDAC_REJECT_MASK << FLASH_CTRL_INT_STA_INDAC_REJECT_SHIFT),
    EN_FLASH_CTRL_WRITE_PROTECT_INT         = (FLASH_CTRL_INT_STA_WRITE_PROTECT_MASK << FLASH_CTRL_INT_STA_WRITE_PROTECT_SHIFT),
    EN_FLASH_CTRL_ILLEGAL_AHB_INT           = (FLASH_CTRL_INT_STA_ILLEGAL_AHB_ACCESS_MASK << FLASH_CTRL_INT_STA_ILLEGAL_AHB_ACCESS_SHIFT),
    EN_FLASH_CTRL_REACH_INDAC_INT           = (FLASH_CTRL_INT_STA_REACH_INDAC_WATERMARK_MASK << FLASH_CTRL_INT_STA_REACH_INDAC_WATERMARK_SHIFT),
    EN_FLASH_CTRL_SRAM_FULL_INT             = (FLASH_CTRL_INT_STA_SRAM_FULL_MASK << FLASH_CTRL_INT_STA_SRAM_FULL_SHIFT),
    EN_FLASH_CTRL_POLLING_TIMEOUT_INT       = (FLASH_CTRL_INT_STA_POLLING_TIMEOUT_MASK << FLASH_CTRL_INT_STA_POLLING_TIMEOUT_SHIFT),
}EN_FLASH_CTRL_INT_STA_T;

typedef enum
{
    EN_FLASH_READ_MODE_READ =   0x0,
    EN_FLASH_READ_MODE_FAST_READ    =   0x1,
    EN_FLASH_READ_MODE_DUAL_READ    =   0x2,
    EN_FLASH_READ_MODE_QUAD_READ    =   0x3,
    EN_FLASH_READ_MODE_DUAL_FAST_READ   =   0x4,
    EN_FLASH_READ_MODE_QUAD_FAST_READ   =   0x5
}EN_FLASH_READ_MODE_T;

typedef enum
{
    EN_FLASH_WRITE_MODE_PP          =   0x0,
    EN_FLASH_WRITE_MODE_QPP         =   0x1,
}EN_FLASH_WRITE_MODE_T;

/**
 * @brief  Flash ctrl init struct definition.
 */
typedef union
{
    struct
    {
        uint32_t QspiEn     : 1;  /** BIT 0 */
        uint32_t SpiPOL     : 1;  /** BIT 1 */
        uint32_t SpiPHA     : 1;  /** BIT 2*/
        uint32_t Res1       : 4;  /** BIT 4-6*/
        uint32_t DACEn      : 1;  /** BIT 7*/
        uint32_t Res2       : 9;  /** BIT 8-16*/
        uint32_t NextXipEn  : 1;  /** BIT 17*/
        uint32_t ImmeXipEn  : 1;  /** BIT 18*/
        uint32_t ClkDiv     : 4;  /** BIT 19-22*/
        uint32_t Res3       : 8;  /** BIT 23-30*/
        uint32_t QspiSta    : 1;  /** BIT 31*/
    } stFlashReadInst;
    uint32_t u32Cfg;
} unFlashCrtlInit_t;


/**
 * @brief  Flash read instruction struct definition.
 */
typedef union
{
    struct
    {
        uint32_t ReadOpcode : 8;  /** BIT 0-7 */
        uint32_t CmdType    : 2;  /** BIT 8-9 */
        uint32_t Res1       : 2;  /** BIT 10-11*/
        uint32_t AddrMode   : 2;  /** BIT 12-13*/
        uint32_t Res2       : 2;  /** BIT 14-15*/
        uint32_t DataMode   : 2;  /** BIT 16-17*/
        uint32_t Res3       : 2;  /** BIT 18-19*/
        uint32_t ModeBitEn  : 1;  /** BIT 20*/
        uint32_t Res4        : 3;  /** BIT 21-23*/
        uint32_t DummyClkNum : 5;  /** BIT 24-28*/
        uint32_t Res5        : 3;  /** BIT 29-31*/
    } stFlashReadInst;
    uint32_t u32Cfg;
} unFlashReadInst_t;

/**
 * @brief  Flash write instruction struct definition.
 */
typedef union
{
    struct
    {
        uint32_t WriteOpcode : 8;  /** BIT 0-7 */
        uint32_t WELDisable  : 1;  /** BIT 8 */
        uint32_t Res1       : 3;  /** BIT 9-11*/
        uint32_t AddrMode   : 2;  /** BIT 12-13*/
        uint32_t Res2       : 2;  /** BIT 14-15*/
        uint32_t DataMode   : 2;  /** BIT 16-17*/
        uint32_t Res3       : 6;  /** BIT 18-23*/
        uint32_t DummyClkNum : 5;  /** BIT 24-28*/
        uint32_t Res5        : 3;  /** BIT 29-31*/
    } stFlashWriteInst;
    uint32_t u32Cfg;
} unFlashWriteInst_t;

/**
 * @brief Flash write instruction struct definition.
 * 
 */
typedef union
{
    struct
    {
        uint8_t u8CSDA;
        uint8_t u8CSDADA;
        uint8_t u8CSEOT;
        uint8_t u8CSSOT;
    }stFlashQspiDelay;
    uint32_t u32Cfg;
} unFlashQspiDelay_t;




//------------------------------------------------------------------------------
//Flash ctrl apis
//------------------------------------------------------------------------------
void rom_hw_flash_set_type(EN_FLASH_TYPE_T type);
uint8_t rom_hw_flash_get_type(void);

/**
 * @brief Select the QSPI mode.
 * 
 * @param enMode @ref EN_FLASH_CTRL_SPI_MODE_T.
 * @return EN_ERR_STA_T 
 */
EN_ERR_STA_T rom_hw_flash_ctrl_set_qspi_mode(EN_FLASH_CTRL_SPI_MODE_T enMode);

/**
 * @brief Set qspi clock divider.
 * 
 * @param enDiv @ref EN_FLASH_CLK_DIV_T
 * @return EN_ERR_STA_T 
 */
EN_ERR_STA_T rom_hw_flash_ctrl_set_clk_div(EN_FLASH_CLK_DIV_T enDiv);

/**
 * @brief Init flash ctrl, given flash_ctrl setting.
 * 
 * @param unCfg @ref unFlashCrtlInit_t.
 * @return EN_ERR_STA_T 
 */
EN_ERR_STA_T rom_hw_flash_ctrl_init(unFlashCrtlInit_t unCfg);

/**
 * @brief Configure the data read command structure.
 * 
 * @param unInst @ref unFlashReadInst_t.
 * @return EN_ERR_STA_T 
 */
EN_ERR_STA_T rom_hw_flash_ctrl_set_read_instruction(unFlashReadInst_t unInst);

/**
 * @brief Configure the command structure for writing data.
 * 
 * @param unInst @ref unFlashWriteInst_t.
 * @return EN_ERR_STA_T 
 */
EN_ERR_STA_T rom_hw_flash_ctrl_set_write_instruction(unFlashWriteInst_t unInst);

/**
 * @brief For details about how to configure the waiting time during Flash operations, 
 *        see the Flash data manual (GT25Q20D).
 * 
 * @param unDelay @ref unFlashQspiDelay_t.
 * @return EN_ERR_STA_T 
 */
EN_ERR_STA_T rom_hw_flash_ctrl_set_qspi_delay(unFlashQspiDelay_t unDelay);

/**
 * @brief Configure the delay and edge when reading QSPI. 
 * 
 * @param u8SampDelay The delay is the number of reference clocks.
 * @param u8Edge 0: Use ref_clk to sample flash output along the falling edge. 
 *               1: use ref_clk to sample flash output along the rising edge
 * @return EN_ERR_STA_T 
 */
EN_ERR_STA_T rom_hw_flash_ctrl_set_read_data_capture(uint8_t u8SampDelay, uint8_t u8Edge);

/**
 * @brief The Mode bit is configured. After the mode bit is enabled, it is sent 
 *        to the flash immediately following the address byte (1byte).
 * 
 * @param u8ModeBits Mode byte wait to send.
 * @return EN_ERR_STA_T 
 */
EN_ERR_STA_T rom_hw_flash_ctrl_set_mode_bits(uint8_t u8ModeBits);

/**
 * @brief If write protection is required, set the current Flash parameters.
 *
 * @param u8BlockSize The number of bytes in each block must be a power of two.
 *                    0 = 1Byte
 *                    1 = 2Bytes
 *                    2 = 4Bytes
 *                    16 = 65535Bytes
 * @return EN_ERR_STA_T
 */
// EN_ERR_STA_T rom_hw_flash_ctrl_set_protect_block_size(uint8_t u8BlockSize);

/**
 * @brief Defines the block number of the address range to be protected.
 * 
 * @param u32LowIdx Low Area block number.
 * @param u32HiIdx High Area block number
 * @return EN_ERR_STA_T 
 */
// EN_ERR_STA_T rom_hw_flash_ctrl_set_write_protection_block_index(uint32_t u32LowIdx, uint32_t u32HiIdx);

/**
 * @brief Enable write protection
 * 
 * @param bEnable True: enable write protection.
 *                False: disable write protection.
 * @return EN_ERR_STA_T 
 */
// EN_ERR_STA_T rom_hw_flash_ctrl_enable_write_protecion(bool bEnable);

/**
 * @brief Enable write protection reverse.
 * 
 * @param bReverse 
 *        true: When set to "1", the protected region defined in the 
 *              "high region" and "low region" write protect registers is inverted,
 *              meaning that it is the region to which the system is allowed to write.
 *        false: When set to "0", the protected areas defined in the "high zone" and "Low zone" 
 *               write protection registers are the areas that the system does not allow to write. 
 *               This bit is modified only if the write protection function (by the first bit of the write protection register) is 0.
 * @return EN_ERR_STA_T 
 */
// EN_ERR_STA_T rom_hw_flash_ctrl_enbale_write_protection_reverse(bool bReverse);

/**
 * @brief Send commands to Flash through QSPI.
 * 
 * @param u32Cmd Wait for the command to be sent.
 * @return EN_ERR_STA_T 
 */
EN_ERR_STA_T rom_hw_flash_ctrl_send_cmd(uint32_t u32Cmd);

/** Query flash ctrl status.*/
// extern bool rom_hw_flash_ctrl_cmd_done(void);

/**
 * @brief Configure the address for sending commands.
 * 
 * @param u32Addr The addr wait to send.
 * @return EN_ERR_STA_T 
 */
EN_ERR_STA_T rom_hw_flash_ctrl_set_cmd_addr(uint32_t u32Addr);

/**
 * @brief Obtain the data after reading the Flash by running commands.
 * 
 * @param pu8Buffer Points to the stored Buffer.
 * @param u8Len The read length cannot exceed 8 bytes.
 * @return EN_ERR_STA_T 
 */
EN_ERR_STA_T rom_hw_flash_ctrl_get_read_data(uint8_t *pu8Buffer, uint8_t u8Len);

/**
 * @brief Data written to the Flash by running commands.
 * 
 * @param pu8Buffer A pointer to the Buffer where data is written.
 * @param u8Len The write length cannot exceed 8 bytes.
 * @return EN_ERR_STA_T 
 */
EN_ERR_STA_T rom_hw_flash_ctrl_set_write_data(uint8_t *pu8Buffer, uint8_t u8Len);

//------------------------------------------------------------------------------
//Flash apis
//------------------------------------------------------------------------------

//Query cmd
// extern bool rom_hw_flash_wait_cmd_done(void);

/**
 * @brief Send the write enable command to the Flash.
 * 
 * @return EN_ERR_STA_T 
 */
EN_ERR_STA_T rom_hw_flash_write_enable_cmd(void);

/**
 * @brief Erase Erases the contents held by the specified Flash address.
 * 
 * @param enErase @ref EN_FLASH_ERASE_T.
 * @param u32Addr Absolute Flash address to be erased.
 * @return EN_ERR_STA_T 
 */
EN_ERR_STA_T rom_hw_flash_erase(EN_FLASH_ERASE_T enErase, uint32_t u32Addr);

EN_ERR_STA_T rom_hw_flash_erase_by_length(uint32_t u32Addr, uint32_t u32Len);

/**
 * @brief Write data to Flash using DAC mode.
 * 
 * @param u32Addr The address to which data needs to be written.
 * @param pu8Buffer A pointer to the Buffer to which data is to be written.
 * @param u16Len Write data length.
 * @return EN_ERR_STA_T 
 */
EN_ERR_STA_T rom_hw_flash_write_data(uint32_t u32Addr, uint8_t* pu8Buffer, uint32_t u32Len);

/**
 * @brief Read data from the flash.
 * 
 * @param u32Addr The absolute address at which the data is read.
 * @param pu8Buffer A pointer to the buffer from which the data was read.
 * @param u16Len Read data length.
 * @return EN_ERR_STA_T 
 */
EN_ERR_STA_T rom_hw_flash_read_data(uint32_t u32Addr, uint8_t* pu8Buffer, uint32_t u32Len);

/**
 * @brief Sends low-power commands to the flash.
 * 
 * @return EN_ERR_STA_T 
 */
EN_ERR_STA_T rom_hw_flash_deep_power_down(void);

/**
 * @brief The command to disable low power consumption is sent to the flash.
 * 
 * @return EN_ERR_STA_T 
 */
EN_ERR_STA_T rom_hw_flash_release_deep_power_down(void);

/**
 * @brief Read the flash status register 1~3.
 * 
 * @param enReg @ref EN_FLASH_READ_STA_REG_T.
 * @param pu8Sta Pointer to buffer.
 * @return EN_ERR_STA_T 
 */
EN_ERR_STA_T rom_hw_flash_read_status_reg(EN_FLASH_READ_STA_REG_T enReg, uint8_t *pu8Sta);

/**
 * @brief Enable/disable flash qspi mode.
 * 
 * @param bEnable true: enable, false: disable.
 * @return EN_ERR_STA_T 
 */
EN_ERR_STA_T rom_hw_flash_enable_qspi(bool bEnable);

/**
 * @brief flash initialization, flash frequency division is not performed by default, 
 *        and dac mode is enabled.
 * @return EN_ERR_STA_T 
 */
EN_ERR_STA_T rom_hw_flash_init(void);


EN_ERR_STA_T rom_hw_flash_read_data_u32(uint32_t u32Addr, uint32_t* pu32Buffer, uint32_t u32Len);
EN_ERR_STA_T rom_hw_flash_write_data_u32(uint32_t u32Addr, uint32_t* pu32Buffer, uint32_t u32Len);
EN_ERR_STA_T rom_hw_flash_read_uid(uint8_t *pu8Buffer, uint8_t u8Len);
EN_ERR_STA_T rom_hw_flash_read_dev_id(uint8_t *pu8Buffer, uint8_t u8Len);
EN_ERR_STA_T rom_hw_flash_read_security_mem(EN_FLASH_SEC_MEM_T enType, uint16_t u16Offset, uint8_t* pu8Buffer, uint16_t u16Len);
EN_ERR_STA_T rom_hw_flash_write_security_mem(EN_FLASH_SEC_MEM_T enType, uint16_t u16Offset, uint8_t* pu8Buffer, uint16_t u16Len);
EN_ERR_STA_T rom_hw_flash_erase_security_mem(EN_FLASH_SEC_MEM_T enType, uint32_t u32Key);
EN_ERR_STA_T rom_hw_flash_set_read_mode(EN_FLASH_READ_MODE_T enMode);
EN_ERR_STA_T rom_hw_flash_set_write_mode(EN_FLASH_WRITE_MODE_T enMode);
EN_ERR_STA_T rom_hw_flash_write_status_reg(EN_FLASH_READ_STA_REG_T enReg, uint8_t u8Sta);
EN_ERR_STA_T rom_hw_flash_config_read_write_mode(EN_FLASH_RW_MODE_T enMode);

#ifdef __cplusplus
}
#endif


#endif
