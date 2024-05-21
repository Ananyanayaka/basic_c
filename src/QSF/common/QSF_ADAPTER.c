/******************************************************************************
 * Copyright  : 2023 by Dassault Systemes
 * Name       : QSF_ADAPTER.c
 * Description: Generated with Rte Generator - AUTOSAR Builder 2021x
 ******************************************************************************
 * Project    : apc_ar
 * Runnables  : This files contains implementation for the following runnables
 *        QsfA_Fls_BlankCheck
 *        QsfA_Fls_Cancel
 *        QsfA_Fls_Compare
 *        QsfA_Fls_Erase
 *        QsfA_Fls_Erase_4K
 *        QsfA_Fls_GetJobResult
 *        QsfA_Fls_GetSectorSize
 *        QsfA_Fls_GetStatus
 *        QsfA_Fls_GetVersionInfo
 *        QsfA_Fls_Init
 *        QsfA_Fls_MainFunction
 *        QsfA_Fls_Read
 *        QsfA_Fls_SetMode
 *        QsfA_Fls_Write
 ******************************************************************************
 * Author     : uif43305
 * Date       : Fri Jan 06 13:14:06 CET 2023
 ******************************************************************************/

#include "Rte_QSF_ADAPTER.h"
#include "Fls.h"

#define QSF_ADAPTER_START_SEC_CODE
#include "QSF_ADAPTER_MemMap.h"

/// This function is a wrapper for Fls_BlankCheck(). It only routes the call to QSF component.
///
/// \pre
///   QsfA_Fls_Init must be called successfully.
///
/// \post
///   QsfA_Fls_MainFunction must be called repeatedly until job is processed
///
/// \param[in] targetAddress: - Address in SDF where the blank check starts, must be aligned to 4,
/// \param[in] length: - amount of bytes which are checked for blank, must be multiple of 4.
///
/// \return Std_ReturnType - E_OK in case of success, otherwise E_NOT_OK
///
/// \globals
///   none
///
/// \InOutCorrelation
///   none
///
/// \callsequence
///   \image html QsfA_Fls_BlankCheck.png
///   \startuml QsfA_Fls_BlankCheck.png
///     title QsfA_Fls_BlankCheck
///     participant ENV
///     participant QSF_ADAPTER
///     participant QSF
///     ENV -> QSF_ADAPTER: QsfA_Fls_BlankCheck()
///     activate QSF_ADAPTER
///     QSF_ADAPTER -> QSF: Fls_BlankCheck()
///     activate QSF
///     QSF_ADAPTER <- QSF
///     deactivate QSF
///     ENV <- QSF_ADAPTER
///     deactivate QSF_ADAPTER
///   \enduml
///
/// \testmethod Refer to the caller graph with link to unit test.
// 2023-02-10; uif43305
// summary:(3227) Parameter is never modified and so it could be declared with the 'const' qualifier.
// reason: (3227) RTE generator does not support keyword "const" in function parameters.
// PRQA S 3227 ++
FUNC(Std_ReturnType, QSF_ADAPTER_CODE) QsfA_Fls_BlankCheck (VAR(QsfA_Fls_AddressType, AUTOMATIC) targetAddress, \
                                                            VAR(QsfA_Fls_LengthType, AUTOMATIC) length)
// PRQA S 3227 --
{
  Std_ReturnType retValue = Fls_BlankCheck(targetAddress, length);

  return retValue;
}

/// This function is a wrapper for Fls_Cancel(). It only routes the call to QSF component.
///
/// \pre
///  none
///
/// \post
///   none
///
/// \param
///   none
///
/// \return Std_ReturnType - E_OK
///
/// \globals
///   none
///
/// \InOutCorrelation
///   none
///
/// \callsequence
///   \image html QsfA_Fls_Cancel.png
///   \startuml QsfA_Fls_Cancel.png
///     title QsfA_Fls_Cancel
///     participant ENV
///     participant QSF_ADAPTER
///     participant QSF
///     ENV -> QSF_ADAPTER: QsfA_Fls_Cancel()
///     activate QSF_ADAPTER
///     QSF_ADAPTER -> QSF: Fls_Cancel()
///     activate QSF
///     QSF_ADAPTER <- QSF
///     deactivate QSF
///     ENV <- QSF_ADAPTER
///     deactivate QSF_ADAPTER
///   \enduml
///
/// \testmethod Refer to the caller graph with link to unit test.
FUNC(Std_ReturnType, QSF_ADAPTER_CODE) QsfA_Fls_Cancel (void)
{
  Std_ReturnType retValue = RTE_E_OK;

  Fls_Cancel();

  return retValue;
}

