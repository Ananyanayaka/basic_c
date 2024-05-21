/******************************************************************************
 ** File: Rte_QSF_ADAPTER.h
 ** Role: Header File containing the definition for the SWC: QSF_ADAPTER
 ** Generated by Dassault Systèmes Rte Generator
 ** Date: 2023-02-09 08:57:00
 ** Template Version: 0.1
 **
 ** Controller / Compiler / Hardware independent
 **
 ** Copyright 2012 by Dassault Systèmes
 ** All rights exclusively reserved for Dassault Systèmes,
 ** unless expressly agreed to otherwise.
 ** Traceability: [rte sws 1003][rte sws 3786][rte sws 1000][rte sws 1004]
**               [rte sws 1005][rte sws 1315][rte sws 1195]
*******************************************************************************/
#ifndef RTE_QSF_ADAPTER_H
#define RTE_QSF_ADAPTER_H

/** The declarations below are included only by the SwC, not by the RTE */
#ifndef RTE_C
/** Traceability [rte_sws_1006] */
#ifdef RTE_APPLICATION_HEADER_FILE
#error Multiple application header files included. see [rte_sws_1006].
#endif /* RTE_APPLICATION_HEADER_FILE */
#define RTE_APPLICATION_HEADER_FILE
#endif /* #ifndef RTE_C */

/** Traceability: [rte sws 3709]*/
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* Rte Generator V2 section ***************************************************/

/* Inclusions ================================================================*/
#include "Rte_QSF_ADAPTER_Type.h"        /* Applications Types Header File. Traceability [rte_sws_7131]*/
#ifdef INCLUDE_USER_HEADER /* User include for TypeEmitter */
#include INCLUDE_USER_HEADER
#endif
#include "Rte_DataHandleType.h"        /* Applications Types Header File. Traceability [rte_sws_7924]*/

#ifndef RTE_GENERATION_PHASE

/* Rte Contract Phase API ====================================================*/

#endif /* #ifdef RTE_GENERATION_PHASE */


/* Condition Value Macros=====================================================*/

/* Port Data Structures: [SWS_Rte_07137]======================================*/

/* Component Data Structure ========*/
typedef struct {

	/* Data Handles section Traceability [rte_sws_3718]==========================*/

	/* Per Instance Memory Handles section Traceability [rte_sws_3719]===========*/

	/* Implicit Inter-runnable Variable Handles section Traceability [rte_sws_1349]=======*/

	/* Calibration Parameter Handles section Traceability [rte_sws_3720][SWS_Rte_03835][SWS_Rte_03949][SWS_Rte_08782]=========*/

	/* Exclusive-area API section Traceability [rte_sws_3721]====================*/


	/* Port API section Traceability [SWS_Rte_07137]======================*/

	/* inter Runnable Variable section for SwComponent which supports MultipleInstatiation Traceability [rte_sws_3717]===============*/

	/* inter Runnable Triggering API section Traceability [rte_sws_7225]=========*/

	/* Instance ID Section Traceability [SWS_Rte_07838]=========*/

	/* Vendor specific section Traceability [rte_sws_3722]=======================*/

	/* _dummy initialization */ /* Traceability [rte sws 3724] ==================*/
	uint8 _dummy;
}Rte_CDS_QSF_ADAPTER;
/** The declarations below are only included by the SwC, not by the RTE */
#ifndef RTE_C

/* Port Handles: [SWS_Rte_01053][SWS_Rte_01343][SWS_Rte_01342]================*/

/* Instance handle type ======================================================*/
/** The declarations below are included only by the SwC, not by the RTE */
/* [SWS_Rte_01148][SWS_Rte_01150][SWS_Rte_06810] */
typedef CONSTP2CONST(Rte_CDS_QSF_ADAPTER, AUTOMATIC, RTE_CONST) Rte_Instance;
/* [SWS_Rte_03793] */
extern Rte_Instance Rte_Inst_QSF_ADAPTER;

#ifdef RTE_GENERATION_PHASE

/* API mapping ===============================================================*/

/* API functions declaration =================================================*/

#define RTE_START_SEC_CODE
#include "MemMap.h"


#define RTE_STOP_SEC_CODE
#include "MemMap.h"
/* External Datas *************************************************************/

/* IRV declaration -----------------------------------------------------------*/

/* IRV local buffer Definition for Implicit Communication --------------------*/

#endif /* #ifndef RTE_C */


