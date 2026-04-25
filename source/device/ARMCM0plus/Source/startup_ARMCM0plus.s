;/*******************************************************************************
; * @file     startup_ARMCM0plus.s
; * @brief    CMSIS Core Device Startup File for
; *           ARMCM0plus Device
; * @version  V1.0.1
; * @date     23. July 2019
; ******************************************************************************/
;/*
; * Copyright (c) 2009-2019 Arm Limited. All rights reserved.
; *
; * SPDX-License-Identifier: Apache-2.0
; *
; * Licensed under the Apache License, Version 2.0 (the License); you may
; * not use this file except in compliance with the License.
; * You may obtain a copy of the License at
; *
; * www.apache.org/licenses/LICENSE-2.0
; *
; * Unless required by applicable law or agreed to in writing, software
; * distributed under the License is distributed on an AS IS BASIS, WITHOUT
; * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
; * See the License for the specific language governing permissions and
; * limitations under the License.
; */

;//-------- <<< Use Configuration Wizard in Context Menu >>> ------------------

;//this file has been modified

;<h> Stack Configuration
;  <o> Stack Size (in Bytes) <0x0-0xFFFFFFFF:8>
;</h>

Stack_Size      EQU      0x00002000

                AREA     STACK, NOINIT, READWRITE, ALIGN=3
__stack_limit
Stack_Mem       SPACE    Stack_Size
__initial_sp


;<h> Heap Configuration
;  <o> Heap Size (in Bytes) <0x0-0xFFFFFFFF:8>
;</h>

Heap_Size       EQU      0x00002000

                IF       Heap_Size != 0                      ; Heap is provided
                AREA     HEAP, NOINIT, READWRITE, ALIGN=3
__heap_base
Heap_Mem        SPACE    Heap_Size
__heap_limit
                ENDIF


                PRESERVE8
                THUMB


; Vector Table Mapped to Address 0 at Reset

                AREA     RESET, DATA, READONLY
                EXPORT   __Vectors
                EXPORT   __Vectors_End
                EXPORT   __Vectors_Size
                

__Vectors       DCD      __initial_sp                        ;     Top of Stack
                DCD      Reset_Handler                       ;     Reset Handler
                DCD      NMI_Handler                         ; -14 NMI Handler
                DCD      HardFault_Handler                   ; -13 Hard Fault Handler
                DCD      0                                   ;     Reserved
                DCD      0                                   ;     Reserved
                DCD      0                                   ;     Reserved
                DCD      0                                   ;     Reserved
                DCD      0                                   ;     Reserved
                DCD      0                                   ;     Reserved
                DCD      0                                   ;     Reserved
                DCD      SVC_Handler                         ;  -5 SVCall Handler
                DCD      0                                   ;     Reserved
                DCD      0                                   ;     Reserved
                DCD      PendSV_Handler                      ;  -2 PendSV Handler
                DCD      SysTick_Handler                     ;  -1 SysTick Handler

                ; Interrupts
                DCD      PHY0_IRQ_Handler                    ; bit0
                DCD      PHY1_IRQ_Handler                    ; bit1 
                DCD      LP_PAD_WAKEUP_IRQ_Handler           ; bit2
                DCD      LP_PAD_STOP2_WAKEUP_IRQ_Handler     ; bit3
                DCD      I2C0_IRQ_Handler                    ; bit4
                DCD      I2C1_IRQ_Handler                    ; bit5
                DCD      FLASH_CTRL_IRQ_Handler              ; bit6
                DCD      UART0_IRQ_Handler                   ; bit7
                DCD      UART1_IRQ_Handler                   ; bit8
                DCD      RFANA_IRQ_Handler                   ; bit9
                DCD      SPI0_IRQ_Handler                    ; bit10
                DCD      SPI1_IRQ_Handler                    ; bit11
                DCD      LPUART_IRQ_Handler                  ; bit12
                DCD      TIMER0_IRQ_Handler                  ; bit13
                DCD      TIMER1_IRQ_Handler                  ; bit14
                DCD      TIMER2_IRQ_Handler                  ; bit15
                DCD      TIMER3_IRQ_Handler                  ; bit16
                DCD      GPIO_IRQ_Handler                    ; bit17
                DCD      LPTIMER0_IRQ_Handler                ; bit18
                DCD      LPTIMER1_IRQ_Handler                ; bit19
                DCD      CTIMER_IRQ_Handler                  ; bit20
                DCD      WTD_IRQ_Handler                     ; bit21
                DCD      IWTD_IRQ_Handler                    ; bit22
                DCD      DMA_IRQ_Handler                     ; bit23
                DCD      SECURE_IRQ_Handler                  ; bit24
                DCD      SLOW_UPDATE_IRQ_Handler             ; bit25
                DCD      RTC_IRQ_Handler                     ; bit26