/// This function is a wrapper for Fls_Compare(). It only routes the call to QSF component.
///
/// \pre
///   QsfA_Fls_Init must be called successfully.
///
/// \post
///   QsfA_Fls_MainFunction must be called repeatedly until job is processed
///
/// \param[in] sourceAddress: - Address in SDF where to start the compare action,
/// \param[in] targetAddressPtr: - Ram pointer to the referenced data,
/// \param[in] length: - amount of bytes to be compared.
///
/// \return Std_ReturnType - E_OK in case of success, otherwise E_NOT_OK
///
/// \globals
///   none
///
/// \InOutCorrelation
///   none
///
/// \callsequence
///   \image html QsfA_Fls_Compare.png
///   \startuml QsfA_Fls_Compare.png
///     title QsfA_Fls_Compare
///     participant ENV
///     participant QSF_ADAPTER
///     participant QSF
///     ENV -> QSF_ADAPTER: QsfA_Fls_Compare()
///     activate QSF_ADAPTER
///     QSF_ADAPTER -> QSF: Fls_Compare()
///     activate QSF
///     QSF_ADAPTER <- QSF
///     deactivate QSF
///     ENV <- QSF_ADAPTER
///     deactivate QSF_ADAPTER
///   \enduml
///
/// \testmethod Refer to the caller graph with link to unit test.
// 2023-02-10; uif43305
// summary:(3227) Parameter is never modified and so it could be declared with the 'const' qualifier.
// reason: (3227) RTE generator does not support keyword "const" in function parameters.
// 2023-02-10; uif43305
// summary:(3673) The object addressed by the pointer parameter is not modified and so the pointer could be of type 'pointer to const'.
// reason: (3673) RTE generator does not support keyword "const" in function parameters.
// PRQA S 3227, 3673 ++
FUNC(Std_ReturnType, QSF_ADAPTER_CODE) QsfA_Fls_Compare (VAR(QsfA_Fls_AddressType, AUTOMATIC) sourceAddress, \
                                                         VAR(p_uint8, AUTOMATIC) targetAddressPtr, \
                                                         VAR(QsfA_Fls_LengthType, AUTOMATIC) length)
// PRQA S 3227, 3673 --
{
  // 2023-02-10; uif43305
  // summary:(0317) [I] Implicit conversion from a pointer to void to a pointer to object type.
  // reazon: (0317) targetAddressPtr is same type as the function parameter
  // PRQA S 0317 1
  Std_ReturnType retValue = Fls_Compare(sourceAddress, targetAddressPtr, length);

  return retValue;
}


/// This function is a wrapper for Fls_Erase(). It only routes the call to QSF component.
///
/// \pre
///   QsfA_Fls_Init must be called successfully.
///
/// \post
///   QsfA_Fls_MainFunction must be called repeatedly until job is processed
///
/// \param[in] targetAddress: - start address of sector to be erased, must be aligned to sector size (64kB),
/// \param[in] length: - amount of bytes to erase, this must be a multible of sector size (64kB).
///
/// \return Std_ReturnType - E_OK in case of success, otherwise E_NOT_OK
///
/// \globals
///   none
///
/// \InOutCorrelation
///   none
///
/// \callsequence
///   \image html QsfA_Fls_Erase.png
///   \startuml QsfA_Fls_Erase.png
///     title QsfA_Fls_Erase
///     participant ENV
///     participant QSF_ADAPTER
///     participant QSF
///     ENV -> QSF_ADAPTER: QsfA_Fls_Erase()
///     activate QSF_ADAPTER
///     QSF_ADAPTER -> QSF: Fls_Erase()
///     activate QSF
///     QSF_ADAPTER <- QSF
///     deactivate QSF
///     ENV <- QSF_ADAPTER
///     deactivate QSF_ADAPTER
///   \enduml
///
/// \testmethod Refer to the caller graph with link to unit test.
// 2023-02-10; uif43305
// summary:(3227) Parameter is never modified and so it could be declared with the 'const' qualifier.
// reason: (3227) RTE generator does not support keyword "const" in function parameters.
// PRQA S 3227 ++
FUNC(Std_ReturnType, QSF_ADAPTER_CODE) QsfA_Fls_Erase (VAR(QsfA_Fls_AddressType, AUTOMATIC) targetAddress, \
                                                       VAR(QsfA_Fls_LengthType, AUTOMATIC) length)
