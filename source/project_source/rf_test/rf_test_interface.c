#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "string.h"
#include "reg_Dev.h"
#include "error_def.h"
#include "utility.h"
#include "debug.h"
#include "slc_hal_uart.h"
#include "hw_flash.h"
#include "slc_hal_gpio.h"
#include "hw_dma.h"
#include "slc_hal_sysctrl.h"
#include "slc_hal_delay.h"
#include "app_cfg.h"
#include "rf_test_def.h"

#include "slc_hal_spi.h"

void *pf_transmit;
void *pf_get_char;

#define SUBG_CS_PIN     (HAL_GPIO_PIN4)
#define SUBG_CLK_PIN    (HAL_GPIO_PIN5)
#define SUBG_MOSI_PIN   (HAL_GPIO_PIN6)
#define SUBG_MISO_PIN   (HAL_GPIO_PIN7)

#define SUBG_SPI_HANDLE (HAL_SPI1)

void rf_sub_spi_init(void)
{
    slc_hal_gpio_set_iomux(SUBG_CS_PIN, HAL_IOMUX_MODE2);
    slc_hal_gpio_set_iomux(SUBG_CLK_PIN, HAL_IOMUX_MODE2);
    slc_hal_gpio_set_iomux(SUBG_MOSI_PIN, HAL_IOMUX_MODE2);
    slc_hal_gpio_set_iomux(SUBG_MISO_PIN, HAL_IOMUX_MODE2);
    slc_hal_gpio_set_mode(SUBG_CS_PIN, HAL_GPIO_PULL_UP);
    slc_hal_gpio_set_mode(SUBG_CLK_PIN, HAL_GPIO_PULL_UP);
    slc_hal_gpio_set_mode(SUBG_MOSI_PIN, HAL_GPIO_PULL_UP);
    slc_hal_gpio_set_mode(SUBG_MISO_PIN, HAL_GPIO_PULL_UP);

    slc_hal_sysctrl_peripheral_clk_enable(HAL_CLK_SPI1, true);
    slc_hal_sysctrl_peripheral_mod_reset(HAL_CLK_SPI1);

    hal_spi_cfg_t config = {0};
    config.mode = HAL_SPI_MASTER;
    config.clk_div = HAL_SPI_DIV_20;
    config.data_mode = HAL_SPI_DATA_MSB;
    config.polarity_phase = HAL_SPI_CPOL0_CPHA0;
    config.data_len = HAL_SPI_DATA_LEN_8BIT;
    config.cs_holding_time = 0x10;
    config.clk_adjust_en = true;
    config.sw_cs_en = true;
    config.cs_gap_time = 0x10;
    config.anti_noise_level = 0;
    config.tx_fifo_pfull_th = 12;
    config.rx_fifo_pfull_th = 12;
    config.rx_fifo_pempty_th = 4;
    config.tx_fifo_pempty_th = 4;

    slc_hal_spi_init(SUBG_SPI_HANDLE, &config);
}

uint8_t au8TxBuffer[300];
uint8_t au8SpiRxBuffer[300];

void rf_subg_write32_cmd(uint16_t u16Addr, uint8_t *pu8Buffer, uint16_t u16Len);
void rf_subg_read32_cmd(uint16_t u16Addr, uint8_t *pu8Buffer, uint16_t u16Len);
void rf_subg_read32_with_dummy(uint16_t u16Addr, uint8_t *pu8Buffer, uint16_t u16Len);

void rf_subg_write32_cmd(uint16_t u16Addr, uint8_t *pu8Buffer, uint16_t u16Len)
{
    memset(au8TxBuffer, 0, sizeof(au8TxBuffer));
    memset(au8SpiRxBuffer, 0, sizeof(au8SpiRxBuffer));
    au8TxBuffer[0] = 0x2;
    au8TxBuffer[1] = (u16Addr >>8) & 0xFF;
    au8TxBuffer[2]  = u16Addr & 0xFF;
    memcpy(&au8TxBuffer[3], pu8Buffer, u16Len);
    // PRINTF("Subg write32 cmd addr = 0x%x, tx len = %d\n", u16Addr, u16Len);

    slc_hal_spi_set_sw_cs(SUBG_SPI_HANDLE, HAL_SPI_CS_LO);
    slc_hal_spi_master_trx(SUBG_SPI_HANDLE, au8TxBuffer, au8SpiRxBuffer, u16Len+3, HAL_SPI_TIMEOUT_US);
    slc_hal_spi_set_sw_cs(SUBG_SPI_HANDLE, HAL_SPI_CS_HI);
}