__Vectors_End
__Vectors_Size  EQU      __Vectors_End - __Vectors


                AREA     |.text|, CODE, READONLY

; Reset Handler
Reset_Handler   PROC
                EXPORT   Reset_Handler
                IMPORT   SystemInit
                IMPORT   __main

                LDR      R0, =__initial_sp
                MSR      MSP, R0

                LDR      R0, =SystemInit
                BLX      R0
                LDR      R0, =__main
                BX       R0
                ENDP

; NMI_Handler
NMI_Handler PROC
                EXPORT  NMI_Handler
;                IMPORT  NMI_Handler_Proc
;                
;                MOV     R0, LR
;                MRS     R1, MSP
;                MRS     R2, PSP
;                
;                LDR     R3, =NMI_Handler_Proc
;                BLX     R3
                
                B        .
                ENDP

; HardFault_Handler
HardFault_Handler PROC
                EXPORT  HardFault_Handler
                IMPORT  HardFault_Handler_Proc
                
                MOV     R0, LR
                MRS     R1, MSP
                MRS     R2, PSP
                
                LDR     R3, =HardFault_Handler_Proc
                BLX     R3
                
                B        .
                
                ENDP

; Macro to define default exception/interrupt handlers.
; Default handler are weak symbols with an endless loop.
; They can be overwritten by real handlers.
                MACRO
                Set_Default_Handler  $Handler_Name
$Handler_Name   PROC
                EXPORT   $Handler_Name             [WEAK]
                LDR      r4, =HardFault_Handler
                BLX      r4
                B        .
                ENDP
                MEND


; Default exception/interrupt handler
;                Set_Default_Handler NMI_Handler
;                Set_Default_Handler HardFault_Handler
                Set_Default_Handler SVC_Handler
                Set_Default_Handler PendSV_Handler
                Set_Default_Handler SysTick_Handler

                ; set default handler for others
                Set_Default_Handler PHY0_IRQ_Handler
                Set_Default_Handler PHY1_IRQ_Handler
                Set_Default_Handler LP_PAD_WAKEUP_IRQ_Handler
                Set_Default_Handler LP_PAD_STOP2_WAKEUP_IRQ_Handler
                Set_Default_Handler I2C0_IRQ_Handler
                Set_Default_Handler I2C1_IRQ_Handler
                Set_Default_Handler FLASH_CTRL_IRQ_Handler
                Set_Default_Handler UART0_IRQ_Handler
                Set_Default_Handler UART1_IRQ_Handler
                Set_Default_Handler RFANA_IRQ_Handler
                Set_Default_Handler SPI0_IRQ_Handler
                Set_Default_Handler SPI1_IRQ_Handler
                Set_Default_Handler LPUART_IRQ_Handler
                Set_Default_Handler TIMER0_IRQ_Handler
                Set_Default_Handler TIMER1_IRQ_Handler
                Set_Default_Handler TIMER2_IRQ_Handler
                Set_Default_Handler TIMER3_IRQ_Handler
                Set_Default_Handler GPIO_IRQ_Handler
                Set_Default_Handler LPTIMER0_IRQ_Handler
                Set_Default_Handler LPTIMER1_IRQ_Handler
                Set_Default_Handler CTIMER_IRQ_Handler
                Set_Default_Handler WTD_IRQ_Handler
                Set_Default_Handler IWTD_IRQ_Handler
                Set_Default_Handler DMA_IRQ_Handler
                Set_Default_Handler SECURE_IRQ_Handler
                Set_Default_Handler SLOW_UPDATE_IRQ_Handler
                Set_Default_Handler RTC_IRQ_Handler
                ALIGN


; User setup Stack & Heap

                IF       :LNOT::DEF:__MICROLIB
                IMPORT   __use_two_region_memory
                ENDIF

                EXPORT   __stack_limit
                EXPORT   __initial_sp
                IF       Heap_Size != 0                      ; Heap is provided
                EXPORT   __heap_base
                EXPORT   __heap_limit
                ENDIF

                END
