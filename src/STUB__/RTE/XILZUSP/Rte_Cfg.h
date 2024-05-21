/******************************************************************************
 ** File: Rte_Cfg.h
 ** Role: Provide the configuration of the RTE
 ** Generated by Dassault Systèmes Rte Generator
 ** Date: 2023-02-09 08:56:35
 ** Template Version: 0.1
 **
 ** Controller / Compiler / Hardware independent
 **
 ** Copyright 2012 by Dassault Systèmes
 ** All rights exclusively reserved for Dassault Systèmes,
 ** unless expressly agreed to otherwise.
 ** Traceability: [rte sws 1003][rte sws 1321][COMPILER055][COMPILER054]
*******************************************************************************/
#ifndef RTE_CFG_H
#define RTE_CFG_H

#include "Std_Types.h" 	/*Traceability [SWS_Rte_07641]*/



/* VFB Tracing functions definition for Runnables */
#define Rte_Runnable_SYS_TIME_ADAPTER_SysTime_GetHwCounterInTicks_Return /*XXXXXXXXXX*/
#define Rte_Runnable_SYS_TIME_ADAPTER_SysTime_GetHwCounterInTicks_Start /*XXXXXXXXXX*/
#define Rte_Runnable_SYS_TIME_ADAPTER_SysTime_GetTimeInTicks_Return /*XXXXXXXXXX*/
#define Rte_Runnable_SYS_TIME_ADAPTER_SysTime_GetTimeInTicks_Start /*XXXXXXXXXX*/
#define Rte_Runnable_SYS_TIME_ADAPTER_SysTime_GetTime_us_Return /*XXXXXXXXXX*/
#define Rte_Runnable_SYS_TIME_ADAPTER_SysTime_GetTime_us_Start /*XXXXXXXXXX*/
#define Rte_Runnable_SYS_TIME_ADAPTER_SysTime_GetTimeSinceBoot_ms_Return /*XXXXXXXXXX*/
#define Rte_Runnable_SYS_TIME_ADAPTER_SysTime_GetTimeSinceBoot_ms_Start /*XXXXXXXXXX*/
#define Rte_Runnable_SYS_TIME_ADAPTER_SysTime_GetTimeSince_ms_Return /*XXXXXXXXXX*/
#define Rte_Runnable_SYS_TIME_ADAPTER_SysTime_GetTimeSince_ms_Start /*XXXXXXXXXX*/
#define Rte_Runnable_SYS_TIME_ADAPTER_SysTime_GetTimeSince_us_Return /*XXXXXXXXXX*/
#define Rte_Runnable_SYS_TIME_ADAPTER_SysTime_GetTimeSince_us_Start /*XXXXXXXXXX*/
#define Rte_Runnable_SYS_TIME_ADAPTER_SysTime_TimeDelay_Return /*XXXXXXXXXX*/
#define Rte_Runnable_SYS_TIME_ADAPTER_SysTime_TimeDelay_Start /*XXXXXXXXXX*/
#define Rte_Runnable_SYS_TIME_ADAPTER_SysTime_TimeInit_Return /*XXXXXXXXXX*/
#define Rte_Runnable_SYS_TIME_ADAPTER_SysTime_TimeInit_Start /*XXXXXXXXXX*/
#define Rte_Runnable_SYS_KPI_ADAPTER_SysKpi_GetIndexOfLastEntry_Return /*XXXXXXXXXX*/
#define Rte_Runnable_SYS_KPI_ADAPTER_SysKpi_GetIndexOfLastEntry_Start /*XXXXXXXXXX*/
#define Rte_Runnable_SYS_KPI_ADAPTER_SysKpi_GetIndexedStartupEntry_Return /*XXXXXXXXXX*/
#define Rte_Runnable_SYS_KPI_ADAPTER_SysKpi_GetIndexedStartupEntry_Start /*XXXXXXXXXX*/
#define Rte_Runnable_SYS_KPI_ADAPTER_SysKpi_WriteStartupEntry_Return /*XXXXXXXXXX*/
#define Rte_Runnable_SYS_KPI_ADAPTER_SysKpi_WriteStartupEntry_Start /*XXXXXXXXXX*/
#define Rte_Runnable_SYS_EXT_ADAPTER_SysExt_FPU_FailureExceptionReaction_Return /*XXXXXXXXXX*/
#define Rte_Runnable_SYS_EXT_ADAPTER_SysExt_FPU_FailureExceptionReaction_Start /*XXXXXXXXXX*/
#define Rte_Runnable_SYS_EXT_ADAPTER_SysExt_GetCoreId_Return /*XXXXXXXXXX*/
#define Rte_Runnable_SYS_EXT_ADAPTER_SysExt_GetCoreId_Start /*XXXXXXXXXX*/
#define Rte_Runnable_SYS_EXT_ADAPTER_SysExt_GetHardwareSample_Return /*XXXXXXXXXX*/
#define Rte_Runnable_SYS_EXT_ADAPTER_SysExt_GetHardwareSample_Start /*XXXXXXXXXX*/
#define Rte_Runnable_SYS_EXT_ADAPTER_SysExt_StoreCurrentStateToPMU_Return /*XXXXXXXXXX*/
#define Rte_Runnable_SYS_EXT_ADAPTER_SysExt_StoreCurrentStateToPMU_Start /*XXXXXXXXXX*/
#define Rte_Runnable_DMA_ADAPTER_Dma_Init_Return /*XXXXXXXXXX*/
#define Rte_Runnable_DMA_ADAPTER_Dma_Init_Start /*XXXXXXXXXX*/
#define Rte_Runnable_DMA_ADAPTER_Dma_MemCpy_Return /*XXXXXXXXXX*/
#define Rte_Runnable_DMA_ADAPTER_Dma_MemCpy_Start /*XXXXXXXXXX*/
#define Rte_Runnable_DMA_ADAPTER_Dma_ShutDown_Return /*XXXXXXXXXX*/
#define Rte_Runnable_DMA_ADAPTER_Dma_ShutDown_Start /*XXXXXXXXXX*/
#define Rte_Runnable_SYS_PAL_ADAPTER_SysPal_GetCoreId_Return /*XXXXXXXXXX*/
#define Rte_Runnable_SYS_PAL_ADAPTER_SysPal_GetCoreId_Start /*XXXXXXXXXX*/
#define Rte_Runnable_SYS_PAL_ADAPTER_SysPal_GetHardwareSample_Return /*XXXXXXXXXX*/
#define Rte_Runnable_SYS_PAL_ADAPTER_SysPal_GetHardwareSample_Start /*XXXXXXXXXX*/
#define Rte_Runnable_SYS_PAL_ADAPTER_SysPal_GetTicksElapsed_Return /*XXXXXXXXXX*/
#define Rte_Runnable_SYS_PAL_ADAPTER_SysPal_GetTicksElapsed_Start /*XXXXXXXXXX*/
#define Rte_Runnable_SYS_PAL_ADAPTER_SysPal_GetTimeInTicks_Return /*XXXXXXXXXX*/
#define Rte_Runnable_SYS_PAL_ADAPTER_SysPal_GetTimeInTicks_Start /*XXXXXXXXXX*/
#define Rte_Runnable_SYS_PAL_ADAPTER_SysPal_GetTimeInUs_Return /*XXXXXXXXXX*/
#define Rte_Runnable_SYS_PAL_ADAPTER_SysPal_GetTimeInUs_Start /*XXXXXXXXXX*/
#define Rte_Runnable_SYS_PAL_ADAPTER_SysPal_StartIntervalTimer_Return /*XXXXXXXXXX*/
#define Rte_Runnable_SYS_PAL_ADAPTER_SysPal_StartIntervalTimer_Start /*XXXXXXXXXX*/
#define Rte_Runnable_SYS_PAL_ADAPTER_SysPal_TicksToUs_Return /*XXXXXXXXXX*/
#define Rte_Runnable_SYS_PAL_ADAPTER_SysPal_TicksToUs_Start /*XXXXXXXXXX*/
#define Rte_Runnable_SYS_PAL_ADAPTER_SysPal_TimeInit_Return /*XXXXXXXXXX*/
#define Rte_Runnable_SYS_PAL_ADAPTER_SysPal_TimeInit_Start /*XXXXXXXXXX*/
#define Rte_Runnable_QSF_ADAPTER_QsfA_Fls_BlankCheck_Return /*XXXXXXXXXX*/
#define Rte_Runnable_QSF_ADAPTER_QsfA_Fls_BlankCheck_Start /*XXXXXXXXXX*/
#define Rte_Runnable_QSF_ADAPTER_QsfA_Fls_Cancel_Return /*XXXXXXXXXX*/
#define Rte_Runnable_QSF_ADAPTER_QsfA_Fls_Cancel_Start /*XXXXXXXXXX*/
#define Rte_Runnable_QSF_ADAPTER_QsfA_Fls_Compare_Return /*XXXXXXXXXX*/
#define Rte_Runnable_QSF_ADAPTER_QsfA_Fls_Compare_Start /*XXXXXXXXXX*/
#define Rte_Runnable_QSF_ADAPTER_QsfA_Fls_Erase_Return /*XXXXXXXXXX*/
#define Rte_Runnable_QSF_ADAPTER_QsfA_Fls_Erase_Start /*XXXXXXXXXX*/
#define Rte_Runnable_QSF_ADAPTER_QsfA_Fls_Erase_4K_Return /*XXXXXXXXXX*/
#define Rte_Runnable_QSF_ADAPTER_QsfA_Fls_Erase_4K_Start /*XXXXXXXXXX*/
#define Rte_Runnable_QSF_ADAPTER_QsfA_Fls_GetJobResult_Return /*XXXXXXXXXX*/
#define Rte_Runnable_QSF_ADAPTER_QsfA_Fls_GetJobResult_Start /*XXXXXXXXXX*/
#define Rte_Runnable_QSF_ADAPTER_QsfA_Fls_GetSectorSize_Return /*XXXXXXXXXX*/
#define Rte_Runnable_QSF_ADAPTER_QsfA_Fls_GetSectorSize_Start /*XXXXXXXXXX*/
#define Rte_Runnable_QSF_ADAPTER_QsfA_Fls_GetStatus_Return /*XXXXXXXXXX*/
#define Rte_Runnable_QSF_ADAPTER_QsfA_Fls_GetStatus_Start /*XXXXXXXXXX*/
#define Rte_Runnable_QSF_ADAPTER_QsfA_Fls_GetVersionInfo_Return /*XXXXXXXXXX*/
#define Rte_Runnable_QSF_ADAPTER_QsfA_Fls_GetVersionInfo_Start /*XXXXXXXXXX*/
#define Rte_Runnable_QSF_ADAPTER_QsfA_Fls_Init_Return /*XXXXXXXXXX*/
#define Rte_Runnable_QSF_ADAPTER_QsfA_Fls_Init_Start /*XXXXXXXXXX*/
#define Rte_Runnable_QSF_ADAPTER_QsfA_Fls_MainFunction_Return /*XXXXXXXXXX*/
#define Rte_Runnable_QSF_ADAPTER_QsfA_Fls_MainFunction_Start /*XXXXXXXXXX*/
#define Rte_Runnable_QSF_ADAPTER_QsfA_Fls_Read_Return /*XXXXXXXXXX*/
#define Rte_Runnable_QSF_ADAPTER_QsfA_Fls_Read_Start /*XXXXXXXXXX*/
#define Rte_Runnable_QSF_ADAPTER_QsfA_Fls_SetMode_Return /*XXXXXXXXXX*/
#define Rte_Runnable_QSF_ADAPTER_QsfA_Fls_SetMode_Start /*XXXXXXXXXX*/
#define Rte_Runnable_QSF_ADAPTER_QsfA_Fls_Write_Return /*XXXXXXXXXX*/
#define Rte_Runnable_QSF_ADAPTER_QsfA_Fls_Write_Start /*XXXXXXXXXX*/
#define Rte_Runnable_CACHE_API_ADAPTER_Cache_Invalidate_Return /*XXXXXXXXXX*/
#define Rte_Runnable_CACHE_API_ADAPTER_Cache_Invalidate_Start /*XXXXXXXXXX*/
#define Rte_Runnable_CACHE_API_ADAPTER_Cache_WriteBack_Return /*XXXXXXXXXX*/
#define Rte_Runnable_CACHE_API_ADAPTER_Cache_WriteBack_Start /*XXXXXXXXXX*/
#define Rte_Runnable_CACHE_API_ADAPTER_Cache_WriteBackAndInvalidate_Return /*XXXXXXXXXX*/
#define Rte_Runnable_CACHE_API_ADAPTER_Cache_WriteBackAndInvalidate_Start /*XXXXXXXXXX*/

/* Condition Value Macro Definition [SWS_Rte_06513] */

/* Hook functions configured by the user */

#endif /* ifndef RTE_CFG_H  */

/* End of File Rte_Cfg.h======================================================*/
