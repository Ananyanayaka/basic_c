// COMPANY:   Continental Automotive
// COMPONENT: QSF
// VERSION:   $Revision: 1.17 $
/// @file  \04_Engineering\01_Source_Code\QSF\Fls.c
/// @brief Source of FLS module, that is managing the requests for different operations with the external flash memory( eg: reading,writing,erasing).


#include <Fls.h>
#include <pal_qsf.h>
#ifdef QSF_ENTER_CRITICAL_SECTION
 #ifdef QSF_EXIT_CRITICAL_SECTION
  #include <Os.h>
 #endif
#endif

#ifdef __cplusplus
extern "C"
{
#endif

// critical section handling
#ifndef QSF_ENTER_CRITICAL_SECTION
/// entering a critical section handling with SuspendAllInterrupts()
#define QSF_ENTER_CRITICAL_SECTION
#endif
#ifndef QSF_EXIT_CRITICAL_SECTION
/// exiting a critical section handling with ResumeAllInterrupts()
#define QSF_EXIT_CRITICAL_SECTION
#endif


#ifdef EB_MEMMAP_USE
#define FLS_START_SEC_VAR_NO_INIT_UNSPECIFIED
#include "Fls_MemMap.h"
#endif
// 2021-03-10; uie23485
// Summary: Message(3:3408) 'sQsf' has external linkage
// Reason:  Its ideal to have globals in header file
// PRQA S 3408 3
// all static object data, is of interest when debugging the component
/// configuration of QSF SWC
volatile tQsfState sQsf;

#ifdef EB_MEMMAP_USE
#define FLS_STOP_SEC_VAR_NO_INIT_UNSPECIFIED
#include "Fls_MemMap.h"
#endif
// local functions

// @brief               Helper function to calculate size to be written next
//
// @pre                 Fls_MainFunction must be called with case sQsf.u_Job == QSF_WRITE (writing to SDF).
//                      Write API enabled, so the code for write API is compiled in.
// @post                In Fls_MainFunction PAL_u_QsfProgramPage is called to start programing data into flash. \n
//                      Afterwards, current address and current ram are stored into sQsf.
// @param[in]           void
// @return              uint32 u_Step memory size that is written in the next step
//
// @globals
//             sQsf structure which contains status information
//
// @InOutCorrelation
//    The sQsf structure is read to determine the step size.
//
// @callsequence
//
// @image  html SequenceDiagram_QsfWriteSize.png
// @startuml "SequenceDiagram_QsfWriteSize.png"
// title Sequence Diagram of u_QsfWriteSize
//
// Fls_MainFunction -> u_QsfWriteSize: global sQsf
// activate u_QsfWriteSize
// note right u_QsfWriteSize: determine the step size
// u_QsfWriteSize --> Fls_MainFunction: uint32 u_step
// deactivate u_QsfWriteSize
// @enduml
//
// @testmethod
// e.g. test of Input-Output-Correlation (dynamic module test),
//             test order of calls (dynamic module test).
//
// @traceability
// Enter the link to the corresponding function in L4 design specification in Doors
//               <a href="linkURL">link text</a>
static uint32 u_QsfWriteSize(void);

// @brief               Helper function for actions at job end
//
// @pre                 Fls_MainFunction must be called and a job (erase sector, write, read, verify, blank check) has at least tried to be started.
// @post                Job in Fls_MainFunction is finished.
//                      Current address and current ram is stored into sQsf.
// @param[in]           void
// @return              void
//
// @globals
//             sQsf structure which contains status information
//
// @InOutCorrelation
//    In sQsf structure the QSF status and job is set to idle state. \n
//    The job status is set to JOB_OK or JOB_FAILED depending on the return value of the previous action. \n
//    FEE_JOB_END_NOTIFICATION(void) or FEE_JOB_ERROR_NOTIFICATION() is called accordingly.
//
// @callsequence
//
// @image  html SequenceDiagram_QsfJobEnd.png
// @startuml "SequenceDiagram_QsfJobEnd.png"
// title Sequence Diagram of u_QsfJobEnd
//
// Fls_MainFunction -> u_QsfJobEnd: global sQsf
// activate u_QsfJobEnd
// note right u_QsfJobEnd: set u_QsfStatus, u_Job and u_QsfJobStatus
// activate Fee
// u_QsfJobEnd -> Fee: if JOB_OK FEE_JOB_END_NOTIFICATION(), else FEE_JOB_ERROR_NOTIFICATION()
// deactivate Fee
// u_QsfJobEnd --> Fls_MainFunction
// deactivate u_QsfJobEnd
// @enduml
//
// @testmethod
// e.g. test of Input-Output-Correlation (dynamic module test),
//             test order of calls (dynamic module test).
//
// @traceability
// Enter the link to the corresponding function in L4 design specification in Doors
//               <a href="linkURL">link text</a>
static void v_QsfJobEnd(void);

// @brief               Helper function to get the lock status of QSF module and set it to BUSY.
//
// @pre                 u_Qsf_GetLock is called in Fls_Init or a function for preparation for a jobs.
// @post                If the lock status E_OK is returned, the global sQsf status values are set for the current job.
// @param[in]           void
// @return              Std_ReturnType status
//
// @globals
//             sQsf structure which contains status information
//
// @InOutCorrelation
//    sQsf.Lock is set to PAL_QSF_BUSY, if it was E_OK before. \n
//    The sQsf.Lock from the beginning is returned.
//
// @callsequence
//
// @image  html SequenceDiagram_Qsf_GetLock.png
// @startuml "SequenceDiagram_Qsf_GetLock.png"
// title Sequence Diagram of u_Qsf_GetLock
//
// Fls -> u_Qsf_GetLock: global sQsf
// activate u_Qsf_GetLock
// u_Qsf_GetLock -> OS: SuspendAllInterrupts()
// activate OS
// note right u_Qsf_GetLock: set sQsf.Lock E_OK -> PAL_QSF_BUSY
// u_Qsf_GetLock -> OS: ResumeAllInterrupts()
// deactivate OS
// u_Qsf_GetLock --> Fls: status of lock
// deactivate u_Qsf_GetLock
// @enduml
//
// @testmethod
// e.g. test of Input-Output-Correlation (dynamic module test),
//             test order of calls (dynamic module test).
//
// @traceability
// Enter the link to the corresponding function in L4 design specification in Doors
//               <a href="linkURL">link text</a>
static Std_ReturnType u_Qsf_GetLock(void);

// @brief               Helper function to set the lock status of QSF module to OK.
//
// @pre                 u_Qsf_GetLock is called and the global sQsf status values are set for the current job.
// @post                end of Function
// @param[in]           void
// @return              void
//
// @globals
//             sQsf structure which contains status information
//
// @InOutCorrelation
//    The sQsf.Lock is set E_OK.
//
// @callsequence
//
// @image  html SequenceDiagram_Qsf_Unlock.png
// @startuml "SequenceDiagram_Qsf_Unlock.png"
// title Sequence Diagram of v_Qsf_Unlock
//
// Fls -> v_Qsf_Unlock: global sQsf
// activate v_Qsf_Unlock
// v_Qsf_Unlock -> OS: SuspendAllInterrupts()
// activate OS
// note right v_Qsf_Unlock: set sQsf.Lock to E_OK
// v_Qsf_Unlock -> OS: ResumeAllInterrupts()
// deactivate OS
// v_Qsf_Unlock --> Fls: void
// deactivate v_Qsf_Unlock
// @enduml
//
// @testmethod
// e.g. test of Input-Output-Correation (dynamic module test),test order of calls (dynamic module test).
//
// @traceability
// Enter the link to the corresponding function in L4 design specification in Doors
//               <a href="linkURL">link text</a>
static void v_Qsf_Unlock(void);

// module initialisation
void Fls_Init(const Fls_ConfigType* ConfigPtr)
{
  Std_ReturnType u_lock = u_Qsf_GetLock();
  if (E_OK != u_lock)
  {
    return;
  }
  sQsf.u_Job = QSF_INIT;

#if (QSF_WRITE_API == STD_ON)
  //avoid reinitialisation
  uint32 Done = sQsf.u_InitDone;
  if (Done != 1u)
#endif
  {
    // PRQA S 4342 3 // 2017-10-25; uidv7790
    // summary: Msg(3:4342) An expression of 'essentially unsigned' type (unsigned char) is being cast to enum type 'e_Qsf_ErrorCodes'.
    // reason: enum is used to ensure debug comfort
    sQsf.u_RetVal = (e_Qsf_ErrorCodes)PAL_u_QsfInit(ConfigPtr);
  }
  e_Qsf_ErrorCodes RetVal = sQsf.u_RetVal;
  if (RetVal == PAL_QSF_OK)
  {
    sQsf.p_QsfCfg = ConfigPtr;
    sQsf.Qsf_Mode = ConfigPtr->u_InitMode;
    sQsf.u_PageSize = PAL_u_QsfGetPageSize();
    sQsf.u_SectorSize = PAL_u_QsfGetSectorSize();
    sQsf.u_QsfStatus = MEMIF_IDLE;
    sQsf.u_QsfJobStatus = MEMIF_JOB_OK;
  }
  else
  {
    sQsf.u_QsfStatus = MEMIF_UNINIT;
    sQsf.u_QsfJobStatus = MEMIF_JOB_FAILED;
  }
  sQsf.u_Job = QSF_IDLE;

  v_Qsf_Unlock();
}

// prepare read action
Std_ReturnType Fls_Read(Fls_AddressType SourceAddress, uint8* TargetAddressPtr, Fls_LengthType Length)
{
  Std_ReturnType u_RetVal = E_NOT_OK;
  uint32 u_SourceAddress = SourceAddress + sQsf.p_QsfCfg->u_NvmOffset;
  Std_ReturnType u_lock = u_Qsf_GetLock();

  // if status is ok, unlock it
  if (E_OK == u_lock)
  {
    MemIf_StatusType Status = sQsf.u_QsfStatus;
    if (Status  == MEMIF_IDLE)
    {
      sQsf.u_QsfStatus = MEMIF_BUSY;
      sQsf.u_Job = QSF_READ;
      sQsf.u_QsfJobStatus = MEMIF_JOB_PENDING;
      sQsf.u_CurrentAddr = u_SourceAddress;
      sQsf.u_EndAddr = u_SourceAddress + Length;
      sQsf.p_CurrentRam = TargetAddressPtr;
      sQsf.u_RetVal = PAL_QSF_OK;
      u_RetVal = (Std_ReturnType)(sQsf.u_RetVal);
    }
    v_Qsf_Unlock();
  }

  return u_RetVal;
}

// prepare read action
Std_ReturnType Fls_OTP_Read(Fls_AddressType SourceAddress, uint8* TargetAddressPtr, Fls_LengthType Length)
{
  Std_ReturnType u_RetVal = E_NOT_OK;
  uint32 u_SourceAddress = SourceAddress + sQsf.p_QsfCfg->u_NvmOffset;
  Std_ReturnType u_lock = u_Qsf_GetLock();

  // if status is ok, unlock it
  if (E_OK == u_lock)
  {
    MemIf_StatusType Status = sQsf.u_QsfStatus;
    if (Status == MEMIF_IDLE)
    {
      sQsf.u_QsfStatus = MEMIF_BUSY;
      sQsf.u_Job = QSF_OTP_READ;
      sQsf.u_QsfJobStatus = MEMIF_JOB_PENDING;
      sQsf.u_CurrentAddr = u_SourceAddress;
      sQsf.u_EndAddr = u_SourceAddress + Length;
      sQsf.p_CurrentRam = TargetAddressPtr;
      sQsf.u_RetVal = PAL_QSF_OK;
      u_RetVal = (Std_ReturnType)(sQsf.u_RetVal);
    }
    v_Qsf_Unlock();
  }

  return u_RetVal;
}

#if (QSF_WRITE_API == STD_ON)
// prepare write action
Std_ReturnType Fls_Write(Fls_AddressType TargetAddress, const uint8* SourceAddressPtr, Fls_LengthType Length)
{
  Std_ReturnType u_RetVal = E_NOT_OK;
  Std_ReturnType u_lock;
  uint32 u_TargetAddress = TargetAddress + sQsf.p_QsfCfg->u_NvmOffset;
  const Fls_ConfigType* QsfCfg = sQsf.p_QsfCfg;

  if (u_TargetAddress < QsfCfg->u_WriteWindowStart)
  {
    u_RetVal = E_NOT_OK;
  }
  else if (u_TargetAddress+Length > QsfCfg->u_WriteWindowStart+QsfCfg->u_WriteWindowSize )
  {
    u_RetVal = E_NOT_OK;
  }
  else
  {
    u_lock = u_Qsf_GetLock();
    if (E_OK == u_lock)
    {
      MemIf_StatusType Status = sQsf.u_QsfStatus;
      if (Status == MEMIF_IDLE)
      {
        sQsf.u_QsfStatus = MEMIF_BUSY;
        sQsf.u_Job = QSF_WRITE;
        sQsf.u_QsfJobStatus = MEMIF_JOB_PENDING;
        sQsf.u_CurrentAddr = u_TargetAddress;
        sQsf.u_EndAddr = u_TargetAddress + Length;
        // PRQA S 0311 3 // 2017-10-25; uidv7790
        // summary: Msg(3:0311) Dangerous pointer cast results in loss of const qualification.
        // reason: We use p_CurrentRam in both cases (read and write access). Cast is needed in the second case here.
        sQsf.p_CurrentRam = (uint8*)SourceAddressPtr;
        sQsf.u_RetVal = PAL_QSF_OK;
        u_RetVal = (Std_ReturnType)(sQsf.u_RetVal);
      }
      v_Qsf_Unlock();
    }
  }
  return u_RetVal;
}

Std_ReturnType Fls_OTP_Write(Fls_AddressType TargetAddress, const uint8* SourceAddressPtr, Fls_LengthType Length)
{
  Std_ReturnType u_RetVal = E_NOT_OK;
  Std_ReturnType u_lock;
  uint32 u_TargetAddress = TargetAddress + sQsf.p_QsfCfg->u_NvmOffset;
  const Fls_ConfigType* QsfCfg = sQsf.p_QsfCfg;

  if (u_TargetAddress < QsfCfg->u_WriteWindowStart)
  {
    u_RetVal = E_NOT_OK;
  }
  else if (u_TargetAddress+Length > QsfCfg->u_WriteWindowStart+QsfCfg->u_WriteWindowSize )
  {
    u_RetVal = E_NOT_OK;
  }
  else
  {
    u_lock = u_Qsf_GetLock();
    if (E_OK == u_lock)
    {
     MemIf_StatusType status = sQsf.u_QsfStatus;
      if (status == MEMIF_IDLE)
      {
        sQsf.u_QsfStatus = MEMIF_BUSY;
        sQsf.u_Job = QSF_OTP_WRITE;
        sQsf.u_QsfJobStatus = MEMIF_JOB_PENDING;
        sQsf.u_CurrentAddr = u_TargetAddress;
        sQsf.u_EndAddr = u_TargetAddress + Length;
        // PRQA S 0311 3 // 2022-09-06; uif08910
        // summary: Msg(3:0311) Dangerous pointer cast results in loss of const qualification.
        // reason: We use p_CurrentRam in both cases (read and write access). Cast is needed in the second case here.
        sQsf.p_CurrentRam = (uint8*)SourceAddressPtr;
        sQsf.u_RetVal = PAL_QSF_OK;
        u_RetVal = (Std_ReturnType)(sQsf.u_RetVal);
      }
      v_Qsf_Unlock();
    }
  }
  return u_RetVal;
}

// prepare erase action
Std_ReturnType Fls_Erase(Fls_AddressType TargetAddress, Fls_LengthType Length)
{
  Std_ReturnType u_RetVal = E_NOT_OK;
  Std_ReturnType u_lock;
  uint32 u_TargetAddress = TargetAddress + sQsf.p_QsfCfg->u_NvmOffset;
  const Fls_ConfigType* QsfCfg = sQsf.p_QsfCfg;
  uint32 Sector = sQsf.u_SectorSize;
  if (u_TargetAddress < QsfCfg->u_WriteWindowStart)
  {
    u_RetVal = E_NOT_OK;
  }
  else if (u_TargetAddress+Length > QsfCfg->u_WriteWindowStart+QsfCfg->u_WriteWindowSize )
  {
    u_RetVal = E_NOT_OK;
  }
  else if ((u_TargetAddress & (Sector-1u)) != 0u)
  {
    u_RetVal = E_NOT_OK;
  }
  else if ((Length & (Sector-1u)) != 0u)
  {
    u_RetVal = E_NOT_OK;
  }
  else
  {
    u_lock = u_Qsf_GetLock();
    if (E_OK == u_lock)
    {
     MemIf_StatusType status = sQsf.u_QsfStatus;
      if (status == MEMIF_IDLE)
      {
        sQsf.u_QsfStatus = MEMIF_BUSY;
        sQsf.u_Job = QSF_ERASE;
        sQsf.u_QsfJobStatus = MEMIF_JOB_PENDING;
        sQsf.u_CurrentAddr = u_TargetAddress;
        sQsf.u_EndAddr = u_TargetAddress + Length;
        sQsf.u_RetVal = PAL_QSF_OK;
        u_RetVal = (Std_ReturnType)(sQsf.u_RetVal);
      }
      v_Qsf_Unlock();
    }
  }
  return u_RetVal;
}

// prepare erase action
Std_ReturnType Fls_Erase_4K(Fls_AddressType TargetAddress, Fls_LengthType Length)
{
  Std_ReturnType u_RetVal = E_NOT_OK;
  Std_ReturnType u_lock;
  uint32 u_TargetAddress = TargetAddress + sQsf.p_QsfCfg->u_NvmOffset;
  const Fls_ConfigType* QsfCfg = sQsf.p_QsfCfg;

  uint32 Sector = sQsf.u_SectorSize;

  if (u_TargetAddress < QsfCfg->u_WriteWindowStart)
  {
    u_RetVal = E_NOT_OK;
  }
  else if (u_TargetAddress+Length > QsfCfg->u_WriteWindowStart+QsfCfg->u_WriteWindowSize )
  {
    u_RetVal = E_NOT_OK;
  }
  else if ((u_TargetAddress & (Sector-1u)) != 0u)
  {
    u_RetVal = E_NOT_OK;
  }
  else if ((Length & (Sector-1u)) != 0u)
  {
    u_RetVal = E_NOT_OK;
  }
  else
  {
    u_lock = u_Qsf_GetLock();
    if (E_OK == u_lock)
    {
    MemIf_StatusType status = sQsf.u_QsfStatus;
      if (status == MEMIF_IDLE)
      {
        sQsf.u_QsfStatus = MEMIF_BUSY;
        sQsf.u_Job = QSF_ERASE_4K;
        sQsf.u_QsfJobStatus = MEMIF_JOB_PENDING;
        sQsf.u_CurrentAddr = u_TargetAddress;
        sQsf.u_EndAddr = u_TargetAddress + Length;
        sQsf.u_RetVal = PAL_QSF_OK;
        u_RetVal = (Std_ReturnType)(sQsf.u_RetVal);
      }
      v_Qsf_Unlock();
    }
  }
  return u_RetVal;
}


// prepare compare action
Std_ReturnType Fls_Compare( Fls_AddressType SourceAddress, const uint8* TargetAddressPtr, Fls_LengthType Length )
{
  Std_ReturnType u_RetVal = E_NOT_OK;
  uint32 u_SourceAddress = SourceAddress + sQsf.p_QsfCfg->u_NvmOffset;
  Std_ReturnType u_lock = u_Qsf_GetLock();

  if (E_OK == u_lock)
  {
   MemIf_StatusType status = sQsf.u_QsfStatus;
    if (status == MEMIF_IDLE)
    {
      sQsf.u_QsfStatus = MEMIF_BUSY;
      sQsf.u_Job = QSF_COMPARE;
      sQsf.u_QsfJobStatus = MEMIF_JOB_PENDING;
      sQsf.u_CurrentAddr = u_SourceAddress;
      sQsf.u_EndAddr = u_SourceAddress + Length;
      // PRQA S 0311 3 // 2017-10-25; uidv7790
      // summary: Msg(3:0311) Dangerous pointer cast results in loss of const qualification.
      // reason: We use p_CurrentRam in both cases (read and write access). Cast is needed in the second case here.
      sQsf.p_CurrentRam = (uint8*)TargetAddressPtr;
      sQsf.u_RetVal = PAL_QSF_OK;
      u_RetVal = (Std_ReturnType)(sQsf.u_RetVal);
    }
    v_Qsf_Unlock();
  }
  return u_RetVal;
}

// prepare action for blank check
Std_ReturnType Fls_BlankCheck( Fls_AddressType TargetAddress, Fls_LengthType Length )
{
  Std_ReturnType u_RetVal = E_NOT_OK;
  uint32 u_TargetAddress = TargetAddress + sQsf.p_QsfCfg->u_NvmOffset;
  Std_ReturnType u_lock = u_Qsf_GetLock();

  if (E_OK == u_lock)
  {
   MemIf_StatusType status = sQsf.u_QsfStatus;
    if (status == MEMIF_IDLE)
    {
      sQsf.u_QsfStatus = MEMIF_BUSY;
      sQsf.u_Job = QSF_BLANKCHECK;
      sQsf.u_QsfJobStatus = MEMIF_JOB_PENDING;
      sQsf.u_CurrentAddr = u_TargetAddress;
      sQsf.u_EndAddr = u_TargetAddress + Length;
      sQsf.u_RetVal = PAL_QSF_OK;
      u_RetVal = (Std_ReturnType)(sQsf.u_RetVal);
    }

    v_Qsf_Unlock();
  }

  return u_RetVal;
}
#endif

// abort action
void Fls_Cancel(void)
{
  Std_ReturnType u_status;
  Std_ReturnType u_lock = u_Qsf_GetLock();

  if (E_OK != u_lock)
  {
    return;
  }
  do
  {
    u_status = PAL_u_QsfIsBusy();
  } while ( u_status == (Std_ReturnType)PAL_QSF_BUSY);
  sQsf.u_QsfJobStatus = MEMIF_JOB_CANCELED;
  sQsf.u_Job = QSF_IDLE;
  sQsf.u_QsfStatus = MEMIF_IDLE;
  v_Qsf_Unlock();
}

// change mode
void Fls_SetMode( MemIf_ModeType Mode )
{
  sQsf.Qsf_Mode = Mode;
}

// module status
MemIf_StatusType Fls_GetStatus(void)
{
  return sQsf.u_QsfStatus;
}

// job status
MemIf_JobResultType Fls_GetJobResult(void)
{
  return sQsf.u_QsfJobStatus;
}

// mandatory Autosar function
void Fls_GetVersionInfo( Std_VersionInfoType* VersioninfoPtr )
{
  //currently empty
}

#if (QSF_WRITE_API == STD_ON)
// helper function to calculate size to be written next
static uint32 u_QsfWriteSize(void)
{
  uint32 u_End;
  // 2021-03-10; uie23485
  // Summary: Message(3:0404)  More than one read access to volatile objects between sequence points.
  // Reason:  While reading two volatiles the order is unspecified.
  // PRQA S 0404 1
  uint32 u_Step = sQsf.u_EndAddr - sQsf.u_CurrentAddr;
  uint32 u_PageMask = sQsf.u_PageSize - 1U;
  MemIf_ModeType Mode = sQsf.Qsf_Mode;
 const Fls_ConfigType* QsfCfg = sQsf.p_QsfCfg;

  if (QsfCfg->ua_WriteAtOnce[Mode] < u_Step)
  {
    u_Step = QsfCfg->ua_WriteAtOnce[Mode];
  }
  uint32 PageSize = sQsf.u_PageSize;
  if (PageSize < u_Step)
  {
    u_Step = sQsf.u_PageSize;
  }
  u_End = sQsf.u_CurrentAddr + u_Step - 1u;
   uint32 CurrentAddr = sQsf.u_CurrentAddr;
  if ( (CurrentAddr & ~u_PageMask) != (u_End & ~u_PageMask) )
  {
    u_End = sQsf.u_CurrentAddr | u_PageMask;
  }
  u_Step = u_End - sQsf.u_CurrentAddr + 1u;

  return u_Step;
}
#endif

// helper function for actions at job end
static void v_QsfJobEnd(void)
{
  sQsf.u_QsfStatus = MEMIF_IDLE;  // currently sync mode
  sQsf.u_Job = QSF_IDLE;
  e_Qsf_ErrorCodes RetVal = sQsf.u_RetVal;
  if (RetVal == PAL_QSF_OK)
  {
    sQsf.u_QsfJobStatus = MEMIF_JOB_OK;
#ifdef FEE_JOB_END_NOTIFICATION
    FEE_JOB_END_NOTIFICATION();
#endif
  }
  else
  {
    sQsf.u_QsfJobStatus = MEMIF_JOB_FAILED;
#ifdef FEE_JOB_ERROR_NOTIFICATION
    FEE_JOB_ERROR_NOTIFICATION();
#endif
  }
}

// 2021-03-10; uie23485
// Summary: Message(3:7002) Amount of non cyclic paths very high
// Reason:  This is necessary for the main function
// PRQA S 7002 ++
// performing the actions
// uig00753 21.11.2023
// msg(3:7004) HIS metrics check: cyclomatic complexitiy.
// msg(3:7012) Info: number of executable lines per function.
// Reason : cannot modify the code
// PRQA S 7004,7012 1
void Fls_MainFunction(void)
{
  uint32 u_Step;
  uint32 u_dtus, u_maxdtus;
  Std_ReturnType u_lock = u_Qsf_GetLock();
  e_Qsf_ErrorCodes RetVal;
  e_Qsf_ErrorCodes RetVal1;
  uint32 current_addr;
  uint32 end_addr;
  MemIf_ModeType Mode;
  uint8* CurrentRam = sQsf.p_CurrentRam;
  const Fls_ConfigType* QsfCfg = sQsf.p_QsfCfg;

  if (E_OK != u_lock)
  {
    return;
  }

  switch(sQsf.u_Job)
  {
    case QSF_IDLE:
      break;
#if (QSF_WRITE_API == STD_ON)
    case QSF_ERASE:
      u_dtus = SYS_TimeGetSince(sQsf.u_StartTime);
      // PRQA S 4342 3 // 2017-10-25; uidv7790
      // summary: Msg(3:4342) An expression of 'essentially unsigned' type (unsigned char) is being cast to enum type 'e_Qsf_ErrorCodes'.
      // reason: enum is used to ensure debug comfort
      sQsf.u_RetVal = (e_Qsf_ErrorCodes)PAL_u_QsfIsBusy();
      RetVal = sQsf.u_RetVal;
      if (RetVal == PAL_QSF_OK)
      {
        u_Step = sQsf.u_SectorSize;
      // 2021-03-10; uie23485
      // Summary: Message(3:0404)  More than one read access to volatile objects between sequence points.
      // Reason:  While reading two volatiles the order is unspecified.
      // PRQA S 0404 1
        current_addr = sQsf.u_CurrentAddr;
        end_addr = sQsf.u_EndAddr;
        if (current_addr + u_Step <= end_addr)
        {
          // PRQA S 4342 3 // 2017-10-25; uidv7790
          // summary: Msg(3:4342) An expression of 'essentially unsigned' type (unsigned char) is being cast to enum type 'e_Qsf_ErrorCodes'.
          // reason: enum is used to ensure debug comfort
          sQsf.u_RetVal = (e_Qsf_ErrorCodes)PAL_u_QsfEraseSector(sQsf.u_CurrentAddr);
          sQsf.u_StartTime = SYS_TimeGetCount();
          sQsf.u_CurrentAddr += u_Step;
          sQsf.u_QsfStatus = MEMIF_BUSY;
          RetVal1 = sQsf.u_RetVal;
          if (RetVal1 != PAL_QSF_OK)
          {
            v_QsfJobEnd();
          }
        }
        else // finished
        {
          v_QsfJobEnd();
        }
      }
      else if (RetVal == PAL_QSF_BUSY)
      {
        u_maxdtus = 1000u * PAL_u_QsfGetSectorEraseTimeout_ms();
        if (u_dtus > u_maxdtus)
        {
          sQsf.u_RetVal = PAL_QSF_ERROR_TIMEOUT;
          v_QsfJobEnd();
        }
        else
        {
          // do nothing, let flash internal processing go on
        }
      }
      else
      {
        v_QsfJobEnd();
      }
      break;
    case QSF_ERASE_4K:
      u_dtus = SYS_TimeGetSince(sQsf.u_StartTime);
      // PRQA S 4342 3 // 2017-10-25; uidv7790
      // summary: Msg(3:4342) An expression of 'essentially unsigned' type (unsigned char) is being cast to enum type 'e_Qsf_ErrorCodes'.
      // reason: enum is used to ensure debug comfort
      sQsf.u_RetVal = (e_Qsf_ErrorCodes)PAL_u_QsfIsBusy();
      RetVal = sQsf.u_RetVal;
      if (RetVal == PAL_QSF_OK)
      {
        u_Step = sQsf.u_SectorSize;
        // 2021-03-10; uie23485
        // Summary: Message(3:0404)  More than one read access to volatile objects between sequence points.
        // Reason:  While reading two volatiles the order is unspecified.
        // PRQA S 0404 1
        current_addr = sQsf.u_CurrentAddr;
        end_addr = sQsf.u_EndAddr;
        if (current_addr + u_Step <= end_addr)
        {
          // PRQA S 4342 3 // 2017-10-25; uidv7790
          // summary: Msg(3:4342) An expression of 'essentially unsigned' type (unsigned char) is being cast to enum type 'e_Qsf_ErrorCodes'.
          // reason: enum is used to ensure debug comfort
          sQsf.u_RetVal = (e_Qsf_ErrorCodes)PAL_u_QsfEraseSector_4K(sQsf.u_CurrentAddr);
          sQsf.u_StartTime = SYS_TimeGetCount();
          sQsf.u_CurrentAddr += u_Step;
          sQsf.u_QsfStatus = MEMIF_BUSY;
           RetVal1 = sQsf.u_RetVal;
          if (RetVal1 != PAL_QSF_OK)
          {
            v_QsfJobEnd();
          }
        }
        else // finished
        {
          v_QsfJobEnd();
        }
      }
      else if (RetVal == PAL_QSF_BUSY)
      {
        u_maxdtus = 1000u * PAL_u_QsfGetSectorEraseTimeout_ms();
        if (u_dtus > u_maxdtus)
        {
          sQsf.u_RetVal = PAL_QSF_ERROR_TIMEOUT;
          v_QsfJobEnd();
        }
        else
        {
          // do nothing, let flash internal processing go on
        }
      }
      else
      {
        v_QsfJobEnd();
      }
      break;
    case QSF_WRITE:
      u_dtus = SYS_TimeGetSince(sQsf.u_StartTime);
      // PRQA S 4342 3 // 2017-10-25; uidv7790
      // summary: Msg(3:4342) An expression of 'essentially unsigned' type (unsigned char) is being cast to enum type 'e_Qsf_ErrorCodes'.
      // reason: enum is used to ensure debug comfort
      sQsf.u_RetVal = (e_Qsf_ErrorCodes)PAL_u_QsfIsBusy();
      RetVal = sQsf.u_RetVal;
      if (RetVal == PAL_QSF_OK) // flash is ready
      {
       current_addr = sQsf.u_CurrentAddr;
        end_addr = sQsf.u_EndAddr;
      // 2021-03-10; uie23485
      // Summary: Message(3:0404)  More than one read access to volatile objects between sequence points.
      // Reason:  While reading two volatiles the order is unspecified.
      // PRQA S 0404 9
        if (current_addr < end_addr)
        {
          u_Step = u_QsfWriteSize();
          // PRQA S 4342 3 // 2017-10-25; uidv7790
          // summary: Msg(3:4342) An expression of 'essentially unsigned' type (unsigned char) is being cast to enum type 'e_Qsf_ErrorCodes'.
          // reason: enum is used to ensure debug comfort
          sQsf.u_RetVal = (e_Qsf_ErrorCodes)PAL_u_QsfProgramPage(sQsf.u_CurrentAddr, sQsf.p_CurrentRam, u_Step);
          sQsf.u_StartTime = SYS_TimeGetCount();
          sQsf.u_CurrentAddr += u_Step;
          // PRQA S 0491 3 // 2017-10-24; uidv7790
          // summary: Msg(3:0491) Array subscripting applied to an object of pointer type.
          // reason: instead of pointer arithmetic: sQsf.p_CurrentRam += u_Step
          sQsf.p_CurrentRam = &(sQsf.p_CurrentRam[u_Step]); // QAC?
          sQsf.u_QsfStatus = MEMIF_BUSY; 
          RetVal1 = sQsf.u_RetVal;
          if (RetVal1 != PAL_QSF_OK)
          {
            v_QsfJobEnd();
          }
        }
        else // finished
        {
          v_QsfJobEnd();
        }
      }
      else if (RetVal == PAL_QSF_BUSY)
      {
        QsfCfg = sQsf.p_QsfCfg;
        Mode = sQsf.Qsf_Mode;
        u_maxdtus = QsfCfg->ua_WriteAtOnce[Mode] * PAL_u_QsfGetWordWriteTimeout_us();
        if (u_dtus > u_maxdtus)
        {
          sQsf.u_RetVal = PAL_QSF_ERROR_TIMEOUT;
          v_QsfJobEnd();
        }
        else
        {
          // do nothing, let flash internal processing go on
        }
      }
      else // write error
      {
        v_QsfJobEnd();
      }
      break;
    case QSF_OTP_WRITE:
      u_dtus = SYS_TimeGetSince(sQsf.u_StartTime);
      // PRQA S 4342 3 // 2022-09-06; uif08910
      // summary: Msg(3:4342) An expression of 'essentially unsigned' type (unsigned char) is being cast to enum type 'e_Qsf_ErrorCodes'.
      // reason: enum is used to ensure debug comfort
      sQsf.u_RetVal = (e_Qsf_ErrorCodes)PAL_u_QsfIsBusy();
      RetVal = sQsf.u_RetVal;
      if (RetVal == PAL_QSF_OK) // flash is ready
      {
        current_addr = sQsf.u_CurrentAddr;
        end_addr = sQsf.u_EndAddr;
        // 2022-12-23; uif08910
        // Summary: Message(3:0404)  More than one read access to volatile objects between sequence points.
        // Reason:  While reading two volatiles the order is unspecified.
        // PRQA S 0404 11
        if (current_addr < end_addr)
        {
          u_Step = sQsf.p_QsfCfg->ua_ReadAtOnce[sQsf.Qsf_Mode];
          current_addr = sQsf.u_CurrentAddr;
          end_addr = sQsf.u_EndAddr;
          if (end_addr - current_addr < u_Step)
          {
            u_Step = sQsf.u_EndAddr - sQsf.u_CurrentAddr;
          }
          // PRQA S 4342 3 // 2022-12-22; uif08910
          // summary: Msg(3:4342) An expression of 'essentially unsigned' type (unsigned char) is being cast to enum type 'e_Qsf_ErrorCodes'.
          // reason: Msg(3:4342) Enum is used to ensure debug comfort.
          sQsf.u_RetVal = (e_Qsf_ErrorCodes)PAL_u_QsfWriteOTPArray(CurrentRam, u_Step, current_addr);

          sQsf.u_StartTime = SYS_TimeGetCount();
          sQsf.u_CurrentAddr += u_Step;
          // PRQA S 0491 3 // 2022-09-06; uif08910
          // summary: Msg(3:0491) Array subscripting applied to an object of pointer type.
          // reason: instead of pointer arithmetic: sQsf.p_CurrentRam += u_Step
          sQsf.p_CurrentRam = &(sQsf.p_CurrentRam[u_Step]); // QAC?
          sQsf.u_QsfStatus = MEMIF_BUSY;
          RetVal1 = sQsf.u_RetVal;
          if (RetVal1 != PAL_QSF_OK)
          {
            v_QsfJobEnd();
          }
        }
        else // finished
        {
          v_QsfJobEnd();
        }
      }
      else if (RetVal == PAL_QSF_BUSY)
      {
        QsfCfg = sQsf.p_QsfCfg;
        Mode = sQsf.Qsf_Mode;
        u_maxdtus = QsfCfg->ua_WriteAtOnce[Mode] * PAL_u_QsfGetWordWriteTimeout_us();
        if (u_dtus > u_maxdtus)
        {
          sQsf.u_RetVal = PAL_QSF_ERROR_TIMEOUT;
          v_QsfJobEnd();
        }
        else
        {
          // do nothing, let flash internal processing go on
        }
      }
      else // write error
      {
        v_QsfJobEnd();
      }
      break;
#endif
    case QSF_READ:
      // PRQA S 4342 3 // 2017-10-25; uidv7790
      // summary: Msg(3:4342) An expression of 'essentially unsigned' type (unsigned char) is being cast to enum type 'e_Qsf_ErrorCodes'.
      // reason: enum is used to ensure debug comfort
      sQsf.u_RetVal = (e_Qsf_ErrorCodes)PAL_u_QsfIsBusy();
      RetVal = sQsf.u_RetVal;
      if (RetVal == PAL_QSF_OK) // flash is ready
      {
       current_addr = sQsf.u_CurrentAddr;
       end_addr = sQsf.u_EndAddr;
      // 2021-03-10; uie23485
      // Summary: Message(3:0404)  More than one read access to volatile objects between sequence points.
      // Reason:  While reading two volatiles the order is unspecified.
      // PRQA S 0404 11
        if (current_addr < end_addr)
        {
          u_Step = sQsf.p_QsfCfg->ua_ReadAtOnce[sQsf.Qsf_Mode];
          current_addr = sQsf.u_CurrentAddr;
          end_addr = sQsf.u_EndAddr;
          if (end_addr - current_addr < u_Step)
          {
            u_Step = sQsf.u_EndAddr - sQsf.u_CurrentAddr;
          }
          CurrentRam = sQsf.p_CurrentRam;
          // PRQA S 4342 3 // 2017-10-25; uidv7790
          // summary: Msg(3:4342) An expression of 'essentially unsigned' type (unsigned char) is being cast to enum type 'e_Qsf_ErrorCodes'.
          // reason: enum is used to ensure debug comfort
          sQsf.u_RetVal = (e_Qsf_ErrorCodes)PAL_u_QsfRead(current_addr , CurrentRam, u_Step);
          sQsf.u_CurrentAddr += u_Step;
          // PRQA S 0491 3 // 2017-10-24; uidv7790
          // summary: Msg(3:0491) Array subscripting applied to an object of pointer type.
          // reason: instead of pointer arithmetic: sQsf.p_CurrentRam += u_Step
          sQsf.p_CurrentRam = &(sQsf.p_CurrentRam[u_Step]); // QAC?
          sQsf.u_QsfStatus = MEMIF_BUSY;
          RetVal1 = sQsf.u_RetVal;
          if (RetVal1 != PAL_QSF_OK)
          {
            v_QsfJobEnd();
          }
        }
        else // finished
        {
          v_QsfJobEnd();
        }
      }
      else if (RetVal == PAL_QSF_BUSY)
      {
        // do nothing, let flash internal processing go on
      }
      else // write error
      {
        v_QsfJobEnd();
      }
      break;
    case QSF_OTP_READ:
      // PRQA S 4342 3 // 2022-09-06; uif08910
      // summary: Msg(3:4342) An expression of 'essentially unsigned' type (unsigned char) is being cast to enum type 'e_Qsf_ErrorCodes'.
      // reason: enum is used to ensure debug comfort
      sQsf.u_RetVal = (e_Qsf_ErrorCodes)PAL_u_QsfIsBusy();
      RetVal = sQsf.u_RetVal;
      if (RetVal == PAL_QSF_OK) // flash is ready
      {
       current_addr = sQsf.u_CurrentAddr;
       end_addr = sQsf.u_EndAddr;
      // 2022-09-06; uif08910
      // Summary: Message(3:0404)  More than one read access to volatile objects between sequence points.
      // Reason:  While reading two volatiles the order is unspecified.
      // PRQA S 0404 13
        if (current_addr < end_addr)
        {
          u_Step = sQsf.p_QsfCfg->ua_ReadAtOnce[sQsf.Qsf_Mode];
          current_addr = sQsf.u_CurrentAddr;
          end_addr = sQsf.u_EndAddr;
          if (end_addr - current_addr < u_Step)
          {
            u_Step = sQsf.u_EndAddr - sQsf.u_CurrentAddr;
          }
          // PRQA S 4342 3 // 2022-09-06; uif08910
          // summary: Msg(3:4342) An expression of 'essentially unsigned' type (unsigned char) is being cast to enum type 'e_Qsf_ErrorCodes'.
          // reason: enum is used to ensure debug comfort
          sQsf.u_RetVal = (e_Qsf_ErrorCodes)PAL_u_QsfReadOTPArray(sQsf.p_CurrentRam, u_Step, sQsf.u_CurrentAddr);

          sQsf.u_CurrentAddr += u_Step;
          // PRQA S 0491 3 // 2022-09-06; uif08910
          // summary: Msg(3:0491) Array subscripting applied to an object of pointer type.
          // reason: instead of pointer arithmetic: sQsf.p_CurrentRam += u_Step
          sQsf.p_CurrentRam = &(sQsf.p_CurrentRam[u_Step]); // QAC?
          sQsf.u_QsfStatus = MEMIF_BUSY;
          RetVal1 = sQsf.u_RetVal;
          if (RetVal1 != PAL_QSF_OK)
          {
            v_QsfJobEnd();
          }
        }
        else // finished
        {
          v_QsfJobEnd();
        }
      }
      else if (RetVal == PAL_QSF_BUSY)
      {
        // do nothing, let flash internal processing go on
      }
      else // write error
      {
        v_QsfJobEnd();
      }
      break;
#if (QSF_WRITE_API == STD_ON)
    case QSF_COMPARE:
      // PRQA S 4342 3 // 2017-10-25; uidv7790
      // summary: Msg(3:4342) An expression of 'essentially unsigned' type (unsigned char) is being cast to enum type 'e_Qsf_ErrorCodes'.
      // reason: enum is used to ensure debug comfort
      sQsf.u_RetVal = (e_Qsf_ErrorCodes)PAL_u_QsfIsBusy();
      RetVal = sQsf.u_RetVal;
      if (RetVal == PAL_QSF_OK) // flash is ready
      {
        current_addr = sQsf.u_CurrentAddr;
        end_addr = sQsf.u_EndAddr;
        // 2021-03-10; uie23485
        // Summary: Message(3:0404)  More than one read access to volatile objects between sequence points.
        // Reason:  While reading two volatiles the order is unspecified.
        // PRQA S 0404 11
        if (current_addr < end_addr)
        {
          u_Step = sQsf.p_QsfCfg->ua_ReadAtOnce[sQsf.Qsf_Mode];
          current_addr = sQsf.u_CurrentAddr;
          end_addr = sQsf.u_EndAddr;
          if (end_addr - current_addr < u_Step)
          {
            u_Step = sQsf.u_EndAddr - sQsf.u_CurrentAddr;
          }
           CurrentRam = sQsf.p_CurrentRam;
          // PRQA S 4342 3 // 2017-10-25; uidv7790
          // summary: Msg(3:4342) An expression of 'essentially unsigned' type (unsigned char) is being cast to enum type 'e_Qsf_ErrorCodes'.
          // reason: enum is used to ensure debug comfort
          sQsf.u_RetVal = (e_Qsf_ErrorCodes)PAL_u_QsfVerify(current_addr, CurrentRam, u_Step);
          sQsf.u_CurrentAddr += u_Step;
          // PRQA S 0491 3 // 2017-10-24; uidv7790
          // summary: Msg(3:0491) Array subscripting applied to an object of pointer type.
          // reason: instead of pointer arithmetic: sQsf.p_CurrentRam += u_Step
          sQsf.p_CurrentRam = &(sQsf.p_CurrentRam[u_Step]);
          sQsf.u_QsfStatus = MEMIF_BUSY;
          RetVal1 = sQsf.u_RetVal;
          if (RetVal1 != PAL_QSF_OK)
          {
            v_QsfJobEnd();
          }
        }
        else // finished
        {
          v_QsfJobEnd();
        }
      }
      else if (RetVal == PAL_QSF_BUSY)
      {
        // do nothing, let flash internal processing go on
      }
      else // write error
      {
        v_QsfJobEnd();
      }
      break;
    case QSF_BLANKCHECK:
      // PRQA S 4342 3 // 2017-10-25; uidv7790
      // summary: Msg(3:4342) An expression of 'essentially unsigned' type (unsigned char) is being cast to enum type 'e_Qsf_ErrorCodes'.
      // reason: enum is used to ensure debug comfort
      sQsf.u_RetVal = (e_Qsf_ErrorCodes)PAL_u_QsfIsBusy();
      RetVal = sQsf.u_RetVal;
      if (RetVal == PAL_QSF_OK) // flash is ready
      {
        current_addr = sQsf.u_CurrentAddr;
          end_addr = sQsf.u_EndAddr;
        // 2021-03-10; uie23485
        // Summary: Message(3:0404)  More than one read access to volatile objects between sequence points.
        // Reason:  While reading two volatiles the order is unspecified.
        // PRQA S 0404 6
        if (current_addr < end_addr)
        {
          u_Step = sQsf.p_QsfCfg->ua_ReadAtOnce[sQsf.Qsf_Mode];
          current_addr = sQsf.u_CurrentAddr;
          end_addr = sQsf.u_EndAddr;
          if (end_addr - current_addr < u_Step)
          {
            u_Step = end_addr - current_addr;
          }
          // PRQA S 4342 3 // 2017-10-25; uidv7790
          // summary: Msg(3:4342) An expression of 'essentially unsigned' type (unsigned char) is being cast to enum type 'e_Qsf_ErrorCodes'.
          // reason: enum is used to ensure debug comfort
          sQsf.u_RetVal = (e_Qsf_ErrorCodes)PAL_u_QsfBlankCheck(sQsf.u_CurrentAddr, u_Step);
          sQsf.u_CurrentAddr += u_Step;
          sQsf.u_QsfStatus = MEMIF_BUSY;
          RetVal1 = sQsf.u_RetVal;
          if (RetVal1 != PAL_QSF_OK)
          {
            v_QsfJobEnd();
          }
        }
        else // finished
        {
          v_QsfJobEnd();
        }
      }
      else if (RetVal == PAL_QSF_BUSY)
      {
        // do nothing, let flash internal processing go on
      }
      else // write error
      {
        v_QsfJobEnd();
      }
      break;
#endif
    default:
      // internal error
      sQsf.u_QsfStatus = MEMIF_UNINIT;
      sQsf.u_QsfJobStatus = MEMIF_JOB_FAILED;
      break;
  }
  v_Qsf_Unlock();
  // PRQA S 7004 3 // 2017-10-25; uidv7790
  // summary: Msg(3:7004) HIS metrics check: cyclomatic complexitiy.
  // reason: Each case of this large switch-case could go into separate function.
}
// PRQA S 7002 --

static Std_ReturnType u_Qsf_GetLock(void)
{
  Std_ReturnType status;
  QSF_ENTER_CRITICAL_SECTION;
  status = (Std_ReturnType)(sQsf.Lock);
  if (status == E_OK)
  {
    sQsf.Lock = PAL_QSF_BUSY;
  }
  QSF_EXIT_CRITICAL_SECTION;
  return status;
}

static void v_Qsf_Unlock(void)
{
  QSF_ENTER_CRITICAL_SECTION;
  sQsf.Lock = PAL_QSF_OK;
  QSF_EXIT_CRITICAL_SECTION;
}


uint32 Fls_v_GetSectorSize(void)
{
  return sQsf.u_SectorSize;
}


Std_ReturnType Fls_v_SetSectorSize( uint32 u_SectorSize)
{
  Std_ReturnType u_RetVal = E_NOT_OK;
  uint8 u_PalRetVal;
  u_PalRetVal = Pal_u_Qsf_SetSectorSize (u_SectorSize);
  if(u_PalRetVal == (uint8)PAL_QSF_OK )
  {
    u_RetVal = E_OK;
    sQsf.u_SectorSize = PAL_u_QsfGetSectorSize();
  }
  return u_RetVal;
}

Std_ReturnType Fls_v_EmergencyUnlock(void)
{
  MemIf_JobResultType u_JobStatus;
  Std_ReturnType      u_Status;
  uint32              u_StartTime_us;
  uint32              u_TimeElapsed_us;

  // Get current time
  u_StartTime_us = SYS_TimeGetCount();
  do
  {
    // Get QSF busy status
    u_Status = PAL_u_QsfIsBusy();
    // Measure time spent in a loop
    u_TimeElapsed_us = SYS_TimeGetSince(u_StartTime_us);
  }
  while ((u_TimeElapsed_us < PAL_QSF_SDF_MAX_tPP_US) && (u_Status == (Std_ReturnType) PAL_QSF_BUSY));

  // Unlock QSF
  sQsf.Lock = PAL_QSF_OK;

  // Get job status
  u_JobStatus = Fls_GetJobResult();
  // If job status is PENDING
  if(u_JobStatus == MEMIF_JOB_PENDING)
  {
    // Cancel curent job
    sQsf.u_QsfJobStatus = MEMIF_JOB_CANCELED;
  }

  // Prepare QSF for next job
  sQsf.u_Job = QSF_IDLE;
  sQsf.u_QsfStatus = MEMIF_IDLE;

  return u_Status;
}

#ifdef __cplusplus
}
#endif

