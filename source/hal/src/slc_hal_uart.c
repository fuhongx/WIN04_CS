/**
 * ****************************************************************************
 * @file slc_hal_uart.c
 * @author SLC software team.
 * @brief
 * @version  1.0
 * ****************************************************************************
 *
 * THIS CODE IS LIMITED TO GUIDE THE APPLICATION AND IMPROVE THE EFFICIENCY OF RESEARCH AND DEVELOPMENT.
 * AS A RESULT, SLC IS NOT RESPONSIBLE FOR ANY CLAIMS ARISING DIRECTLY OR INDIRECTLY FROM SUCH FIRMWARE AND/OR CONTENT.
 *
 * Copyright (c) 2026 SLC Semiconductor all rights reserved.
 *
 * ****************************************************************************
 */
#include "slc_hal_sysctrl.h"
#include "slc_hal_uart.h"
#include "reg_Dev.h"
#include "utility.h"
#include "error_def.h"
#include "hw_uart.h"
#include "compiler.h"

#if HAL_UART_DMA_SUPPORT
static uint8_t g_uart_dma_txch[HAL_UART_MAX] __RETENTION_DATA = {0};
static uint8_t g_uart_dma_rxch[HAL_UART_MAX] __RETENTION_DATA = {0};
#endif  // #if HAL_UART_DMA_SUPPORT

stUartHandle_t *slc_hal_uart_get_handle(hal_uart_id_e uart)
{
    switch (uart) {
        case HAL_UART0:
            return UART0;
        case HAL_UART1:
            return UART1;
        default:
            return NULL;
    }
}

int slc_hal_uart_init(hal_uart_id_e uart, hal_uart_init_t *config)
{
    stUartInit_t Init = {0};
    stUartHandle_t *uartHandle = slc_hal_uart_get_handle(uart);

    if ((config->baudrate > (slc_hal_sysctrl_get_system_clock() / 16)) || (uartHandle == NULL))
        return -1;

    Init.unMode.stUartMode.DataLen = config->databits;
    Init.unMode.stUartMode.StopLen = config->stopbit;
    Init.unMode.stUartMode.ParityMode = (config->parity == HAL_UART_PARITY_EVEN) ? EN_UART_PARITY_EVEN : EN_UART_PARITY_ODD;
    Init.unMode.stUartMode.ParityEn = (config->parity == HAL_UART_PARITY_NONE) ? false : true;
    Init.unMode.stUartMode.DllCfgEn =  false;
    Init.stFifo.enDmaMode = EN_UART_DMA_MODE0;
    Init.stFifo.bFifoEn         =   config->fifo_en;
    Init.stFifo.bRxFifoClean    =   true;
    Init.stFifo.bTxFifoClean    =   true;
    Init.stFifo.enRxFifoThld    =   (EN_UART_RX_FIFO_THLD_T)config->rx_fifo_thld;
    Init.stFifo.enTxFifoThld    =   (EN_UART_TX_FIFO_THLD_T)config->tx_fifo_thld;
    Init.u32BaudRate = config->baudrate;

    return rom_hw_uart_init(uartHandle, &Init);
}

void slc_hal_uart_deinit(hal_uart_id_e uart)
{
    hal_peripheral_clk_e uart_clk = HAL_CLK_UART0;
    if (uart == HAL_UART0) {
        uart_clk = HAL_CLK_UART0;
    } else if (uart == HAL_UART1) {
        uart_clk = HAL_CLK_UART1;
    } else {
        return;
    }

    slc_hal_sysctrl_peripheral_mod_reset(uart_clk);
    slc_hal_sysctrl_peripheral_clk_enable(uart_clk, false);
}

int slc_hal_uart_send_data(hal_uart_id_e uart, uint8_t *data, uint32_t len, uint32_t timeout)
{
    stUartHandle_t *uartHandle = slc_hal_uart_get_handle(uart);
    if (uartHandle == NULL)
        return -1;

    return rom_hw_uart_transmit_bytes(uartHandle, data, len, timeout);
}

