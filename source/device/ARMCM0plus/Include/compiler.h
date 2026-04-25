/**
 * ****************************************************************************
 * @file complier.h
 * @author QMX software team. 
 * @brief 
 * @version  1.0
 * ****************************************************************************
 * 
 * Copyright (c) 2026 QMX Semiconductor all rights reserved.
 * 
 * ****************************************************************************
 */

#ifndef __COMPILER_H__
#define __COMPILER_H__

#ifdef __cplusplus
extern "C"
{
#endif

#define __ALIGN4            __attribute__((aligned(4)))
#define __ALIGN8            __attribute__((aligned(8)))

#define __MAYBE_UNUSED      __attribute__((unused))

#ifdef ROM_LIB_PROJECT
#define __ROM_RAM_SHARED    __attribute__ ((section(".rom.shared")))
#endif

#define __RAM_FUNC          __attribute__ ((section(".ram.function")))
#define __RETENTION_FUNC    __attribute__ ((section(".retention.func")))
#define __RETENTION_DATA    __attribute__ ((section(".retention.data")))

#ifdef __cplusplus
}
#endif


#endif