#endif /* #ifdef RTE_GENERATION_PHASE */

/* Initial Values Traceability: [rte_sws_5078]================================*/


/* PerInstanceMemory Traceability: [rte_sws_3782]=============================*/


/* Application Errors Traceability: [rte_sws_2575][rte_sws_2576]==============*/
#define RTE_E_CSI_QSF_RTE_E_NOT_OK 1U

/* Runnable Entity Prototypes ======================================================*/
/** Traceability [SWS_Rte_01126][rte_sws_6713][rte_sws_1132][rte_sws_1274][rte_sws_7194] */
#define RTE_RUNNABLE_QsfA_Fls_BlankCheck QsfA_Fls_BlankCheck
#define QSF_ADAPTER_START_SEC_CODE
#include "QSF_ADAPTER_MemMap.h"
FUNC(Std_ReturnType, QSF_ADAPTER_CODE) QsfA_Fls_BlankCheck (VAR(QsfA_Fls_AddressType, AUTOMATIC), VAR(QsfA_Fls_LengthType, AUTOMATIC));
#define QSF_ADAPTER_STOP_SEC_CODE
#include "QSF_ADAPTER_MemMap.h"

#define RTE_RUNNABLE_QsfA_Fls_Cancel QsfA_Fls_Cancel
#define QSF_ADAPTER_START_SEC_CODE
#include "QSF_ADAPTER_MemMap.h"
FUNC(Std_ReturnType, QSF_ADAPTER_CODE) QsfA_Fls_Cancel (void);
#define QSF_ADAPTER_STOP_SEC_CODE
#include "QSF_ADAPTER_MemMap.h"

#define RTE_RUNNABLE_QsfA_Fls_Compare QsfA_Fls_Compare
#define QSF_ADAPTER_START_SEC_CODE
#include "QSF_ADAPTER_MemMap.h"
FUNC(Std_ReturnType, QSF_ADAPTER_CODE) QsfA_Fls_Compare (VAR(QsfA_Fls_AddressType, AUTOMATIC), VAR(p_uint8, AUTOMATIC), VAR(QsfA_Fls_LengthType, AUTOMATIC));
#define QSF_ADAPTER_STOP_SEC_CODE
#include "QSF_ADAPTER_MemMap.h"

#define RTE_RUNNABLE_QsfA_Fls_Erase QsfA_Fls_Erase
#define QSF_ADAPTER_START_SEC_CODE
#include "QSF_ADAPTER_MemMap.h"
FUNC(Std_ReturnType, QSF_ADAPTER_CODE) QsfA_Fls_Erase (VAR(QsfA_Fls_AddressType, AUTOMATIC), VAR(QsfA_Fls_LengthType, AUTOMATIC));
#define QSF_ADAPTER_STOP_SEC_CODE
#include "QSF_ADAPTER_MemMap.h"

#define RTE_RUNNABLE_QsfA_Fls_Erase_4K QsfA_Fls_Erase_4K
#define QSF_ADAPTER_START_SEC_CODE
#include "QSF_ADAPTER_MemMap.h"
FUNC(Std_ReturnType, QSF_ADAPTER_CODE) QsfA_Fls_Erase_4K (VAR(QsfA_Fls_AddressType, AUTOMATIC), VAR(QsfA_Fls_LengthType, AUTOMATIC));
#define QSF_ADAPTER_STOP_SEC_CODE
#include "QSF_ADAPTER_MemMap.h"

#define RTE_RUNNABLE_QsfA_Fls_GetJobResult QsfA_Fls_GetJobResult
#define QSF_ADAPTER_START_SEC_CODE
#include "QSF_ADAPTER_MemMap.h"
FUNC(Std_ReturnType, QSF_ADAPTER_CODE) QsfA_Fls_GetJobResult (P2VAR(QsfA_MemIf_JobResultType, AUTOMATIC, QSF_ADAPTER_APPL_DATA));
#define QSF_ADAPTER_STOP_SEC_CODE
#include "QSF_ADAPTER_MemMap.h"

#define RTE_RUNNABLE_QsfA_Fls_GetSectorSize QsfA_Fls_GetSectorSize
#define QSF_ADAPTER_START_SEC_CODE
#include "QSF_ADAPTER_MemMap.h"
FUNC(Std_ReturnType, QSF_ADAPTER_CODE) QsfA_Fls_GetSectorSize (P2VAR(uint32, AUTOMATIC, QSF_ADAPTER_APPL_DATA));
#define QSF_ADAPTER_STOP_SEC_CODE
#include "QSF_ADAPTER_MemMap.h"

