/******************************************************************************
 ** File: Rte.h
 ** Role: H module containing the generated RTE
 ** Generated by Dassault Systèmes Rte Generator
 ** Date: 2023-02-09 08:55:42
 ** Template Version: 0.1
 **
 ** Controller / Compiler / Hardware independent
 **
 ** Copyright 2012 by Dassault Systèmes
 ** All rights exclusively reserved for Dassault Systèmes,
 ** unless expressly agreed to otherwise.
 ** Traceability: [rte sws 1157][rte sws 1164]
*******************************************************************************/
#ifndef RTE_H
#define RTE_H

/* Inclusions=================================================================*/
#include "Std_Types.h"               /* BSW Standard Type header file *//** Traceability: [rte sws 1164]*/

/* Definition of RTE Module===================================================*/

/* Definition of Client Server Predefined Error Codes - Traceability: [SWS_Rte_01269]*/
/** Traceability: [rte sws 1058]*/
#define RTE_E_OK ((Std_ReturnType) 0)
/** Traceability: [rte sws 2594]*/
#define RTE_E_INVALID ((Std_ReturnType) 1)
/** Traceability: [rte sws 1060]*/
#define RTE_E_COM_STOPPED ((Std_ReturnType) 128)
/** Traceability: [rte sws 1064]*/
#define RTE_E_TIMEOUT ((Std_ReturnType) 129)
/** Traceability: [rte sws 1317]*/
#define RTE_E_LIMIT ((Std_ReturnType) 130)
/** Traceability: [rte sws 1061]*/
#define RTE_E_NO_DATA ((Std_ReturnType) 131)
/** Traceability: [rte sws 1065]*/
#define RTE_E_TRANSMIT_ACK ((Std_ReturnType) 132)
/** Traceability: [SWS_Rte_07384]*/
#define RTE_E_NEVER_RECEIVED ((Std_ReturnType) 133)
/** Traceability: [SWS_Rte_07655]*/
#define RTE_E_UNCONNECTED ((Std_ReturnType) 134)
/** Traceability: [SWS_Rte_02739]*/
#define RTE_E_IN_EXCLUSIVE_AREA ((Std_ReturnType) 135)
/** Traceability: [SWS_Rte_02757]*/
#define RTE_E_SEG_FAULT ((Std_ReturnType) 136)
/** Traceability: [SWS_Rte_08065]*/
#define RTE_E_OUT_OF_RANGE ((Std_ReturnType) 137)
/** Traceability: [SWS_Rte_08725]*/
#define RTE_E_SERIALIZATION_ERROR ((Std_ReturnType) 138)
/** Traceability: [SWS_Rte_08726]*/
#define RTE_E_SERIALIZATION_LIMIT ((Std_ReturnType) 139)
/** Traceability: [rte sws 2571]*/
#define RTE_E_LOST_DATA    ((Std_ReturnType) 64)
/** Traceability: [rte sws 2702]*/
#define RTE_E_MAX_AGE_EXCEEDED    ((Std_ReturnType) 64)

/* Additional Macros==========================================================*/
/** Traceability: [rte sws 7404]*/
#define Rte_IsInfrastructureError(status) ((status & 128U) != 0)
/** Traceability: [rte sws 7405]*/
#define Rte_HasOverlayedError(status) ((status & 64U) != 0)
/** Traceability: [rte sws 7406]*/
#define Rte_ApplicationError(status) (status & 63U)

/* DASSAULT SYSTEMES RTE implementation specific definitions==================*/

typedef uint8 RtegOperationStateType;
#define RTE_OPERATION_IDLE ((RtegOperationStateType)0U)
#define RTE_OPERATION_BUSY ((RtegOperationStateType)1U)
#define RTE_OPERATION_TIMEOUT ((RtegOperationStateType)2U)
#define RTE_OPERATION_DONE ((RtegOperationStateType)3U)

typedef uint8 RtegWakeupModeType;
#define RTE_WAKEUP_NONE ((RtegWakeupModeType)0U)
#define RTE_WAKEUP_SETEVENT ((RtegWakeupModeType)1U)
#define RTE_WAKEUP_ACTIVATETASK ((RtegWakeupModeType)2U)

typedef uint8 Rte_TransAckType;
#define RTE_TRANSMIT_NO_DATA ((Rte_TransAckType) 0U)
#define RTE_TRANSMIT_ACK ((Rte_TransAckType) 1U)
#define RTE_TRANSMIT_TIMEOUT ((Rte_TransAckType) 2U)

#endif /* ifndef RTE_H */


/* End of File Rte.h==========================================================*/
