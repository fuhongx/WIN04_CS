/******************************************************************************
 * @file     gpio_example.ld
 * @brief    GNU Linker Script for Cortex-M based device
 * @version  V2.0.0
 * @date     21. May 2019
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

/*
 *-------- <<< Use Configuration Wizard in Context Menu >>> -------------------
 */

/* Boot2 Configuration */
__ROM_BASE  = 0x08000000;
__BOOT2_LEN = 0x00001000; /* 4KB */

/* App Configuration */
__APP_BASE  = 0x08001000;
__APPHEADER_SIZE = 0x18;
__APP_LEN   = 0x0001DFE8;

/* RAM Configuration */
__RAM_BASE          = 0x20000000;
__RAM_VECTOR_LEN    = 0x00000200;
__RAM_RETENTION_LEN = 0x00001E00; /* 0x20002000 - 0x20000200 */
__RAM_APP_CODE_LEN  = 0x00001000; /* 0x20005800 - 0x20004800 */
__RAM_APP_DATA_LEN  = 0x00008800; /* 0x2000E000 - 0x20005800 */

/* Stack and Heap */
__STACK_SIZE = 0x00002000; /* 0x20004000 - 0x20002000 */
__HEAP_SIZE  = 0x00002000; /* 0x20010000 - 0x2000E000 */

/*
 *-------------------- <<< end of configuration section >>> -------------------
 */

MEMORY
{
  /* FLASH Regions */
  FLASH_BOOT2 (rx)  : ORIGIN = __ROM_BASE,          LENGTH = __BOOT2_LEN
  FLASH_APP_HEADER (rx): ORIGIN = __APP_BASE,       LENGTH = __APPHEADER_SIZE
  FLASH_APP   (rx)  : ORIGIN = __APP_BASE + __APPHEADER_SIZE, LENGTH = __APP_LEN

  /* RAM Regions */
  RAM_VECTOR    (rwx) : ORIGIN = 0x20000000, LENGTH = __RAM_VECTOR_LEN
  RAM_RETENTION (rwx) : ORIGIN = 0x20000200, LENGTH = __RAM_RETENTION_LEN
  RAM_APP_CODE  (rwx) : ORIGIN = 0x20004800, LENGTH = __RAM_APP_CODE_LEN
  RAM_APP_DATA  (rwx) : ORIGIN = 0x20005800, LENGTH = __RAM_APP_DATA_LEN
  RAM_HEAP      (rw)  : ORIGIN = 0x2000E000, LENGTH = __HEAP_SIZE
  RAM_STACK     (rw)  : ORIGIN = 0x20002000, LENGTH = __STACK_SIZE
}

/* Linker script to place sections and symbol values. Should be used together
 * with other linker script that defines memory regions FLASH and RAM.
 * It references following symbols, which must be defined in code:
 *   Reset_Handler : Entry of reset handler
 *
 * It defines following symbols, which code can use without definition:
 *   __exidx_start
 *   __exidx_end
 *   __copy_table_start__
 *   __copy_table_end__
 *   __zero_table_start__
 *   __zero_table_end__
 *   __etext
 *   __data_start__
 *   __preinit_array_start
 *   __preinit_array_end
 *   __init_array_start
 *   __init_array_end
 *   __fini_array_start
 *   __fini_array_end
 *   __data_end__
 *   __bss_start__
 *   __bss_end__
 *   __end__
 *   end
 *   __HeapLimit
 *   __StackLimit
 *   __StackTop
 *   __stack
 */
GROUP(libgcc.a libc.a libm.a libnosys.a)

ENTRY(Reset_Handler)