// PRQA S 3227 --
{
  Std_ReturnType retValue = Fls_Erase(targetAddress, length);

  return retValue;
}

/// This function is a wrapper for Fls_Erase_4K(). It only routes the call to QSF component.
///
/// \pre
///   QsfA_Fls_Init must be called successfully.
///
/// \post
///   QsfA_Fls_MainFunction must be called repeatedly until job is processed
///
/// \param[in] targetAddress: - start address of sector to be erased, must be aligned to subsector size (4kB),
/// \param[in] length: - amount of bytes to erase, this must be a multible of subsector (4kB).
///
/// \return Std_ReturnType - E_OK in case of success, otherwise E_NOT_OK
///
/// \globals
///   none
///
/// \InOutCorrelation
///   none
///
/// \callsequence
///   \image html QsfA_Fls_Erase_4K.png
///   \startuml QsfA_Fls_Erase_4K.png
///     title QsfA_Fls_Erase_4K
///     participant ENV
///     participant QSF_ADAPTER
///     participant QSF
///     ENV -> QSF_ADAPTER: QsfA_Fls_Erase_4K()
///     activate QSF_ADAPTER
///     QSF_ADAPTER -> QSF: Fls_Erase4K()
///     activate QSF
///     QSF_ADAPTER <- QSF
///     deactivate QSF
///     ENV <- QSF_ADAPTER
///     deactivate QSF_ADAPTER
///   \enduml
///
/// \testmethod Refer to the caller graph with link to unit test.
// 2023-02-10; uif43305
// summary:(3227) Parameter is never modified and so it could be declared with the 'const' qualifier.
// reason: (3227) RTE generator does not support keyword "const" in function parameters.
// PRQA S 3227 ++
FUNC(Std_ReturnType, QSF_ADAPTER_CODE) QsfA_Fls_Erase_4K (VAR(QsfA_Fls_AddressType, AUTOMATIC) targetAddress, 
                                                          VAR(QsfA_Fls_LengthType, AUTOMATIC) length)
// PRQA S 3227 ++
{
  Std_ReturnType retValue = Fls_Erase_4K(targetAddress,length);

  return retValue;
}


/// This function is a wrapper for Fls_GetJobResult(). It only routes the call to QSF component.
///
/// \pre
///   none
///
/// \post
///   none
///
/// \param[out] jobResult: - result of last job.
///
/// \return Std_ReturnType - E_OK
///
/// \globals
///   none
///
/// \InOutCorrelation
///   none
///
/// \callsequence
///   \image html QsfA_Fls_GetJobResult.png
///   \startuml QsfA_Fls_GetJobResult.png
///     title QsfA_Fls_GetJobResult
///     participant ENV
///     participant QSF_ADAPTER
///     participant QSF
///     ENV -> QSF_ADAPTER: QsfA_Fls_GetJobResult()
///     activate QSF_ADAPTER
///     QSF_ADAPTER -> QSF: Fls_GetJobResult()
///     activate QSF
///     QSF_ADAPTER <- QSF
///     deactivate QSF
///     ENV <- QSF_ADAPTER
///     deactivate QSF_ADAPTER
///   \enduml
///
/// \testmethod Refer to the caller graph with link to unit test.
// 2023-02-10; uif43305
// summary:(3227) Parameter is never modified and so it could be declared with the 'const' qualifier.
// reason: (3227) RTE generator does not support keyword "const" in function parameters.
// 2023-02-10; uif43305
// summary:(3432) Simple macro argument expression is not parenthesized.
// reason: (3432) Warning is generated by P2VAR macro which is part of Compiler.h file. File cannot be modified, as it is part of an external package.
// PRQA S 3227, 3432 1
FUNC(Std_ReturnType, QSF_ADAPTER_CODE) QsfA_Fls_GetJobResult (P2VAR(QsfA_MemIf_JobResultType, AUTOMATIC, QSF_ADAPTER_APPL_DATA) jobResult)
{
  Std_ReturnType retValue = RTE_E_OK;

  *jobResult = (QsfA_MemIf_JobResultType)Fls_GetJobResult();

  return retValue;
}

