/**
 * ****************************************************************************
 * @file app_cfg.h
 * @author SLC(SC) software team. 
 * @brief 
 * @version  1.0
 * ****************************************************************************
 * 
 * Copyright (c) 2024 SLC Semiconductor all rights reserved.
 * 
 * ****************************************************************************
 */

#ifndef __APP_CFG_H__
#define __APP_CFG_H__

//#define UNIT_1K         (1024)
//#define UNIT_PAGE       (256)


//DEBUG Config
#define DEBUG_UART_TX_PIN                   (HAL_GPIO_PIN2)
#define DEBUG_UART_RX_PIN                   (HAL_GPIO_PIN3)
#define DEBUG_UART_IOMUX                    (HAL_IOMUX_MODE0)
#define DEBUG_UART_HANDLE                   (HAL_UART1)

#define DEBUG_UART_BAUDRATE                 (921600)


#define APP_DEBUG_ENABLED                   (0)

#if APP_DEBUG_ENABLED
    #define PRINTF(fmt, args...)           printf(fmt, ##args)
#else
    #define PRINTF(fmt, args...)
#endif


#endif
