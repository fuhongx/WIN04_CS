/**
 * ****************************************************************************
 * @file reg_rtc.h
 * @author SLC(SC) software team. 
 * @brief 
 * @version  1.0
 * ****************************************************************************
 * 
 * Copyright (c) 2024 SLC Semiconductor all rights reserved.
 * 
 * ****************************************************************************
 */

#ifndef __REG_RTC_H__
#define __REG_RTC_H__

typedef struct
{
    __IOM uint32_t RTC_RCR;               // 0x00
    __IOM uint32_t RTC_PVR;               // 0x04
    __IOM uint32_t RTC_SVR;         // 0x08
    __IOM uint32_t RTC_STR;           // 0x0C
    __IOM uint32_t RTC_SDR;           // 0x10
    __OM uint32_t  RTC_SER;             // 0x14
    __IOM uint32_t RTC_ACR;              // 0x18
    __IOM uint32_t RTC_ASR;           // 0x1C
    __IOM uint32_t RTC_ATR;             // 0x20
    __IOM uint32_t RTC_ADR;             // 0x24
    __IOM uint32_t RTC_BSR;           // 0x28
    __IOM uint32_t RTC_BTR;             // 0x2C
    __IOM uint32_t RTC_BDR;             // 0x30
    __IM uint32_t RTC_CSR;             // 0x34
    __IM uint32_t RTC_CTR;               // 0x38
    __IM uint32_t RTC_CDR;               // 0x3C
    __IOM uint32_t RTC_IER;                // 0x40
    __IOM uint32_t RTC_ISR;               // 0x44
}stRtcHandle_t, *pstRtcHandle_t;

//------------------------------------------------------------------------------
//STOP EN
//------------------------------------------------------------------------------
#define RTC_RCR_CMD_STOP                    (0x1234ABCD)
#define RTC_RCR_CMD_START                   (0xABCD1234)
#define RTC_RCR_REG_MASK                    (0xFFFFFFFF)

//------------------------------------------------------------------------------
//CMR
//------------------------------------------------------------------------------
#define RTC_PVR_DEFAULT_VAL                 (31)
#define RTC_PVR_REG_MASK                    (0xFF)

//------------------------------------------------------------------------------
//RTC_SVR
//------------------------------------------------------------------------------
#define RTC_SVR_DEFAULT_VAL           (1023)
#define RTC_SVR_REG_MASK              (0xFFFF)

//------------------------------------------------------------------------------
//CCR
//------------------------------------------------------------------------------
//BIT[0:7]
#define RTC_STR_SECOND_MASK             (0xFF)
#define RTC_STR_SECOND_SHIFT            (0)

//BIT[8:15]
#define RTC_STR_MINUTE_MASK             (0xFF)
#define RTC_STR_MINUTE_SHIFT            (8)

//BIT[16:23]
#define RTC_STR_HOUR_MASK               (0xFF)
#define RTC_STR_HOUR_SHIFT              (16)

//BIT[24:27]
#define RTC_STR_WEEK_MASK               (0xF)
#define RTC_STR_WEEK_SHIFT              (24)

#define RTC_STR_REG_MASK                ((RTC_STR_SECOND_MASK << RTC_STR_SECOND_SHIFT)|\
                                                (RTC_STR_MINUTE_MASK << RTC_STR_MINUTE_SHIFT)|\
                                                (RTC_STR_HOUR_MASK << RTC_STR_HOUR_SHIFT)|\
                                                (RTC_STR_WEEK_MASK << RTC_STR_WEEK_SHIFT))


//------------------------------------------------------------------------------
//RTC_SDR
//------------------------------------------------------------------------------
//BIT[0:7]
#define RTC_SDR_DAY_MASK                (0xFF)
#define RTC_SDR_DAY_SHIFT               (0)

//BIT[8:15]
#define RTC_SDR_MONTH_MASK              (0xFF)
#define RTC_SDR_MONTH_SHIFT             (8)

//BIT[16:31]
#define RTC_SDR_YEAR_MASK               (0xFFFF)
#define RTC_SDR_YEAR_SHIFT              (16)


#define RTC_SDR_REG_MASK                ((RTC_SDR_DAY_MASK << RTC_SDR_DAY_SHIFT)|\
                                                (RTC_SDR_MONTH_MASK << RTC_SDR_MONTH_SHIFT)|\
                                                (RTC_SDR_YEAR_MASK << RTC_SDR_YEAR_SHIFT))