/// This function is a wrapper for Fls_v_GetSectorSize(). It only routes the call to QSF component.
///
/// \pre
///   none
///
/// \post
///   none
///
/// \param[out] sectorSize: - Sector size.
///
/// \return Std_ReturnType - E_OK
///
/// \globals
///   none
///
/// \InOutCorrelation
///   none
///
/// \callsequence
///   \image html QsfA_Fls_GetSectorSize.png
///   \startuml QsfA_Fls_GetSectorSize.png
///     title QsfA_Fls_GetSectorSize
///     participant ENV
///     participant QSF_ADAPTER
///     participant QSF
///     ENV -> QSF_ADAPTER: QsfA_Fls_GetSectorSize()
///     activate QSF_ADAPTER
///     QSF_ADAPTER -> QSF: Fls_v_GetSectorSize()
///     activate QSF
///     QSF_ADAPTER <- QSF
///     deactivate QSF
///     ENV <- QSF_ADAPTER
///     deactivate QSF_ADAPTER
///   \enduml
///
/// \testmethod Refer to the caller graph with link to unit test.
// 2023-02-10; uif43305
// summary:(3227) Parameter is never modified and so it could be declared with the 'const' qualifier.
// reason: (3227) RTE generator does not support keyword "const" in function parameters.
// 2023-02-10; uif43305
// summary:(3432) Simple macro argument expression is not parenthesized.
// reason: (3432) Warning is generated by P2VAR macro which is part of Compiler.h file. File cannot be modified, as it is part of an external package.
// PRQA S 3227, 3432 1
FUNC(Std_ReturnType, QSF_ADAPTER_CODE) QsfA_Fls_GetSectorSize (P2VAR(uint32, AUTOMATIC, QSF_ADAPTER_APPL_DATA) sectorSize)
{
  Std_ReturnType retValue = RTE_E_OK;

  *sectorSize = Fls_v_GetSectorSize();

  return retValue;
}

/// This function is a wrapper for Fls_GetStatus(). It only routes the call to QSF component.
///
/// \pre
///   none
///
/// \post
///   none
///
/// \param[out] status: - Status of driver.
///
/// \return Std_ReturnType - E_OK
///
/// \globals
///   none
///
/// \InOutCorrelation
///   none
///
/// \callsequence
///   \image html QsfA_Fls_GetStatus.png
///   \startuml QsfA_Fls_GetStatus.png
///     title QsfA_Fls_GetStatus
///     participant ENV
///     participant QSF_ADAPTER
///     participant QSF
///     ENV -> QSF_ADAPTER: QsfA_Fls_GetStatus()
///     activate QSF_ADAPTER
///     QSF_ADAPTER -> QSF: Fls_GetStatus()
///     activate QSF
///     QSF_ADAPTER <- QSF
///     deactivate QSF
///     ENV <- QSF_ADAPTER
///     deactivate QSF_ADAPTER
///   \enduml
///
/// \testmethod Refer to the caller graph with link to unit test.
// 2023-02-10; uif43305
// summary:(3227) Parameter is never modified and so it could be declared with the 'const' qualifier.
// reason: (3227) RTE generator does not support keyword "const" in function parameters.
// 2023-02-10; uif43305
// summary:(3432) Simple macro argument expression is not parenthesized.
// reason: (3432) Warning is generated by P2VAR macro which is part of Compiler.h file. File cannot be modified, as it is part of an external package.
// PRQA S 3227, 3432 1
FUNC(Std_ReturnType, QSF_ADAPTER_CODE) QsfA_Fls_GetStatus (P2VAR(QsfA_MemIf_StatusType, AUTOMATIC, QSF_ADAPTER_APPL_DATA) status)
{
  Std_ReturnType retValue = RTE_E_OK;

  *status = (QsfA_MemIf_StatusType)Fls_GetStatus();

  return retValue;
}

