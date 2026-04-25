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

//DEBUG Config
#ifdef QMX_FPGA
#define DEBUG_UART_TX_PIN                   (HAL_GPIO_PIN0)
#define DEBUG_UART_RX_PIN                   (HAL_GPIO_PIN1)
#define DEBUG_UART_IOMUX                    (HAL_IOMUX_MODE0)
#else
#define DEBUG_UART_TX_PIN                   (HAL_GPIO_PIN12)
#define DEBUG_UART_RX_PIN                   (HAL_GPIO_PIN13)
#define DEBUG_UART_IOMUX                    (HAL_IOMUX_MODE0)
#endif

#define DEBUG_UART_HANDLE                   (HAL_UART0)
#define DEBUG_UART_IRQ                      (UART0_IRQ)
#define DUBUG_UART_CLK_GATE                 (HAL_CLK_UART0)
#define DEBUG_UART_BAUDRATE                 (921600)

#ifdef QMX_AUTOTEST
#define APP_DEBUG_ENABLED                   (0)
#else
#define APP_DEBUG_ENABLED                   (1)
#endif

#if APP_DEBUG_ENABLED
    extern void debug_print(const char *fmt, ...);
    #define PRINTF(fmt, args...)            debug_print(fmt, ##args)
#else
    #define PRINTF(fmt, args...)
#endif


#endif
