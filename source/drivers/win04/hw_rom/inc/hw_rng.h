/**
 * ****************************************************************************
 * @file hw_rng.h
 * @author SLC(SC) software team. 
 * @brief 
 * @version  1.0
 * ****************************************************************************
 * 
 * THIS CODE IS LIMITED TO GUIDE THE APPLICATION AND IMPROVE THE EFFICIENCY OF RESEARCH AND DEVELOPMENT.
 * AS A RESULT, SILICONDUCTOR IS NOT RESPONSIBLE FOR ANY CLAIMS ARISING DIRECTLY OR INDIRECTLY FROM SUCH FIRMWARE AND/OR CONTENT.
 * 
 * Copyright (c) 2025 Siliconductor Semiconductor all rights reserved.
 * 
 * ****************************************************************************
 */

#ifndef __HW_RNG_H__
#define __HW_RNG_H__

#define RNG_ENABLE_SHIFT        (0U)
#define RNG_ENABLE_MASK         (0x1U << RNG_FLUSH_FREQ_SHIFT)

#define RNG_FLUSH_FREQ_SHIFT    (4U)
#define RNG_FLUSH_FREQ_MASK     (0xFU << RNG_FLUSH_FREQ_SHIFT)

#define RNG_DATA_STATUS_SHIFT   (0U)
#define RNG_DATA_STATUS_MASK    (0x1U << RNG_DATA_STATUS_SHIFT)

/**
 * @brief RNG flush frequency and enable config.
 *
 * @param freq_div RNG flush frequency divisor, 0~15. system_clock/(freq_div+1)
 * @param enable   RNG enable
 */
void rom_hw_rng_init(uint8_t freq_div, bool enable);

/**
 * @brief Update RNG seed.
 * 
 * @param seed  RNG seed.
 */
void rom_hw_update_rng_seed(uint32_t seed);

/**
 * @brief Get random data.
 * 
 * @param data Random data buffer pointer u32.
 * @param len  Length of random data buffer, in bytes.(need be 4 byte aligned)
 * @param timeout  Timeout value, unit us.
 * @return EN_ERR_STA_T
 */
EN_ERR_STA_T rom_hw_get_random_u32(uint32_t *data, uint32_t len, uint32_t timeout);

/**
 * @brief Get random data.
 * 
 * @param data Random data buffer pointer u8.
 * @param len  Length of random data buffer, in bytes.
 * @param timeout  Timeout value, unit us.
 * @return EN_ERR_STA_T
 */
EN_ERR_STA_T rom_hw_get_random_u8(uint8_t *data, uint32_t len, uint32_t timeout);

#endif  // __HW_RNG_H__