/// This function is a wrapper for Fls_GetVersionInfo(). It only routes the call to QSF component.
///
/// \pre
///   none
///
/// \post
///   none
///
/// \param[in] versionInfoPtr: - Ram pointer to version structure.
///
/// \return Std_ReturnType - E_OK
///
/// \globals
///   none
///
/// \InOutCorrelation
///   none
///
/// \callsequence
///   \image html QsfA_Fls_GetVersionInfo.png
///   \startuml QsfA_Fls_GetVersionInfo.png
///     title QsfA_Fls_GetVersionInfo
///     participant ENV
///     participant QSF_ADAPTER
///     participant QSF
///     ENV -> QSF_ADAPTER: QsfA_Fls_GetVersionInfo()
///     activate QSF_ADAPTER
///     QSF_ADAPTER -> QSF: Fls_GetVersionInfo()
///     activate QSF
///     QSF_ADAPTER <- QSF
///     deactivate QSF
///     ENV <- QSF_ADAPTER
///     deactivate QSF_ADAPTER
///   \enduml
///
/// \testmethod Refer to the caller graph with link to unit test.
// 2023-02-10; uif43305
// summary:(3227) Parameter is never modified and so it could be declared with the 'const' qualifier.
// reason: (3227) RTE generator does not support keyword "const" in function parameters.
// 2023-02-10; uif43305
// summary:(3432) Simple macro argument expression is not parenthesized.
// reason: (3432) Warning is generated by P2VAR macro which is part of Compiler.h file. File cannot be modified, as it is part of an external package.
// PRQA S 3227, 3432 1
FUNC(Std_ReturnType, QSF_ADAPTER_CODE) QsfA_Fls_GetVersionInfo (P2VAR(QsfA_Std_VersionInfoType, AUTOMATIC, QSF_ADAPTER_APPL_DATA) versionInfoPtr)
{
  Std_ReturnType      retValue = RTE_E_OK;

  Std_VersionInfoType localVersionInfo;

  Fls_GetVersionInfo(&localVersionInfo);

  versionInfoPtr->ModuleID = localVersionInfo.moduleID;
  versionInfoPtr->VendorID = localVersionInfo.vendorID;
  versionInfoPtr->sw_major_version = localVersionInfo.sw_major_version;
  versionInfoPtr->sw_minor_version = localVersionInfo.sw_minor_version;
  versionInfoPtr->sw_patch_version = localVersionInfo.sw_patch_version;

  return retValue;
}

/// This function is a wrapper for Fls_Init(). It only routes the call to QSF component.
///
/// \pre
///   none
///
/// \post
///   none
///
/// \param[in] configPtr: - Pointer to config structure.
///
/// \return Std_ReturnType - E_OK
///
/// \globals
///   none
///
/// \InOutCorrelation
///   none
///
/// \callsequence
///   \image html QsfA_Fls_Init.png
///   \startuml QsfA_Fls_Init.png
///     title QsfA_Fls_Init
///     participant ENV
///     participant QSF_ADAPTER
///     participant QSF
///     ENV -> QSF_ADAPTER: QsfA_Fls_Init()
///     activate QSF_ADAPTER
///     QSF_ADAPTER -> QSF: Fls_Init()
///     activate QSF
///     QSF_ADAPTER <- QSF
///     deactivate QSF
///     ENV <- QSF_ADAPTER
///     deactivate QSF_ADAPTER
///   \enduml
///
/// \testmethod Refer to the caller graph with link to unit test.
// 2023-02-10; uif43305
// summary:(3206) The parameter 'configPtr' is not used in this function.
// reason: (3206) Function is currently not implemented, warnning will be removed when function is reworked.
// PRQA S 3206 1
FUNC(Std_ReturnType, QSF_ADAPTER_CODE) QsfA_Fls_Init (P2CONST(QsfA_Fls_ConfigType, AUTOMATIC, QSF_ADAPTER_APPL_CONST) configPtr)
{
  Std_ReturnType  retValue = RTE_E_OK;

  // 
  //   0U,
  //   0U,
  //   0U,
  //   MEMIF_MODE_SLOW,
  //   {0U,0U},
  //   {0U,0U}
  // };

  // lConfig.u_WriteWindowStart = configPtr->WriteWindowStart;
  // lConfig.u_WriteWindowSize = configPtr->WriteWindowSize;
  // lConfig.u_NvmOffset = configPtr->NvmOffset;
  // lConfig.u_InitMode = (MemIf_ModeType)configPtr->InitMode;

  // lConfig.ua_ReadAtOnce[0] = configPtr->ReadAtOnce[0];
  // lConfig.ua_ReadAtOnce[1] = configPtr->ReadAtOnce[1];
  // lConfig.ua_WriteAtOnce[0] = configPtr->WriteAtOnce[0];
  // lConfig.ua_WriteAtOnce[1] = configPtr->WriteAtOnce[1];
  
  // Fls_Init(&lConfig);

  return retValue;
}