#define RTE_RUNNABLE_QsfA_Fls_GetStatus QsfA_Fls_GetStatus
#define QSF_ADAPTER_START_SEC_CODE
#include "QSF_ADAPTER_MemMap.h"
FUNC(Std_ReturnType, QSF_ADAPTER_CODE) QsfA_Fls_GetStatus (P2VAR(QsfA_MemIf_StatusType, AUTOMATIC, QSF_ADAPTER_APPL_DATA));
#define QSF_ADAPTER_STOP_SEC_CODE
#include "QSF_ADAPTER_MemMap.h"

#define RTE_RUNNABLE_QsfA_Fls_GetVersionInfo QsfA_Fls_GetVersionInfo
#define QSF_ADAPTER_START_SEC_CODE
#include "QSF_ADAPTER_MemMap.h"
FUNC(Std_ReturnType, QSF_ADAPTER_CODE) QsfA_Fls_GetVersionInfo (P2VAR(QsfA_Std_VersionInfoType, AUTOMATIC, QSF_ADAPTER_APPL_DATA));
#define QSF_ADAPTER_STOP_SEC_CODE
#include "QSF_ADAPTER_MemMap.h"

#define RTE_RUNNABLE_QsfA_Fls_Init QsfA_Fls_Init
#define QSF_ADAPTER_START_SEC_CODE
#include "QSF_ADAPTER_MemMap.h"
FUNC(Std_ReturnType, QSF_ADAPTER_CODE) QsfA_Fls_Init (P2CONST(QsfA_Fls_ConfigType, AUTOMATIC, QSF_ADAPTER_APPL_CONST));
#define QSF_ADAPTER_STOP_SEC_CODE
#include "QSF_ADAPTER_MemMap.h"

#define RTE_RUNNABLE_QsfA_Fls_MainFunction QsfA_Fls_MainFunction
#define QSF_ADAPTER_START_SEC_CODE
#include "QSF_ADAPTER_MemMap.h"
FUNC(void, QSF_ADAPTER_CODE) QsfA_Fls_MainFunction (void);
#define QSF_ADAPTER_STOP_SEC_CODE
#include "QSF_ADAPTER_MemMap.h"

#define RTE_RUNNABLE_QsfA_Fls_Read QsfA_Fls_Read
#define QSF_ADAPTER_START_SEC_CODE
#include "QSF_ADAPTER_MemMap.h"
FUNC(Std_ReturnType, QSF_ADAPTER_CODE) QsfA_Fls_Read (VAR(QsfA_Fls_AddressType, AUTOMATIC), VAR(p_uint8, AUTOMATIC), VAR(QsfA_Fls_LengthType, AUTOMATIC));
#define QSF_ADAPTER_STOP_SEC_CODE
#include "QSF_ADAPTER_MemMap.h"

#define RTE_RUNNABLE_QsfA_Fls_SetMode QsfA_Fls_SetMode
#define QSF_ADAPTER_START_SEC_CODE
#include "QSF_ADAPTER_MemMap.h"
FUNC(Std_ReturnType, QSF_ADAPTER_CODE) QsfA_Fls_SetMode (VAR(QsfA_MemIf_ModeType, AUTOMATIC));
#define QSF_ADAPTER_STOP_SEC_CODE
#include "QSF_ADAPTER_MemMap.h"

#define RTE_RUNNABLE_QsfA_Fls_Write QsfA_Fls_Write
#define QSF_ADAPTER_START_SEC_CODE
#include "QSF_ADAPTER_MemMap.h"
FUNC(Std_ReturnType, QSF_ADAPTER_CODE) QsfA_Fls_Write (VAR(QsfA_Fls_AddressType, AUTOMATIC), VAR(p_uint8, AUTOMATIC), VAR(QsfA_Fls_LengthType, AUTOMATIC));
#define QSF_ADAPTER_STOP_SEC_CODE
#include "QSF_ADAPTER_MemMap.h"


/* End Rte Generator V2 section ***********************************************/

/** Traceability: [rte sws 3710]*/
#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

/* Exclusive Area macro-------------------------------------------------------*/

#endif /* RTE_QSF_ADAPTER_H */

/* End of File Rte_QSF_ADAPTER.h==============================================*/
