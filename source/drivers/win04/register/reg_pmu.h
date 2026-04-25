/**
 * ****************************************************************************
 * @file reg_pmu.h
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

#ifndef __REG_PMU_H__
#define __REG_PMU_H__

typedef struct
{
    __IOM uint32_t  LP_MODE;
    __IOM uint32_t  PHY_PWR;
    __IOM uint32_t  WKP_MSK;
    __IOM uint32_t  WKP_STA;
    __IOM uint32_t  PWR_DWN;
    __IOM uint32_t  LP_MAN;
    __IOM uint32_t  LPIO_EN;
}stPmuHandle_t;

#define PMU_LP_MODE_NORMAL                          (0x0)
#define PMU_LP_MODE_STOP                            (0x1)
#define PMU_LP_MODE_STANDBY                         (0x2)
#define PMU_LP_MODE_SHUTDOWN                        (0x3)

#define PMU_LP_MODE_REG_MASK                        (0x3)

#define PMU_PHY_PWR_REG_MASK                        (0x1)

//------------------------------------------------------------------------------
//PMU_WKP_MSK
//------------------------------------------------------------------------------
//BIT[0]
#define PMU_WKP_MSK_WAKEUP_PHY_INT0_MASK            (0x1)
#define PMU_WKP_MSK_WAKEUP_PHY_INT0_SHIFT           (0)

//BIT[1]
#define PMU_WKP_MSK_WAKEUP_LPTIMER1_MASK           (0x1)
#define PMU_WKP_MSK_WAKEUP_LPTIMER1_SHIFT          (1)

//BIT[2]
#define PMU_WKP_MSK_WAKEUP_RTC_MASK                (0x1)
#define PMU_WKP_MSK_WAKEUP_RTC_SHIFT               (2)

//BIT[3]
#define PMU_WKP_MSK_WAKEUP_LPTIMER0_MASK           (0x1)
#define PMU_WKP_MSK_WAKEUP_LPTIMER0_SHIFT          (3)

//BIT[4]
#define PMU_WKP_MSK_WAKEUP_LPUART_MASK             (0x1)
#define PMU_WKP_MSK_WAKEUP_LPUART_SHIFT            (4)

//BIT[5]
#define PMU_WKP_MSK_WAKEUP_IWDT_MASK               (0x1)
#define PMU_WKP_MSK_WAKEUP_IWDT_SHIFT              (5)

//BIT[6]
#define PMU_WKP_MSK_WAKEUP_LPIO_MASK                (0x1)
#define PMU_WKP_MSK_WAKEUP_LPIO_SHIFT               (6)

//BIT[7]
#define PMU_WKP_MSK_WAKEUP_RF_MASK                 (0x1)
#define PMU_WKP_MSK_WAKEUP_RF_SHIFT                (7)



#define PMU_WKP_MSK_REG_MASK           ((PMU_WKP_MSK_WAKEUP_PHY_INT0_MASK << PMU_WKP_MSK_WAKEUP_PHY_INT0_SHIFT)|\
                                        (PMU_WKP_MSK_WAKEUP_LPTIMER1_MASK << PMU_WKP_MSK_WAKEUP_LPTIMER1_SHIFT)|\
                                        (PMU_WKP_MSK_WAKEUP_RTC_MASK << PMU_WKP_MSK_WAKEUP_RTC_SHIFT)|\
                                        (PMU_WKP_MSK_WAKEUP_LPTIMER0_MASK << PMU_WKP_MSK_WAKEUP_LPTIMER0_SHIFT)|\
                                        (PMU_WKP_MSK_WAKEUP_LPUART_MASK << PMU_WKP_MSK_WAKEUP_LPUART_SHIFT)|\
                                        (PMU_WKP_MSK_WAKEUP_IWDT_MASK << PMU_WKP_MSK_WAKEUP_IWDT_SHIFT)|\
                                        (PMU_WKP_MSK_WAKEUP_LPIO_MASK << PMU_WKP_MSK_WAKEUP_LPIO_SHIFT)|\
                                        (PMU_WKP_MSK_WAKEUP_RF_MASK << PMU_WKP_MSK_WAKEUP_RF_SHIFT))


#define PMU_PWR_DWN_REG_MASK            (0x1)

#define PMU_LP_MAN_REG_MASK             (0x1)

#define PMU_LPIO_EN_REG_MASK            (0x1)

#endif
