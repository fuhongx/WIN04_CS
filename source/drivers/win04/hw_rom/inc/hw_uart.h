/**
 * ****************************************************************************
 * @file hw_uart.h
 * @author SLC(SC) software team. 
 * @brief 
 * @version  1.0
 * ****************************************************************************
 * 
 * Copyright (c) 2024 SLC Semiconductor all rights reserved.
 * 
 * ****************************************************************************
 */

#ifndef __HW_UART_H__
#define __HW_UART_H__

#define UART_TRANSMIT_TIMEOUT_US            (10000)

#define UART_RS485_SW_MODE   0x1
#define UART_RS485_HW_MODE   0x2

//New drivers 
typedef enum
{
    EN_UART_DMA_MODE0       =   0,      //Tx over thld stop, rx less than thld stop.
    EN_UART_DMA_MODE1       =   1,      //Tx full stop, Rx empty stop
}EN_UART_DMA_MODE_T;

typedef enum
{
    EN_UART_TX_FIFO_THLD_EMPTY      =   0,
    EN_UART_TX_FIFO_THLD_2BYTES     =   1,
    EN_UART_TX_FIFO_THLD_4BYTES     =   2,
    EN_UART_TX_FIFO_THLD_8BYTES     =   3,
}EN_UART_TX_FIFO_THLD_T;

typedef enum
{
    EN_UART_RX_FIFO_THLD_1BYTES      =   0,
    EN_UART_RX_FIFO_THLD_4BYTES      =   1,
    EN_UART_RX_FIFO_THLD_8BYTES      =   2,
    EN_UART_RX_FIFO_THLD_14BYTES     =   3,
}EN_UART_RX_FIFO_THLD_T;

typedef struct 
{
    bool bFifoEn;
    bool bRxFifoClean;
    bool bTxFifoClean;
    EN_UART_DMA_MODE_T enDmaMode;
    EN_UART_TX_FIFO_THLD_T enTxFifoThld;
    EN_UART_RX_FIFO_THLD_T enRxFifoThld;
}stUartFifoCfg_t;

typedef enum
{
    EN_UART_PARITY_ODD                      =   0,
    EN_UART_PARITY_EVEN                     =   1,
}EN_UART_PARITY_MODE_T;

typedef enum
{
    EN_UART_STOP_1BIT                       =   0,
    EN_UART_STOP_2BIT                       =   1,
}EN_UART_STOP_LEN_T;

typedef enum
{
    EN_UART_DATA_LEN_5BIT                   =   0,
    EN_UART_DATA_LEN_6BIT                   =   1,
    EN_UART_DATA_LEN_7BIT                   =   2,
    EN_UART_DATA_LEN_8BIT                   =   3,
}EN_UART_DATA_LEN_T;

typedef union 
{
    struct
    {
        uint32_t DataLen        : 2;  /** BIT 0 - 1 */
        uint32_t StopLen        : 1;  /** BIT 2 */
        uint32_t ParityEn       : 1;  /** BIT 3*/
        uint32_t ParityMode     : 1;  /** BIT 4*/
        uint32_t ForceParityEn  : 2;  /** BIT 5*/
        uint32_t BreakCtrl      : 1;  /** BIT 6*/
        uint32_t DllCfgEn       : 1;  /** BIT 7*/
        uint32_t Res            : 24;
    }stUartMode;
    uint32_t u32Cfg;
}unUartMode_t;

typedef union 
{
    struct
    {
        uint32_t Rs485En        : 1;  /** BIT 0 */
        uint32_t Res1           : 1;  /** BIT 1 */
        uint32_t DePol          : 1;  /** BIT 2*/
        uint32_t TxMode         : 2;  /** BIT 3-4*/
        uint32_t Res            : 28;
    }stRs485Mode;
    uint32_t u32Cfg;
}unRs485Mode_t;