/// This function is a wrapper for Fls_MainFunction(). It only routes the call to QSF component.
///
/// \pre
///   none
///
/// \post
///   none
///
/// \param
///   none
///
/// \return 
///   none
///
/// \globals
///   none
///
/// \InOutCorrelation
///   none
///
/// \callsequence
///   \image html QsfA_Fls_MainFunction.png
///   \startuml QsfA_Fls_MainFunction.png
///     title QsfA_Fls_MainFunction
///     participant ENV
///     participant QSF_ADAPTER
///     participant QSF
///     ENV -> QSF_ADAPTER: QsfA_Fls_MainFunction()
///     activate QSF_ADAPTER
///     QSF_ADAPTER -> QSF: Fls_MainFunction()
///     activate QSF
///     QSF_ADAPTER <- QSF
///     deactivate QSF
///     ENV <- QSF_ADAPTER
///     deactivate QSF_ADAPTER
///   \enduml
///
/// \testmethod Refer to the caller graph with link to unit test.
FUNC(void, QSF_ADAPTER_CODE) QsfA_Fls_MainFunction (void)
{

  Fls_MainFunction();

}

/// This function is a wrapper for Fls_Read(). It only routes the call to QSF component.
///
/// \pre
///   QsfA_Fls_Init must be called successfully.
///
/// \post
///   QsfA_Fls_MainFunction must be called repeatedly until job is processed
///
/// \param[in] sourceAddress: - Address in SDF from where to read,
/// \param[in] targetAddressPtr: - Ram pointer to where the data shall be written,
/// \param[in] length: - Amount of bytes which shall be read.
///
/// \return Std_ReturnType - E_OK in case of success, otherwise E_NOT_OK
///
/// \globals
///   none
///
/// \InOutCorrelation
///   none
///
/// \callsequence
///   \image html QsfA_Fls_Read.png
///   \startuml QsfA_Fls_Read.png
///     title QsfA_Fls_Read
///     participant ENV
///     participant QSF_ADAPTER
///     participant QSF
///     ENV -> QSF_ADAPTER: QsfA_Fls_Read()
///     activate QSF_ADAPTER
///     QSF_ADAPTER -> QSF: Fls_Read()
///     activate QSF
///     QSF_ADAPTER <- QSF
///     deactivate QSF
///     ENV <- QSF_ADAPTER
///     deactivate QSF_ADAPTER
///   \enduml
///
// 2023-02-10; uif43305
// summary:(3227) Parameter is never modified and so it could be declared with the 'const' qualifier.
// reason: (3227) RTE generator does not support keyword "const" in function parameters.
// PRQA S 3227 ++
FUNC(Std_ReturnType, QSF_ADAPTER_CODE) QsfA_Fls_Read (VAR(QsfA_Fls_AddressType, AUTOMATIC) sourceAddress, \
                                                      VAR(p_uint8, AUTOMATIC) targetAddresPtr, \
                                                      VAR(QsfA_Fls_LengthType, AUTOMATIC) length)
// PRQA S 3227 --
{
  // 2023-02-10; uif43305
  // summary:(0317) [I] Implicit conversion from a pointer to void to a pointer to object type.
  // reazon: (0317) targetAddresPtr is same type as the function parameter
  // PRQA S 0317 1
  Std_ReturnType retValue = Fls_Read(sourceAddress, targetAddresPtr, length);

  return retValue;
}

