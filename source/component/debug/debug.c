/**
 * ****************************************************************************
 * @file debug.c
 * @author SLC software team.
 * @brief
 * @version  0.1!!!!!!!!!!!!!!
 * ****************************************************************************
 *
 * Copyright (c) 2024 SLC Semiconductor all rights reserved.
 *
 * ****************************************************************************
 */

// This file is old driver or temp use things.
// It will be modified in future.
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#include "reg_Dev.h"
#include "error_def.h"
#include "slc_hal_gpio.h"
#include "slc_hal_sysctrl.h"
#include "slc_hal_uart.h"
#include "app_cfg.h"
#include "utility.h"

void debug_uart_init(hal_uart_id_e uart_id, uint32_t u32BaudRate)
{
    // Clock init
    if (uart_id == HAL_UART0) {
        slc_hal_sysctrl_peripheral_clk_enable(HAL_CLK_UART0, true);
        slc_hal_sysctrl_peripheral_mod_reset(HAL_CLK_UART0);
    } else {
        slc_hal_sysctrl_peripheral_clk_enable(HAL_CLK_UART1, true);
        slc_hal_sysctrl_peripheral_mod_reset(HAL_CLK_UART1);
    }

    hal_uart_init_t config = {0};

    config.baudrate = u32BaudRate;
    config.parity = HAL_UART_PARITY_NONE;
    config.stopbit = HAL_UART_STOPBIT_1;
    config.databits = HAL_UART_DATA_8BIT;

    config.tx_fifo_thld = HAL_UART_TXFIFO_THLD_EMPTY;
    config.rx_fifo_thld = HAL_UART_RXFIFO_THLD_1BYTE;
    config.fifo_en = true;
    config.flow_ctrl_en = false;

    slc_hal_uart_init(uart_id, &config);
}

void debug_printf_init(void)
{
    slc_hal_gpio_set_iomux(DEBUG_UART_TX_PIN, DEBUG_UART_IOMUX);
    slc_hal_gpio_set_iomux(DEBUG_UART_RX_PIN, DEBUG_UART_IOMUX);

    debug_uart_init(DEBUG_UART_HANDLE, DEBUG_UART_BAUDRATE);
}

void HardFault_Handler_Proc(uint32_t u32ExcReturn, uint32_t u32Msp, uint32_t u32Psp)
{
#if APP_DEBUG_ENABLED
#ifndef BOOT2PROJECT
    PRINTF("\n[HardFault_Handler] Current SP: 0x%08X\n", __get_MSP());

    volatile uint32_t *u32Sp = NULL;

    if (u32ExcReturn & 0x08)
    {
        PRINTF("Occured in task, ");
    }
    else
    {
        PRINTF("Occured in interrupt, ");
    }

    if (u32ExcReturn & 0x04)
    {
        PRINTF("use PSP.\n");
        u32Sp = (volatile uint32_t *)u32Psp;
    }
    else
    {
        PRINTF("use MSP.\n");
        u32Sp = (volatile uint32_t *)u32Msp;
    }

    PRINTF("Stack frame:\n");
    PRINTF("R0  : 0x%08X\n", u32Sp[0]);
    PRINTF("R1  : 0x%08X\n", u32Sp[1]);
    PRINTF("R2  : 0x%08X\n", u32Sp[2]);
    PRINTF("R3  : 0x%08X\n", u32Sp[3]);
    PRINTF("R12 : 0x%08X\n", u32Sp[4]);
    PRINTF("LR  : 0x%08X\n", u32Sp[5]);
    PRINTF("PC  : 0x%08X\n", u32Sp[6]);
    PRINTF("xPSR: 0x%08X\n", u32Sp[7]);
#endif
#endif

    while(1);
}

#if APP_DEBUG_ENABLED
#if Coremark
__attribute__((used))
#endif
int
fputc(int ch, FILE *f)
{
    uint8_t u8Char = ch & 0xff;

    slc_hal_uart_send_data(DEBUG_UART_HANDLE, &u8Char, 1, HAL_UART_TIMEOUT_US);

    return 0;
}

#ifndef BOOT2PROJECT
#include <stdarg.h>
void debug_print(const char *fmt, ...)
{
    char buf[256];
    int size;

    va_list args;

    va_start(args, fmt);
    size = vsnprintf((char *)buf, sizeof(buf), fmt, args);
    va_end(args);

    slc_hal_uart_send_data(DEBUG_UART_HANDLE, (uint8_t *)buf, size, HAL_UART_TIMEOUT_US);
}
#endif

#endif
