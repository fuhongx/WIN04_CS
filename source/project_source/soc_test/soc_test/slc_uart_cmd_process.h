#ifndef __SLC_UART_CMD_PROCESS_H__
#define __SLC_UART_CMD_PROCESS_H__

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "reg_Dev.h"
#include "error_def.h"
#include "utility.h"
#include "debug.h"

// 每个发送帧和接收帧帧最后都会附带2字节CRC校验值，不在此处体现
enum slc_uart_rx_rw_frame_idx {
    RX_FRAME_START_IDX     = 0,
    RX_FRAME_LEN_IDX       = 1,
    RX_FRAME_RW_FLAG_IDX   = 2,
    RX_FRAME_ADDR_IDX      = 3,
    RX_FRAME_DATA_IDX      = 7,
    RX_FRAME_END_IDX       = 11,
};

enum slc_uart_tx_rw_frame_idx {
    TX_FRAME_START_IDX     = 0,
    TX_FRAME_LEN_IDX       = 1,
    TX_FRAME_RW_FLAG_IDX   = 2,
    TX_FRAME_ADDR_IDX      = 3,
    TX_FRAME_STATUS_IDX    = 7,
    TX_FRAME_END_IDX       = 8,
};

enum frame_rw_option_e {
    SLC_UART_READ          = 0,
    SLC_UART_WRITE         = 1,
    SLC_UART_BLOCK_READ    = 2,
};

enum rw_space_e {
    CPU_OPTION      = 0,
    PHY_SPI_OPTION  = 1,
    RF_SPI_OPTION   = 2,
};

enum slc_autotest_frame_idx {
    AUTOTEST_START_IDX  = 0,
    AUTOTEST_LEN_IDX    = 1,
    AUTOTEST_RESV_IDX   = 2,
    AUTOTEST_CMD_IDX    = 3,
    AUTOTEST_VAL0_IDX   = 4,
    AUTOTEST_VAL1_IDX   = 5,
    AUTOTEST_END_IDX    = 6,
};

enum slc_autotest_cmd_e {
    AUTOTEST_SET_CLK_CMD    = 0x11,
    AUTOTEST_SEL_CASE_CMD   = 0x21,
};

#define SLC_UART_FRAME_START        0x7E
#define SLC_UART_FRAME_END          0x7E
#define SLC_UART_RX_FRAME_LEN(n)   (0x8+n*4)
#define SLC_UART_TX_FRAME_LEN      (0x9)

void slc_uart_cmd_process_main(void);

#endif
