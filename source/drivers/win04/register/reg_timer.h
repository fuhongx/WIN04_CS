/**
 * ****************************************************************************
 * @file reg_timer.h
 * @author SLC(SC) software team.
 * @brief
 * @version  1.0
 * ****************************************************************************
 *
 * THIS CODE IS LIMITED TO GUIDE APPLICATION DEVELOPMENT AND INCREASE DEVELOPMENT SPEED.
 * AS A RESULT, SILICONDUCTOR IS NOT RESPONSIBLE FOR ANY CLAIMS ARISING DIRECTLY OR INDIRECTLY FROM SUCH FIRMWARE AND/OR CONTENT.
 *
 * Copyright (c) 2025 Siliconductor Semiconductor all rights reserved.
 *
 * ****************************************************************************
 */

#ifndef __REG_TIMER_H__
#define __REG_TIMER_H__

typedef struct
{
    __IOM uint32_t TIM_SCR;
    __IOM uint32_t TIM_RDR;
    __IOM uint32_t TIM_CDR;
    __IOM uint32_t TIM_IER;
    __IOM uint32_t TIM_IMR;
    __IOM uint32_t TIM_ISR;
}stTimerHandle_t, *pstTimerHandle_t;

typedef struct
{
    __IOM uint32_t TIM123_SCR;
    __IOM uint32_t TIM1_RDR;
    __IOM uint32_t TIM1_CDR;
    __IOM uint32_t PWM1_CCR;
    __IOM uint32_t TIM2_RDR;
    __IOM uint32_t TIM2_CDR;
    __IOM uint32_t PWM2_CCR;
    __IOM uint32_t TIM3_RDR;
    __IOM uint32_t TIM3_CDR;
    __IOM uint32_t PWM3_CCR;
    __IOM uint32_t TIM123_IER;
    __IOM uint32_t TIM123_IMR;
    __IOM uint32_t TIM123_ISR;
}stPwmTimerHandle_t, *pstPwmTimerHandle_t;

#define IS_VALID_NORMAL_TIMER(handle)   ((handle == TIM0) | (handle == TIM1) | (handle == TIM2) | (handle == LPTIM0) | (handle == LPTIM1))

#define IS_VALID_PWM_TIMER(handle)      ((handle == TIM3))


//------------------------------------------------------------------------------
//TIM_SCR
//------------------------------------------------------------------------------
#define TIM_SCR_REG_MASK                    (0x1)

//------------------------------------------------------------------------------
//TIM_SCR
//------------------------------------------------------------------------------
#define TIM_RDR_REG_MASK                    (0xFFFFFFFF)

//------------------------------------------------------------------------------
//TIM_CDR
//------------------------------------------------------------------------------
#define TIM_CDR_REG_MASK                    (0xFFFFFFFF)

//------------------------------------------------------------------------------
//TIM_IER
//------------------------------------------------------------------------------
#define TIM_IER_REG_MASK                    (0x1)

//------------------------------------------------------------------------------
//TIM_IMR
//------------------------------------------------------------------------------
#define TIM_IMR_REG_MASK                    (0x1)

//------------------------------------------------------------------------------
//TIM_ISR
//------------------------------------------------------------------------------
#define TIM_ISR_REG_MASK                    (0x1)

//------------------------------------------------------------------------------
//TIM123_SCR
//------------------------------------------------------------------------------
//BIT[0]
#define TIM123_SCR_TIM1_EN_MASK             (0x1)
#define TIM123_SCR_TIM1_EN_SHIFT            (0)

//BIT[1]
#define TIM123_SCR_PWM1_EN_MASK             (0x1)
#define TIM123_SCR_PWM1_EN_SHIFT            (1)

//BIT[2]
#define TIM123_SCR_PWM1_POL_CTRL_MASK       (0x1)
#define TIM123_SCR_PWM1_POL_CTRL_SHIFT      (2)

//BIT[3]
#define TIM123_SCR_TIM2_EN_MASK             (0x1)
#define TIM123_SCR_TIM2_EN_SHIFT            (3)

//BIT[4]
#define TIM123_SCR_PWM2_EN_MASK             (0x1)
#define TIM123_SCR_PWM2_EN_SHIFT            (4)

//BIT[5]
#define TIM123_SCR_PWM2_POL_CTRL_MASK       (0x1)
#define TIM123_SCR_PWM2_POL_CTRL_SHIFT      (5)

//BIT[6]
#define TIM123_SCR_TIM3_EN_MASK             (0x1)
#define TIM123_SCR_TIM3_EN_SHIFT            (6)

//BIT[7]
#define TIM123_SCR_PWM3_EN_MASK             (0x1)
#define TIM123_SCR_PWM3_EN_SHIFT            (7)

//BIT[8]
#define TIM123_SCR_PWM3_POL_CTRL_MASK       (0x1)
#define TIM123_SCR_PWM3_POL_CTRL_SHIFT      (8)

