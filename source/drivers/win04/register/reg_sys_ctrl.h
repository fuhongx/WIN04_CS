/**
 * ****************************************************************************
 * @file reg_sys_ctrl.h
 * @author SLC(SC) software team. 
 * @brief 
 * @version  1.0
 * ****************************************************************************
 * 
 * Copyright (c) 2024 SLC Semiconductor all rights reserved.
 * 
 * ****************************************************************************
 */

#ifndef __REG_SYS_CTRL_H__
#define __REG_SYS_CTRL_H__

//#define     __IM     volatile const      /*! Defines 'read only' structure member permissions */
//#define     __OM     volatile            /*! Defines 'write only' structure member permissions */ 
//#define     __IOM    volatile            /*! Defines 'read / write' structure member permissions */

typedef struct
{
    __IOM uint32_t MCLK_CFG;                // 0x00
    __IOM uint32_t SYS_CLK_CFG;             // 0x04
    __IOM uint32_t PER_CLK_CFG;             // 0x08
    __IOM uint32_t AHB_CLK_CFG;             // 0x0C
    __IOM uint32_t APB0_CLK_CFG;            // 0x10
    __IOM uint32_t AHB_SW_RST;              // 0x14
    __IOM uint32_t APB0_SW_RST;             // 0x18
    __IOM uint32_t PHY_SW_RST;              // 0x1C
    __IOM uint32_t LP_CLK_SEL;              // 0x20
    __IOM uint32_t LP_CLK_CFG;              // 0x24
    __IOM uint32_t LP_RST_CFG;              // 0x28
    __IOM uint32_t LP_CDC_ICR;              // 0x2C
    __IOM uint32_t DEBUG_CTRL;              // 0x30
    __IOM uint32_t Res1;                    // 0x34
    __IOM uint32_t SEC_CTRL;                // 0x38
    __IOM uint32_t SEC_USRKEY;              // 0x3C
    __IOM uint32_t CACHE_CTRL;              // 0x40
    __IOM uint32_t CACHE_TOTAL_REQ;         // 0x44
    __IOM uint32_t CACHE_HIT_CNT;           // 0x48
    __IOM uint32_t CACHE_PREFETCH_CNT;      // 0x4C
    __IOM uint32_t CACHE_DOWN_REQ;          // 0x50
    __IOM uint32_t CHIP_ID;                 // 0x54
}stSysCtrlHandle_t, *pstSysCtrlHandle_t;


//------------------------------------------------------------------------------
//MCLK_CFG
//------------------------------------------------------------------------------
//BIT[0]
#define SYSCTRL_MCLK_CFG_CLK_SRC_SEL_MAKS           (0x1)
#define SYSCTRL_MCLK_CFG_CLK_SRC_SEL_SHIFT          (0)

#define SYSCTRL_MCLK_CFG_REG_MASK           (SYSCTRL_MCLK_CFG_CLK_SRC_SEL_MAKS << SYSCTRL_MCLK_CFG_CLK_SRC_SEL_SHIFT)

//------------------------------------------------------------------------------
//SYS_CLK_CFG
//------------------------------------------------------------------------------
//BIT[0:3]
#define SYSCTRL_SYS_CLK_CFG_SYS_CLK_DIV_MASK        (0xF)
#define SYSCTRL_SYS_CLK_CFG_SYS_CLK_DIV_SHIFT       (0)

//BIT[8:10]
#define SYSCTRL_SYS_CLK_CFG_FLASH_CLK_DIV_MASK      (0x7)
#define SYSCTRL_SYS_CLK_CFG_FLASH_CLK_DIV_SHIFT     (8)



#define SYSCTRL_SYS_CLK_CFG_REG_MASK        ((SYSCTRL_SYS_CLK_CFG_SYS_CLK_DIV_MASK << SYSCTRL_SYS_CLK_CFG_SYS_CLK_DIV_SHIFT)|\
                                            (SYSCTRL_SYS_CLK_CFG_FLASH_CLK_DIV_MASK << SYSCTRL_SYS_CLK_CFG_FLASH_CLK_DIV_SHIFT))

