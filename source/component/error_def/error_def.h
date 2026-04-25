/**
 * ****************************************************************************
 * @file error_def.h
 * @author SLC(SC) software team. 
 * @brief 
 * @version  1.0
 * ****************************************************************************
 * 
 * Copyright (c) 2024 SLC Semiconductor all rights reserved.
 * 
 * ****************************************************************************
 */

#ifndef __ERROR_DEF_H__
#define __ERROR_DEF_H__

#ifdef __cplusplus
extern "C"
{
#endif

typedef enum
{
    EN_ERROR_STA_OK           = 0,
    EN_ERROR_STA_TIMEOUT      = 1,
    EN_ERROR_STA_ERROR        = 2,
    EN_ERROR_STA_INVALID      = 3,
}EN_ERR_STA_T;

#define RETURN_IF_ERR(u8Ret)    \
    if(u8Ret == EN_ERROR_STA_OK)   \
    {                       \
        return EN_ERROR_STA_ERROR;    \
    }  


#ifdef __cplusplus
}
#endif

#endif


