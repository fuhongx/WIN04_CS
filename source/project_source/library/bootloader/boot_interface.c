/**
 * ****************************************************************************
 * @file boot_interface.c
 * @author SLC(SC) software team. 
 * @brief 
 * @version  1.0
 * ****************************************************************************
 * 
 * Copyright (c) 2024 SLC Semiconductor all rights reserved.
 * 
 * ****************************************************************************
 */

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "reg_Dev.h"
#include "error_def.h"
#include "hw_gpio.h"
#include "hw_flash.h"
#include "hw_uart.h"
#include "hw_dma.h"
#include "hw_sysctrl.h"

#include "utility.h"
#include "boot_interface.h"
#include "app_cfg.h"

#define DOWN_BIN_GPIO       (GPIO_PIN18)

void boot_interface_gpio_init(void)
{
    rom_hw_sysctrl_enable_clock_gate(EN_SYSCTRL_GPIO, true);

    rom_hw_gpio_set_output_mode(DOWN_BIN_GPIO, EN_GPIO_PIN_MODE_INPUT);
    rom_hw_gpio_enable_pull_mode(DOWN_BIN_GPIO, true);

    rom_hw_gpio_enable_iomux(DOWN_BIN_GPIO, false);
}

bool boot_interface_need_down_bin(void)
{
    uint32_t u32Level = 0;
    bool u8need_upgrade = 0;
    rom_hw_gpio_get_input_level(DOWN_BIN_GPIO, &u32Level);

    u8need_upgrade = (u32Level == 0) ? true : false;

    return u8need_upgrade;
}


void boot_interface_uart_init(void)
{
    // RC50M此时未校准，使用外部晶振更稳定
    rom_hw_sysctrl_system_clock_init(EN_SYS_CLK_TCXO25M, EN_SYS_CLK_MCLK_DIV_NONE);

    // Clock init
    if(UART0 == BOOT_UART_UPDATE_HANDLE)
    {
        rom_hw_sysctrl_enable_clock_gate(EN_SYSCTRL_UART0, true);
    }
    else
    {
        rom_hw_sysctrl_enable_clock_gate(EN_SYSCTRL_UART1, true);
    }

    // GPIO INIT
    rom_hw_gpio_enable_iomux(BOOT_UART_UPDATE_TX_PIN | BOOT_UART_UPDATE_RX_PIN, true);
    rom_hw_gpio_set_iomux_mode(BOOT_UART_UPDATE_TX_PIN | BOOT_UART_UPDATE_RX_PIN, BOOT_UART_GPIO_IOMUX_MODE);

    // Uart PIN init
    stUartInit_t stInit;

    stInit.stFifo.bFifoEn = true;
    stInit.stFifo.enTxFifoThld = EN_UART_TX_FIFO_THLD_EMPTY;
    stInit.stFifo.enRxFifoThld = EN_UART_RX_FIFO_THLD_14BYTES;

    stInit.u32BaudRate = BOOT_UART_UPDATE_BAUDRATE;
    stInit.unMode.u32Cfg = UART_MODE_CONFIG_DEFAULT(EN_UART_DATA_LEN_8BIT, EN_UART_STOP_1BIT);

    rom_hw_uart_init(BOOT_UART_UPDATE_HANDLE, &stInit);
}

uint16_t boot_interface_get_rx_data(uint8_t *pu8Buffer)
{
    uint16_t u16RxNum = BOOT_TRX_BUFFER_SIZE;

    rom_hw_uart_recieve_bytes(BOOT_UART_UPDATE_HANDLE, pu8Buffer, &u16RxNum, 5000);

    return u16RxNum;
}

void boot_interface_send_data(uint8_t *pu8Buffer, uint16_t u16Len)
{
    rom_hw_uart_transmit_bytes(BOOT_UART_UPDATE_HANDLE, pu8Buffer, u16Len, UART_TRANSMIT_TIMEOUT_US);
}

void boot_interface_change_baudrate(uint32_t u32Baud)
{
    rom_hw_uart_set_baudrate(BOOT_UART_UPDATE_HANDLE, u32Baud);
}