void rf_subg_read32_cmd(uint16_t u16Addr, uint8_t *pu8Buffer, uint16_t u16Len)
{
    memset(au8TxBuffer, 0, sizeof(au8TxBuffer));
    memset(au8SpiRxBuffer, 0, sizeof(au8SpiRxBuffer));
    au8TxBuffer[0] = 0x3;
    au8TxBuffer[1] = (u16Addr >>8) & 0xFF;
    au8TxBuffer[2]  = u16Addr & 0xFF;
    
    // PRINTF("Subg Read32 cmd addr = 0x%x, rx len = %d\n", u16Addr, u16Len);
    slc_hal_spi_set_sw_cs(SUBG_SPI_HANDLE, HAL_SPI_CS_LO);
    slc_hal_spi_master_trx(SUBG_SPI_HANDLE, au8TxBuffer, au8SpiRxBuffer, u16Len+3, HAL_SPI_TIMEOUT_US);
    slc_hal_spi_set_sw_cs(SUBG_SPI_HANDLE, HAL_SPI_CS_HI);
    for(int i = 0; i < u16Len; i++)
    {
        pu8Buffer[i] = au8SpiRxBuffer[i + 3];
        // PRINTF("Subg Rx %d = 0x%x\n", i, pu8Buffer[i]);
    }
}

void rf_subg_read32_with_dummy(uint16_t u16Addr, uint8_t *pu8Buffer, uint16_t u16Len)
{
    memset(au8TxBuffer, 0, sizeof(au8TxBuffer));
    memset(au8SpiRxBuffer, 0, sizeof(au8SpiRxBuffer));
    au8TxBuffer[0] = 0xB;
    au8TxBuffer[1] = (u16Addr >>8) & 0xFF;
    au8TxBuffer[2]  = u16Addr & 0xFF;
    
    // PRINTF("Subg Read32 with dummy cmd addr = 0x%x, tx len = %d\n", u16Addr, u16Len);
    slc_hal_spi_set_sw_cs(SUBG_SPI_HANDLE, HAL_SPI_CS_LO);
    slc_hal_spi_master_trx(SUBG_SPI_HANDLE, au8TxBuffer, au8SpiRxBuffer, u16Len+4, HAL_SPI_TIMEOUT_US);
    slc_hal_spi_set_sw_cs(SUBG_SPI_HANDLE, HAL_SPI_CS_HI);
    for(int i = 0; i < u16Len; i++)
    {
        pu8Buffer[i] = au8SpiRxBuffer[i + 4];
        // PRINTF("Subg Rx %d = 0x%x\n", i, pu8Buffer[i]);
    }
}

void subg_read_test(void)
{
    rf_sub_spi_init();
    
    uint16_t u16Addr = 0xBC;
    uint8_t au8ReadBuffer[10] = {0};
    uint8_t au8WriteBuffer[4] = {0};
    
    rom_utility_little_endian_store_32(au8WriteBuffer,0,0x55AA);
    
    while(1)
    {
        rf_subg_write32_cmd(u16Addr, au8WriteBuffer, 4);
        
        rf_subg_read32_cmd(u16Addr, au8ReadBuffer, 4);
        uint32_t u32Data = rom_utility_little_endian_read_32(au8ReadBuffer, 0);
        PRINTF("Read 0x%x = 0x%x\n", u16Addr, u32Data);
        slc_hal_nop_delay_ms(1000);
    }
    
    
    
}