int slc_hal_uart_receive_data(hal_uart_id_e uart, uint8_t *data, uint32_t *len, uint32_t timeout)
{
    stUartHandle_t *uartHandle = slc_hal_uart_get_handle(uart);
    if (uartHandle == NULL)
        return -1;

    return rom_hw_uart_recieve_bytes(uartHandle, data, (uint16_t *)len, timeout);
}

void slc_hal_uart_enable_irq(hal_uart_id_e uart, uint32_t irq_mask)
{
    stUartHandle_t *uartHandle = slc_hal_uart_get_handle(uart);
    if (uartHandle == NULL)
        return;

    rom_hw_uart_enable_interrupt(uartHandle, (uint8_t)irq_mask);
}

void slc_hal_uart_disable_irq(hal_uart_id_e uart, uint32_t irq_mask)
{
    stUartHandle_t *uartHandle = slc_hal_uart_get_handle(uart);
    if (uartHandle == NULL)
        return;

    rom_hw_uart_disable_interrupt(uartHandle, (uint8_t)irq_mask);
}

uint32_t slc_hal_uart_get_irq_status(hal_uart_id_e uart)
{
    uint16_t status;
    stUartHandle_t *uartHandle = slc_hal_uart_get_handle(uart);
    if (uartHandle == NULL)
        return 0;

    rom_hw_uart_get_uart_status(uartHandle, &status);
    return status;
}

void slc_hal_uart_clear_irq_status(hal_uart_id_e uart, uint32_t irq_mask)
{
    uint16_t status;
    stUartHandle_t *uartHandle = slc_hal_uart_get_handle(uart);
    if (uartHandle == NULL)
        return;

    rom_hw_uart_get_uart_status(uartHandle, &status);
}

#if HAL_UART_RTS_CTS_SUPPORT
void slc_hal_uart_rts_cts_enable(hal_uart_id_e uart, bool enable)
{
    stUartHandle_t *uartHandle = slc_hal_uart_get_handle(uart);
    if (uartHandle == NULL)
        return;

    rom_hw_uart_flow_control_enable(uartHandle, enable);
}

uint32_t slc_hal_uart_get_cts_status(hal_uart_id_e uart)
{
    stUartHandle_t *uartHandle = slc_hal_uart_get_handle(uart);
    if (uartHandle == NULL)
        return 0;

    return rom_hw_uart_get_cts_sta(uartHandle);
}
#endif  // HAL_UART_RTS_CTS_SUPPORT

#if HAL_UART_RS485_SUPPORT
void slc_hal_rs485_enable(hal_uart_id_e uart, bool enable)
{
    stUartHandle_t *uartHandle = slc_hal_uart_get_handle(uart);
    if (uartHandle == NULL)
        return;

    rom_hw_uart_rs485_enable(uartHandle, enable);
}

void slc_hal_rs485_config(hal_uart_id_e uart, hal_rs485_cfg_t *config)
{
    stUartHandle_t *uartHandle = slc_hal_uart_get_handle(uart);
    unRs485Mode_t mode = {0};
    if (uartHandle == NULL)
        return;

    mode.stRs485Mode.Rs485En = false;
    mode.stRs485Mode.DePol = config->de_polarity;
    mode.stRs485Mode.TxMode = (config->mode == HAL_RS485_SW_DE) ? UART_RS485_SW_MODE : UART_RS485_HW_MODE;
    rom_hw_uart_set_rs485_mode(uartHandle, mode);

    rom_hw_uart_set_rs485_data_enable_timing(uartHandle, config->de_assertion_time, config->de_deassertion_time);

    rom_hw_uart_set_rs485_software_data_enable(uartHandle, true);
    rom_hw_uart_rs485_re_enable(uartHandle, true);
}

void slc_hal_rs485_manual_set_de(hal_uart_id_e uart, bool enable)
{
    stUartHandle_t *uartHandle = slc_hal_uart_get_handle(uart);
    if (uartHandle == NULL)
        return;

    rom_hw_uart_set_rs485_software_data_enable(uartHandle, enable);
}
#endif  // HAL_UART_RS485_SUPPORT

#if HAL_UART_DMA_SUPPORT