typedef enum
{
    EN_UART_STA_DATA_READY              = UART_LINE_STA_DATA_READY_MASK << UART_LINE_STA_DATA_READY_SHIFT,
    EN_UART_STA_RX_FIFO_OVERFLOW        = UART_LINE_STA_RX_FIFO_OVERFLOW_MASK << UART_LINE_STA_RX_FIFO_OVERFLOW_SHIFT,
    EN_UART_STA_PARITY_ERROR            = UART_LINE_STA_PARITY_ERROR_MASK << UART_LINE_STA_PARITY_ERROR_SHIFT,
    EN_UART_STA_STOP_BIT_ERROR          = UART_LINE_STA_STOP_BIT_ERROR_MASK << UART_LINE_STA_STOP_BIT_ERROR_SHIFT,
    EN_UART_STA_LINE_BREAK              = UART_LINE_STA_LINE_BREAK_MASK << UART_LINE_STA_LINE_BREAK_SHIFT,
    EN_UART_STA_TX_FIFO_UNDER_THLD      = UART_LINE_STA_TX_FIFO_UNDER_THLD_MASK << UART_LINE_STA_TX_FIFO_UNDER_THLD_SHIFT,
    EN_UART_STA_TX_DONE_AND_FIFO_EMPTY  = UART_LINE_STA_TX_DONE_AND_FIFO_EMPTY_MASK << UART_LINE_STA_TX_DONE_AND_FIFO_EMPTY_SHIFT,
    EN_UART_STA_RX_PARITY_ERROR         = UART_LINE_STA_RX_FIFO_PARITY_ERROR_MASK << UART_LINE_STA_RX_FIFO_PARITY_ERROR_SHIFT,
    EN_UART_STA_UART_BUSY               = UART_STA_BUSY_MASK << (UART_STA_BUSY_SHIFT + UART_LINE_STA_REG_LEN),
    EN_UART_STA_TX_FIFO_NOT_FULL        = UART_STA_TX_FIFO_NOT_FULL_MASK << (UART_STA_TX_FIFO_NOT_FULL_SHIFT + UART_LINE_STA_REG_LEN),
    EN_UART_STA_TX_FIFO_EMPTY           = UART_STA_TX_FIFO_EMPTY_MASK << (UART_STA_TX_FIFO_EMPTY_SHIFT + UART_LINE_STA_REG_LEN),
    EN_UART_STA_RX_FIFO_NOT_EMPTY       = UART_STA_RX_FIFO_NOT_EMPTY_MASK << (UART_STA_RX_FIFO_NOT_EMPTY_SHIFT + UART_LINE_STA_REG_LEN),
    EN_UART_STA_RX_FIFO_FULL            = UART_STA_RX_FIFO_FULL_MASK << (UART_STA_RX_FIFO_FULL_SHIFT + UART_LINE_STA_REG_LEN),
}EN_UART_STA_T;

typedef enum
{
    EN_UART_INT_EN_RX_START                 =   UART_INT_EN_RX_START_MASK << UART_INT_EN_RX_START_SHIFT,
    EN_UART_INT_EN_TX_FIFO_EMPTY            =   UART_INT_EN_TX_FIFO_EMPTY_MASK << UART_INT_EN_TX_FIFO_EMPTY_SHIFT,
    EN_UART_INT_EN_STA_INT                  =   UART_INT_EN_LINE_STA_MASK   << UART_INT_EN_LINE_STA_SHIFT,
    EN_UART_INT_EN_FLOW_CTRL_INT            =   UART_INT_EN_CTS_MASK << UART_INT_EN_CTS_SHIFT,
    EN_UART_INT_EN_READ_CLEAN               =   UART_INT_EN_READ_CLEAN_MODE_MASK << UART_INT_EN_READ_CLEAN_MODE_SHIFT,
    EN_UART_INT_EN_TX_FIFO_UNDER_THLD       =   UART_INT_EN_TX_FIFO_UNDER_THLD_MASK << UART_INT_EN_TX_FIFO_UNDER_THLD_SHIFT,
}EN_UART_INT_EN_T;

typedef enum
{
    EN_UART_FLOW_CTRL_CTS_CHANGE        = UART_MODEM_STA_CTS_CHANGE_MASK << UART_MODEM_STA_CTS_CHANGE_SHIFT,
    EN_UART_FLOW_CTRL_CTS_LEVEL_LO      = UART_MODEM_STA_CST_LEVEL_MASK << UART_MODEM_STA_CST_LEVEL_SHIFT,
}EN_UART_FLOW_CTRL_T;

typedef struct
{
    stUartFifoCfg_t stFifo;
    unUartMode_t    unMode;
    uint32_t u32BaudRate;
}stUartInit_t;