//------------------------------------------------------------------------------
//PER_CLK_CFG
//------------------------------------------------------------------------------
//BIT[0:2]
#define SYSCTRL_PER_CLK_CFG_AHB_CLK_DIV_MASK        (0x7)
#define SYSCTRL_PER_CLK_CFG_AHB_CLK_DIV_SHIFT       (0)

//BIT[8:10]
#define SYSCTRL_PER_CLK_CFG_APB0_CLK_DIV_MASK       (0x7)
#define SYSCTRL_PER_CLK_CFG_APB0_CLK_DIV_SHIFT      (8)

#define SYSCTRL_PER_CLK_CFG_REG_MASK        ((SYSCTRL_PER_CLK_CFG_AHB_CLK_DIV_MASK << SYSCTRL_PER_CLK_CFG_AHB_CLK_DIV_SHIFT)|\
                                            (SYSCTRL_PER_CLK_CFG_APB0_CLK_DIV_MASK << SYSCTRL_PER_CLK_CFG_APB0_CLK_DIV_SHIFT))

//------------------------------------------------------------------------------
//AHB_CLK_CFG
//------------------------------------------------------------------------------
#define SYSCTRL_AHB_CLK_EN_REG_LEN                  (6)
//BIT[0] 
#define SYSCTRL_AHB_CLK_EN_DMA_EN_MASK              (0x1)
#define SYSCTRL_AHB_CLK_EN_DMA_EN_SHIFT             (0)

//BIT[1]
#define SYSCTRL_AHB_CLK_EN_RAND_EN_MASK             (0x1)
#define SYSCTRL_AHB_CLK_EN_RAND_EN_SHIFT            (1)

//BIT[2]
#define SYSCTRL_AHB_CLK_EN_CRC16_EN_MASK            (0x1)
#define SYSCTRL_AHB_CLK_EN_CRC16_EN_SHIFT           (2)

//BIT[3]
#define SYSCTRL_AHB_CLK_EN_CRC24_EN_MASK            (0x1)
#define SYSCTRL_AHB_CLK_EN_CRC24_EN_SHIFT           (3)

//BIT[4]
#define SYSCTRL_AHB_CLK_EN_CRC32_EN_MASK            (0x1)
#define SYSCTRL_AHB_CLK_EN_CRC32_EN_SHIFT           (4)

//BIT[5]
#define SYSCTRL_AHB_CLK_EN_AES128_EN_MASK           (0x1)
#define SYSCTRL_AHB_CLK_EN_AES128_EN_SHIFT          (5)

#define SYSCTRL_AHB_CLK_EN_REG_MASK         ((SYSCTRL_AHB_CLK_EN_DMA_EN_MASK << SYSCTRL_AHB_CLK_EN_DMA_EN_SHIFT)|\
                                            (SYSCTRL_AHB_CLK_EN_RAND_EN_MASK << SYSCTRL_AHB_CLK_EN_RAND_EN_SHIFT)|\
                                            (SYSCTRL_AHB_CLK_EN_CRC16_EN_MASK << SYSCTRL_AHB_CLK_EN_CRC16_EN_SHIFT)|\
                                            (SYSCTRL_AHB_CLK_EN_CRC24_EN_MASK << SYSCTRL_AHB_CLK_EN_CRC24_EN_SHIFT)|\
                                            (SYSCTRL_AHB_CLK_EN_CRC32_EN_MASK << SYSCTRL_AHB_CLK_EN_CRC32_EN_SHIFT)|\
                                            (SYSCTRL_AHB_CLK_EN_AES128_EN_MASK << SYSCTRL_AHB_CLK_EN_AES128_EN_SHIFT))




//------------------------------------------------------------------------------
//APB0_CLK_CFG
//------------------------------------------------------------------------------
#define SYSCTRL_APB0_CLK_CFG_REG_LEN                (13)
//BIT[0]
#define SYSCTRL_APB0_CLK_CFG_GPIO_EN_MASK           (0x1)
#define SYSCTRL_APB0_CLK_CFG_GPIO_EN_SHIFT          (0)

//BIT[1]
#define SYSCTRL_APB0_CLK_CFG_UART0_EN_MASK          (0x1)
#define SYSCTRL_APB0_CLK_CFG_UART0_EN_SHIFT         (1)
          