//------------------------------------------------------------------------------
//RTC_SER
//------------------------------------------------------------------------------
#define RTC_TIRG_SYNC_REG_MASK                  (0x1)

//------------------------------------------------------------------------------
//RTC_ACR
//------------------------------------------------------------------------------
#define RTC_ACR_SUB_SECOND_MASK_SHIT        (0)
#define RTC_ACR_SECOND_MASK_SHIT            (1)
#define RTC_ACR_MINUTE_MASK_SHIT            (2)
#define RTC_ACR_HOUR_MASK_SHIT              (3)
#define RTC_ACR_WEEK_MASK_SHIT              (4)
#define RTC_ACR_DAY_MASK_SHIT               (5)
#define RTC_ACR_MONTH_MASK_SHIT             (6)
#define RTC_ACR_YEAR_MASK_SHIT              (7)

//BIT[0:7]
#define RTC_ACR_A_MASK                     (0xFF)
#define RTC_ACR_A_SHIFT                    (0)

//BIT[8:15]
#define RTC_ACR_B_MASK                     (0xFF)
#define RTC_ACR_B_SHIFT                    (8)

#define RTC_ACR_REG_MASK                   ((RTC_ACR_A_MASK << RTC_ACR_A_SHIFT)|\
                                                (RTC_ACR_B_MASK << RTC_ACR_B_SHIFT))

//------------------------------------------------------------------------------
//RTC_ASR
//------------------------------------------------------------------------------
#define RTC_ASR_REG_MASK                (0xFFFF)

//------------------------------------------------------------------------------
//RTC_ATR
//------------------------------------------------------------------------------
#define RTC_ATR_REG_MASK                  (RTC_STR_REG_MASK)

//------------------------------------------------------------------------------
//RTC_ADR
//------------------------------------------------------------------------------
#define RTC_ADR_REG_MASK                  (RTC_STR_REG_MASK)

//------------------------------------------------------------------------------
//RTC_BSR
//------------------------------------------------------------------------------
#define RTC_BSR_REG_MASK                (0xFFFF)

//------------------------------------------------------------------------------
//RTC_BTR
//------------------------------------------------------------------------------
#define RTC_BTR_REG_MASK                  (RTC_STR_REG_MASK)

//------------------------------------------------------------------------------
//RTC_BDR
//------------------------------------------------------------------------------
#define RTC_BDR_REG_MASK                  (RTC_STR_REG_MASK)

//------------------------------------------------------------------------------
//RTC_CSR
//------------------------------------------------------------------------------
#define RTC_CSR_REG_MASK                  (0xFFFF)

//------------------------------------------------------------------------------
//RTC_CTR
//------------------------------------------------------------------------------
#define RTC_CTR_REG_MASK                    (RTC_STR_REG_MASK)

//------------------------------------------------------------------------------
//RTC_CDR
//------------------------------------------------------------------------------
#define RTC_CDR_REG_MASK                    (RTC_STR_REG_MASK)

//------------------------------------------------------------------------------
//RTC_IER
//------------------------------------------------------------------------------
//BIT[0] 
#define RTC_IER_A_MASK                       (0x1)
#define RTC_IER_A_SHIFT                      (0)

//BIT[1]
#define RTC_IER_B_MASK                       (0x1)
#define RTC_IER_B_SHIFT                      (1)

#define RTC_IER_REG_MASK                     ((RTC_IER_A_MASK << RTC_IER_A_SHIFT)|\
                                             (RTC_IER_B_MASK << RTC_IER_B_SHIFT))

//------------------------------------------------------------------------------
//RTC_ISR
//------------------------------------------------------------------------------
//BIT[0] 
#define RTC_ISR_A_MASK                       (0x1)
#define RTC_ISR_A_SHIFT                      (0)

//BIT[1]
#define RTC_ISR_B_MASK                       (0x1)
#define RTC_ISR_B_SHIFT                      (1)

#define RTC_ISR_REG_MASK                     ((RTC_ISR_A_MASK << RTC_ISR_A_SHIFT)|\
                                                 (RTC_ISR_B_MASK << RTC_ISR_B_SHIFT))


#endif
