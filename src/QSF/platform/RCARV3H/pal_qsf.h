// PRQA S 1011 EOF // 2017-10-25; uidv7790
// summary: Msg(3:1011) [C99] Use of '//' comment.
// reason: According coding template the comment style // shall be used.
// COMPANY:   Continental Automotive
// COMPONENT: QSF
// VERSION:   $Revision: 1.15 $
/// @file  \04_Engineering\01_Source_Code\QSF\PAL_RcarV3M\pal_qsf.h
/// @brief Header of PAL layer for FLS module.


#ifndef PAL_QSF_H
#define PAL_QSF_H

#include <Std_Types.h>
#include "Fls.h"
#ifdef __cplusplus
extern "C"
{
#endif

/// access code for HW register of size 32 bit
#define SFR32(addr) (*((volatile uint32*)(addr)))

/// RPC, SPI Multi I/O Bus Controller
#if 0
// TODO: check compiler warnings if this part is active. This unsigned one is better than that one in Renesas include file Reg_RCarV3M.h
#define RPC_BASE      0xEE200000ul
#define RPC_CMNCR     ( RPC_BASE + 0x0000u ) // R/W
#define RPC_SSLDR     ( RPC_BASE + 0x0004u ) // R/W
#define RPC_DRCR      ( RPC_BASE + 0x000Cu ) // R/W
#define RPC_DRCMR     ( RPC_BASE + 0x0010u ) // R/W
#define RPC_DREAR     ( RPC_BASE + 0x0014u ) // R/W
#define RPC_DROPR     ( RPC_BASE + 0x0018u ) // R/W
#define RPC_DRENR     ( RPC_BASE + 0x001Cu ) // R/W
#define RPC_SMCR      ( RPC_BASE + 0x0020u ) // R/W
#define RPC_SMCMR     ( RPC_BASE + 0x0024u ) // R/W
#define RPC_SMADR     ( RPC_BASE + 0x0028u ) // R/W
#define RPC_SMOPR     ( RPC_BASE + 0x002Cu ) // R/W
#define RPC_SMENR     ( RPC_BASE + 0x0030u ) // R/W
#define RPC_SMRDR0    ( RPC_BASE + 0x0038u ) // R
#define RPC_SMRDR1    ( RPC_BASE + 0x003Cu ) // R
#define RPC_SMWDR0    ( RPC_BASE + 0x0040u ) // R/W
#define RPC_SMWDR1    ( RPC_BASE + 0x0044u ) // R/W
#define RPC_CMNSR     ( RPC_BASE + 0x0048u ) // R
#define RPC_DRDMCR    ( RPC_BASE + 0x0058u ) // R/W
#define RPC_DRDRENR   ( RPC_BASE + 0x005Cu ) // R/W
#define RPC_SMDMCR    ( RPC_BASE + 0x0060u ) // R/W
#define RPC_SMDRENR   ( RPC_BASE + 0x0064u ) // R/W
#define RPC_PHYCNT    ( RPC_BASE + 0x007Cu ) // R/W
#define RPC_OFFSET1   ( RPC_BASE + 0x0080u ) //
#define RPC_PHYINT    ( RPC_BASE + 0x0088u ) // R/W
#define RPC_DIVREG    ( RPC_BASE + 0x00A8u ) // R/W
#define RPC_WRBUF     ( RPC_BASE + 0x8000u ) // W  RPC Write buffer (Access size=4/8/16/32/64Byte)
#endif

/// Maximal page program time (tPP) for MACRONIX chip
#define PAL_QSF_MACRONIX_MAX_tPP_US 750u
/// Maximal page program time (tPP) for MICRON chip
#define PAL_QSF_MICRON_MAX_tPP_US 1800u

/// Maximal page program time (tPP) for used chip
#if (defined(PAL_QSF_MACRONIX_MAX_tPP_US) && defined(PAL_QSF_MICRON_MAX_tPP_US))
  #if (PAL_QSF_MACRONIX_MAX_tPP_US > PAL_QSF_MICRON_MAX_tPP_US)
    #define PAL_QSF_SDF_MAX_tPP_US PAL_QSF_MACRONIX_MAX_tPP_US
  #else
    #define PAL_QSF_SDF_MAX_tPP_US PAL_QSF_MICRON_MAX_tPP_US
  #endif
#elif defined(PAL_QSF_MACRONIX_MAX_tPP_US)
  #define PAL_QSF_SDF_MAX_tPP_US PAL_QSF_MACRONIX_MAX_tPP_US
#elif defined(PAL_QSF_MICRON_MAX_tPP_US)
  #define PAL_QSF_SDF_MAX_tPP_US PAL_QSF_MICRON_MAX_tPP_US
#else
  #error "Maximal page program time (tPP) is not defined!"
#endif

/// internal state indicating the current action
typedef enum
{
  QSF_IDLE,        ///< ready for next action
  QSF_INIT,        ///< initialization ongoing
  QSF_READ,        ///< reading from SDF
  QSF_WRITE,       ///< writing to SDF
  QSF_ERASE,       ///< erasing SDF sector(s)
  QSF_ERASE_4K,    ///< erasing SDF 4K subsector /* Conti Change uids8871 */
  QSF_COMPARE,     ///< compare given RAM data with SDF data
  QSF_BLANKCHECK,  ///< check if area is erased
  QSF_OTP_READ,    ///< reading from OTP area in SDF
  QSF_OTP_WRITE    ///< writing to OTP area in SDF
} tQsfJob;

/// Error codes for function returns
typedef enum
 {
  PAL_QSF_OK      ,          ///< operation completed successfully
  PAL_QSF_BUSY    ,          ///< erase/write operation still ongoing
  PAL_QSF_ERROR_PARAMETER ,  ///< illegal parameter passed to SetParameters
  PAL_QSF_ERROR_TIMEOUT ,    ///< operation timed out
  PAL_QSF_ERROR_ERASE ,      ///< error during erase or non-blank for BlankCheck
  PAL_QSF_ERROR_WRITE ,      ///< error during write
  PAL_QSF_ERROR_VERIFY  ,    ///< data mismatch found during verify
  PAL_QSF_ERROR_ALIGN ,      ///< when erasing, start or end address is not aligned with sector; when programming a page, crossing a page boundary
  PAL_QSF_ERROR_SIZE    ,    ///< PageSizeBytes is 0 or no multiple of 4; SectorSizeBytes is no multiple of PageSizeBytes; DeviceSizeBytes is no multiple of SectorSizeBytes
  PAL_QSF_ERROR_NOTSUPPORTED ///< unknown SDF device is not supported
 } e_Qsf_ErrorCodes;

/// all static object data, is of interest when debugging the component
typedef struct
{
  MemIf_StatusType u_QsfStatus;        ///< current status of the module
  tQsfJob u_Job;                       ///< current active job
  MemIf_JobResultType u_QsfJobStatus;  ///< result of current or last job
  e_Qsf_ErrorCodes u_RetVal;           ///< result of last function call to pal, could be enum for better debug
  MemIf_ModeType Qsf_Mode;             ///< current MemIf mode (slow or fast)
  uint32 u_CurrentAddr;                ///< start address for current action
  uint32 u_EndAddr;                    ///< end address for current action
  uint8* p_CurrentRam;                 ///< RAM pointer for current action
  const Fls_ConfigType* p_QsfCfg;      ///< pointer to current configuration
  uint32 u_PageSize;                   ///< page size of current SDF
  uint32 u_SectorSize;                 ///< sector size of current SDF
  uint32 u_InitDone;                   ///< flag indicating if module was already initialized
  uint32 u_StartTime;                  ///< store
  e_Qsf_ErrorCodes Lock;               ///< Lock flag to implement exclusive area
} tQsfState;


/// Locks OTP area in SDF
///
/// @pre                 initialized
/// @post                OTP area can't be programmed anymore
/// @param               none
///
/// @globals             Access flash specific parameters from structure sPAL_Qsf for consistency check.
///
/// @InOutCorrelation    OTP area in flash is locked and can't be programmed again
///
/// @callsequence
///
/// @image html Call_sequence_for_PAL_u_QsfLockOTPArray.png
/// @startuml "Call_sequence_for_PAL_u_QsfLockOTPArray.png"
///
/// legend left
///   Task: unknown
///   Task priority: 0
///   Cycle time: 0
/// endlegend
///
/// Caller -> pal_qsf: Fls_MainFunction() calls PAL_u_QsfLockOTPArray()
/// note left: Call condition: current job is OTP_Write
/// activate pal_qsf
///   pal_qsf -> pal_qsf: Checks type of SDF
///   note over pal_qsf: Sends command to SDF for locking of OTP area
/// deactivate pal_qsf
/// @enduml
Std_ReturnType PAL_u_QsfLockOTPArray(void);

/// Start programming data to OTP area in flash.
///
/// @pre                 initialized
/// @post                Flash is busy until writing is finished
/// @param[in] p_data    Pointer to data which shall be programed
/// @param[in] size      Size of data to be programed
/// @param[in] addr      Address inside OTP area in flash where to start programing.
/// @return              Std_ReturnType E_OK if program operation is successful.
///                      Error code >0 otherwise.
///
/// @globals             Access flash specific parameters from structure sPAL_Qsf for consistency check.
///
/// @InOutCorrelation    Program operation for given OTP memory area in flash with given data buffer is started.
///
/// @callsequence
///
/// @image html Call_sequence_for_PAL_u_QsfWriteOTPArray.png
/// @startuml "Call_sequence_for_PAL_u_QsfWriteOTPArray.png"
///
/// legend left
///   Task: unknown
///   Task priority: 0
///   Cycle time: 0
/// endlegend
///
/// Fls -> pal_qsf: Fls_MainFunction() calls PAL_u_QsfWriteOTPArray()
/// note left: Call condition: current job is OTP_Write
/// activate pal_qsf
///   pal_qsf -> pal_qsf: Checks type of SDF
///   note left
///     Call condition: consistency check ok
///     Parameters: none
///     Result: Std_ReturnType
///   end note
///   pal_qsf -> pal_qsf: call internal u_Qsf_WriteEnable
///   pal_qsf -> pal_qsf: sets registers for transfer
///   pal_qsf -> pal_qsf: call internal u_Qsf_WaitTxEnd
///   pal_qsf -> pal_qsf: call internal u_Qsf_WaitWIP
///
///   pal_qsf -> Fls:  returns Std_ReturnType
/// deactivate pal_qsf
/// @enduml
Std_ReturnType PAL_u_QsfWriteOTPArray(uint8* p_data, uint32 size, uint32 addr);

/// Start reading data from OTP area in flash.
///
/// @pre                 initialized
/// @post                Flash is busy until writing is finished
/// @param[in] addr      Address inside OTP area in flash where to start reading
/// @param[in] read      Pointer to data where to store read data
/// @param[in] size      Size of data to be which has to be read.
/// @return              Std_ReturnType E_OK if read operation was started successfully.
///                      Error code >0 otherwise.
///
/// @globals             Access flash specific parameters from structure sPAL_Qsf for consistency check.
///
/// @InOutCorrelation    Read operation for given OTP memory area in flash with given data buffer is started.
///
/// @callsequence
///
/// @image html Call_sequence_for_PAL_u_QsfReadOTPArray.png
/// @startuml "Call_sequence_for_PAL_u_QsfReadOTPArray.png"
///
/// legend left
///   Task: unknown
///   Task priority: 0
///   Cycle time: 0
/// endlegend
///
/// Fls -> pal_qsf: Fls_MainFunction() calls PAL_u_QsfReadOTPArray()
/// note left: Call condition: current job is OTP_Read
/// activate pal_qsf
///   pal_qsf -> pal_qsf: Checks type of SDF
///   note left
///     Call condition: consistency check ok
///     Parameters: none
///     Result: Std_ReturnType
///   end note
///   pal_qsf -> pal_qsf: sets registers for read transfer
///   pal_qsf -> pal_qsf: call internal u_Qsf_WaitTxEnd
///
///   pal_qsf -> Fls:  returns Std_ReturnType
/// deactivate pal_qsf
/// @enduml
Std_ReturnType PAL_u_QsfReadOTPArray(uint8* read, uint32 size, uint32 addr);

/// Low level initialisation.
///
/// @pre                 Pin muxing for the interface must be set.
/// @post                none
/// @param[in]           none
/// @return              Std_ReturnType: 0 success or >0 fail
///
/// @globals             Structure sPAL_Qsf is updated with values specific for the present SDF type.
///
/// @InOutCorrelation    If the present SDF is properly detected and is supported then success (E_OK) is returned.
///                      The structure sPAL_Qsf is updated with values specific for the present SDF type.
///                      Otherwise error code is returned.
///
/// @callsequence
/// @image html Call_sequence_for_PAL_u_QsfInit.png
/// @startuml "Call_sequence_for_PAL_u_QsfInit.png"
///
/// legend left
///   Task: Init
///   Task priority: 0
///   Cycle time: 0
/// endlegend
///
/// Fls -> pal_qsf: Fls_Init() calls PAL_u_QsfInit()
/// note left: Call condition: if not yet initialised
/// activate pal_qsf
///   pal_qsf -> SYS_Time: PAL_u_QsfInit() calls SYS_TimeGetCount()
///   note left
///     Call condition: if clock register changed
///     Parameters: none
///     Result: microseconds
///   end note
///   activate SYS_Time
///     SYS_Time -> pal_qsf: done
///   deactivate SYS_Time
///
///   pal_qsf -> pal_qsf: call internal u_Qsf_ReadID
///   note left
///     Call condition: clock setting successfull
///     Parameters: none
///     Result: SDF id
///   end note
///
///   pal_qsf -> Fls:  returns Std_ErrorType
/// deactivate pal_qsf
/// @enduml
///
/// @testmethod
///   1) @ref TEST01_U_QSF_INIT \n
///   2) @ref TEST02_U_QSF_INIT \n
///   3) @ref TEST03_U_QSF_INIT \n
///   4) @ref TEST04_U_QSF_INIT \n
///   5) @ref TEST05_U_QSF_INIT \n
///   6) @ref TEST06_U_QSF_INIT
///
/// @traceability Enter the link to the corresponding function in L4 design specification in Doors
///               <a href="linkURL">doors://rbgs854a:40000/?version=2&prodID=0&urn=urn:telelogic::1-503e822e5ec3651e-O-524-0007caa0</a>
Std_ReturnType PAL_u_QsfInit(const Fls_ConfigType* ConfigPtr);

/// Low level read access to flash.
///
/// @pre                 PAL_u_QsfInit() must be called successfully once.
/// @post                none
/// @param[in] FlashAddr: Address inside flash from where to read.
/// @param[in] RAMptr: Location in RAM where to place the read data.
/// @param[in] SizeBytes: Amount of bytes to read.
/// @return              Std_ReturnType indicates success (E_OK) or fail by error code.
///
/// @globals             Takes the flash specific parameters from structure sPAL_Qsf for consistency checks..
///
/// @InOutCorrelation    Reads the specified amount of bytes from
///
/// @callsequence
///
/// @image html Call_sequence_for_PAL_u_QsfRead.png
/// @startuml "Call_sequence_for_PAL_u_QsfRead.png"
///
/// legend left
///   Task: unknown
///   Task priority: 0
///   Cycle time: 0
/// endlegend
///
/// Fls -> pal_qsf: Fls_MainFunction() calls PAL_u_QsfRead()
/// note left: Call condition: if allowed
/// activate pal_qsf
///
///   pal_qsf -> pal_qsf: call internal u_Qsf_ReadRPCWindow
///   note left
///     Call condition: if parameter check successfull
///     Parameters: FlashAddress, RamPtr, Size
///     Result: Std_ReturnType
///   end note
///
///   pal_qsf -> Fls:  returns Std_ReturnType
/// deactivate pal_qsf
/// @enduml
///
/// @testmethod
///   1) @ref TEST01_PAL_U_QSFREAD \n
///   2) @ref TEST02_PAL_U_QSFREAD \n
///   3) @ref TEST03_PAL_U_QSFREAD \n
///   4) @ref TEST04_PAL_U_QSFREAD
///
/// @traceability Enter the link to the corresponding function in L4 design specification in Doors
///               <a href="linkURL">doors://rbgs854a:40000/?version=2&prodID=0&urn=urn:telelogic::1-503e822e5ec3651e-O-510-0007caa0</a>
// PRQA S 0777 3 // 2017-10-25; uidv7790
// summary: Msg(3:0777) [U] External identifier does not differ from other identifier(s) (e.g. 'Fls_GetStatus') within the specified number of significant characters.
// reason: at least 31 characters are distinguished
Std_ReturnType PAL_u_QsfRead(uint32 FlashAddr, uint8 *RAMptr, uint32 SizeBytes);

/// Compare provided data with data inside flash.
///
/// @pre                 initialized
/// @post                none
/// @param[in] FlashAddr: Address inside flash from where to read.
/// @param[in] RAMptr: Location in RAM where to place the read data.
/// @param[in] SizeBytes: Amount of bytes to read.
/// @return              Std_ReturnType indicates success (E_OK) or fail by error code.
///
/// @globals             Takes the flash specific parameters from structure sPAL_Qsf for consistency checks..
///
/// @InOutCorrelation    Reads the specified amount of bytes from SDF and compares them with given buffer.
///                      Returns E_OK if all is same. Returns error code >0 otherwise.
///
/// @callsequence
///
/// @image html Call_sequence_for_PAL_u_QsfVerify.png
/// @startuml "Call_sequence_for_PAL_u_QsfVerify.png"
///
/// legend left
///   Task: unknown
///   Task priority: 0
///   Cycle time: 0
/// endlegend
///
/// Fls -> pal_qsf: Fls_MainFunction() calls PAL_u_QsfVerify()
/// note left: Call condition: current job is compare
/// activate pal_qsf
///
///   pal_qsf -> pal_qsf: call internal u_Qsf_VerifyRPCWindow
///   note left
///     Call condition: parameters ok
///     Parameters: flashAddress, RamPtr, Size
///     Result: Std_ReturnType
///   end note
///
///   pal_qsf -> Fls:  returns Std_ReturnType
/// deactivate pal_qsf
/// @enduml
///
/// @testmethod
///   1) @ref TEST01_PAL_U_QSFVERIFY \n
///   2) @ref TEST02_PAL_U_QSFVERIFY \n
///   3) @ref TEST03_PAL_U_QSFVERIFY
///
/// @traceability Enter the link to the corresponding function in L4 design specification in Doors
///               <a href="linkURL">doors://rbgs854a:40000/?version=2&prodID=0&urn=urn:telelogic::1-503e822e5ec3651e-O-510-0007caa0</a>
// PRQA S 0777 3 // 2017-10-25; uidv7790
// summary: Msg(3:0777) [U] External identifier does not differ from other identifier(s) (e.g. 'Fls_GetStatus') within the specified number of significant characters.
// reason: at least 31 characters are distinguished
Std_ReturnType PAL_u_QsfVerify(uint32 FlashAddr, uint8 *RAMptr, uint32 SizeBytes);

/// Start low level sector erase
///
/// @pre                 initialized
/// @post                The flash will be busy untill erase is finished
/// @param[in] FlashAddr  Start address of sector which shall be erased
/// @return              Std_ReturnType indicates if start of sector erase was successful
///
/// @globals             Flash specific parameters are checked from structure sPAL_Qsf.
///
/// @InOutCorrelation    If the given address is a sector start address, then the sector erase is started and E_OK is returned.
///                      Otherwise error code >0 is returned.
///
/// @callsequence
///
/// @image html Call_sequence_for_PAL_u_QsfEraseSector.png
/// @startuml "Call_sequence_for_PAL_u_QsfEraseSector.png"
///
/// legend left
///   Task: unknown
///   Task priority: 0
///   Cycle time: 0
/// endlegend
///
/// Fls -> pal_qsf: Fls_MainFunction() calls PAL_u_QsfEraseSector()
/// note left: Call condition: current job is Erase
/// activate pal_qsf
///
///   pal_qsf -> pal_qsf: call internal u_Qsf_WaitTxEnd
///   note left
///     Call condition: sector start address is ok
///     Parameters: none
///     Result: Std_ReturnType
///   end note
///
///   pal_qsf -> Fls:  returns Std_ReturnType
/// deactivate pal_qsf
/// @enduml
///
/// @testmethod
///   1) @ref TEST01_PAL_U_QSFERASESECTOR \n
///   2) @ref TEST02_PAL_U_QSFERASESECTOR \n
///   3) @ref TEST03_PAL_U_QSFERASESECTOR \n
///   4) @ref TEST04_PAL_U_QSFERASESECTOR \n
///   5) @ref TEST05_PAL_U_QSFERASESECTOR \n
///   6) @ref TEST06_PAL_U_QSFERASESECTOR \n
///   7) @ref TEST07_PAL_U_QSFERASESECTOR
///
/// @traceability Enter the link to the corresponding function in L4 design specification in Doors
///               <a href="linkURL">doors://rbgs854a:40000/?version=2&prodID=0&urn=urn:telelogic::1-503e822e5ec3651e-O-512-0007caa0</a>
// PRQA S 0777 3 // 2017-10-25; uidv7790
// summary: Msg(3:0777) [U] External identifier does not differ from other identifier(s) (e.g. 'Fls_GetStatus') within the specified number of significant characters.
// reason: at least 31 characters are distinguished
Std_ReturnType PAL_u_QsfEraseSector(uint32 FlashAddr);

/// Perform low level blank check.
///
/// @pre                 initialized
/// @post                none
/// @param[in] FlashAddr  Start address of the area which should be checked for blank.
/// @param[in] SizeBytes  Size of the area which should be checked for blank.
/// @return              Std_ReturnType returns E_OK if complete area is erased.
///                      Otherwise returns error code >0.
///
/// @globals             Reads flash specific parameters from structure sPAL_Qsf for consistency checks.
///
/// @InOutCorrelation    Returns E_OK if given area is erased and means blank.
///                      Otherwise returns error code >0.
///
/// @callsequence
///
/// @image html Call_sequence_for_PAL_u_QsfBlankCheck.png
/// @startuml "Call_sequence_for_PAL_u_QsfBlankCheck.png"
///
/// legend left
///   Task: unknown
///   Task priority: 0
///   Cycle time: 0
/// endlegend
///
/// Fls -> pal_qsf: Fls_MainFunction() calls PAL_u_QsfBlankCheck()
/// note left: Call condition: current job is BlankCheck
/// activate pal_qsf
///
///   pal_qsf -> pal_qsf: call internal u_Qsf_BlankCheckRPCWindow
///   note left
///     Call condition: parameter checked ok
///     Parameters: StartAddress, Size
///     Result: Std_ReturnType
///   end note
///
///   pal_qsf -> Fls:  returns Std_ReturnType
/// deactivate pal_qsf
/// @enduml
///
/// @testmethod
///   1) @ref TEST01_PAL_U_QSFBLANKCHECK \n
///   2) @ref TEST02_PAL_U_QSFBLANKCHECK \n
///   3) @ref TEST03_PAL_U_QSFBLANKCHECK \n
///   4) @ref TEST04_PAL_U_QSFBLANKCHECK

///
/// @traceability Enter the link to the corresponding function in L4 design specification in Doors
///               <a href="linkURL">doors://rbgs854a:40000/?version=2&prodID=0&urn=urn:telelogic::1-503e822e5ec3651e-O-513-0007caa0</a>
// PRQA S 0777 3 // 2017-10-25; uidv7790
// summary: Msg(3:0777) [U] External identifier does not differ from other identifier(s) (e.g. 'Fls_GetStatus') within the specified number of significant characters.
// reason: at least 31 characters are distinguished
Std_ReturnType PAL_u_QsfBlankCheck(uint32 FlashAddr, uint32 SizeBytes);

/// Start programing data into flash.
///
/// @pre                 initialized
/// @post                Flash is busy until programing is finished
/// @param[in] FlashAddr Address inside flash where to start programing
/// @param[in] RAMptr    Pointer to data which shall be programed
/// @param[in] SizeBytes Size of data to be programed.
/// @return              Std_ReturnType E_OK if program operation was started successfully.
///                      Error code >0 otherwise.
///
/// @globals             Access flash specific parameters from structure sPAL_Qsf for consistency check.
///
/// @InOutCorrelation    Program operation for given flash area with given data buffer is started.
///
/// @callsequence
///
/// @image html Call_sequence_for_PAL_u_QsfProgramPage.png
/// @startuml "Call_sequence_for_PAL_u_QsfProgramPage.png"
///
/// legend left
///   Task: unknown
///   Task priority: 0
///   Cycle time: 0
/// endlegend
///
/// Fls -> pal_qsf: Fls_MainFunction() calls PAL_u_QsfProgramPage()
/// note left: Call condition: current job is Write
/// activate pal_qsf
///
///   pal_qsf -> pal_qsf: call internal u_Qsf_WriteEnable
///   note left
///     Call condition: consistency check ok
///     Parameters: none
///     Result: Std_ReturnType
///   end note
///
///   pal_qsf -> Fls:  returns Std_ReturnType
/// deactivate pal_qsf
/// @enduml
///
/// @testmethod
///   1) @ref TEST01_PAL_U_QSFPROGRAMPAGE \n
///   2) @ref TEST02_PAL_U_QSFPROGRAMPAGE \n
///   3) @ref TEST03_PAL_U_QSFPROGRAMPAGE \n
///   4) @ref TEST04_PAL_U_QSFPROGRAMPAGE \n
///   5) @ref TEST05_PAL_U_QSFPROGRAMPAGE \n
///   6) @ref TEST06_PAL_U_QSFPROGRAMPAGE \n
///   7) @ref TEST07_PAL_U_QSFPROGRAMPAGE \n
///   8) @ref TEST08_PAL_U_QSFPROGRAMPAGE \n
///   9) @ref TEST09_PAL_U_QSFPROGRAMPAGE \n
///   10) @ref TEST10_PAL_U_QSFPROGRAMPAGE \n
///   11) @ref TEST11_PAL_U_QSFPROGRAMPAGE
///
/// @traceability Enter the link to the corresponding function in L4 design specification in Doors
///               <a href="linkURL">doors://rbgs854a:40000/?version=2&prodID=0&urn=urn:telelogic::1-503e822e5ec3651e-O-514-0007caa0</a>
// PRQA S 0777 3 // 2017-10-25; uidv7790
// summary: Msg(3:0777) [U] External identifier does not differ from other identifier(s) (e.g. 'Fls_GetStatus') within the specified number of significant characters.
// reason: at least 31 characters are distinguished
Std_ReturnType PAL_u_QsfProgramPage(uint32 FlashAddr, uint8 *RAMptr, uint32 SizeBytes);

/// Check if internal flash operation is still ongoing or is finished.
///
/// @pre                 initialized
/// @post                none
/// @param[in] none
/// @return              Std_ReturnType PAL_QSF_OK, PAL_QSF_BUSY or PAL_QSF_TIMEOUT
///
/// @globals             Gets type of SDF from structure sPAL_Qsf and performs SDF specific operation.
///
/// @InOutCorrelation    Returns PAL_QSF_BUSY if the internal operation is still ongoing.
///                      Returns PAL_QSF_OK if the internal operation has finished.
///                      Returns PAL_QSF_TIMEOUT if there was no answer from SDF.
///
/// @callsequence        In case of interrupts: Replace task with interrupt in legend of plantuml sequence
///                      and describe the interrupt condition
///
/// @image html Call_sequence_for_PAL_u_QsfIsBusy.png
/// @startuml "Call_sequence_for_PAL_u_QsfIsBusy.png"
///
/// legend left
///   Task: unknown
///   Task priority: 0
///   Cycle time: 0
/// endlegend
///
/// Fls -> pal_qsf: Fls_MainFunction() calls PAL_u_QsfIsBusy()
/// note left: Call condition: if not Idle
/// activate pal_qsf
///
///   pal_qsf -> pal_qsf: call internal u_Qsf_ReadCommandData
///   note left
///     Call condition: always
///     Parameters: cmd ReadStatusRegister
///     Result: content of Status Register
///   end note
///
///   pal_qsf -> Fls:  returns Std_ReturnType
/// deactivate pal_qsf
/// @enduml
///
/// @testmethod
///   1) @ref TEST01_PAL_U_QSFPROGRAMPAGE \n
///   2) @ref TEST02_PAL_U_QSFPROGRAMPAGE \n
///   3) @ref TEST03_PAL_U_QSFPROGRAMPAGE \n
///   4) @ref TEST04_PAL_U_QSFPROGRAMPAGE \n
///   5) @ref TEST05_PAL_U_QSFPROGRAMPAGE \n
///   6) @ref TEST06_PAL_U_QSFPROGRAMPAGE \n
///   7) @ref TEST07_PAL_U_QSFPROGRAMPAGE \n
///   8) @ref TEST08_PAL_U_QSFPROGRAMPAGE \n
///   9) @ref TEST09_PAL_U_QSFPROGRAMPAGE \n
///   10) @ref TEST10_PAL_U_QSFPROGRAMPAGE \n
///   11) @ref TEST11_PAL_U_QSFPROGRAMPAGE
///
/// @traceability Enter the link to the corresponding function in L4 design specification in Doors
///               <a href="linkURL">doors://rbgs854a:40000/?version=2&prodID=0&urn=urn:telelogic::1-503e822e5ec3651e-O-515-0007caa0</a>
// PRQA S 0777 3 // 2017-10-25; uidv7790
// summary: Msg(3:0777) [U] External identifier does not differ from other identifier(s) (e.g. 'Fls_GetStatus') within the specified number of significant characters.
// reason: at least 31 characters are distinguished
Std_ReturnType PAL_u_QsfIsBusy( void );

/// Get the PageSize of used flash.
///
/// @pre                 initialized
/// @post                none
/// @param[in] none
/// @return              PageSize
///
/// @globals             sPAL_Qsf
///
/// @InOutCorrelation    returns sPAL_Qsf.PageSize
///
/// @callsequence
///
/// @image html Call_sequence_for_PAL_u_QsfGetPageSize.png
/// @startuml "Call_sequence_for_PAL_u_QsfGetPageSize.png"
///
/// legend left
///   Task: init
///   Task priority: 0
///   Cycle time: 0
/// endlegend
///
/// Fls -> pal_qsf: Fls_Init() calls PAL_u_QsfGetPageSize()
/// note left: Call condition: supported flash device present
/// activate pal_qsf
///
///   pal_qsf -> Fls:  returns PageSize
/// deactivate pal_qsf
/// @enduml
///
/// @testmethod ///
///   1) @ref TEST01_PAL_U_QSFGETPAGESIZE
///
/// @traceability Enter the link to the corresponding function in L4 design specification in Doors
///               <a href="linkURL">doors://rbgs854a:40000/?version=2&prodID=0&urn=urn:telelogic::1-503e822e5ec3651e-O-516-0007caa0</a>
// PRQA S 0777 3 // 2017-10-25; uidv7790
// summary: Msg(3:0777) [U] External identifier does not differ from other identifier(s) (e.g. 'Fls_GetStatus') within the specified number of significant characters.
// reason: at least 31 characters are distinguished
uint32 PAL_u_QsfGetPageSize(void);

/// Get the SectorSize of used flash.
///
/// @pre                 initialized
/// @post                none
/// @param[in] none
/// @return              SectorSize
///
/// @globals             sPAL_Qsf
///
/// @InOutCorrelation    returns sPAL_Qsf.SectorSize
///
/// @callsequence
///
/// @image html Call_sequence_for_PAL_u_QsfGetSectorSize.png
/// @startuml "Call_sequence_for_PAL_u_QsfGetSectorSize.png"
///
/// legend left
///   Task: init
///   Task priority: 0
///   Cycle time: 0
/// endlegend
///
/// Fls -> pal_qsf: Fls_Init() calls PAL_u_QsfGetSectorSize()
/// note left: Call condition: supported flash device present
/// activate pal_qsf
///
///   pal_qsf -> Fls:  returns SectorSize
/// deactivate pal_qsf
/// @enduml
///
/// @testmethod e.g. test of Input-Output-Correlation (dynamic module test),
///             test order of calls (dynamic module test).
///
/// @traceability Enter the link to the corresponding function in L4 design specification in Doors
///               <a href="linkURL">doors://rbgs854a:40000/?version=2&prodID=0&urn=urn:telelogic::1-503e822e5ec3651e-O-517-0007caa0</a>
///
// PRQA S 0777 3 // 2017-10-25; uidv7790
// summary: Msg(3:0777) [U] External identifier does not differ from other identifier(s) (e.g. 'Fls_GetStatus') within the specified number of significant characters.
// reason: at least 31 characters are distinguished
uint32 PAL_u_QsfGetSectorSize(void);

/// Get the timeout for word write operation of used flash.
///
/// @pre                 initialized
/// @post                none
/// @param[in] none
/// @return              WordWriteTimeout_us
///
/// @globals             sPAL_Qsf
///
/// @InOutCorrelation    returns sPAL_Qsf.TIMEOUT_WriteWord_us
///
/// @callsequence
///
/// @image html Call_sequence_for_PAL_u_QsfGetWordWriteTimeout_us.png
/// @startuml "Call_sequence_for_PAL_u_QsfGetWordWriteTimeout_us.png"
///
/// legend left
///   Task: unknown
///   Task priority: 0
///   Cycle time: 0
/// endlegend
///
/// Fls -> pal_qsf: Fls_MainFunction() calls PAL_u_QsfGetWordWriteTimeout_us()
/// note left: Call condition: supported flash device present
/// activate pal_qsf
///
///   pal_qsf -> Fls:  returns TIMEOUT_WriteWord_us
/// deactivate pal_qsf
/// @enduml
///
/// @testmethod
///   1) @ref TEST01_PAL_U_QSFGETWORDWRITETIMEOUT_US
///
/// @traceability Enter the link to the corresponding function in L4 design specification in Doors
///               <a href="linkURL">doors://rbgs854a:40000/?version=2&prodID=0&urn=urn:telelogic::1-503e822e5ec3651e-O-518-0007caa0</a>
// PRQA S 0777 3 // 2017-10-25; uidv7790
// summary: Msg(3:0777) [U] External identifier does not differ from other identifier(s) (e.g. 'Fls_GetStatus') within the specified number of significant characters.
// reason: at least 31 characters are distinguished
uint32 PAL_u_QsfGetWordWriteTimeout_us(void);

/// Get the timeout for sector erase operation of used flash.
///
/// @pre                 initialized
/// @post                none
/// @param[in] none
/// @return              SectorEraseTimeout_ms
///
/// @globals             sPAL_Qsf
///
/// @InOutCorrelation    returns sPAL_Qsf.TIMEOUT_EraseSector_ms
///
/// @callsequence
///
/// @image html Call_sequence_for_PAL_u_QsfGetSectorEraseTimeout_ms.png
/// @startuml "Call_sequence_for_PAL_u_QsfGetSectorEraseTimeout_ms.png"
///
/// legend left
///   Task: unknown
///   Task priority: 0
///   Cycle time: 0
/// endlegend
///
/// Fls -> pal_qsf: Fls_MainFunction() calls PAL_u_QsfGetSectorEraseTimeout_ms()
/// note left: Call condition: supported flash device present
/// activate pal_qsf
///
///   pal_qsf -> Fls:  returns TIMEOUT_SectorErase_ms
/// deactivate pal_qsf
/// @enduml
///
/// @testmethod
///   1) @ref TEST01_PAL_U_QSFGETSECTORERASETIMEOUT_MS
///
/// @traceability Enter the link to the corresponding function in L4 design specification in Doors
///               <a href="linkURL">doors://rbgs854a:40000/?version=2&prodID=0&urn=urn:telelogic::1-503e822e5ec3651e-O-519-0007caa0</a>
// PRQA S 0777 3 // 2017-10-25; uidv7790
// summary: Msg(3:0777) [U] External identifier does not differ from other identifier(s) (e.g. 'Fls_GetStatus') within the specified number of significant characters.
// reason: at least 31 characters are distinguished
uint32 PAL_u_QsfGetSectorEraseTimeout_ms(void);

/// Submits a "write enable" command to the flash device for a subsequent write operation.
///
/// @pre                 initialized
/// @post                none
/// @param[in]           none
/// @return              Std_ReturnType
///
/// @globals             none
///
/// @InOutCorrelation    Submits a "write enable" command to the flash device for a subsequent write operation
///
/// @callsequence
///
/// @image html Call_sequence_u_Qsf_WriteEnable.png
/// @startuml "Call_sequence_u_Qsf_WriteEnable.png"
///
/// legend left
///   Task: unknown
///   Task priority: 0
///   Cycle time: 0
/// endlegend
///
/// activate FileX
/// FileX -> pal_qsf: PAL_u_QsfGetSectorEraseTimeout_ms()
/// deactivate FileX
/// activate pal_qsf
/// pal_qsf -> FileX: returns u_RetVal
/// deactivate pal_qsf
/// activate FileX
///
/// @enduml
///
/// @testmethod
///   1) @ref TEST01_U_QSF_WRITEENABLE
///
/// @traceability Enter the link to the corresponding function in L4 design specification in Doors
///               <a href="linkURL">doors://rbgs854a:40000/?version=2&prodID=0&urn=urn:telelogic::1-503e822e5ec3651e-O-520-0007caa0</a>
Std_ReturnType u_Qsf_WriteEnable(void);

/// Waits for an SPI transfer to complete.
///
/// @pre                 initialized
/// @post                none
/// @param[in] none
/// @return              Std_ReturnType
///
/// @globals             none
///
/// @InOutCorrelation    returns E_OK or E_NOT_OK depends on if SPI transfer is completed
///
/// @callsequence
///
/// @image html Call_sequence_for_u_QsfWriteEnable.png
/// @startuml "Call_sequence_for_u_QsfWriteEnable.png"
///
/// legend left
///   Task: unknown
///   Task priority: 0
///   Cycle time: 0
/// endlegend
///
/// activate FileX
/// FileX -> pal_qsf: u_Qsf_WaitTxEnd()
/// deactivate FileX
/// activate pal_qsf
/// pal_qsf -> FileX: returns u_RetVal
/// deactivate pal_qsf
/// activate FileX
///
/// @enduml
///
/// @testmethod
///   1) @ref TEST01_U_QSF_WAITTXEND \n
///   2) @ref TEST02_U_QSF_WAITTXEND
///
/// @traceability Enter the link to the corresponding function in L4 design specification in Doors
///               <a href="linkURL">doors://rbgs854a:40000/?version=2&prodID=0&urn=urn:telelogic::1-503e822e5ec3651e-O-521-0007caa0</a>
Std_ReturnType u_Qsf_WaitTxEnd(void);

/// Start low level sector erase
///
/// @pre                 initialized
/// @post                The flash will be busy untill erase is finished
/// @param[in] FlashAddr  Start address of sector which shall be erased
/// @return              Std_ReturnType indicates if start of sector erase was successful
///
/// @globals             Flash specific parameters are checked from structure sPAL_Qsf.
///
/// @InOutCorrelation    If the given address is a sector start address, then the sector erase is started and E_OK is returned.
///                      Otherwise error code >0 is returned.
///
/// @callsequence
///
/// @image html Call_sequence_for_PAL_u_QsfEraseSector4K.png
/// @startuml "Call_sequence_for_PAL_u_QsfEraseSector4K.png"
///
/// legend left
///   Task: unknown
///   Task priority: 0
///   Cycle time: 0
/// endlegend
///
/// Fls -> pal_qsf: Fls_MainFunction() calls PAL_u_QsfEraseSector4K()
/// note left: Call condition: current job is Erase
/// activate pal_qsf
///
///   pal_qsf -> pal_qsf: call internal u_Qsf_WaitTxEnd
///   note left
///     Call condition: sector start address is ok
///     Parameters: none
///     Result: Std_ReturnType
///   end note
///
///   pal_qsf -> Fls:  returns Std_ReturnType
/// deactivate pal_qsf
/// @enduml
///
/// @testmethod
///   1) @ref TEST01_PAL_U_QSFERASESECTOR_4K \n
///   2) @ref TEST02_PAL_U_QSFERASESECTOR_4K \n
///   3) @ref TEST03_PAL_U_QSFERASESECTOR_4K \n
///   4) @ref TEST04_PAL_U_QSFERASESECTOR_4K
///
/// @traceability Enter the link to the corresponding function in L4 design specification in Doors
///               <a href="linkURL">doors://rbgs854a:40000/?version=2&prodID=0&urn=urn:telelogic::1-503e822e5ec3651e-O-522-0007caa0</a>
// PRQA S 0777 3 // 2017-10-25; uidv7790
// summary: Msg(3:0777) [U] External identifier does not differ from other identifier(s) (e.g. 'Fls_GetStatus') within the specified number of significant characters.
// reason: at least 31 characters are distinguished
Std_ReturnType PAL_u_QsfEraseSector_4K(uint32 FlashAddr);

/// Sets the parameters for QSPI flash device access
///
/// @pre                        initialized
/// @post                       Sector size will be set
/// @param[in] SectorSizeBytes  Size of sector
/// @return                     Std_ReturnType indicates if everything is ok
///
/// @globals                    sPAL_Qsf.
///
/// @InOutCorrelation           Sets the parameters for QSPI flash device access.
///
/// @callsequence
///
/// @image html Call_sequence_for_Pal_u_Qsf_SetSectorSize.png
/// @startuml "Call_sequence_for_Pal_u_Qsf_SetSectorSize.png"
///
/// legend left
///   Task: unknown
///   Task priority: 0
///   Cycle time: 0
/// endlegend
///
/// Fls -> pal_qsf: Pal_u_Qsf_SetSectorSize()
///
/// activate pal_qsf
///   pal_qsf -> Fls:  returns Std_ReturnType
/// deactivate pal_qsf
/// @enduml
///
/// @testmethod
///   1) @ref TEST01_PAL_U_QSFGETSECTORSIZE
///
/// @traceability Enter the link to the corresponding function in L4 design specification in Doors
///               <a href="linkURL">doors://rbgs854a:40000/?version=2&prodID=0&urn=urn:telelogic::1-503e822e5ec3651e-O-523-0007caa0</a>
// PRQA S 0777 3 // 2017-10-25; uidv7790
// summary: Msg(3:0777) [U] External identifier does not differ from other identifier(s) (e.g. 'Fls_GetStatus') within the specified number of significant characters.
// reason: at least 31 characters are distinguished
Std_ReturnType Pal_u_Qsf_SetSectorSize(uint32 SectorSizeBytes);

#ifdef __cplusplus
}
#endif

#endif // #ifndef FILE_NAME_H

