/**
 * ****************************************************************************
 * @file boot_interface.h
 * @author SLC(SC) software team. 
 * @brief 
 * @version  1.0
 * ****************************************************************************
 * 
 * Copyright (c) 2024 SLC Semiconductor all rights reserved.
 * 
 * ****************************************************************************
 */
#ifndef __BOOT_INTERFACE_H__
#define __BOOT_INTERFACE_H__

/*

* GPIO2  | UART1_TX  
* GPIO3  | UART1_RX  | 
*/
/**
 * Bootloader Uart interface define.
 */
#define BOOT_UART_UPDATE_HANDLE         (UART1)
#define BOOT_UART_UPDATE_BAUDRATE       (115200)
#define BOOT_UART_UPDATE_TX_PIN         (GPIO_PIN2)
#define BOOT_UART_UPDATE_RX_PIN         (GPIO_PIN3)
#define BOOT_UART_GPIO_IOMUX_MODE       (EN_GPIO_IOMUX_MODE0)

#define BOOT_TRX_BUFFER_SIZE            (300)
#define FIRM_UPGRADE_UART_RX_LEN        (4096 + 32)

extern void boot_interface_gpio_init(void);
extern bool boot_interface_need_down_bin(void);
extern void boot_interface_uart_init(void);
extern uint16_t boot_interface_get_rx_data(uint8_t *pu8Buffer);
extern void boot_interface_change_baudrate(uint32_t u32Baud);
extern void boot_interface_send_data(uint8_t *pu8Buffer, uint16_t u16Len);

#endif