/// This function is a wrapper for Fls_SetMode(). It only routes the call to QSF component.
///
/// \pre
///   none
///
/// \post
///   none
///
/// \param[in] mode: - MEMIF_MODE_SLOW or MEMIF_MODE_FAST.
///
/// \return Std_ReturnType - E_OK
///
/// \globals
///   none
///
/// \InOutCorrelation
///   none
///
/// \callsequence
///   \image html QsfA_Fls_SetMode.png
///   \startuml QsfA_Fls_SetMode.png
///     title QsfA_Fls_SetMode
///     participant ENV
///     participant QSF_ADAPTER
///     participant QSF
///     ENV -> QSF_ADAPTER: QsfA_Fls_SetMode()
///     activate QSF_ADAPTER
///     QSF_ADAPTER -> QSF: Fls_SetMode()
///     activate QSF
///     QSF_ADAPTER <- QSF
///     deactivate QSF
///     ENV <- QSF_ADAPTER
///     deactivate QSF_ADAPTER
///   \enduml
///
/// \testmethod Refer to the caller graph with link to unit test.
// 2023-02-10; uif43305
// summary:(3227) Parameter is never modified and so it could be declared with the 'const' qualifier.
// reason: (3227) RTE generator does not support keyword "const" in function parameters.
// PRQA S 3227 1
FUNC(Std_ReturnType, QSF_ADAPTER_CODE) QsfA_Fls_SetMode (VAR(QsfA_MemIf_ModeType, AUTOMATIC) mode)
{
  Std_ReturnType retValue = RTE_E_OK;
  MemIf_ModeType lMode = MEMIF_MODE_SLOW;

  if (mode == (uint8)MEMIF_MODE_SLOW)
  {
    Fls_SetMode(MEMIF_MODE_SLOW);
  }
  else if(mode == (uint8)MEMIF_MODE_FAST)
  {
    Fls_SetMode(MEMIF_MODE_FAST);
  }
  else
  {
    retValue = E_NOT_OK;
  }

  return retValue;
}

/// This function is a wrapper for Fls_Write(). It only routes the call to QSF component.
///
/// \pre
///   QsfA_Fls_Init must be called successfully.
///
/// \post
///   QsfA_Fls_MainFunction must be called repeatedly until job is processed
///
/// \param[in] targetAddress: - Address inside SDF where writing starts, must be aligned to 16,
/// \param[in] sourceAddressPtr: - Ram pointer to data for writing,
/// \param[in] length: - amount of bytes to write, must be a multible of 16.
///
/// \return Std_ReturnType - E_OK in case of success, otherwise E_NOT_OK
///
/// \globals
///   none
///
/// \InOutCorrelation
///   none
///
/// \callsequence
///   \image html QsfA_Fls_Write.png
///   \startuml QsfA_Fls_Write.png
///     title QsfA_Fls_Write
///     participant ENV
///     participant QSF_ADAPTER
///     participant QSF
///     ENV -> QSF_ADAPTER: QsfA_Fls_Write()
///     activate QSF_ADAPTER
///     QSF_ADAPTER -> QSF: Fls_Write()
///     activate QSF
///     QSF_ADAPTER <- QSF
///     deactivate QSF
///     ENV <- QSF_ADAPTER
///     deactivate QSF_ADAPTER
///   \enduml
///
/// \testmethod Refer to the caller graph with link to unit test.
// 2023-02-10; uif43305
// summary:(3227) Parameter is never modified and so it could be declared with the 'const' qualifier.
// reason: (3227) RTE generator does not support keyword "const" in function parameters.
// 2023-02-10; uif43305
// summary:(3673) The object addressed by the pointer parameter is not modified and so the pointer could be of type 'pointer to const'.
// reason: (3673) RTE generator does not support keyword "const" in function parameters.
// PRQA S 3227,3673 ++
FUNC(Std_ReturnType, QSF_ADAPTER_CODE) QsfA_Fls_Write (VAR(QsfA_Fls_AddressType, AUTOMATIC) targetAddress, \
                                                       VAR(p_uint8, AUTOMATIC) sourceAddressPtr, \
                                                       VAR(QsfA_Fls_LengthType, AUTOMATIC) length)
// PRQA S 3227,3673 ++
{
  // 2023-02-10; uif43305
  // summary:(0317) [I] Implicit conversion from a pointer to void to a pointer to object type.
  // reazon: (0317) sourceAddressPtr is same type as the function parameter
  // PRQA S 0317 1
  Std_ReturnType retValue = Fls_Write(targetAddress, sourceAddressPtr, length);

  return retValue;
}


// 2023-02-10; uif43305
// summary:(5087) Use of #include directive after code fragment.
// reason: (5087) Multiple header inclusion is a MemMap mechanism to ensure that correct memory sections are used.
// PRQA S 5087 6
// 2023-02-10; uif43305
// summary:(5002) The header '%s' file name shadows a previously included header.
// reason: (5002) Multiple header inclusion is a MemMap mechanism to ensure that correct memory sections are used.
// PRQA S 5002 2
#define QSF_ADAPTER_STOP_SEC_CODE
#include "QSF_ADAPTER_MemMap.h"