void slc_hal_uart_dma_config(hal_uart_id_e uart, hal_uart_dma_cfg_t *config)
{
    hal_dma_init_t dma_config;
    stUartHandle_t *uartHandle = slc_hal_uart_get_handle(uart);
    stUartFifoCfg_t fifi_cfg;
    if (uartHandle == NULL)
        return;

    fifi_cfg.bFifoEn = true;
    fifi_cfg.bRxFifoClean = true;
    fifi_cfg.bTxFifoClean = true;
    fifi_cfg.enDmaMode = (EN_UART_DMA_MODE_T)config->mode;
    fifi_cfg.enTxFifoThld = EN_UART_TX_FIFO_THLD_EMPTY;
    fifi_cfg.enRxFifoThld = EN_UART_RX_FIFO_THLD_1BYTES;
    rom_hw_uart_set_fifo_mode(uartHandle, &fifi_cfg);

    if (config->trx_dir == 0) {
        g_uart_dma_txch[uart] = config->dma_ch;
        dma_config.req = (uart == HAL_UART1) ? HAL_DMA_REQ_UART1_TX : HAL_DMA_REQ_UART0_TX;
        dma_config.src_addr_rise = true;
        dma_config.dst_addr_rise = false;
    } else {
        g_uart_dma_rxch[uart] = config->dma_ch;
        dma_config.req = (uart == HAL_UART1) ? HAL_DMA_REQ_UART1_RX : HAL_DMA_REQ_UART0_RX;
        dma_config.src_addr_rise = false;
        dma_config.dst_addr_rise = true;
    }

    dma_config.src_width = HAL_DMA_WIDTH_BYTE;
    dma_config.dst_width = HAL_DMA_WIDTH_BYTE;
    dma_config.src_addr = 0;
    dma_config.dst_addr = 0;
    dma_config.single_pkg_size = 1;
    dma_config.muli_trigger_num = 2;
    dma_config.irq_enable = config->dma_irq_enable;
    dma_config.timeout = (uint16_t)config->timeout;
    dma_config.soft_req = false;
    dma_config.high_priority = config->hign_pri;

    slc_hal_dma_init(config->dma_ch, &dma_config);
}

int slc_hal_uart_dma_send(hal_uart_id_e uart, uint8_t *data, uint32_t len)
{
    stUartHandle_t *uartHandle = slc_hal_uart_get_handle(uart);
    hal_dma_cfg_t dma_cfg;

    if (uartHandle == NULL)
        return -1;

    dma_cfg.src_addr = (uint32_t)data;
    dma_cfg.dst_addr = (uint32_t)&(uartHandle->DATA_FIFO_AND_CLK_DLV_LO);
    dma_cfg.single_pkg_size = 1;    // 需要和FIFO阈值一致
    dma_cfg.muli_trigger_num = len;
    dma_cfg.src_addr_rise = true;
    dma_cfg.dst_addr_rise = false;

    slc_hal_dma_transfer_cfg((hal_dma_ch_e)g_uart_dma_txch[uart], &dma_cfg);
    slc_hal_dma_start((hal_dma_ch_e)g_uart_dma_txch[uart]);

    return 0;
}

int slc_hal_uart_dma_receive(hal_uart_id_e uart, uint8_t *data, uint32_t len)
{
    stUartHandle_t *uartHandle = slc_hal_uart_get_handle(uart);
    hal_dma_cfg_t dma_cfg;

    if (uartHandle == NULL)
        return -1;

    dma_cfg.src_addr = (uint32_t)&(uartHandle->DATA_FIFO_AND_CLK_DLV_LO);
    dma_cfg.dst_addr = (uint32_t)data;
    dma_cfg.single_pkg_size = 1;    // 需要和FIFO阈值一致
    dma_cfg.muli_trigger_num = len;
    dma_cfg.src_addr_rise = false;
    dma_cfg.dst_addr_rise = true;

    slc_hal_dma_transfer_cfg((hal_dma_ch_e)g_uart_dma_rxch[uart], &dma_cfg);
    slc_hal_dma_start((hal_dma_ch_e)g_uart_dma_rxch[uart]);

    return 0;
}

#endif  // HAL_UART_DMA_SUPPORT
