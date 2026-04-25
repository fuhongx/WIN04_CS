/**************************************************************************//**
 * @file     startup_ARMCM0plus.S
 * @brief    CMSIS-Core(M) Device Startup File for Cortex-M0plus Device
 * @version  V2.0.1
 * @date     23. July 2019
 ******************************************************************************/
/*
 * Copyright (c) 2009-2019 Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

                .syntax  unified
                .arch    armv6-m

                .section .vectors
                .align   2
                .globl   __Vectors
                .globl   __Vectors_End
                .globl   __Vectors_Size
__Vectors:
                .long    __StackTop                         /*     Top of Stack */
                .long    Reset_Handler                      /*     Reset Handler */
                .long    NMI_Handler                        /* -14 NMI Handler */
                .long    HardFault_Handler                  /* -13 Hard Fault Handler */
                .long    0                                  /*     Reserved */
                .long    0                                  /*     Reserved */
                .long    0                                  /*     Reserved */
                .long    0                                  /*     Reserved */
                .long    0                                  /*     Reserved */
                .long    0                                  /*     Reserved */
                .long    0                                  /*     Reserved */
                .long    SVC_Handler                        /*  -5 SVCall Handler */
                .long    0                                  /*     Reserved */
                .long    0                                  /*     Reserved */
                .long    PendSV_Handler                     /*  -2 PendSV Handler */
                .long    SysTick_Handler                    /*  -1 SysTick Handler */

                /* Interrupts */
                .long    PHY0_IRQ_Handler                 /*   0 Interrupt 0 */
                .long    PHY1_IRQ_Handler                 /*   1 Interrupt 1 */
                .long    LP_PAD_WAKEUP_IRQ_Handler                 /*   2 Interrupt 2 */
                .long    LP_PAD_STOP2_WAKEUP_IRQ_Handler                 /*   3 Interrupt 3 */
                .long    I2C0_IRQ_Handler                 /*   4 Interrupt 4 */
                .long    I2C1_IRQ_Handler                 /*   5 Interrupt 5 */
                .long    FLASH_CTRL_IRQ_Handler                 /*   6 Interrupt 6 */
                .long    UART1_IRQ_Handler                 /*   7 Interrupt 7 */
                .long    UART2_IRQ_Handler                 /*   8 Interrupt 8 */
                .long    RFANA_IRQ_Handler                 /*   9 Interrupt 9 */

                .long    SPI1_IRQ_Handler                 /*   9 Interrupt 9 */
                .long    SPI2_IRQ_Handler                 /*   9 Interrupt 9 */
                .long    LPUART_IRQ_Handler                 /*   9 Interrupt 9 */
                .long    TIMER1_IRQ_Handler                 /*   9 Interrupt 9 */
                .long    TIMER2_IRQ_Handler                 /*   9 Interrupt 9 */
                .long    TIMER3_IRQ_Handler                 /*   9 Interrupt 9 */
                .long    TIMER4_IRQ_Handler                 /*   9 Interrupt 9 */
                .long    GPIO1_IRQ_Handler                 /*   9 Interrupt 9 */
                .long    LPTIMER0_IRQ_Handler                 /*   9 Interrupt 9 */
                
                .long    LPTIMER1_IRQ_Handler                 /*   9 Interrupt 9 */
                .long    CTIMER_IRQ_Handler                 /*   9 Interrupt 9 */
                .long    WTD_IRQ_Handler                 /*   9 Interrupt 9 */
                .long    IWTD_IRQ_Handler                 /*   9 Interrupt 9 */
                .long    DMA_IRQ_Handler                 /*   9 Interrupt 9 */
                .long    SECURE_IRQ_Handler                 /*   9 Interrupt 9 */
                .long    SLOW_UPDATE_IRQ_Handler                 /*   9 Interrupt 9 */
                .long    RTC_IRQ_Handler                 /*   9 Interrupt 9 */


                //.space   ( 22 * 4)                          /* Interrupts 10 .. 31 are left out */
__Vectors_End:
                .equ     __Vectors_Size, __Vectors_End - __Vectors
                .size    __Vectors, . - __Vectors


/* The default macro is not used for HardFault_Handler
 * because this results in a poor debug illusion.
 */
                .thumb_func
                .type    HardFault_Handler, %function
                .weak    HardFault_Handler
                .fnstart
HardFault_Handler:
                b        .
                .fnend
                .size    HardFault_Handler, . - HardFault_Handler

                .thumb_func
                .type    Default_Handler, %function
                .weak    Default_Handler
                .fnstart
Default_Handler:
                b        .
                .fnend
                .size    Default_Handler, . - Default_Handler

/* Macro to define default exception/interrupt handlers.
 * Default handler are weak symbols with an endless loop.
 * They can be overwritten by real handlers.
 */
                .macro   Set_Default_Handler  Handler_Name
                .weak    \Handler_Name
                .set     \Handler_Name, Default_Handler
                .endm


/* Default exception/interrupt handler */

                Set_Default_Handler  NMI_Handler
                Set_Default_Handler  SVC_Handler
                Set_Default_Handler  PendSV_Handler
                Set_Default_Handler  SysTick_Handler

                Set_Default_Handler  PHY0_IRQ_Handler
                Set_Default_Handler  PHY1_IRQ_Handler
                Set_Default_Handler  LP_PAD_WAKEUP_IRQ_Handler
                Set_Default_Handler  LP_PAD_STOP2_WAKEUP_IRQ_Handler
                Set_Default_Handler  I2C0_IRQ_Handler
                Set_Default_Handler  I2C1_IRQ_Handler
                Set_Default_Handler  FLASH_CTRL_IRQ_Handler
                Set_Default_Handler  UART1_IRQ_Handler
                Set_Default_Handler  UART2_IRQ_Handler
                Set_Default_Handler  RFANA_IRQ_Handler

                Set_Default_Handler  SPI1_IRQ_Handler
                Set_Default_Handler  SPI2_IRQ_Handler
                Set_Default_Handler  LPUART_IRQ_Handler
                Set_Default_Handler  TIMER1_IRQ_Handler
                Set_Default_Handler  TIMER2_IRQ_Handler
                Set_Default_Handler  TIMER3_IRQ_Handler
                Set_Default_Handler  TIMER4_IRQ_Handler
                Set_Default_Handler  GPIO1_IRQ_Handler
                Set_Default_Handler  LPTIMER0_IRQ_Handler
                Set_Default_Handler  LPTIMER1_IRQ_Handler
                Set_Default_Handler  CTIMER_IRQ_Handler
                Set_Default_Handler  WTD_IRQ_Handler
                Set_Default_Handler  IWTD_IRQ_Handler
                Set_Default_Handler  DMA_IRQ_Handler
                Set_Default_Handler  SECURE_IRQ_Handler
                Set_Default_Handler  SLOW_UPDATE_IRQ_Handler
                Set_Default_Handler  RTC_IRQ_Handler

                .end