#define UART_MODE_CONFIG_DEFAULT(enDataLen, enStopLen)     (((enDataLen & UART_LINE_CTRL_DATA_LEN_MASK) << UART_LINE_CTRL_DATA_LEN_SHIFT)|\
                                        ((enStopLen & UART_LINE_CTRL_STOP_LEN_MASK) << UART_LINE_CTRL_STOP_LEN_SHIFT)|\
                                        ((0x0 & UART_LINE_CTRL_PARITY_EN_MASK) << UART_LINE_CTRL_PARITY_EN_SHIFT)|\
                                        ((0x0 & UART_LINE_CTRL_EVEN_PARITY_EN_MASK) << UART_LINE_CTRL_EVEN_PARITY_EN_SHIFT)|\
                                        ((0x0 & UART_LINE_CTRL_FORCE_PARITY_MASK) << UART_LINE_CTRL_FORCE_PARITY_SHIFT)|\
                                        ((0x0 & UART_LINE_CTRL_BRAEK_CTRL_EN_MASK) << UART_LINE_CTRL_BRAEK_CTRL_EN_SHIFT)|\
                                        ((0x0 & UART_LINE_CTRL_DIV_REG_EN_MASK) << UART_LINE_CTRL_DIV_REG_EN_SHIFT))


//End

bool rom_hw_uart_get_clock_div_reg_status(stUartHandle_t *pstHandle);
EN_ERR_STA_T rom_hw_uart_set_clk_div_reg_enable(stUartHandle_t *pstHandle, bool bEnable);
EN_ERR_STA_T rom_hw_uart_set_baudrate(stUartHandle_t *pstHandle, uint32_t u32BaudRate);
EN_ERR_STA_T rom_hw_uart_set_fifo_mode(stUartHandle_t *pstHandle, stUartFifoCfg_t *pstFifo);
EN_ERR_STA_T rom_hw_uart_enable_interrupt(stUartHandle_t *pstHandle, uint8_t u8Mask);
EN_ERR_STA_T rom_hw_uart_disable_interrupt(stUartHandle_t *pstHandle, uint8_t u8Mask);
EN_ERR_STA_T rom_hw_uart_get_interrupt_flag(stUartHandle_t *pstHandle, uint8_t *pu8Flag);
EN_ERR_STA_T rom_hw_uart_set_mode(stUartHandle_t *pstHandle, unUartMode_t unMode);
EN_ERR_STA_T rom_hw_uart_get_uart_status(stUartHandle_t *pstHandle, uint16_t *pu8Sta);
uint32_t rom_hw_uart_get_cts_sta(stUartHandle_t *pstHandle);
void rom_hw_uart_flow_control_enable(stUartHandle_t *pstHandle, bool enable);
EN_ERR_STA_T rom_hw_uart_set_rs485_mode(stUartHandle_t *pstHandle, unRs485Mode_t unMode);
EN_ERR_STA_T rom_hw_uart_set_rs485_software_data_enable(stUartHandle_t *pstHandle, bool bEnable);
void rom_hw_uart_rs485_re_enable(stUartHandle_t *pstHandle, bool enable);
EN_ERR_STA_T rom_hw_uart_set_rs485_data_enable_timing(stUartHandle_t *pstHandle, uint8_t u8AssertionTime, uint8_t u8DeAssertionTime);
void rom_hw_uart_rs485_enable(stUartHandle_t *pstHandle, bool enable);

/**
 * @brief uart handle init
 * 
 * @param pstHandle uart handle, @ref stUartHandle_t, sucn as UART0, UART1, LPUART.
 * @param pstInit  uart init config, @ref stUartInit_t.
 * @return EN_ERR_STA_T
 */
EN_ERR_STA_T rom_hw_uart_init(stUartHandle_t *pstHandle, stUartInit_t *pstInit);

/**
 * @brief uart transmit bytes
 * 
 * @param pstHandle uart handle, @ref stUartHandle_t, sucn as UART0, UART1.
 * @param pu8Data   data buffer pointer
 * @param u16Len    data length
 * @param TimeOutUs  Timeout time, unit is us.
 * @return EN_ERR_STA_T 
 */
EN_ERR_STA_T rom_hw_uart_transmit_bytes(stUartHandle_t *pstHandle, uint8_t *pu8Data, uint16_t u16Len, uint32_t TimeOutUs);

/**
 * @brief uart receive bytes
 * 
 * @param pstHandle  uart handle, @ref stUartHandle_t, sucn as UART0, UART1.
 * @param pu8Buffer  data buffer pointer
 * @param pu16RxLen  Pointer to store received data length.
 * @param u32TimeOutUs  Timeout time, unit is us.
 * @return EN_ERR_STA_T 
 */
EN_ERR_STA_T rom_hw_uart_recieve_bytes(stUartHandle_t *pstHandle, uint8_t *pu8Buffer, uint16_t *pu16RxLen, uint32_t u32TimeOutUs);


#endif