void rf_test_uart_init(void)
{
    //Clock init
    if(HAL_UART0 == RF_TEST_UART_HANDLE)
    {
        slc_hal_sysctrl_peripheral_clk_enable(HAL_CLK_UART0, true);
    }
    else
    {
        slc_hal_sysctrl_peripheral_clk_enable(HAL_CLK_UART1, true);
    }

    //GPIO INIT
    slc_hal_gpio_set_iomux(RF_TEST_UART_TX_PIN, HAL_IOMUX_MODE0);
    slc_hal_gpio_set_iomux(RF_TEST_UART_RX_PIN, HAL_IOMUX_MODE0);

    //Uart PIN init
    hal_uart_init_t config = {0};

    config.baudrate = RF_TEST_UART_BAUDRATE;
    config.parity = HAL_UART_PARITY_NONE;
    config.stopbit = HAL_UART_STOPBIT_1;
    config.databits = HAL_UART_DATA_8BIT;

    config.tx_fifo_thld = HAL_UART_TXFIFO_THLD_EMPTY;
    config.rx_fifo_thld = HAL_UART_RXFIFO_THLD_1BYTE;
    config.fifo_en = false;
    config.flow_ctrl_en = false;

    slc_hal_uart_init(RF_TEST_UART_HANDLE, &config);
}

void rf_test_uart_tx(uint8_t *pu8Buffer, uint16_t u16Len)
{
    slc_hal_uart_send_data(RF_TEST_UART_HANDLE, pu8Buffer, u16Len, HAL_UART_TIMEOUT_US);
}

bool bNoRet = false;
extern stUartHandle_t *slc_hal_uart_get_handle(hal_uart_id_e uart);
uint8_t rf_test_uart_get_char(void)
{
    uint16_t u16Sta = 0;
    uint8_t rx_data;
    uint32_t u32Timeout = 0;
    stUartHandle_t *uartHandle = slc_hal_uart_get_handle(DEBUG_UART_HANDLE);
    stUartHandle_t *rf_test_uartHandle = slc_hal_uart_get_handle(RF_TEST_UART_HANDLE);
    while(1)
    {
        u16Sta = slc_hal_uart_get_irq_status(RF_TEST_UART_HANDLE);
        while(HAL_UART_STA_RX_READY != (u16Sta & HAL_UART_STA_RX_READY))
        {
            u16Sta = slc_hal_uart_get_irq_status(RF_TEST_UART_HANDLE);
            
            if(HAL_UART_STA_RX_READY != (u16Sta & HAL_UART_STA_RX_READY))
            {
                u32Timeout++;
            }
            
            if(u32Timeout > 3000)
            {
                extern uint8_t au8PrtBuffer[1024 * 4];
                extern uint32_t u32PrtNum;
                extern uint32_t u32PutNum;

                if(u32PrtNum != u32PutNum)
                {
                    uint8_t u8Cnt = uartHandle->TX_FIFO_CNT;
                    if(u8Cnt < 15)
                    {
                        uint8_t u8PutNum;
                        
                        if((15 - u8Cnt) <= (u32PrtNum - u32PutNum))
                        {
                            u8PutNum = 15 - u8Cnt;
                        }
                        else
                        {
                            u8PutNum = (u32PrtNum - u32PutNum);
                        }
                        
                        for(int i=0; i < u8PutNum; i++)
                        {
                            uartHandle->DATA_FIFO_AND_CLK_DLV_LO = au8PrtBuffer[u32PutNum];
                            u32PutNum++;
                        }
                    }
                    
                }
                if(u32PrtNum == u32PutNum)
                {
                    u32PrtNum = 0;
                    u32PutNum = 0;
                }
            }
            
        }
        u32Timeout = 0;
        
        rx_data = rf_test_uartHandle->DATA_FIFO_AND_CLK_DLV_LO & UART_DATA_FIFO_REG_MASK;
        if(!bNoRet)
        {
        break;
        }
    }
    
    return rx_data;
}

uint8_t rf_get_one_char_new(void)
{
    uint8_t u8RxData = 0;
    int ret = 0;
    uint32_t len = 1;
    while(1)
    {
        ret = slc_hal_uart_receive_data(RF_TEST_UART_HANDLE, &u8RxData, &len, HAL_UART_TIMEOUT_US);
        if(ret == 0)
        {
            break;
        }
    }

    return u8RxData;
}

void rf_test_uart_get_data(uint8_t *pu8Buffer, uint16_t* pu16Len)
{
    slc_hal_uart_receive_data(RF_TEST_UART_HANDLE, pu8Buffer, (uint32_t *)pu16Len, HAL_UART_TIMEOUT_US);
}

void rf_test_interface_init(void)
{
    rf_test_uart_init();
    //Register callback
    pf_transmit = (void*) rf_test_uart_tx;
    pf_get_char = rf_get_one_char_new;
    
    
}



