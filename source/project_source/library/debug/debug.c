/**
 * ****************************************************************************
 * @file debug.c
 * @author SLC(SC) software team.
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

#include "reg_Dev.h"
#include "stdbool.h"
#include "error_def.h"
#include "hw_gpio.h"
#include "hw_uart.h"
#include "hw_sysctrl.h"
#include "app_cfg.h"
#include "utility.h"

// Using Uart0;
void debug_uart_init(stUartHandle_t *pstHandle, uint32_t u32BaudRate)
{
    // Clock init
    if (UART0 == pstHandle) {
        rom_hw_sysctrl_enable_clock_gate(EN_SYSCTRL_UART0, true);
        rom_hw_sysctrl_reset_peripheral(EN_SYSCTRL_UART0);
    } else {
        rom_hw_sysctrl_enable_clock_gate(EN_SYSCTRL_UART1, true);
        rom_hw_sysctrl_reset_peripheral(EN_SYSCTRL_UART1);
    }

    stUartInit_t stInit;

    stInit.stFifo.bFifoEn = true;

    stInit.u32BaudRate = u32BaudRate;
    stInit.unMode.u32Cfg = UART_MODE_CONFIG_DEFAULT(EN_UART_DATA_LEN_8BIT, EN_UART_STOP_1BIT);

    rom_hw_uart_init(pstHandle, &stInit);
}

void debug_printf_init(void)
{
    rom_hw_gpio_enable_iomux(DEBUG_UART_TX_PIN | DEBUG_UART_RX_PIN, true);
    rom_hw_gpio_set_iomux_mode(DEBUG_UART_TX_PIN | DEBUG_UART_RX_PIN, EN_GPIO_IOMUX_MODE0);

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

#if CHIP_TEST
    extern volatile bool bClosePrintf;
    if (bClosePrintf)
    {
        return 0;
    }
    else
    {
        rom_hw_uart_transmit_bytes(DEBUG_UART_HANDLE, &u8Char, 1, UART_TRANSMIT_TIMEOUT_US);
    }
#else
    rom_hw_uart_transmit_bytes(DEBUG_UART_HANDLE, &u8Char, 1, UART_TRANSMIT_TIMEOUT_US);

#endif

    return 0;
}

#ifndef BOOT2PROJECT
#include "stdarg.h"
void debug_print(const char *fmt, ...)
{
    static char buf[256];
    uint16_t size;

    va_list args;

    va_start(args, fmt);
    size = vsnprintf((char *)buf, sizeof(buf), fmt, args);
    va_end(args);

    rom_hw_uart_transmit_bytes(DEBUG_UART_HANDLE, (uint8_t *)buf, size, UART_TRANSMIT_TIMEOUT_US);
}
#endif

#endif