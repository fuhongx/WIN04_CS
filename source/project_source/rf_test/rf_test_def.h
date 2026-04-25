/**
 * ****************************************************************************
 * @file rf_test_main.h
 * @author SLC(SC) software team. 
 * @brief 
 * @version  1.0
 * ****************************************************************************
 * 
 * Copyright (c) 2024 SLC Semiconductor all rights reserved.
 * 
 * ****************************************************************************
 */

#ifndef __RF_TEST_MAIN_H__
#define __RF_TEST_MAIN_H__

#define RF_TEST_UART_BAUDRATE                   (115200)

#ifndef QMX_FPGA
//Using UART0
#define RF_TEST_UART_HANDLE                  (HAL_UART0)
#define RF_TEST_UART_TX_PIN                  (HAL_GPIO_PIN12)
#define RF_TEST_UART_RX_PIN                  (HAL_GPIO_PIN13)
// #define RF_TEST_UART_TX_PIN                  (HAL_GPIO_PIN0)
// #define RF_TEST_UART_RX_PIN                  (HAL_GPIO_PIN1)
#else
//Using UART1
#define RF_TEST_UART_HANDLE                  (HAL_UART1)
#define RF_TEST_UART_TX_PIN                  (HAL_GPIO_PIN2)
#define RF_TEST_UART_RX_PIN                  (HAL_GPIO_PIN3)
#endif




typedef void (*pf_rf_test_transmit_callback)(uint8_t *pu8Buffer, uint16_t u16Len);

typedef uint8_t (*pf_rf_test_get_char_callback)(void);





#endif