//BIT[2]
#define SYSCTRL_APB0_CLK_CFG_UART1_EN_MASK          (0x1)
#define SYSCTRL_APB0_CLK_CFG_UART1_EN_SHIFT         (2)

//BIT[3] No use in win04
// #define SYSCTRL_APB0_CLK_CFG_UART2_EN_MASK          (0x1)
// #define SYSCTRL_APB0_CLK_CFG_UART2_EN_SHIFT         (3)

//BIT[4]
#define SYSCTRL_APB0_CLK_CFG_SPI0_EN_MASK          (0x1)
#define SYSCTRL_APB0_CLK_CFG_SPI0_EN_SHIFT         (4)

//BIT[5]
#define SYSCTRL_APB0_CLK_CFG_SPI1_EN_MASK          (0x1)
#define SYSCTRL_APB0_CLK_CFG_SPI1_EN_SHIFT         (5)

//BIT[6]
#define SYSCTRL_APB0_CLK_CFG_TIM0_EN_MASK          (0x1)
#define SYSCTRL_APB0_CLK_CFG_TIM0_EN_SHIFT         (6)

//BIT[7]
#define SYSCTRL_APB0_CLK_CFG_TIM1_EN_MASK          (0x1)
#define SYSCTRL_APB0_CLK_CFG_TIM1_EN_SHIFT         (7)

//BIT[8]
#define SYSCTRL_APB0_CLK_CFG_TIM2_EN_MASK          (0x1)
#define SYSCTRL_APB0_CLK_CFG_TIM2_EN_SHIFT         (8)

//BIT[9]
#define SYSCTRL_APB0_CLK_CFG_TIM3_EN_MASK          (0x1)
#define SYSCTRL_APB0_CLK_CFG_TIM3_EN_SHIFT         (9)

//BIT[10]
#define SYSCTRL_APB0_CLK_CFG_I2C0_EN_MASK          (0x1)
#define SYSCTRL_APB0_CLK_CFG_I2C0_EN_SHIFT         (10)

//BIT[11]
#define SYSCTRL_APB0_CLK_CFG_I2C1_EN_MASK          (0x1)
#define SYSCTRL_APB0_CLK_CFG_I2C1_EN_SHIFT         (11)

//BIT[12]
#define SYSCTRL_APB0_CLK_CFG_WDT_EN_MASK           (0x1)
#define SYSCTRL_APB0_CLK_CFG_WDT_EN_SHIFT          (12)


#define SYSCTRL_APB0_CLK_CFG_REG_MASK        ((SYSCTRL_APB0_CLK_CFG_GPIO_EN_MASK << SYSCTRL_APB0_CLK_CFG_GPIO_EN_SHIFT)|\
                                             (SYSCTRL_APB0_CLK_CFG_UART0_EN_MASK << SYSCTRL_APB0_CLK_CFG_UART0_EN_SHIFT)|\
                                             (SYSCTRL_APB0_CLK_CFG_UART1_EN_MASK << SYSCTRL_APB0_CLK_CFG_UART1_EN_SHIFT)|\
                                             (SYSCTRL_APB0_CLK_CFG_SPI0_EN_MASK << SYSCTRL_APB0_CLK_CFG_SPI0_EN_SHIFT)|\
                                             (SYSCTRL_APB0_CLK_CFG_SPI1_EN_MASK << SYSCTRL_APB0_CLK_CFG_SPI1_EN_SHIFT)|\
                                             (SYSCTRL_APB0_CLK_CFG_TIM0_EN_MASK << SYSCTRL_APB0_CLK_CFG_TIM0_EN_SHIFT)|\
                                             (SYSCTRL_APB0_CLK_CFG_TIM1_EN_MASK << SYSCTRL_APB0_CLK_CFG_TIM1_EN_SHIFT)|\
                                             (SYSCTRL_APB0_CLK_CFG_TIM2_EN_MASK << SYSCTRL_APB0_CLK_CFG_TIM2_EN_SHIFT)|\
                                             (SYSCTRL_APB0_CLK_CFG_TIM3_EN_MASK << SYSCTRL_APB0_CLK_CFG_TIM3_EN_SHIFT)|\
                                             (SYSCTRL_APB0_CLK_CFG_I2C0_EN_MASK << SYSCTRL_APB0_CLK_CFG_I2C0_EN_SHIFT)|\
                                             (SYSCTRL_APB0_CLK_CFG_I2C1_EN_MASK << SYSCTRL_APB0_CLK_CFG_I2C1_EN_SHIFT)|\
                                             (SYSCTRL_APB0_CLK_CFG_WDT_EN_MASK << SYSCTRL_APB0_CLK_CFG_WDT_EN_SHIFT))