#define TIME123_SCR_CHANNLE_EN_MASK     ((TIM123_SCR_TIM1_EN_MASK << TIM123_SCR_TIM1_EN_SHIFT)|\
                                        (TIM123_SCR_TIM2_EN_MASK << TIM123_SCR_TIM2_EN_SHIFT)|\
                                        (TIM123_SCR_TIM3_EN_MASK << TIM123_SCR_TIM3_EN_SHIFT))

#define TIM123_SCR_REG_MASK             ((TIM123_SCR_TIM1_EN_MASK << TIM123_SCR_TIM1_EN_SHIFT)|\
                                        (TIM123_SCR_PWM1_EN_MASK << TIM123_SCR_PWM1_EN_SHIFT)|\
                                        (TIM123_SCR_PWM1_POL_CTRL_MASK << TIM123_SCR_PWM1_POL_CTRL_SHIFT)|\
                                        (TIM123_SCR_TIM2_EN_MASK << TIM123_SCR_TIM2_EN_SHIFT)|\
                                        (TIM123_SCR_PWM2_EN_MASK << TIM123_SCR_PWM2_EN_SHIFT)|\
                                        (TIM123_SCR_PWM2_POL_CTRL_MASK << TIM123_SCR_PWM2_POL_CTRL_SHIFT)|\
                                        (TIM123_SCR_TIM3_EN_MASK << TIM123_SCR_TIM3_EN_SHIFT)|\
                                        (TIM123_SCR_PWM3_EN_MASK << TIM123_SCR_PWM3_EN_SHIFT)|\
                                        (TIM123_SCR_PWM3_POL_CTRL_MASK << TIM123_SCR_PWM3_POL_CTRL_SHIFT))


//------------------------------------------------------------------------------
//TIM1_RDR
//------------------------------------------------------------------------------
#define TIM1_RDR_REG_MASK               (0xFFFFFFFF)

//------------------------------------------------------------------------------
//TIM1_CDR
//------------------------------------------------------------------------------
#define TIM1_CDR_REG_MASK               (0xFFFFFFFF)

//------------------------------------------------------------------------------
//PWM1_CCR
//------------------------------------------------------------------------------
#define PWM1_CCR_REG_MASK               (0xFFFFFFFF)

//------------------------------------------------------------------------------
//TIM2_RDR
//------------------------------------------------------------------------------
#define TIM2_RDR_REG_MASK               (0xFFFFFFFF)

//------------------------------------------------------------------------------
//TIM2_CDR
//------------------------------------------------------------------------------
#define TIM2_CDR_REG_MASK               (0xFFFFFFFF)

//------------------------------------------------------------------------------
//PWM2_CCR
//------------------------------------------------------------------------------
#define PWM2_CCR_REG_MASK               (0xFFFFFFFF)

//------------------------------------------------------------------------------
//TIM3_RDR
//------------------------------------------------------------------------------
#define TIM3_RDR_REG_MASK               (0xFFFFFFFF)

//------------------------------------------------------------------------------
//TIM3_CDR
//------------------------------------------------------------------------------
#define TIM3_CDR_REG_MASK               (0xFFFFFFFF)

//------------------------------------------------------------------------------
//PWM3_CCR
//------------------------------------------------------------------------------
#define PWM3_CCR_REG_MASK               (0xFFFFFFFF)

//------------------------------------------------------------------------------
//TIM123_IER
//------------------------------------------------------------------------------
//BIT[0]
#define TIM123_IER_TIM3_1_INT_EN_MASK       (0x1)
#define TIM123_IER_TIM3_1_INT_EN_SHIFT      (0)

//BIT[1]
#define TIM123_IER_TIM3_2_INT_EN_MASK       (0x1)
#define TIM123_IER_TIM3_2_INT_EN_SHIFT      (1)

//BIT[2]
#define TIM123_IER_TIM3_3_INT_EN_MASK       (0x1)
#define TIM123_IER_TIM3_3_INT_EN_SHIFT      (2)


#define TIM123_IER_REG_MASK             ((TIM123_IER_TIM3_1_INT_EN_MASK << TIM123_IER_TIM3_1_INT_EN_SHIFT)|\
                                        (TIM123_IER_TIM3_2_INT_EN_MASK << TIM123_IER_TIM3_2_INT_EN_SHIFT)|\
                                        (TIM123_IER_TIM3_3_INT_EN_MASK << TIM123_IER_TIM3_3_INT_EN_SHIFT))

//------------------------------------------------------------------------------
//TIM123_IMR
//------------------------------------------------------------------------------

#define TIM123_IMR_REG_MASK         (TIM123_IER_REG_MASK)


//------------------------------------------------------------------------------
//TIM123_ISR
//------------------------------------------------------------------------------
#define TIM123_ISR_REG_MASK         (TIM123_IER_REG_MASK)









#endif