SECTIONS
{
  .boot :
  {
    KEEP(*(.boot2.code))
  } > FLASH_BOOT2

  .firmware_header :
  {
    KEEP(*(.firmware.header))
  } > FLASH_APP_HEADER

  .text :
  {
    KEEP(*(.vectors))
    startup_*.o(.text*)
    *(.text*)

    KEEP(*(.init))
    KEEP(*(.fini))

    /* .ctors */
    *crtbegin.o(.ctors)
    *crtbegin?.o(.ctors)
    *(EXCLUDE_FILE(*crtend?.o *crtend.o) .ctors)
    *(SORT(.ctors.*))
    *(.ctors)

    /* .dtors */
    *crtbegin.o(.dtors)
    *crtbegin?.o(.dtors)
    *(EXCLUDE_FILE(*crtend?.o *crtend.o) .dtors)
    *(SORT(.dtors.*))
    *(.dtors)

    *(.rodata*)
    KEEP(*(.eh_frame*))
  } > FLASH_APP

  /*
   * SG veneers:
   * All SG veneers are placed in the special output section .gnu.sgstubs. Its start address
   * must be set, either with the command line option ?-section-start?or in a linker script,
   * to indicate where to place these veneers in memory.
   */

  .ARM.extab :
  {
    *(.ARM.extab* .gnu.linkonce.armextab.*)
  } > FLASH_APP

  __exidx_start = .;
  .ARM.exidx :
  {
    *(.ARM.exidx* .gnu.linkonce.armexidx.*)
  } > FLASH_APP
  __exidx_end = .;

  /**
   * Location counter can end up 2byte aligned with narrow Thumb code but
   * __etext is assumed by startup code to be the LMA of a section in RAM
   * which must be 4byte aligned 
   */
  __etext = ALIGN (4);

  .ram_vector :
  {
    . = ALIGN(4);
    __ram_vector_start__ = .;
    *(.bss.new_vector)
    . = ALIGN(4);
    __ram_vector_end__ = .;
  } > RAM_VECTOR

  .data :
  {
    __data_start__ = .;
    *(vtable)
    *(.data)
    *(.data.*)
    . = ALIGN(4);

    /* preinit data */
    PROVIDE_HIDDEN (__preinit_array_start = .);
    KEEP(*(.preinit_array))
    PROVIDE_HIDDEN (__preinit_array_end = .);

    . = ALIGN(4);

    /* init data */
    PROVIDE_HIDDEN (__init_array_start = .);
    KEEP(*(SORT(.init_array.*)))
    KEEP(*(.init_array))
    PROVIDE_HIDDEN (__init_array_end = .);

    . = ALIGN(4);

    /* finit data */
    PROVIDE_HIDDEN (__fini_array_start = .);
    KEEP(*(SORT(.fini_array.*)))
    KEEP(*(.fini_array))
    PROVIDE_HIDDEN (__fini_array_end = .);

    KEEP(*(.jcr*))
    
    . = ALIGN(4);
    /* All data end */
    __data_end__ = .;

  } > RAM_APP_DATA AT > FLASH_APP

  __edata = ALIGN (4);

  /* RAM App Code (Functions in RAM) */
  .ramcode :
  {
    . = ALIGN(4);
    __ram_func_start__ = .;
    *(.ram.function)
    . = ALIGN(4);
    __ram_func_end__ = .;
  } > RAM_APP_CODE AT > FLASH_APP

  __eramcode = ALIGN (4);

  .retention :
  {
    . = ALIGN(4);
    __ram_retention_start__ = .;
    *(.retention.func)
    *(.retention.data)
    . = ALIGN(4);
    __ram_retention_end__ = .;
  } > RAM_RETENTION AT > FLASH_APP

  .bss (NOLOAD):
  {
    . = ALIGN(4);
    __bss_start__ = .;
    *(.bss)
    *(.bss.*)
    *(COMMON)
    . = ALIGN(4);
    __bss_end__ = .;
  } > RAM_APP_DATA AT > RAM_APP_DATA

  .ila (NOLOAD) :
  {
    __ila_start__ = .;
    *(.ram.ila)
    __ila_end__ = .;
  } > RAM_APP_DATA AT > RAM_APP_DATA

  .copy.table :
  {
    . = ALIGN(4);
    __copy_table_start__ = .;
    LONG (__etext)
    LONG (__data_start__)
    LONG (__data_end__ - __data_start__)
    LONG (__edata)
    LONG (__ram_func_start__)
    LONG (__ram_func_end__ - __ram_func_start__)
    LONG (__eramcode)
    LONG (__ram_retention_start__)
    LONG (__ram_retention_end__ - __ram_retention_start__)

    __copy_table_end__ = .;
  } > FLASH_APP

  .zero.table :
  {
    . = ALIGN(4);
    __zero_table_start__ = .;
    LONG (__bss_start__)
    LONG (__bss_end__ - __bss_start__)
    LONG (__ila_start__)
    LONG (__ila_end__ - __ila_start__)
    __zero_table_end__ = .;
  } > FLASH_APP

  .heap (COPY) :
  {
    . = ALIGN(8);
    __end__ = .;
    PROVIDE(end = .);
    . = . + __HEAP_SIZE;
    . = ALIGN(8);
    __HeapLimit = .;
  } > RAM_HEAP

  .stack (COPY) :
  {
    . = ALIGN(8);
    __StackLimit = .;
    . = . + __STACK_SIZE;
    . = ALIGN(8);
    __StackTop = .;
  } > RAM_STACK
  PROVIDE(__stack = __StackTop);
}