//  (SYSCTRL_APB0_CLK_CFG_UART2_EN_MASK << SYSCTRL_APB0_CLK_CFG_UART2_EN_SHIFT)|\

//------------------------------------------------------------------------------
//AHB_SW_RST
//------------------------------------------------------------------------------
#define SYSCTRL_AHB_SW_RST_LEN                      (6)

#define SYSCTRL_AHB_SW_RST_MASK                     (0x1)
//BIT[0]
#define SYSCTRL_AHB_SW_RST_DMA_RST_SHIFT            (0)

//BIT[1]
#define SYSCTRL_AHB_SW_RST_RAND_RST_SHIFT           (1)

//BIT[2]
#define SYSCTRL_AHB_SW_RST_CRC16_RST_SHIFT          (2)

//BIT[3]
#define SYSCTRL_AHB_SW_RST_CRC24_RST_SHIFT          (3)

//BIT[4]
#define SYSCTRL_AHB_SW_RST_CRC32_RST_SHIFT          (4)

//BIT[5]
#define SYSCTRL_AHB_SW_RST_AES128_RST_SHIFT         (5)



#define SYSCTRL_AHB_SW_RST_REG_MASK         ((SYSCTRL_AHB_SW_RST_MASK << SYSCTRL_AHB_SW_RST_DMA_RST_SHIFT)|\
                                            (SYSCTRL_AHB_SW_RST_MASK << SYSCTRL_AHB_SW_RST_RAND_RST_SHIFT)|\
                                            (SYSCTRL_AHB_SW_RST_MASK << SYSCTRL_AHB_SW_RST_CRC16_RST_SHIFT)|\
                                            (SYSCTRL_AHB_SW_RST_MASK << SYSCTRL_AHB_SW_RST_CRC24_RST_SHIFT)|\
                                            (SYSCTRL_AHB_SW_RST_MASK << SYSCTRL_AHB_SW_RST_CRC32_RST_SHIFT)|\
                                            (SYSCTRL_AHB_SW_RST_MASK << SYSCTRL_AHB_SW_RST_AES128_RST_SHIFT))


//------------------------------------------------------------------------------
//APB0_SW_RST
//------------------------------------------------------------------------------
#define SYSCTRL_APB0_SW_RST_MASK                    (0x1)
#define SYSCTRL_APB0_SW_RST_REG_LEN                (13)
//BIT[0]
#define SYSCTRL_APB0_SW_RST_GPIO_RST_SHIFT          (0)

//BIT[1]
#define SYSCTRL_APB0_SW_RST_UART0_RST_SHIFT          (1)

//BIT[2]
#define SYSCTRL_APB0_SW_RST_UART1_RST_SHIFT          (2)

//BIT[3]
// #define SYSCTRL_APB0_SW_RST_UART2_RST_SHIFT          (3)

//BIT[4]
#define SYSCTRL_APB0_SW_RST_SPI0_RST_SHIFT          (4)

//BIT[5]
#define SYSCTRL_APB0_SW_RST_SPI1_RST_SHIFT          (5)

//BIT[6]
#define SYSCTRL_APB0_SW_RST_TIM0_RST_SHIFT          (6)

//BIT[7]
#define SYSCTRL_APB0_SW_RST_TIM1_RST_SHIFT          (7)

//BIT[8]
#define SYSCTRL_APB0_SW_RST_TIM2_RST_SHIFT          (8)

//BIT[9]
#define SYSCTRL_APB0_SW_RST_TIM3_RST_SHIFT          (9)

//BIT[10]
#define SYSCTRL_APB0_SW_RST_I2C0_RST_SHIFT          (10)

//BIT[11]
#define SYSCTRL_APB0_SW_RST_I2C1_RST_SHIFT          (11)

//BIT[12]
#define SYSCTRL_APB0_SW_RST_WDT_RST_SHIFT          (12)



