// COMPANY:   Continental Automotive
// COMPONENT: QSF
// VERSION:   $Revision: 1.4 $

#include <Std_Types.h>
#include <Fls.h>
#include <MemIf.h>
#include <Fls_Cfg.h>
#include <sys_time.h>
#include <pal_qsf.h>

#if defined(QSF_USE_DMA_ASYNC) || defined(QSF_USE_DMA)
#include <Dma.h>
#endif

#if defined(QSF_ENTER_CRITICAL_SECTION) && defined(QSF_EXIT_CRITICAL_SECTION)
#include <Os.h>
#endif

#ifdef __cplusplus
extern "C"
{
#endif

// PRQA S 1011 EOF // 2017-10-25; uidv7790
// summary: Msg(3:1011) [C99] Use of '//' comment.
// reason: According coding template the comment style // shall be used.

// PRQA S 0404 EOF // 2022-10-06; uic12551
// summary: Msg(3:0404) More than one read access to volatile objects between sequence points.
// reason: Multiple reads of sQsf, which is volatile

// critical section handling
#ifndef QSF_ENTER_CRITICAL_SECTION
#define QSF_ENTER_CRITICAL_SECTION
#endif
#ifndef QSF_EXIT_CRITICAL_SECTION
#define QSF_EXIT_CRITICAL_SECTION
#endif
// internal state indicating the current action
typedef enum
{
  QSF_IDLE,        // ready for next action
  QSF_INIT,        // initialization ongoing
  QSF_READ,        // reading from SDF
  QSF_WRITE,       // writing to SDF
  QSF_ERASE,       // erasing SDF sector(s)
  QSF_ERASE_4K,     // erasing SDF 4K subsector /* Conti Change uids8871 */
  QSF_COMPARE,     // compare given RAM data with SDF data
  QSF_BLANKCHECK   // check if area is erased
} tQsfJob;

// all static object data, is of interest when debugging the component
typedef struct
{
  MemIf_StatusType u_QsfStatus;        // current status of the module
  tQsfJob u_Job;                       // current active job
  MemIf_JobResultType u_QsfJobStatus;  // result of current or last job
  e_Qsf_ErrorCodes u_RetVal;             // result of last function call to pal, could be enum for better debug
  MemIf_ModeType Qsf_Mode;             // current MemIf mode (slow or fast)
  uint32 u_CurrentAddr;                // start address for current action
  uint32 u_EndAddr;                    // end address for current action
  uint8 *p_CurrentRam;           // RAM pointer for current action
  const Fls_ConfigType *p_QsfCfg;      // pointer to current configuration
  uint32 u_PageSize;                   // page size of current SDF
  uint32 u_SectorSize;                 // sector size of current SDF
  uint32 u_InitDone;                   // flag indicating if module was already initialized
  uint32 u_StartTime;                  // store
  e_Qsf_ErrorCodes Lock;               ///> Lock flag to implement exclusive area
} tQsfState;

// all static object data, is of interest when debugging the component
static tQsfState sQsf;

// local functions
static uint32 u_QsfWriteSize(void);
static void v_QsfJobEnd(void);
static Std_ReturnType u_Qsf_GetLock(void);
static void v_Qsf_Unlock(void);

// module initialisation
void Fls_Init(const Fls_ConfigType* ConfigPtr)
{
  Std_ReturnType u_lock = u_Qsf_GetLock();
  if (E_OK != u_lock)
  {
    /* do nothing */
  }
  else
  {
    sQsf.u_Job = QSF_INIT;

#if (QSF_WRITE_API == STD_ON)
    /* avoid reinitialisation */
    if (sQsf.u_InitDone != 1u)
#endif
    {
      // PRQA S 4342 3 // 2017-10-25; uidv7790
      // summary: Msg(3:4342) An expression of 'essentially unsigned' type (unsigned char) is being cast to enum type 'e_Qsf_ErrorCodes'.
      // reason: enum is used to ensure debug comfort
      sQsf.u_RetVal = (e_Qsf_ErrorCodes)PAL_u_QsfInit(ConfigPtr);
    }

    if (sQsf.u_RetVal == PAL_QSF_OK)
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
}

// prepare read action
Std_ReturnType Fls_Read(Fls_AddressType SourceAddress, uint8* TargetAddressPtr, Fls_LengthType Length)
{
  Std_ReturnType u_RetVal = E_NOT_OK;
  uint32 u_SourceAddress = SourceAddress + sQsf.p_QsfCfg->u_NvmOffset;
  Std_ReturnType u_lock = u_Qsf_GetLock();

  if (E_OK != u_lock)
  {
    u_RetVal = E_NOT_OK;
  }
  else
  {
    if (sQsf.u_QsfStatus == MEMIF_IDLE)
    {
      sQsf.u_QsfStatus = MEMIF_BUSY;
      sQsf.u_Job = QSF_READ;
      sQsf.u_QsfJobStatus = MEMIF_JOB_PENDING;
      sQsf.u_CurrentAddr = u_SourceAddress;
      sQsf.u_EndAddr = u_SourceAddress + Length;
      sQsf.p_CurrentRam = TargetAddressPtr;
      sQsf.u_RetVal = PAL_QSF_OK;
      u_RetVal = E_OK;
    }
    else
    {
      /* do nothing */
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

  if (u_TargetAddress < sQsf.p_QsfCfg->u_WriteWindowStart)
  {
    u_RetVal = E_NOT_OK;
  }
  else if ((u_TargetAddress + Length) > (sQsf.p_QsfCfg->u_WriteWindowStart + sQsf.p_QsfCfg->u_WriteWindowSize))
  {
    u_RetVal = E_NOT_OK;
  }
  else
  {
    u_lock = u_Qsf_GetLock();
    if (E_OK != u_lock)
    {
      u_RetVal = E_NOT_OK;
    }
    else
    {
      if (sQsf.u_QsfStatus == MEMIF_IDLE)
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
        u_RetVal = E_OK;
      }
      else
      {
        u_RetVal = E_NOT_OK;
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

  if (u_TargetAddress < sQsf.p_QsfCfg->u_WriteWindowStart)
  {
    u_RetVal = E_NOT_OK;
  }
  else if (u_TargetAddress + Length > sQsf.p_QsfCfg->u_WriteWindowStart + sQsf.p_QsfCfg->u_WriteWindowSize)
  {
    u_RetVal = E_NOT_OK;
  }
  else if (0U != (u_TargetAddress & (sQsf.u_SectorSize - 1u)))
  {
    u_RetVal = E_NOT_OK;
  }
  else if (0U != (Length & (sQsf.u_SectorSize - 1u)))
  {
    u_RetVal = E_NOT_OK;
  }
  else
  {
    u_lock = u_Qsf_GetLock();
    if (E_OK != u_lock)
    {
      u_RetVal = E_NOT_OK;
    }
    else
    {
      if (sQsf.u_QsfStatus == MEMIF_IDLE)
      {
        sQsf.u_QsfStatus = MEMIF_BUSY;
        sQsf.u_Job = QSF_ERASE;
        sQsf.u_QsfJobStatus = MEMIF_JOB_PENDING;
        sQsf.u_CurrentAddr = u_TargetAddress;
        sQsf.u_EndAddr = u_TargetAddress + Length;
        sQsf.u_RetVal = PAL_QSF_OK;
        u_RetVal = E_OK;
      }
      else
      {
        u_RetVal = E_NOT_OK;
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

  if (u_TargetAddress < sQsf.p_QsfCfg->u_WriteWindowStart)
  {
    u_RetVal = E_NOT_OK;
  }
  else if (u_TargetAddress + Length > sQsf.p_QsfCfg->u_WriteWindowStart + sQsf.p_QsfCfg->u_WriteWindowSize)
  {
    u_RetVal = E_NOT_OK;
  }
  else if (0U != (u_TargetAddress & (sQsf.u_SectorSize - 1u)))
  {
    u_RetVal = E_NOT_OK;
  }
  else if (0U != (Length & (sQsf.u_SectorSize - 1u)))
  {
    u_RetVal = E_NOT_OK;
  }
  else
  {
    u_lock = u_Qsf_GetLock();
    if (E_OK != u_lock)
    {
      u_RetVal = E_NOT_OK;
    }
    else
    {
      if (sQsf.u_QsfStatus == MEMIF_IDLE)
      {
        sQsf.u_QsfStatus = MEMIF_BUSY;
        sQsf.u_Job = QSF_ERASE_4K;
        sQsf.u_QsfJobStatus = MEMIF_JOB_PENDING;
        sQsf.u_CurrentAddr = u_TargetAddress;
        sQsf.u_EndAddr = u_TargetAddress + Length;
        sQsf.u_RetVal = PAL_QSF_OK;
        u_RetVal = (Std_ReturnType)(sQsf.u_RetVal);
      }
      else
      {
        u_RetVal = E_NOT_OK;
      }
      v_Qsf_Unlock();
    }
  }

  return u_RetVal;
}

// prepare compare action
Std_ReturnType Fls_Compare(Fls_AddressType SourceAddress, const uint8* TargetAddressPtr, Fls_LengthType Length)
{
  Std_ReturnType u_RetVal = E_NOT_OK;
  uint32 u_SourceAddress = SourceAddress + sQsf.p_QsfCfg->u_NvmOffset;
  Std_ReturnType u_lock = u_Qsf_GetLock();

  if (E_OK != u_lock)
  {
    u_RetVal = E_NOT_OK;
  }
  else
  {
    if (sQsf.u_QsfStatus == MEMIF_IDLE)
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
      u_RetVal = E_OK;
    }
    else
    {
      u_RetVal = E_NOT_OK;
    }
    v_Qsf_Unlock();
  }

  return u_RetVal;
}

// prepare action for blank check
Std_ReturnType Fls_BlankCheck(Fls_AddressType TargetAddress, Fls_LengthType Length)
{
  Std_ReturnType u_RetVal = E_NOT_OK;
  uint32 u_TargetAddress = TargetAddress + sQsf.p_QsfCfg->u_NvmOffset;
  Std_ReturnType u_lock = u_Qsf_GetLock();

  if (E_OK != u_lock)
  {
    u_RetVal = E_NOT_OK;
  }
  else
  {
    if (sQsf.u_QsfStatus == MEMIF_IDLE)
    {
      sQsf.u_QsfStatus = MEMIF_BUSY;
      sQsf.u_Job = QSF_BLANKCHECK;
      sQsf.u_QsfJobStatus = MEMIF_JOB_PENDING;
      sQsf.u_CurrentAddr = u_TargetAddress;
      sQsf.u_EndAddr = u_TargetAddress + Length;
      sQsf.u_RetVal = PAL_QSF_OK;
      u_RetVal = (Std_ReturnType)(sQsf.u_RetVal);
    }
    else
    {
      u_RetVal = E_NOT_OK;
    }
    v_Qsf_Unlock();
  }

  return u_RetVal;
}
#endif /* QSF_WRITE_API == STD_ON */

// abort action
void Fls_Cancel(void)
{
  Std_ReturnType u_status;
  Std_ReturnType u_lock = u_Qsf_GetLock();

  if (E_OK != u_lock)
  {
    /* do nothing */
  }
  else
  {
    do
    {
      u_status = PAL_u_QsfIsBusy();
    } while (u_status == (Std_ReturnType)PAL_QSF_BUSY);
    sQsf.u_QsfJobStatus = MEMIF_JOB_CANCELED;
    sQsf.u_Job = QSF_IDLE;
    sQsf.u_QsfStatus = MEMIF_IDLE;
    v_Qsf_Unlock();
  }
}

// change mode
void Fls_SetMode(MemIf_ModeType Mode)
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
void Fls_GetVersionInfo(Std_VersionInfoType* VersioninfoPtr)
{
//currently empty
}

#if (QSF_WRITE_API == STD_ON)
// helper function to calculate size to be written next
static uint32 u_QsfWriteSize(void)
{
  uint32 u_End;
  uint32 u_Step = sQsf.u_EndAddr - sQsf.u_CurrentAddr;
  uint32 u_PageMask = sQsf.u_PageSize - 1U;

  if (sQsf.p_QsfCfg->ua_WriteAtOnce[sQsf.Qsf_Mode] < u_Step)
  {
    u_Step = sQsf.p_QsfCfg->ua_WriteAtOnce[sQsf.Qsf_Mode];
  }
  if (sQsf.u_PageSize < u_Step)
  {
    u_Step = sQsf.u_PageSize;
  }
  u_End = sQsf.u_CurrentAddr + u_Step - 1u;
  if ((sQsf.u_CurrentAddr & ~u_PageMask) != (u_End & ~u_PageMask))
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
  if (sQsf.u_RetVal == PAL_QSF_OK)
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

// performing the actions
// 2023-10-25; uidv8891
// summary: Msg(3:7004) HIS metrics check: cyclomatic complexitiy.
// reason: Complexity is accepted.
// PRQA S 7004 1
void Fls_MainFunction(void)
{
  uint32 u_Step;
  uint32 u_dtus, u_maxdtus;
  Std_ReturnType u_lock = u_Qsf_GetLock();

  if (E_OK != u_lock)
  {
    /* do nothing */
  }
  else
  {

    switch (sQsf.u_Job)
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
        if (sQsf.u_RetVal == PAL_QSF_OK)
        {
          u_Step = sQsf.u_SectorSize;
          if (sQsf.u_CurrentAddr + u_Step <= sQsf.u_EndAddr)
          {
            // PRQA S 4342 3 // 2017-10-25; uidv7790
            // summary: Msg(3:4342) An expression of 'essentially unsigned' type (unsigned char) is being cast to enum type 'e_Qsf_ErrorCodes'.
            // reason: enum is used to ensure debug comfort
            sQsf.u_RetVal = (e_Qsf_ErrorCodes)PAL_u_QsfEraseSector(sQsf.u_CurrentAddr);
            sQsf.u_StartTime = SYS_TimeGetCount();
            sQsf.u_CurrentAddr += u_Step;
            sQsf.u_QsfStatus = MEMIF_BUSY;
            if (sQsf.u_RetVal != PAL_QSF_OK)
            {
              v_QsfJobEnd();
            }
          }
          else // finished
          {
            v_QsfJobEnd();
          }
        }
        else if (sQsf.u_RetVal == PAL_QSF_BUSY)
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
        if (sQsf.u_RetVal == PAL_QSF_OK)
        {
          u_Step = sQsf.u_SectorSize;
          if (sQsf.u_CurrentAddr + u_Step <= sQsf.u_EndAddr)
          {
            // PRQA S 4342 3 // 2017-10-25; uidv7790
            // summary: Msg(3:4342) An expression of 'essentially unsigned' type (unsigned char) is being cast to enum type 'e_Qsf_ErrorCodes'.
            // reason: enum is used to ensure debug comfort
            sQsf.u_RetVal = (e_Qsf_ErrorCodes)PAL_u_QsfEraseSector_4K(sQsf.u_CurrentAddr);
            sQsf.u_StartTime = SYS_TimeGetCount();
            sQsf.u_CurrentAddr += u_Step;
            sQsf.u_QsfStatus = MEMIF_BUSY;
            if (sQsf.u_RetVal != PAL_QSF_OK)
            {
              v_QsfJobEnd();
            }
          }
          else // finished
          {
            v_QsfJobEnd();
          }
        }
        else if (sQsf.u_RetVal == PAL_QSF_BUSY)
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
        if (sQsf.u_RetVal == PAL_QSF_OK) // flash is ready
        {
          if (sQsf.u_CurrentAddr < sQsf.u_EndAddr)
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
            if (sQsf.u_RetVal != PAL_QSF_OK)
            {
              v_QsfJobEnd();
            }
          }
          else // finished
          {
            v_QsfJobEnd();
          }
        }
        else if (sQsf.u_RetVal == PAL_QSF_BUSY)
        {
          u_maxdtus = sQsf.p_QsfCfg->ua_WriteAtOnce[sQsf.Qsf_Mode] * PAL_u_QsfGetWordWriteTimeout_us();
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
        if (sQsf.u_RetVal == PAL_QSF_OK) // flash is ready
        {
          if (sQsf.u_CurrentAddr < sQsf.u_EndAddr)
          {
            u_Step = sQsf.p_QsfCfg->ua_ReadAtOnce[sQsf.Qsf_Mode];
            if (sQsf.u_EndAddr - sQsf.u_CurrentAddr < u_Step)
            {
              u_Step = sQsf.u_EndAddr - sQsf.u_CurrentAddr;
            }
            // PRQA S 4342 3 // 2017-10-25; uidv7790
            // summary: Msg(3:4342) An expression of 'essentially unsigned' type (unsigned char) is being cast to enum type 'e_Qsf_ErrorCodes'.
            // reason: enum is used to ensure debug comfort
#if QSF_USE_DMA_ASYNC
          sQsf.u_RetVal = (e_Qsf_ErrorCodes)PAL_u_QsfRead(sQsf.u_CurrentAddr, sQsf.p_CurrentRam, u_Step, NULL);
#else
            sQsf.u_RetVal = (e_Qsf_ErrorCodes)PAL_u_QsfRead(sQsf.u_CurrentAddr, sQsf.p_CurrentRam, u_Step);
#endif
            sQsf.u_CurrentAddr += u_Step;
            // PRQA S 0491 3 // 2017-10-24; uidv7790
            // summary: Msg(3:0491) Array subscripting applied to an object of pointer type.
            // reason: instead of pointer arithmetic: sQsf.p_CurrentRam += u_Step
            sQsf.p_CurrentRam = &(sQsf.p_CurrentRam[u_Step]); // QAC?
            sQsf.u_QsfStatus = MEMIF_BUSY;
            if (sQsf.u_RetVal != PAL_QSF_OK)
            {
              v_QsfJobEnd();
            }
          }
          else // finished
          {
            v_QsfJobEnd();
          }
        }
        else if (sQsf.u_RetVal == PAL_QSF_BUSY)
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
        if (sQsf.u_RetVal == PAL_QSF_OK) // flash is ready
        {
          if (sQsf.u_CurrentAddr < sQsf.u_EndAddr)
          {
            u_Step = sQsf.p_QsfCfg->ua_ReadAtOnce[sQsf.Qsf_Mode];
            if (sQsf.u_EndAddr - sQsf.u_CurrentAddr < u_Step)
            {
              u_Step = sQsf.u_EndAddr - sQsf.u_CurrentAddr;
            }
            // PRQA S 4342 3 // 2017-10-25; uidv7790
            // summary: Msg(3:4342) An expression of 'essentially unsigned' type (unsigned char) is being cast to enum type 'e_Qsf_ErrorCodes'.
            // reason: enum is used to ensure debug comfort
            sQsf.u_RetVal = (e_Qsf_ErrorCodes)PAL_u_QsfVerify(sQsf.u_CurrentAddr, sQsf.p_CurrentRam, u_Step);
            sQsf.u_CurrentAddr += u_Step;
            // PRQA S 0491 3 // 2017-10-24; uidv7790
            // summary: Msg(3:0491) Array subscripting applied to an object of pointer type.
            // reason: instead of pointer arithmetic: sQsf.p_CurrentRam += u_Step
            sQsf.p_CurrentRam = &(sQsf.p_CurrentRam[u_Step]);
            sQsf.u_QsfStatus = MEMIF_BUSY;
            if (sQsf.u_RetVal != PAL_QSF_OK)
            {
              v_QsfJobEnd();
            }
          }
          else // finished
          {
            v_QsfJobEnd();
          }
        }
        else if (sQsf.u_RetVal == PAL_QSF_BUSY)
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
        if (sQsf.u_RetVal == PAL_QSF_OK) // flash is ready
        {
          if (sQsf.u_CurrentAddr < sQsf.u_EndAddr)
          {
            u_Step = sQsf.p_QsfCfg->ua_ReadAtOnce[sQsf.Qsf_Mode];
            if (sQsf.u_EndAddr - sQsf.u_CurrentAddr < u_Step)
            {
              u_Step = sQsf.u_EndAddr - sQsf.u_CurrentAddr;
            }
            // PRQA S 4342 3 // 2017-10-25; uidv7790
            // summary: Msg(3:4342) An expression of 'essentially unsigned' type (unsigned char) is being cast to enum type 'e_Qsf_ErrorCodes'.
            // reason: enum is used to ensure debug comfort
            sQsf.u_RetVal = (e_Qsf_ErrorCodes)PAL_u_QsfBlankCheck(sQsf.u_CurrentAddr, u_Step);
            sQsf.u_CurrentAddr += u_Step;
            sQsf.u_QsfStatus = MEMIF_BUSY;
            if (sQsf.u_RetVal != PAL_QSF_OK)
            {
              v_QsfJobEnd();
            }
          }
          else // finished
          {
            v_QsfJobEnd();
          }
        }
        else if (sQsf.u_RetVal == PAL_QSF_BUSY)
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
  }
  // PRQA S 7002, 7004 4 // 2022-10-06; uic12551
  // summary: Msg(3:7002) 7002 HIS metrics check: amount of non-cyclic paths - minimum number of necessary test cases.
  // summary: Msg(3:7004) HIS metrics check: cyclomatic complexitiy.
  // reason: Each case of this large switch-case could go into separate function.
}

static Std_ReturnType u_Qsf_GetLock(void)
{
  Std_ReturnType status;
  QSF_ENTER_CRITICAL_SECTION;
  status = (Std_ReturnType)(sQsf.Lock);
  if (status == E_OK)
  {
    sQsf.Lock = PAL_QSF_BUSY;
  }QSF_EXIT_CRITICAL_SECTION;
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

#ifdef __cplusplus
}
#endif

