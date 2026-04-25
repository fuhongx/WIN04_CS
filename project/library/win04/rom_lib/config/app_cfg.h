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
#define DEBUG_UART_TX_PIN                   (GPIO_PIN0)
#define DEBUG_UART_RX_PIN                   (GPIO_PIN1)

#define DEBUG_UART_HANDLE                   (UART0)
#define DUBUG_UART_CLK_GATE                 (EN_SYSCTRL_UART0)
#define DEBUG_UART_BAUDRATE                 (921600)

#define APP_DEBUG_ENABLED                   (0)

#if APP_DEBUG_ENABLED
    #define PRINTF(fmt, args...)            printf(fmt, ##args)
#else
    #define PRINTF(fmt, args...)
#endif


#endif