#define SYSCTRL_APB0_SW_RST_REG_MASK            ((SYSCTRL_APB0_SW_RST_MASK << SYSCTRL_APB0_SW_RST_GPIO_RST_SHIFT)|\
                                                (SYSCTRL_APB0_SW_RST_MASK << SYSCTRL_APB0_SW_RST_UART0_RST_SHIFT)|\
                                                (SYSCTRL_APB0_SW_RST_MASK << SYSCTRL_APB0_SW_RST_UART1_RST_SHIFT)|\
                                                (SYSCTRL_APB0_SW_RST_MASK << SYSCTRL_APB0_SW_RST_SPI0_RST_SHIFT)|\
                                                (SYSCTRL_APB0_SW_RST_MASK << SYSCTRL_APB0_SW_RST_SPI1_RST_SHIFT)|\
                                                (SYSCTRL_APB0_SW_RST_MASK << SYSCTRL_APB0_SW_RST_TIM0_RST_SHIFT)|\
                                                (SYSCTRL_APB0_SW_RST_MASK << SYSCTRL_APB0_SW_RST_TIM1_RST_SHIFT)|\
                                                (SYSCTRL_APB0_SW_RST_MASK << SYSCTRL_APB0_SW_RST_TIM2_RST_SHIFT)|\
                                                (SYSCTRL_APB0_SW_RST_MASK << SYSCTRL_APB0_SW_RST_TIM3_RST_SHIFT)|\
                                                (SYSCTRL_APB0_SW_RST_MASK << SYSCTRL_APB0_SW_RST_I2C0_RST_SHIFT)|\
                                                (SYSCTRL_APB0_SW_RST_MASK << SYSCTRL_APB0_SW_RST_I2C1_RST_SHIFT)|\
                                                (SYSCTRL_APB0_SW_RST_MASK << SYSCTRL_APB0_SW_RST_WDT_RST_SHIFT))
//                                                (SYSCTRL_APB0_SW_RST_MASK << SYSCTRL_APB0_SW_RST_UART2_RST_SHIFT)|\


//------------------------------------------------------------------------------
//PHY_SW_RST
//------------------------------------------------------------------------------
#define SYSCTRL_PHY_SW_RST_REG_MASK                 (0x1)

//------------------------------------------------------------------------------
//LP_CLK_SEL
//------------------------------------------------------------------------------
//BIT[0]
#define SYSCTRL_LP_CLK_SEL_APB1_CLK_SEL_MASK        (0x1)
#define SYSCTRL_LP_CLK_SEL_APB1_CLK_SEL_SHIFT       (0)

//BIT[1]
#define SYSCTRL_LP_CLK_SEL_RTC_CLK_SEL_MASK                    (0x1)
#define SYSCTRL_LP_CLK_SEL_RTC_CLK_SEL_SHIFT                   (1)

#define SYSCTRL_LP_CLK_SEL_REG_MASK                 ((SYSCTRL_LP_CLK_SEL_APB1_CLK_SEL_MASK << SYSCTRL_LP_CLK_SEL_APB1_CLK_SEL_SHIFT)|\
                                                    (SYSCTRL_LP_CLK_SEL_RTC_CLK_SEL_MASK << SYSCTRL_LP_CLK_SEL_RTC_CLK_SEL_SHIFT))


//------------------------------------------------------------------------------
//LP_CLK_CFG
//------------------------------------------------------------------------------
//BIT[0]
#define SYSCTRL_LP_CLK_CFG_LPTIM0_EN_MASK           (0x1)
#define SYSCTRL_LP_CLK_CFG_LPTIM0_EN_SHIFT          (0)

//BIT[1]
#define SYSCTRL_LP_CLK_CFG_LPTIM1_EN_MASK           (0x1)
#define SYSCTRL_LP_CLK_CFG_LPTIM1_EN_SHIFT          (1)

//BIT[2]
#define SYSCTRL_LP_CLK_CFG_LPUART_EN_MASK           (0x1)
#define SYSCTRL_LP_CLK_CFG_LPUART_EN_SHIFT          (2)


#define SYSCTRL_LP_CLK_CFG_REG_MASK                 ((SYSCTRL_LP_CLK_CFG_LPTIM0_EN_MASK << SYSCTRL_LP_CLK_CFG_LPTIM0_EN_SHIFT)|\
                                                    (SYSCTRL_LP_CLK_CFG_LPTIM1_EN_MASK << SYSCTRL_LP_CLK_CFG_LPTIM1_EN_SHIFT)|\
                                                    (SYSCTRL_LP_CLK_CFG_LPUART_EN_MASK << SYSCTRL_LP_CLK_CFG_LPUART_EN_SHIFT))

//------------------------------------------------------------------------------
//LP_RST_CFG
//------------------------------------------------------------------------------
//BIT[0]
#define SYSCTRL_LP_RST_CFG_LPTIM0_SW_RST_MASK       (0x1)
#define SYSCTRL_LP_RST_CFG_LPTIM0_SW_RST_SHIFT      (0)

//BIT[1]
#define SYSCTRL_LP_RST_CFG_LPTIM1_SW_RST_MASK       (0x1)
#define SYSCTRL_LP_RST_CFG_LPTIM1_SW_RST_SHIFT      (1)

//BIT[2]
#define SYSCTRL_LP_RST_CFG_LPUART_SW_RST_MASK       (0x1)
#define SYSCTRL_LP_RST_CFG_LPUART_SW_RST_SHIFT      (2)

#define SYSCTRL_LP_RST_CFG_REG_MASK                 ((SYSCTRL_LP_RST_CFG_LPTIM0_SW_RST_MASK << SYSCTRL_LP_RST_CFG_LPTIM0_SW_RST_SHIFT)|\
                                                    (SYSCTRL_LP_RST_CFG_LPTIM1_SW_RST_MASK << SYSCTRL_LP_RST_CFG_LPTIM1_SW_RST_SHIFT)|\
                                                    (SYSCTRL_LP_RST_CFG_LPUART_SW_RST_MASK << SYSCTRL_LP_RST_CFG_LPUART_SW_RST_SHIFT))

//Lp CDC ICR

#define SYSCTRL_LP_CDC_ICR_CDC_INT_EN_MASK          (0x1)
#define SYSCTRL_LP_CDC_ICR_CDC_INT_EN_SHIFT          (0x4)

#define SYSCTRL_LP_CDC_ICR_CDC_ISR_MASK             (0x1)
#define SYSCTRL_LP_CDC_ICR_CDC_ISR_SHIFT            (0x5)

//------------------------------------------------------------------------------
//CACHE_CTRL
//------------------------------------------------------------------------------
//BIT[0]
#define SYSCTRL_CACHE_CTRL_BYPASS_EN_MASK           (0x1)
#define SYSCTRL_CACHE_CTRL_BYPASS_EN_SHIFT          (0)

//BIT[1]
#define SYSCTRL_CACHE_CTRL_FLUSH_EN_MASK            (0x1)
#define SYSCTRL_CACHE_CTRL_FLUSH_EN_SHIFT           (1)

#define SYSCTRL_CACHE_CTRL_REG_MASK                 ((SYSCTRL_CACHE_CTRL_BYPASS_EN_MASK << SYSCTRL_CACHE_CTRL_BYPASS_EN_SHIFT)|\
                                                    (SYSCTRL_CACHE_CTRL_FLUSH_EN_MASK << SYSCTRL_CACHE_CTRL_FLUSH_EN_SHIFT))

//------------------------------------------------------------------------------
//CACHE_TOTAL_REQ
//------------------------------------------------------------------------------
#define SYSCTRL_CACHE_TOTAL_REQ_REG_MASK            (0xFFFFFFFF)

//------------------------------------------------------------------------------
//CACHE_HIT_CNT
//------------------------------------------------------------------------------
#define SYSCTRL_CACHE_HIT_CNT_REG_MASK              (0xFFFFFFFF)

//------------------------------------------------------------------------------
//CACHE_PREFETCH_CNT
//------------------------------------------------------------------------------
#define SYSCTRL_CACHE_PREFETCH_CNT_REG_MASK              (0xFFFFFFFF)

//------------------------------------------------------------------------------
//CACHE_DOWN_REQ
//------------------------------------------------------------------------------
#define SYSCTRL_CACHE_DOWN_REQ_REG_MASK                  (0xFFFFFFFF)





#endif

