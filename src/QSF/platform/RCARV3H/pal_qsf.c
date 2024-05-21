// COMPANY:   Continental Automotive
// COMPONENT: QSF
// VERSION:   $Revision: 1.49 $
/// @file  \04_Engineering\01_Source_Code\QSF\PAL_RcarV3M\pal_qsf.c
/// @brief Source of PAL layer for FLS module.

//Note:Reference information for 0303_SFR_32 warnings supression
// Date: 2023-11-20,
// Reason: Memory mapped register needs to be accessed directly via pointer.CCB Jira-ID: ADASQMS-3232
// Msg(4:0303) [I] Cast between a pointer to volatile object and an integral type.

//Note id: [4461_UL_Warning]
// Date:2023-11-22, uif85752
// Summary: Message(3:4461) UL is being converted to narrower unsigned type, uint32 on assignment.
// Reason:  4461 is okay as uL is uint32, It is ensured that the copying does not overlap Proven in use.

#include <pal_qsf.h>
#include <sys_time.h>
#include <Reg_RCarV3M.h>
#include <memory_layout.h>
#include <Platform_Types.h>
#include <sys_mem.h>
#ifdef QSF_PRESBL_USE_CACHE
#include <cache.h>
#endif

#ifdef __cplusplus
extern "C"
{
#endif

#if (CPU_TYPE == 32)
/// assembler implementation for Data System Barrier
#ifndef SIM_SCT_CFG
#define pDSB()      do {  __asm volatile("dsb");     } while (0)
#else // SIM_SCT_CFG
/// dummy definition of Data System Barrier
#define pDSB()
#endif //SIM_SCT_CFG
#else // (CPU_TYPE == 32)
/// dummy definition of Data System Barrier
#define pDSB()
#endif // (CPU_TYPE == 32)

/// define to keep the used memcpy routine more flexible
#define Qsf_memcpy memcpy // use general one

#if 0
// TODO: check compiler warnings if this part is active. This unsigned one is better than that one in Renesas include file Reg_RCarV3M.h
/// CPG, Clock Pulse Generator
#ifndef SIM_SCT_CFG
#define CPG_BASE      0xE6150000ul
//// R/W  32  RPC clock frequency control register
#define CPG_RPCCKCR     ( CPG_BASE + 0x0238u )
#endif //SIM_SCT_CFG
#endif

#ifdef RCAR_V3M
#ifndef SIM_SCT_CFG
#define ARMREG_P_COMMON3 0xE627100Cu  // added to switch to 66 MHz, this one is used only for V3M
#endif //SIM_SCT_CFG
#endif //RCAR_V3M

/// BIT macro for bit 0
#define BIT0  0x00000001u
/// BIT macro for bit 5
#define BIT5  0x00000020u
/// BIT macro for bit in position b
#define BIT(b) ( 1uL << (b) )

#ifndef SIM_SCT_CFG //Not Courage
/// start adress of QSPI memory window
#define PAL_QSF_WINDOW_START  0x08000000uL
/// end adress of QSPI memory window
#define PAL_QSF_WINDOW_END    0x0BFFFFFFuL
#else
uint32 emulated_flash [0x0400uL];
// Window Start value is zero due to cause of address mismatch in unit test
uint32 PAL_QSF_WINDOW_START;
#endif //SIM_SCT_CFG
/// size of QSPI memory window
#define PAL_QSF_WINDOW_SIZE 0x04000000uL  // 64 MiB
/// mask used to check QSPI memory window
#define PAL_QSF_WINDOW_MASK 0x03FFFFFFuL

/// adress where to write the calibration pattern
#define PAL_QSF_CALIBRATION_ADDR 0x00090000u
/// pattern used for calibrating the read data rate
#define PAL_QSF_CALIBRATION_PATTERN 0xAA5AA555u

/// readable constant of kilobyte
#define kB    (1024u)
/// readable constant of kilobyte
#define kiB   (1024u)
/// readable constant of kilobyte
#define KiB   (1024u)
/// readable constant of megabyte
#define MB    (1024u*1024u)
/// readable constant of megabyte
#define MiB   (1024u*1024u)

/// RPC buffer size (HW specific)
#define RPC_WRBUF_SIZE  (256u)

/// R7 cache line length
#define CACHE_LINE_LEN (32u)
/// mask of R7 cache line size
#define CACHE_LINE_MASK (~(CACHE_LINE_LEN-1u))

/// enum for selectable clock frequencies
typedef enum
{
  PAL_QSF_SPCLK_ZERO    = 0,  ///< invalid
  PAL_QSF_SPCLK_160MHz  = 160,///< 160.00 MHz (for HyperFlash)
  PAL_QSF_SPCLK_133MHz  = 133,///< 133.33 MHz (for QSPI Flash)
  PAL_QSF_SPCLK_80MHz   = 80, ///<  80.00 MHz (for Hyper/QSPI Flash)
  PAL_QSF_SPCLK_66MHz   = 66, ///<  66.00 MHz (for Hyper/QSPI Flash)
  PAL_QSF_SPCLK_40MHz   = 40  ///<  40.00 MHz (for Hyper/QSPI Flash)
} e_PAL_Qsf_SpClkFrequencies;

/// different QSPI read modes
typedef enum
{
  PAL_SPI_UNINIT,      ///< zero
  PAL_SPI_SDR_BURST,   ///< used with cmd 0x0B
  PAL_QSPI_SDR_BURST,  ///< used with cmd 0xEC
  PAL_QSPI_DDR_SINGLE, ///< used with cmd 0xEE
  PAL_QSPI_DDR_BURST   ///< used with cmd 0xEE
} e_PAL_QspiMode;

/// different SDF types / manufacturer
typedef enum
{
  PAL_SDF_TYPE_UNKNOWN,  ///< not recognized, not supported
  PAL_SDF_TYPE_MICRON,   ///< from manufacturer Micron
  PAL_SDF_TYPE_MACRONIX, ///< from manufacturer Macronix
  PAL_SDF_TYPE_CYPRESS   ///< from manufacturer Cypress
} e_PAL_SdfType;

/// static data collected in this struct.
typedef struct
{
  uint32 DeviceSize;             ///< Size of the entire SDF device.
  uint32 SectorSize;             ///< Size of sector in SDF.
  uint32 PageSize;               ///< Size of the write buffer.
  uint32 TIMEOUT_WriteWord_us;   ///< Timeout for writing to SDF in us.
  uint32 TIMEOUT_EraseSector_ms; ///< Timeout for sector erase in ms.
  e_PAL_QspiMode Mode;           ///< used transfer mode for SDF read access
  e_PAL_SdfType Type;            ///< SDF type/manufacturer
  uint8 DummyCycles;             ///< Dummy Cycles for 1-wire SPI transfer
  uint8 DummyCyclesEE;           ///< Dummy Cycles for 4-wire QSPI transfer in SDR (command 0xEE)
  uint8 DummyCyclesEC;           ///< Dummy Cycles for 4-wire QSPI transfer in DTR (command 0xEC)
}sPAL_Qsf_t;

/// PAL QSF configuration data
static volatile sPAL_Qsf_t sPAL_Qsf;
/// last calibration value
static volatile uint8 u_calibratedStrtimValue;
#ifndef SIM_SCT_CFG
/// last Calibration Time in ticks
static uint32 u_lastCalibrationTime_ticks = 0u;
#else
static uint32 u_lastCalibrationTime_ticks ;
#endif

/// DDR calibration interval: 10 million ticks = 1 second
#define CALIBRATION_INTERVAL_TICKS 10000000u

/// mask to compare with configured page size
#define PAGEMASK    ( sPAL_Qsf.PageSize   - 1u )
/// mask to compare with configured sector size
#define SECTORMASK  ( sPAL_Qsf.SectorSize - 1u )

/// Reference Source file : pal_qsf.c
///
/// @pre None
/// @post None
/// @param  uint32 SPCLKfreq
/// @return status
/// @globals No global variables used
/// @InOutCorrelation None
/// @callsequence To be updated.
/// @InOutCorrelation Frequency at which the runnable is called: N/A
/// @startuml
/// - Call sequence for  u_Qsf_Init()
/// - Std_ReturnType status is updated with (Std_ReturnType)PAL_QSF_OK
/// if (status == (Std_ReturnType)PAL_QSF_OK) then (yes)
/// -  assert reset
/// - release reset
/// - SSL delay is set to  0x00000400u
/// -  Data read extended address is set to 0x00000001u
/// endif
/// -  return( status )
/// @enduml
///
/// @testmethod
///    Dynamic unit test method
///    - This function has to be tested for statement and branch coverage.
///    - For detailed unit test description follow the caller graph to the test function below.
///    - Test script: test_pal_qsf.c
/// @traceability
static Std_ReturnType u_Qsf_Init(uint32 SPCLKfreq);

/// Reference Source file : pal_qsf.c
///
/// @pre None
/// @post None
/// @param (uint32 timeout_us)
/// @return static Std_ReturnType
/// @globals No global variables used
/// @InOutCorrelation None
/// @callsequence To be updated.
/// @InOutCorrelation Frequency at which the runnable is called: N/A
/// @startuml "Call sequence for u_Qsf_WaitWIP()"
///
/// legend left
///    Task: Where the runnables are added, depending on the project (SchM, Com, etc.)
///    Task priority: n/a
///    Cycle time: N/A
/// endlegend
///
/// activate pal_qsf.c
///
///    pal_qsf.c -> sys_time.c: u_Qsf_WaitWIP() calls SYS_TimeGetCount()
///    note left
///       Result: None
///    end note
///    activate sys_time.c
///       sys_time.c -> pal_qsf.c: done
///    deactivate sys_time.c
///    pal_qsf.c -> sys_time.c: u_Qsf_WaitWIP() calls SYS_TimeGetSince()
///    note left
///       Parameters:  t
///       Result: None
///    end note
///    activate sys_time.c
///       sys_time.c -> pal_qsf.c: done
///    deactivate sys_time.c
/// deactivate pal_qsf.c
/// @enduml
///
/// @testmethod
///    Dynamic unit test method
///    - This function has to be tested for statement and branch coverage.
///    - For detailed unit test description follow the caller graph to the test function below.
///    - Test script: test_pal_qsf.c
/// @traceability

static Std_ReturnType u_Qsf_WaitWIP(uint32 timeout_us );

/// Reference Source file : pal_qsf.c
///
/// @pre None
/// @post None
/// @param (uint32 FlashAddr, uint32 SizeBytes)
/// @return static Std_ReturnType
/// @globals No global variables used
/// @InOutCorrelation None
/// @callsequence To be updated.
/// @InOutCorrelation Frequency at which the runnable is called: N/A
/// @startuml
/// - Call sequence for u_Qsf_StartRead()
/// if ( FlashAddr + SizeBytes > sPAL_Qsf.DeviceSize ) then(yes)
/// - status is updated with PAL_QSF_ERROR_SIZE
/// endif
/// if(status == E_OK) then (yes)
/// - FlashAddrEnd is updated with FlashAddr + SizeBytes - 1u
/// if ( (FlashAddr & ~PAL_QSF_WINDOW_MASK) != (FlashAddrEnd & ~PAL_QSF_WINDOW_MASK) ) then (yes)
/// - status is updated with (Std_ReturnType)PAL_QSF_ERROR_ALIGN
/// endif
/// endif
/// if(status == E_OK) then (yes)
/// - calling PAL_v_QsfDDRCalibration
/// if(u_calibratedStrtimValue >= 0x08u)  then (yes)
/// - sPAL_Qsf.Mode is updated with PAL_QSPI_SDR_BURST
/// else
/// - do nothing
/// endif
/// if (sPAL_Qsf.Mode == PAL_SPI_SDR_BURST)  then (yes)
/// - updating PHY timing offset
/// - updating PHY control
/// - updating Common control
/// - updating Data read control
/// - updating Data read command
/// - update Data read dummy cycles
/// else if (sPAL_Qsf.Mode == PAL_QSPI_SDR_BURST)
/// - updating PHY timing offset
/// - updating PHY control
/// - updating Common control
/// - updating Data read control
/// - updating Data read command
/// - update Data read dummy cycles
/// else if (sPAL_Qsf.Mode == PAL_QSPI_DDR_BURST)
/// - updating PHY timing offset
/// - updating PHY control
/// - updating Common control
/// - updating Data read control
/// - updating Data read command
/// - update Data read dummy cycles
/// - update Data read DDR enable
/// else
/// - status is updated with (Std_ReturnType)PAL_QSF_ERROR_NOTSUPPORTED
/// endif
/// endif
///  if(status == E_OK) then (yes)
/// - calling v_Qsf_DCacheRegionInvalidate
/// endif
/// - return(status)
/// @enduml
///
/// @testmethod
///    Dynamic unit test method
///    - This function has to be tested for statement and branch coverage.
///    - For detailed unit test description follow the caller graph to the test function below.
///    - Test script: test_pal_qsf.c
/// @traceability

static Std_ReturnType u_Qsf_StartRead(uint32 FlashAddr, uint32 SizeBytes);

/// Reference Source file : pal_qsf.c
///
/// @pre None
/// @post None
/// @param (uint32 FlashAddr, uint8 *RAMptr, uint32 SizeBytes)
/// @return static Std_ReturnType
/// @globals No global variables used
/// @InOutCorrelation None
/// @callsequence To be updated.
/// @InOutCorrelation Frequency at which the runnable is called: N/A
/// @startuml
/// - Call sequence for u_Qsf_ReadRPCWindow()
/// - Std_ReturnType status set to E_OK
///  if ( SizeBytes != 0u ) then (yes)
/// - status is updated with u_Qsf_StartRead( FlashAddr, SizeBytes )
/// if ( status == E_OK ) then (yes)
/// - calling Qsf_memcpy
/// endif
/// endif
/// - return( status )
/// @enduml
///
/// @testmethod
///    Dynamic unit test method
///    - This function has to be tested for statement and branch coverage.
///    - For detailed unit test description follow the caller graph to the test function below.
///    - Test script: test_pal_qsf.c
/// @traceability

//
static Std_ReturnType u_Qsf_ReadRPCWindow(uint32 FlashAddr, uint8 *RAMptr, uint32 SizeBytes);

/// Reference Source file : pal_qsf.c
///
/// @pre None
/// @post None
/// @param (uint32 FlashAddr, uint8 *RAMptr, uint32 SizeBytes)
/// @return static Std_ReturnType
/// @globals No global variables used
/// @InOutCorrelation None
/// @callsequence To be updated.
/// @InOutCorrelation Frequency at which the runnable is called: N/A
/// @startuml
/// - Call sequence for u_Qsf_VerifyRPCWindow()
/// if ( SizeBytes != 0u ) then (yes)
/// - status is updating with (Std_ReturnType)u_Qsf_StartRead( FlashAddr, SizeBytes )
/// if ( status == E_OK ) then (yes)
/// if ( ((uint32)RAMptr & 3u) != 0u) then (yes)
/// - status is updated with (Std_ReturnType)PAL_QSF_ERROR_ALIGN
/// endif
/// endif
/// if ( status == E_OK ) then (yes)
/// - RAMptr32  is updated with (uint32*)RAMptr
/// - Flashptr32 = (uint32 *)(PAL_QSF_WINDOW_START + FlashAddr)
/// if ( SizeBytes >= 4u ) then (yes)
/// if ( *RAMptr32 != *Flashptr32 ) then (yes)
/// - status is updated with (Std_ReturnType)PAL_QSF_ERROR_VERIFY
/// endif
/// endif
/// - RAMptr8 is updated with (uint8*)RAMptr32
/// - Flashptr8 is updated with (uint8*)Flashptr32
/// if ( SizeBytes != 0u)  then (yes)
/// if ( *RAMptr8 != *Flashptr8 )then (yes)
/// - status is updated with (Std_ReturnType)PAL_QSF_ERROR_VERIFY
/// endif
/// endif
/// endif
/// endif
/// - return( status )
/// @enduml
///
/// @testmethod
///    Dynamic unit test method
///    - This function has to be tested for statement and branch coverage.
///    - For detailed unit test description follow the caller graph to the test function below.
///    - Test script: test_pal_qsf.c
/// @traceability

static Std_ReturnType u_Qsf_VerifyRPCWindow(uint32 FlashAddr, uint8 *RAMptr, uint32 SizeBytes);

/// Reference Source file : pal_qsf.c
///
/// @pre None
/// @post None
/// @param (uint32 FlashAddr, uint32 SizeBytes)
/// @return static Std_ReturnType
/// @globals No global variables used
/// @InOutCorrelation None
/// @callsequence To be updated.
/// @InOutCorrelation Frequency at which the runnable is called: N/A
/// @startuml
/// - Call sequence for u_Qsf_BlankCheckRPCWindow()
/// if ( SizeBytes != 0u )then (yes)
/// - status updated with u_Qsf_StartRead( FlashAddr, SizeBytes )
///  if ( status == E_OK ) then (yes)
/// - Flashptr32 updated with (uint32 *)(PAL_QSF_WINDOW_START + FlashAddr)
/// if ( *Flashptr32 != 0xFFFFFFFFuL ) then (yes)
/// - status is updated with (Std_ReturnType)PAL_QSF_ERROR_ERASE
/// endif
/// - SizeBytes is set to 4u
/// - Flashptr8 is updated with (uint8*)Flashptr32
/// if ( *Flashptr8 != 0xFFu ) then (yes)
/// - status is updated with (Std_ReturnType)PAL_QSF_ERROR_ERASE
/// endif
/// endif
/// endif
/// -  return( status )
/// @enduml
///
/// @testmethod
///    Dynamic unit test method
///    - This function has to be tested for statement and branch coverage.
///    - For detailed unit test description follow the caller graph to the test function below.
///    - Test script: test_pal_qsf.c
/// @traceability

static Std_ReturnType u_Qsf_BlankCheckRPCWindow(uint32 FlashAddr, uint32 SizeBytes);

/// Reference Source file : pal_qsf.c
///
/// @pre None
/// @post None
/// @param (uint8 *ManufacturerID, uint16 *DeviceID)
/// @return static Std_ReturnType
/// @globals No global variables used
/// @InOutCorrelation None
/// @callsequence To be updated.
/// @InOutCorrelation Frequency at which the runnable is called: N/A
/// @startuml
/// - Call sequence for u_Qsf_ReadID()
/// - Std_ReturnType status updated with (Std_ReturnType)PAL_QSF_OK
/// - status is updated with u_Qsf_ReadCommandData
/// if ( status == E_OK ) then (yes)
/// if ( status == E_OK ) then (yes)
/// - address of ManufacturerID is updated with (uint8)(data32 & 0x000000FFu)
/// endif
/// if ( DeviceID     != NULL ) then (yes)
/// - address of DeviceID is updated with (uint16)( ( ( data32 >> 16 ) & 0x000000FFu ) | ( data32 & 0x0000FF00u ) )
/// endif
/// endif
/// -  return((Std_ReturnType)status)
/// @enduml
///
/// @testmethod
///    Dynamic unit test method
///    - This function has to be tested for statement and branch coverage.
///    - For detailed unit test description follow the caller graph to the test function below.
///    - Test script: test_pal_qsf.c
/// @traceability

static Std_ReturnType u_Qsf_ReadID(uint8 *ManufacturerID, uint16 *DeviceID);
/// Reference Source file : pal_qsf.c
///
/// @pre None
/// @post None
/// @param (uint32 PageSizeBytes, uint32 SectorSizeBytes, uint32 DeviceSizeBytes, uint32 TimeOut_WriteWord_us, uint32 TimeOut_EraseSector_ms)
/// @return static Std_ReturnType
/// @globals No global variables used
/// @InOutCorrelation None
/// @callsequence To be updated.
/// @InOutCorrelation Frequency at which the runnable is called: N/A
/// @startuml
/// (*)--> "Call sequence for u_Qsf_SetParameters()"
/// --> " Std_ReturnType status is updated with (Std_ReturnType)PAL_QSF_OK "
/// if  check according to the conddition
/// note left : (   (PageSizeBytes   == 0u)|| (( PageSizeBytes   & 15u ) != 0u)|| ((SectorSizeBytes & ( PageSizeBytes   - 1u )) != 0u) || (SectorSizeBytes <= PageSizeBytes)||((DeviceSizeBytes & ( SectorSizeBytes - 1u )) != 0u ) || (DeviceSizeBytes <= SectorSizeBytes)||	( DeviceSizeBytes > PAL_QSF_WINDOW_SIZE ) )
/// --> " status is updated with (Std_ReturnType)PAL_QSF_ERROR_SIZE  "
/// --> "  return(status) "
/// else
/// --> " sPAL_Qsf structure elements will update "
/// --> "  return(status) "
/// --> (*)
/// @enduml
///
/// @testmethod
///    Dynamic unit test method
///    - This function has to be tested for statement and branch coverage.
///    - For detailed unit test description follow the caller graph to the test function below.
///    - Test script: test_pal_qsf.c
/// @traceability

static Std_ReturnType u_Qsf_SetParameters(uint32 PageSizeBytes, uint32 SectorSizeBytes, uint32 DeviceSizeBytes, uint32 TimeOut_WriteWord_us, uint32 TimeOut_EraseSector_ms);

/// Reference Source file : pal_qsf.c
///
/// @pre None
/// @post None
/// @param (uint8 cmd, uint32 data, uint32 size, uint32 addr)
/// @return static Std_ReturnType
/// @globals No global variables used
/// @InOutCorrelation None
/// @callsequence To be updated.
/// @InOutCorrelation Frequency at which the runnable is called: N/A
/// @startuml
/// (*) --> " Call sequence for u_Qsf_WriteCommandData() "
/// --> switch (size)
/// --> ===B1===
/// --> case : 8u
/// --> " writeData is updated with ( (data & (uint32)0xFF) << 24u ) "
/// --> "break"
/// --> ===B2===
/// ===B1=== --> case : 16u
/// --> " writeData is updated with ( (data & (uint32)0xFFFF) << 16u ) "
/// --> "break "
/// --> ===B2===
/// ===B1=== --> case : default
/// --> " writeData is updated with data "
/// --> "break  "
/// --> ===B2===
/// if addr is check according to the condition
/// note left : (addr==0u)
/// --> " switch (size) "
/// --> ===B3===
/// --> case : 0u
/// --> " SFR32( RPC_SMENR  ) is updated with 0x00004000u "
/// --> " break"
/// --> ===B4===
/// ===B3=== --> case  : 8u
/// --> " SFR32( RPC_SMENR ) is updated with 0x00004008u "
/// --> "  break"
/// --> ===B4===
/// ===B3=== --> case  : 16u
/// --> " SFR32( RPC_SMENR ) is updated with 0x0000400Cu "
/// --> "   break"
/// --> ===B4===
/// ===B3=== --> case  : default
/// --> " SFR32( RPC_SMENR ) is updated with 0x0000400Fu "
/// --> "    break"
/// --> ===B4===
/// --> " SFR32( RPC_SMWDR0 ) is updated with writeData "
/// else
/// --> " SFR32( RPC_SMADR  ) is updated with addr "
/// --> " SFR32( RPC_SMENR  ) is updated with 0x00004708u "
/// --> " SFR32( RPC_SMWDR0 ) is updated with writeData "
/// --> " SFR32( RPC_SMCR   ) is updated with size==0u ? 0x00000001u : 0x00000003u "
/// --> " status is updated with u_Qsf_WaitTxEnd() "
/// if status is check according to the condition
/// note left : ( status == E_OK )
/// --> " status is updated with u_Qsf_WaitWIP(100u) "
/// --> "  return ( status ) "
/// else
/// --> "  return ( status ) "
/// --> (*)
/// @enduml
///
/// @testmethod
///    Dynamic unit test method
///    - This function has to be tested for statement and branch coverage.
///    - For detailed unit test description follow the caller graph to the test function below.
///    - Test script: test_pal_qsf.c
/// @traceability

static Std_ReturnType u_Qsf_WriteCommandData(uint8 cmd, uint32 data, uint32 size, uint32 addr);

/// Reference Source file : pal_qsf.c
///
/// @pre None
/// @post None
/// @param (uint32 *read, uint8 cmd, uint32 size, uint32 addr)
/// @return static Std_ReturnType
/// @globals No global variables used
/// @InOutCorrelation None
/// @callsequence To be updated.
/// @InOutCorrelation Frequency at which the runnable is called: N/A
/// @startuml
/// (*)--> "Call sequence for u_Qsf_ReadCommandData()"
/// if addr is chheck according to the condition then
/// note left : (addr==0u)
/// --> switch (size)
/// --> ===B1===
/// --> case : 8u
/// --> " SFR32( RPC_SMENR  ) is set to 0x00004008u "
/// --> "break"
/// --> ===B2===
///  ===B1=== --> case : 16u
/// --> " SFR32( RPC_SMENR  ) is set to 0x0000400Cu "
/// --> "break "
/// --> ===B2===
///  ===B1=== --> case : default
/// --> " SFR32( RPC_SMENR  ) is set to 0x0000400Fu "
/// --> " break "
/// --> ===B2===
/// --> " SFR32( RPC_SMCR ) is updated with 0x00000005u "
/// else
/// --> "  SFR32( RPC_SMADR  ) is updated with addr "
/// --> "SFR32( RPC_SMDMCR ) is updated with (uint32)(sPAL_Qsf.DummyCycles-1uL)"
/// --> " SFR32( RPC_SMENR  ) is updated with 0x0000C708u "
/// endif
/// --> " SFR32( RPC_SMCR ) is updated with 0x00000005u "
/// --> " status is updated with u_Qsf_WaitTxEnd() "
/// if status is check according to the condition
/// note left : ( status == (Std_ReturnType)PAL_QSF_OK )
/// --> " data32 is updated with SFR32( RPC_SMRDR0 ) "
/// --> switch (size )
/// --> ===B3===
/// --> case :  8u
/// --> " address of read is set to data32 & 0x000000FFu "
/// --> " break"
/// --> ===B4===
///  ===B3=== --> case :  16u
/// --> " address of read is set to data32 & 0x0000FFFFu "
/// --> "  break"
/// --> ===B4===
///  ===B3=== --> case :  default
/// --> " address of read is set to data32 "
/// --> "    break "
/// --> ===B4===
/// --> " return((Std_ReturnType)status) "
/// @enduml
///
/// @testmethod
///    Dynamic unit test method
///    - This function has to be tested for statement and branch coverage.
///    - For detailed unit test description follow the caller graph to the test function below.
///    - Test script: test_pal_qsf.c
/// @traceability

static Std_ReturnType u_Qsf_ReadCommandData(uint32 *read, uint8 cmd, uint32 size, uint32 addr);

/// Reference Source file : pal_qsf.c
///
/// @pre None
/// @post None
/// @param None
/// @return static void
/// @globals No global variables used
/// @InOutCorrelation None
/// @callsequence To be updated.
/// @InOutCorrelation Frequency at which the runnable is called: N/A
/// @startuml "Call sequence for PAL_v_QsfDDRCalibration()"
///
/// legend left
///    Task: Where the runnables are added, depending on the project (SchM, Com, etc.)
///    Task priority: n/a
///    Cycle time: N/A
/// endlegend
///
/// activate pal_qsf.c
///
///    pal_qsf.c -> sys_time.c: PAL_v_QsfDDRCalibration() calls SYS_TimeGetCount()
///    note left
///       Result: None
///    end note
///    activate sys_time.c
///       sys_time.c -> pal_qsf.c: done
///    deactivate sys_time.c
///    pal_qsf.c -> sys_time.c: PAL_v_QsfDDRCalibration() calls SYS_TimeGetCount()
///    note left
///       Call condition: (0u == u_lastCalibrationTime_ticks) || ((u_TimeGetCount - u_lastCalibrationTime_ticks) > CALIBRATION_INTERVAL_TICKS)
///       Result: None
///    end note
///    activate sys_time.c
///       sys_time.c -> pal_qsf.c: done
///    deactivate sys_time.c
/// deactivate pal_qsf.c
/// @enduml
///
/// @testmethod
///    Dynamic unit test method
///    - This function has to be tested for statement and branch coverage.
///    - For detailed unit test description follow the caller graph to the test function below.
///    - Test script: test_pal_qsf.c
/// @traceability

static void PAL_v_QsfDDRCalibration(void);

/// Reference Source file : pal_qsf.c
///
/// @pre None
/// @post None
/// @param (const uint32 u_Start, const uint32 u_Len)
/// @return static void
/// @globals No global variables used
/// @InOutCorrelation None
/// @callsequence To be updated.
/// @InOutCorrelation Frequency at which the runnable is called: N/A
/// @startuml
/// (*)--> "Call sequence for v_Qsf_DCacheRegionInvalidate()"
/// if (u_SCTLR & 4u) is check according to the condition
/// note left : (0u != (u_SCTLR & 4u) )
/// --> "  u_Address + is updated with CACHE_LINE_LEN "
/// --> " calling  pDSB "
/// else
/// --> " calling  pDSB "
/// -->(*)
/// @enduml
///
/// @testmethod
///    Dynamic unit test method
///    - This function has to be tested for statement and branch coverage.
///    - For detailed unit test description follow the caller graph to the test function below.
///    - Test script: test_pal_qsf.c
/// @traceability

static void v_Qsf_DCacheRegionInvalidate(const uint32 u_Start, const uint32 u_Len);
#ifdef QSF_PRESBL_USE_CACHE
static void v_Qsf_SyncMemories(void);
#endif
// uif85744 19-01-2024
// Msg(3:1503) The function 'PAL_u_QsfLockOTPArray' is defined but is not used within this project.
// Reason : The function may used in future scope. no function side effect.
// PRQA S 1503 1
Std_ReturnType PAL_u_QsfLockOTPArray(void)
// Function: Function locks OTP memory. Once this function is called, OTP memory is permanently read only and can not be updated
// Return values:
// PAL_QSF_OK       operation complete
// PAL_QSF_ERROR_TIMEOUT    operation timed out
// PAL_QSF_ERROR_NOTSUPPORTED  SW for this hardware is not available
{
  Std_ReturnType status;
  uint8          data = 0u;
	e_PAL_SdfType type_res = sPAL_Qsf.Type;
  if(type_res == PAL_SDF_TYPE_MACRONIX)
  {
    //Write enable
    status = u_Qsf_WriteEnable();

    if(status == E_OK)
    {
      //WRSCUR - to set the lock-down bit as "1" (once lock-down, cannot be updated)
      //Command : 0x2F
      status = u_Qsf_WriteCommandData(0x2Fu, 0x0, 0x0, 0x0);
    }

  }else if(type_res == PAL_SDF_TYPE_MICRON)
  {
    //OTP Control Byte (Byte 64)
    //Once bit 0 has been programmed to 0, it can no longer be changed to 1.
    status = PAL_u_QsfWriteOTPArray(&data, 1u, 0x64);
  }else
  {
    status = (Std_ReturnType)PAL_QSF_ERROR_NOTSUPPORTED;
  }

  return status;
}

Std_ReturnType PAL_u_QsfReadOTPArray(uint8* read, uint32 size, uint32 addr)
// Function: Reads data from OTP memory
// Return values:
// PAL_QSF_OK       operation complete
// PAL_QSF_ERROR_TIMEOUT    operation timed out
// PAL_QSF_ERROR_NOTSUPPORTED  SW for this hardware is not available
{
  Std_ReturnType status;
  uint32         data32;
  uint32         dummyCyclesEnabled;
  uint8          cmd;
	e_PAL_SdfType type_res = sPAL_Qsf.Type;
  if(type_res == PAL_SDF_TYPE_MACRONIX)
  {
    //*Change from main memory array to secured OTP area.
    //ENSO (B1h)
    status = u_Qsf_WriteCommandData(0xB1u, 0x0, 0x0, 0x0);
    cmd = 0x03u;  //command: 0x03 - READ
  }else if(type_res == PAL_SDF_TYPE_MICRON)
  {
    uint8 DummyCycles_res = (uint8)(sPAL_Qsf.DummyCycles-1uL);
    cmd = 0x4Bu; //command: 0x4B - READ OTP ARRAY
  // PRQA S 0303 1 // 0303_SFR_32
    SFR32( RPC_SMDMCR ) = DummyCycles_res;
    status = E_OK;
  }else
  {
    status = (Std_ReturnType)PAL_QSF_ERROR_NOTSUPPORTED;
  }

  if(status == E_OK)
  {
    // PRQA S 0303 7 //0303_SFR_32
    SFR32( RPC_DRCR   ) = 0x01000000u;    // Data read control        negate SSL in case still in continuous read mode
    SFR32( RPC_OFFSET1    ) = 0x31511144u;    // PHY timing offset        set PHY read mode to SDR
    SFR32( RPC_PHYCNT ) = 0x80030260u;    // PHY control          PHYMEM[1:0] = 2'b00: Serial Flash SDR;  CAL = 1: PHY calibration
    SFR32( RPC_CMNCR  ) = 0x81FFF300u;    // Common control       BSZ[1:0] = 2'b00: Serial Flash (1ch);  MD = 1: Manual mode
    SFR32( RPC_SMCMR  ) = ((uint32) cmd) << 16u; // Manual mode command       CMD[7:0] = 8'h05 = RDSR1: Read status register 1
    SFR32( RPC_SMDRENR    ) = 0x00000000u;    // Manual mode DDR enable   HYPE = 3'b000: SPI Flash;  SPIDRE = 0: data SDR transfer
    SFR32( RPC_SMADR  ) = addr;
    // PRQA S 4461 3 // 4461_UL_Warning
    // PRQA S 0303 1 // 0303_SFR_32
      e_PAL_SdfType type_res1 =  sPAL_Qsf.Type;
    dummyCyclesEnabled = type_res1 == PAL_SDF_TYPE_MICRON ? (1UL << 15UL) : 0UL; //if Micron enable dummy cycles
    switch (size)
    {
      case 1u:
       // PRQA S 0303 1 //0303_SFR_32
        SFR32( RPC_SMENR  ) = 0x00004708u | dummyCyclesEnabled;   // Manual mode enable       31:30 CDB  [1:0] = 2'b00: 1-bit command (MOSI/MISO)
        break;
      case 2u:
    		// PRQA S 0303 1 //0303_SFR_32
        SFR32( RPC_SMENR  ) = 0x0000470Cu | dummyCyclesEnabled;   // Manual mode enable       31:30 CDB  [1:0] = 2'b00: 1-bit command (MOSI/MISO)
        break;
      case 4u:
      default:
    		// PRQA S 0303 1 //0303_SFR_32
        SFR32( RPC_SMENR  ) = 0x0000470Fu | dummyCyclesEnabled;   // Manual mode enable       31:30 CDB  [1:0] = 2'b00: 1-bit command (MOSI/MISO)
        break;
    }
      //              17:16 SPIDB[1:0] = 2'b00: 1-bit data    (MOSI/MISO)
      //              15    DME        =    x : dummy cycle enabled (Macronix 0 / Micron 1)
      //              14    CDE        =    1 : Command enabled
      //              12    OCDE       =    0 : Option Command disabled
      //              11:8  ADE  [3:0] = 0111 : 24-bit address
      //              7:4   OPDE [3:0] = 0000 : option data disabled
      //              3:0   SPIDE[3:0] = xxxx : (1000 8-bit, 1100 16-bit, 1111 32-bit)
    		// PRQA S 0303 1 //0303_SFR_32
    SFR32( RPC_SMCR   ) = 0x00000005u;    // Manual mode control      SPIRE = 1: Data read enable;  SPIWE = 0: Data write disabled;  SPIE = 1: SPI transfer start

    status = u_Qsf_WaitTxEnd();
  }

  if ( status == (Std_ReturnType)PAL_QSF_OK)
  {
    		// PRQA S 0303 1 //0303_SFR_32
    data32 = SFR32( RPC_SMRDR0 );
    if(size > 0u)
    {
      // 2022-09-06; uif08910
      // Summary: Message(3:1496) [I] Destination and source objects may have incompatible types.
      //          Message(3:0314) Implicit conversion from a pointer to object type to a pointer to void.
      //          Message(3:0326) cast b/w pointer to void type and an integral type
      //          Message(3:4461) UL is being converted to narrower unsigned type, uint32 on assignment.
      // Reason:  Internal driver design and memcpy constraint.
      // PRQA S 0326,0314,1496,4461 1
      (void)Qsf_memcpy((void*)read, (void*)&data32, size);
    }
  }
  // e_PAL_SdfType type_res =  sPAL_Qsf.Type;
  if(type_res == PAL_SDF_TYPE_MACRONIX)
  {
    //Return from secured OTP area to main memory array.
    //EXSO (C1h)
    status |= u_Qsf_WriteCommandData(0xC1u,0x0,0x0,0x0);
  }


  return((Std_ReturnType)status);
}


// 2022-09-06; uif08910
// Summary: Message(3:7002) Amount of non-cyclic paths, minimum number of necessary test cases
// Reason:  Not critical, the number of conditional statements are high
// PRQA S 7002 ++
Std_ReturnType PAL_u_QsfWriteOTPArray(uint8* p_data, uint32 size, uint32 addr)
// Function: programs OTP memory. Once bit in this memory is set to 0 it can't be written back to 1.
// Return values:
// PAL_QSF_OK       operation complete
// PAL_QSF_ERROR_TIMEOUT    operation timed out
// PAL_QSF_ERROR_NOTSUPPORTED  SW for this hardware is not available
{
  Std_ReturnType status;
  uint32 writeData;
  uint32 data = 0u;
  uint8  cmd;
  uint8  cnt;
      e_PAL_SdfType type_res =  sPAL_Qsf.Type;
  if(type_res == PAL_SDF_TYPE_MACRONIX)
  {
    //*Change from main memory array to secured OTP area.
    //ENSO (B1h)
    status = u_Qsf_WriteCommandData(0xB1u, 0x0, 0x0, 0x0);
    cmd = 0x02u; //command: 0x02 - WRITE
  }else if(type_res == PAL_SDF_TYPE_MICRON)
  {
    cmd = 0x42u; //command: 0x42 - PROGRAM OTP ARRAY
    status = E_OK;
  }else
  {
    status = (Std_ReturnType)PAL_QSF_ERROR_NOTSUPPORTED;
  }


  for(cnt = 0; (cnt < size) && (cnt < sizeof(uint32)); cnt++)
  {
    ((uint8*)&data)[cnt] = p_data[cnt];
  }

  if( status == E_OK )
  {
    status = u_Qsf_WriteEnable();
    switch(size)
    {
      case 1u:
        writeData = ( (data & (uint32)0xFF) << 24u ); // shift by 24 required
        break;
      case 2u:
        writeData = ( (data & (uint32)0xFFFF) << 16u );
        break;
      case 3u:
        writeData = data | ((uint32)0xFFu << 24u);
        break;
      default:
        writeData = data;
        break;
    }
    		// PRQA S 0303 8 //0303_SFR_32
    SFR32( RPC_DRCR   ) = 0x00000000u;    // Data read control        negate SSL in case still in continuous read mode
    SFR32( RPC_OFFSET1    ) = 0x31511144u;    // PHY timing offset        set PHY read mode to SDR
    SFR32( RPC_PHYCNT ) = 0x80030260u;    // PHY control          PHYMEM[1:0] = 2'b00: Serial Flash SDR;  CAL = 1: PHY calibration
    SFR32( RPC_CMNCR  ) = 0x81FFF300u;    // Common control       BSZ[1:0] = 2'b00: Serial Flash (1ch);  MD = 1: Manual mode
    SFR32( RPC_SMCMR  ) = ((uint32) cmd) << 16u; // Manual mode command       CMD[7:0] = 8'h05 = RDSR1: Read status register 1
    SFR32( RPC_SMDRENR    ) = 0x00000000u;    // Manual mode DDR enable   HYPE = 3'b000: SPI Flash;  SPIDRE = 0: data SDR transfer

    SFR32( RPC_SMADR  ) = addr;
    switch (size)
    {
      case 1u:
    		// PRQA S 0303 1 //0303_SFR_32
        SFR32( RPC_SMENR  ) = 0x00004708u;    // Manual mode enable       31:30 CDB  [1:0] = 2'b00: 1-bit command (MOSI/MISO)
        break;
      case 2u:
    		// PRQA S 0303 1 //0303_SFR_32
        SFR32( RPC_SMENR  ) = 0x0000470Cu;    // Manual mode enable       31:30 CDB  [1:0] = 2'b00: 1-bit command (MOSI/MISO)
        break;
      case 4u:
      default:
    		// PRQA S 0303 1 //0303_SFR_32
        SFR32( RPC_SMENR  ) = 0x0000470Fu;    // Manual mode enable       31:30 CDB  [1:0] = 2'b00: 1-bit command (MOSI/MISO)
        break;
    }
    //              17:16 SPIDB[1:0] = 2'b00: 1-bit data    (MOSI/MISO)
    //              15    DME        =    0 : dummy cycle disabled
    //              14    CDE        =    1 : Command enabled
    //              12    OCDE       =    0 : Option Command disabled
    //              11:8  ADE  [3:0] = 0111 : 24-bit address (0000 disabled, 0111 24-bit, 1111 32-bit)
    //              7:4   OPDE [3:0] = 0000 : option data disabled
    //              3:0   SPIDE[3:0] = xxxx : 8,16 or 32-bit data (1000 8-bit, 1100 16-bit, 1111 32-bit)

    		// PRQA S 0303 2 //0303_SFR_32
    SFR32( RPC_SMWDR0 ) = writeData;
    SFR32( RPC_SMCR   ) = 0x00000003u;   // Manual mode control      SPIRE = 0: Data read disabled;  SPIWE = 1: Data write enabled;  SPIE = 1: SPI transfer start
  }
  if ( status == E_OK )
  {
    status = u_Qsf_WaitTxEnd();
  }

  if ( status == E_OK )
  {
    status = u_Qsf_WaitWIP(100u);
  }

  if(type_res == PAL_SDF_TYPE_MACRONIX)
  {
    //Return from secured OTP area to main memory array.
    //EXSO (C1h)
    status |= u_Qsf_WriteCommandData(0xC1u, 0x0, 0x0, 0x0);
  }

  return ( status );
}
// PRQA S 7002 --


// 2021-02-17; uie23485
// Summary: Message(3:7002) Amount of non-cyclic paths, minimum number of necessary test cases
// Reason:  Not critical, the number of conditional statements are high
// PRQA S 7002 ++
// PRQA S 3006 3
// Date: 2023-11-22; Reviewer : uif85752; 
// Reason: code contains a mixture of in-line assembler and C due to design
static Std_ReturnType u_Qsf_Init ( uint32 SPCLKfreq )
// Function: initializes the RPC macro
// Return values:
// PAL_QSF_OK       unit initialized successfully
// PAL_QSF_ERROR_PARAMETER  unsupported clock frequency requested
// PAL_QSF_ERROR_TIMEOUT    could not enable module clock
{
  uint32 u_RegVal = 0u;
  volatile uint32 i;
  Std_ReturnType status = (Std_ReturnType)PAL_QSF_OK;

#ifdef RCAR_V3H
  // JIRA ticket #CONMFC-185 (don't use the CPG_RPCCKCR register, default value 0x000000CC)
  // set clock mode
  switch ( SPCLKfreq )
  {
  case  (uint32)PAL_QSF_SPCLK_160MHz:
      u_RegVal = 0x00000011u;
  break;    // CPG_RPCCKCR[7:4]=0xC, CPG_RPCCKCR[3:0]=0xC, RPC_DIV_REG[1:0]=0x0: RPC clock=320MHz, RPCD2 clock=160MHz (for HyperFlash      160 MHz)
  case  (uint32)PAL_QSF_SPCLK_133MHz:
      u_RegVal = 0x00000019u;
  break;    // CPG_RPCCKCR[7:4]=0x3, CPG_RPCCKCR[3:0]=0x5, RPC_DIV_REG[1:0]=0x0: RPC clock=266MHz, RPCD2 clock=133MHz (for QSPI Flash      133 MHz)
  case  (uint32)PAL_QSF_SPCLK_80MHz:
      u_RegVal = 0x00000013u;
  break;    // CPG_RPCCKCR[7:4]=0xC, CPG_RPCCKCR[3:0]=0xC, RPC_DIV_REG[1:0]=0x1: RPC clock=160MHz, RPCD2 clock= 80MHz (for Hyper/QSPI Flash 80 MHz)
  case  (uint32)PAL_QSF_SPCLK_66MHz:
      u_RegVal = 0x0000001Bu;
  break;  // 66MHz not yet confirmed by Renesas
  case  (uint32)PAL_QSF_SPCLK_40MHz:
      u_RegVal = 0x00000017u;
  break;    // CPG_RPCCKCR[7:4]=0xC, CPG_RPCCKCR[3:0]=0xC, RPC_DIV_REG[1:0]=0x2: RPC clock= 80MHz, RPCD2 clock= 40MHz (for Hyper/QSPI Flash 40 MHz)
  // 2021-03-11; uie23485
  // Summary: Message(3:2024) Final switch case ends with a jump rather than 'break'
  // Reason: This is due to return statement, but this is necessary to detect invalid SPCLKfreq
  //       : Only V3H part is active, V3M is removed by compiler
  // PRQA S 2024 2
  default:
    status = (Std_ReturnType)PAL_QSF_ERROR_PARAMETER;
  break;
  }
    		// PRQA S 0303 1 //0303_SFR_32
  if ((SFR32( CPG_RPCCKCR ) != u_RegVal)&&(status == (Std_ReturnType)PAL_QSF_OK))
  {    		// PRQA S 0303 1 //0303_SFR_32
    SFR32( CPG_RPCCKCR ) = u_RegVal;
    // PRQA S 3112 3 // 2017-10-25; uidv7790
    // summary: Msg(3:3112) This statement has no side-effect - it can be removed.
    // reason: There is __asm code which is not recognized by static checker.
    pDSB();
    SYS_TimeDelay(1000u);   // was: SoftDelay( 50000 );
  }
#endif

#ifdef RCAR_V3M
  // JIRA ticket #CONMFC-185 (don't use the CPG_RPCCKCR register, default value 0x000000CC)
  // set clock mode
  switch ( SPCLKfreq )
  {
  case  (uint32)PAL_QSF_SPCLK_160MHz:
      u_RegVal = 0x000000CCu;
  break;    // CPG_RPCCKCR[7:4]=0xC, CPG_RPCCKCR[3:0]=0xC, RPC_DIV_REG[1:0]=0x0: RPC clock=320MHz, RPCD2 clock=160MHz (for HyperFlash      160 MHz)
  case  (uint32)PAL_QSF_SPCLK_133MHz:
      u_RegVal = 0x00000035u;
  break;    // CPG_RPCCKCR[7:4]=0x3, CPG_RPCCKCR[3:0]=0x5, RPC_DIV_REG[1:0]=0x0: RPC clock=266MHz, RPCD2 clock=133MHz (for QSPI Flash      133 MHz)
  case  (uint32)PAL_QSF_SPCLK_80MHz:
      u_RegVal = 0x000000CCu;
  break;    // CPG_RPCCKCR[7:4]=0xC, CPG_RPCCKCR[3:0]=0xC, RPC_DIV_REG[1:0]=0x1: RPC clock=160MHz, RPCD2 clock= 80MHz (for Hyper/QSPI Flash 80 MHz)
  case  (uint32)PAL_QSF_SPCLK_66MHz:
      u_RegVal = 0x00000035u;
  break;  // 66MHz not yet confirmed by Renesas
  case  (uint32)PAL_QSF_SPCLK_40MHz:
      u_RegVal = 0x000000CCu;
  break;    // CPG_RPCCKCR[7:4]=0xC, CPG_RPCCKCR[3:0]=0xC, RPC_DIV_REG[1:0]=0x2: RPC clock= 80MHz, RPCD2 clock= 40MHz (for Hyper/QSPI Flash 40 MHz)
  default:
    status = (Std_ReturnType)PAL_QSF_ERROR_PARAMETER;
  }
  // Proceed only if Status is OK
  if(status == (Std_ReturnType)PAL_QSF_OK)
  {
    if ((SFR32( CPG_RPCCKCR ) != u_RegVal))
    {
      SFR32( CPG_RPCCKCR ) = u_RegVal;
      // PRQA S 3112 3 // 2017-10-25; uidv7790
      // summary: Msg(3:3112) This statement has no side-effect - it can be removed.
      // reason: There is __asm code which is not recognized by static checker.
      pDSB();
      SYS_TimeDelay(1000u);   // was: SoftDelay( 50000 );
    }

    switch ( SPCLKfreq )
    {
    case  (uint32)PAL_QSF_SPCLK_160MHz:
        u_RegVal = 0x00000000u;
    break;    // CPG_RPCCKCR[7:4]=0xC, CPG_RPCCKCR[3:0]=0xC, RPC_DIV_REG[1:0]=0x0: RPC clock=320MHz, RPCD2 clock=160MHz (for HyperFlash      160 MHz)
    case  (uint32)PAL_QSF_SPCLK_133MHz:
        u_RegVal = 0x00000000u;
    break;    // CPG_RPCCKCR[7:4]=0x3, CPG_RPCCKCR[3:0]=0x5, RPC_DIV_REG[1:0]=0x0: RPC clock=266MHz, RPCD2 clock=133MHz (for QSPI Flash      133 MHz)
    case  (uint32)PAL_QSF_SPCLK_80MHz:
        u_RegVal = 0x00000001u;
    break;    // CPG_RPCCKCR[7:4]=0xC, CPG_RPCCKCR[3:0]=0xC, RPC_DIV_REG[1:0]=0x1: RPC clock=160MHz, RPCD2 clock= 80MHz (for Hyper/QSPI Flash 80 MHz)
    case  (uint32)PAL_QSF_SPCLK_66MHz:
        u_RegVal = 0x00000001u;
    break;    // 66 MHz not yet confirmed by Renesas
    case  (uint32)PAL_QSF_SPCLK_40MHz:
        u_RegVal = 0x00000002u;
    break;    // CPG_RPCCKCR[7:4]=0xC, CPG_RPCCKCR[3:0]=0xC, RPC_DIV_REG[1:0]=0x2: RPC clock= 80MHz, RPCD2 clock= 40MHz (for Hyper/QSPI Flash 40 MHz)
    default:
      status = (Std_ReturnType)PAL_QSF_ERROR_PARAMETER;
    }

    if (( SFR32( ARMREG_P_COMMON3 ) != u_RegVal )&&(status == E_OK))
    {
      SFR32( ARMREG_P_COMMON3 ) = u_RegVal;
      // PRQA S 3112 3 // 2017-10-25; uidv7790
      // summary: Msg(3:3112) This statement has no side-effect - it can be removed.
      // reason: There is __asm code which is not recognized by static checker.
      pDSB();
      SYS_TimeDelay(1000u);   // was: SoftDelay( 50000 );

    }
  }
#endif
    if(status == (Std_ReturnType)PAL_QSF_OK)
    {
      /*assert reset to the RPC module for the frequency change to take place.*/
    		// PRQA S 0303 6 //0303_SFR_32
      SFR32( CPG_SRCR9   ) |= (uint32) BIT(17uL); // assert reset
// date: 2023-11-22, reviewer: uif85752, No unwanted effects discovered
// Msg(3:3416) Logical operation performed on expression with possible side effects.
// PRQA S 3416 3
      for(i=0u;i<1000u;i++){};           // wait for 1 RCLK (assuming frequency is 32.55kHz)
      SFR32( CPG_SRSTCLR9 ) = BIT(17uL); // release reset
      for(i=0u;i<1000u;i++){};           // wait: tREADY1(35us) - tRHSL(10us) = 25us
    // PRQA S 0303 2 //0303_SFR_32
      SFR32( RPC_SSLDR  ) = 0x00000400u; // SSL delay            idle time: 1 CLK, SSL assert time: 5.5 CLKs, SPCLK activation after SSL: 1 CLK
      SFR32( RPC_DREAR  ) = 0x00000001u; // Data read extended address   use 64MiB window [25:0]
    }

  return(status);
}
// PRQA S 7002 --

Std_ReturnType u_Qsf_WaitTxEnd (void)
// Function: waits for an SPI transfer to complete
// Return values:
// PAL_QSF_OK       OK, transfer complete
// PAL_QSF_ERROR_TIMEOUT    operation timed out
{
  uint32 u_t, u_dt;
  Std_ReturnType u_RetVal = E_NOT_OK;

  u_t = SYS_TimeGetCount();
  do
  {
    u_dt = SYS_TimeGetSince( u_t );
    		// PRQA S 0303 1 //0303_SFR_32
    if ( (SFR32( RPC_CMNSR ) & BIT0 ) == BIT0 )
    {
      u_RetVal = E_OK ;
    }
    else
    {
      u_RetVal = (Std_ReturnType)PAL_QSF_ERROR_TIMEOUT;
    }
  } while ((u_dt < 150u)&&(u_RetVal != E_OK)); // max length = 1 byte command + 4 bytes address + 256 bytes data = 261 bytes = 2088 bits. @80MHz/single/SDR => 26.1us

  return(u_RetVal);
}

// 2021-03-10; uie23485
// Summary: Message(3:7002) HIS metrics check Amount of non cyclic paths
// Reason:  Not critical, too many cases/return statements cause this
// PRQA S 7002 ++
Std_ReturnType PAL_u_QsfInit(const Fls_ConfigType* ConfigPtr)
// Function: initializes the RPC macro and the QSPI flash device
// Return values:
// PAL_QSF_OK       OK, flash initialized
// PAL_QSF_ERROR_PARAMETER  unsupported clock frequency requested
// PAL_QSF_ERROR_TIMEOUT    operation timed out
{

#ifdef RCAR_V3H
  const uint32 SPCLKfreq = (const uint32)PAL_QSF_SPCLK_66MHz;
#endif

#ifdef RCAR_V3M
  const uint32 SPCLKfreq = (const uint32)PAL_QSF_SPCLK_66MHz;
#endif

  uint8 id;
  uint16 device;

  // initialize controller IP
  Std_ReturnType status = u_Qsf_Init( SPCLKfreq );

#if 0
  if ( status == E_OK )
#endif
  {
    // 2021-03-11; uie23485
    // Summary: Message(3:2982) This assignment is redundant. The value of this object is never used before being modified.
    // Reason:  This must have been done for debug purposes?
    // PRQA S 2982 ++
    if(ConfigPtr->u_WriteWindowSize <= 0x10000uL)
    {
      // set default parameters for this library
      status = u_Qsf_SetParameters( 256u, 4u*KiB, 64u*MiB, 5000u, 3000u );// for smaller partition
    }
    else
    {
      status = u_Qsf_SetParameters( 256u, 64u*KiB, 64u*MiB, 5000u, 3000u );// for bigger partition
    }
#if 0
    if ( status == E_OK )
#endif
    {

      status = u_Qsf_ReadID(&id, &device);
      if ( status == E_OK )
      {
        switch (id)
        {
        case 0xC2u:  // device 25
          sPAL_Qsf.Type=PAL_SDF_TYPE_MACRONIX;
          break;
        case 0x20u: // 3V3 device BA and BB
          sPAL_Qsf.Type=PAL_SDF_TYPE_MICRON;
          break;
        case 0x01u: // 128Mb device 20 and 02
          sPAL_Qsf.Type=PAL_SDF_TYPE_CYPRESS;
          break;
        default:
          sPAL_Qsf.Type=PAL_SDF_TYPE_UNKNOWN;
          break;
        }

        switch (sPAL_Qsf.Type)
        {
        case PAL_SDF_TYPE_MACRONIX:  // id=0xC2 device 25
          // Return from secured OTP area to main memory array.
          // EXSO (C1h)
          // This is necessary to ensure that SDF is returned to main memory region with each request
          status = u_Qsf_WriteCommandData(0xC1u,0x0,0x0,0x0);

          status = u_Qsf_WriteEnable();
          status = u_Qsf_WriteCommandData ( 0x01u, (uint32)(0x87uL << 8u) | 0x40u, 16u, 0u );
          // status=0x40 QE bit
          // config[2..0]=b111 default output driver strength (24 Ohms)
          // config[7..6]=b10  8 dummy cycles for all commands

          sPAL_Qsf.Mode = PAL_QSPI_DDR_BURST;
          sPAL_Qsf.DummyCycles = 8u;
          sPAL_Qsf.DummyCyclesEC= 6u;
          sPAL_Qsf.DummyCyclesEE = 8u;
          break;

        case PAL_SDF_TYPE_MICRON: // id=0x20 3V3 device BA and BB
          sPAL_Qsf.Mode = PAL_QSPI_DDR_BURST;
          sPAL_Qsf.DummyCycles = 8u;
          sPAL_Qsf.DummyCyclesEC = 10u;
          sPAL_Qsf.DummyCyclesEE = 8u;
          break;

        case PAL_SDF_TYPE_CYPRESS: // id=0x01 128Mb device 20 and 02
          status = u_Qsf_WriteEnable();
          status = u_Qsf_WriteCommandData ( 0x71u, 0x02u, 8u, 0x00800002u); // CR1V, (still with 3-byte addressing)

          status = u_Qsf_WriteEnable();
          status = u_Qsf_WriteCommandData ( 0x71u, 0x07u, 8u, 0x00800003u); // CR2V
          // PRQA S 2982 --
          sPAL_Qsf.DummyCycles = 7u;
          sPAL_Qsf.DummyCyclesEC = 7u;
          sPAL_Qsf.DummyCyclesEE = 7u;

          sPAL_Qsf.Mode = PAL_QSPI_SDR_BURST;
          break;

        default:
          status = (Std_ReturnType)PAL_QSF_ERROR_NOTSUPPORTED;
          sPAL_Qsf.Mode = PAL_SPI_SDR_BURST;
          break;
        }

        PAL_v_QsfDDRCalibration();
        uint8 u_calibratedStrtimValue_res = u_calibratedStrtimValue;
        if(u_calibratedStrtimValue_res >= 0x08u)
        {
          sPAL_Qsf.Mode = PAL_QSPI_SDR_BURST;
        }
        else
        {
          // do nothing
        }
      }
    }


  }

  return(status );
}
// PRQA S 7002 --

Std_ReturnType Pal_u_Qsf_SetSectorSize (uint32 SectorSizeBytes)
// Function: sets the parameters for QSPI flash device access
// Return values:
// PAL_QSF_OK       parameters set successfully
// PAL_QSF_ERROR_SIZE  SectorSizeBytes is no multiple of 4k;
{
  Std_ReturnType status = E_OK;
  if (!(( SectorSizeBytes == 4u*KiB) || ( SectorSizeBytes == 32u*KiB) || ( SectorSizeBytes == 64u*KiB)))
  {
    status = (Std_ReturnType)PAL_QSF_ERROR_SIZE;
  }

  if(status == E_OK)
  {
    sPAL_Qsf.SectorSize = SectorSizeBytes;
  }

  return status;
}

static Std_ReturnType u_Qsf_SetParameters (
    uint32 PageSizeBytes,
    uint32 SectorSizeBytes,
    uint32 DeviceSizeBytes,
    uint32 TimeOut_WriteWord_us,
    uint32 TimeOut_EraseSector_ms)
// Function: sets the parameters for QSPI flash device access
// Return values:
// PAL_QSF_OK       parameters set successfully
// PAL_QSF_ERROR_SIZE   PageSizeBytes is 0 or no multiple of 16; SectorSizeBytes is no multiple of PageSizeBytes; DeviceSizeBytes is no multiple of SectorSizeBytes, or DeviceSizeBytes > 16 MiB
{
  Std_ReturnType status = (Std_ReturnType)PAL_QSF_OK;
  if (   (PageSizeBytes   == 0u)
      || (( PageSizeBytes   & 15u ) != 0u)
      || ((SectorSizeBytes & ( PageSizeBytes   - 1u )) != 0u) || (SectorSizeBytes <= PageSizeBytes)
      || ((DeviceSizeBytes & ( SectorSizeBytes - 1u )) != 0u ) || (DeviceSizeBytes <= SectorSizeBytes)
      || ( DeviceSizeBytes > PAL_QSF_WINDOW_SIZE ) )
  {
    status = (Std_ReturnType)PAL_QSF_ERROR_SIZE ;
  }
  else
  {
    sPAL_Qsf.PageSize         = PageSizeBytes         ;
    sPAL_Qsf.SectorSize           = SectorSizeBytes       ;
    sPAL_Qsf.DeviceSize           = DeviceSizeBytes       ;
    sPAL_Qsf.TIMEOUT_WriteWord_us     = TimeOut_WriteWord_us      ;
    sPAL_Qsf.TIMEOUT_EraseSector_ms   = TimeOut_EraseSector_ms    ;
  }
  return(status);
}

static Std_ReturnType u_Qsf_ReadID(uint8 *ManufacturerID, uint16 *DeviceID)
// Function: reads the CFI manufacturer and device IDs
// Return values:
// PAL_QSF_OK       OK, ID read successfully
// PAL_QSF_ERROR_TIMEOUT    operation timed out
{
  Std_ReturnType status = (Std_ReturnType)PAL_QSF_OK;
  uint32 data32;

  status = u_Qsf_ReadCommandData(&data32, 0x9Fu, 32u, 0u);
  if ( status == E_OK )
  {
    if ( ManufacturerID   != NULL )
    {
      *ManufacturerID =     (uint8)(data32     & 0x000000FFu);
    }
    if ( DeviceID     != NULL )
    {
      *DeviceID       = (uint16)( ( ( data32 >> 16 ) & 0x000000FFu ) |    // memory size (2^n)
          (   data32         & 0x0000FF00u ) );   // memory type
    }
  }

  return((Std_ReturnType)status);
}

static Std_ReturnType u_Qsf_WriteCommandData(uint8 cmd, uint32 data, uint32 size, uint32 addr)
// Function: sends the command followed by data
// Return values:
// PAL_QSF_OK       operation complete
// PAL_QSF_ERROR_TIMEOUT    operation timed out
{
  Std_ReturnType status;
  uint32 writeData;

  switch(size)
  {
  case 8u:
    writeData = ( (data & (uint32)0xFF) << 24u ); // shift by 24 required
    break;
  case 16u:
    writeData = ( (data & (uint32)0xFFFF) << 16u );
    break;
  default:
    writeData = data;
    break;
  }
    		// PRQA S 0303 6 //0303_SFR_32
  SFR32( RPC_DRCR   ) = 0x01000000u;    // Data read control        negate SSL in case still in continuous read mode
  SFR32( RPC_OFFSET1    ) = 0x31511144u;    // PHY timing offset        set PHY read mode to SDR
  SFR32( RPC_PHYCNT ) = 0x80030260u;    // PHY control          PHYMEM[1:0] = 2'b00: Serial Flash SDR;  CAL = 1: PHY calibration
  SFR32( RPC_CMNCR  ) = 0x81FFF300u;    // Common control       BSZ[1:0] = 2'b00: Serial Flash (1ch);  MD = 1: Manual mode
  SFR32( RPC_SMCMR  ) = ((uint32) cmd) << 16u; // Manual mode command       CMD[7:0] = 8'h05 = RDSR1: Read status register 1
  SFR32( RPC_SMDRENR    ) = 0x00000000u;    // Manual mode DDR enable   HYPE = 3'b000: SPI Flash;  SPIDRE = 0: data SDR transfer

  if(addr==0u)
  {
    switch (size)
    {
    case 0u:
    		// PRQA S 0303 1 //0303_SFR_32
      SFR32( RPC_SMENR  ) = 0x00004000u;    // Manual mode enable       31:30 CDB  [1:0] = 2'b00: 1-bit command (MOSI/MISO)
      break;
    case 8u:
    		// PRQA S 0303 1 //0303_SFR_32
      SFR32( RPC_SMENR  ) = 0x00004008u;    // Manual mode enable       31:30 CDB  [1:0] = 2'b00: 1-bit command (MOSI/MISO)
      break;
    case 16u:
    		// PRQA S 0303 1 //0303_SFR_32
      SFR32( RPC_SMENR  ) = 0x0000400Cu;    // Manual mode enable       31:30 CDB  [1:0] = 2'b00: 1-bit command (MOSI/MISO)
      break;
    case 32u:
    default:
    		// PRQA S 0303 1 //0303_SFR_32
      SFR32( RPC_SMENR  ) = 0x0000400Fu;    // Manual mode enable       31:30 CDB  [1:0] = 2'b00: 1-bit command (MOSI/MISO)
      break;

    }
    //              17:16 SPIDB[1:0] = 2'b00: 1-bit data    (MOSI/MISO)
    //              15    DME        =    0 : dummy cycle disabled
    //              14    CDE        =    1 : Command enabled
    //              12    OCDE       =    0 : Option Command disabled
    //              11:8  ADE  [3:0] = 0000 : address disabled
    //              7:4   OPDE [3:0] = 0000 : option data disabled
    //              3:0   SPIDE[3:0] = 1000 : 8-bit data (1000 8-bit, 1100 16-bit, 1111 32-bit)
  }
  else
  {    		// PRQA S 0303 2 //0303_SFR_32
    SFR32( RPC_SMADR  ) = addr;
    SFR32( RPC_SMENR    ) = 0x00004708u;    // Manual mode enable       31:30 CDB  [1:0] = 2'b00: 1-bit command (MOSI/MISO)
    //              17:16 SPIDB[1:0] = 2'b00: 1-bit data    (MOSI/MISO)
    //              15    DME        =    0 : dummy cycle disabled
    //              14    CDE        =    1 : Command enabled
    //              12    OCDE       =    0 : Option Command disabled
    //              11:8  ADE  [3:0] = 0111 : 24-bit address (0000 disabled, 0111 24-bit, 1111 32-bit)
    //              7:4   OPDE [3:0] = 0000 : option data disabled
    //              3:0   SPIDE[3:0] = 1000 : 8-bit data (1000 8-bit, 1100 16-bit, 1111 32-bit)
  }
    		// PRQA S 0303 2 //0303_SFR_32
  SFR32( RPC_SMWDR0 ) = writeData;
  SFR32( RPC_SMCR   ) = size==0u ? 0x00000001u : 0x00000003u;   // Manual mode control      SPIRE = 0: Data read disabled;  SPIWE = 1: Data write enabled;  SPIE = 1: SPI transfer start

  status = u_Qsf_WaitTxEnd();
  if ( status == E_OK )
  {
    status = u_Qsf_WaitWIP(100u);
  }

  return ( status );
}

static Std_ReturnType u_Qsf_ReadCommandData(uint32 *read, uint8 cmd, uint32 size, uint32 addr)
// Function: sends the command followed by data
// Return values:
// PAL_QSF_OK       operation complete
// PAL_QSF_ERROR_TIMEOUT    operation timed out
{
  Std_ReturnType    status;
  uint32 data32;
    		// PRQA S 0303 6 //0303_SFR_32
  SFR32( RPC_DRCR   ) = 0x01000000u;    // Data read control        negate SSL in case still in continuous read mode
  SFR32( RPC_OFFSET1    ) = 0x31511144u;    // PHY timing offset        set PHY read mode to SDR
  SFR32( RPC_PHYCNT ) = 0x80030260u;    // PHY control          PHYMEM[1:0] = 2'b00: Serial Flash SDR;  CAL = 1: PHY calibration
  SFR32( RPC_CMNCR  ) = 0x81FFF300u;    // Common control       BSZ[1:0] = 2'b00: Serial Flash (1ch);  MD = 1: Manual mode
  SFR32( RPC_SMCMR  ) = ((uint32) cmd) << 16u; // Manual mode command       CMD[7:0] = 8'h05 = RDSR1: Read status register 1
  SFR32( RPC_SMDRENR    ) = 0x00000000u;    // Manual mode DDR enable   HYPE = 3'b000: SPI Flash;  SPIDRE = 0: data SDR transfer
  if(addr==0u)
  {
    switch (size)
    {
    case 8u:
    		// PRQA S 0303 1 //0303_SFR_32
      SFR32( RPC_SMENR  ) = 0x00004008u;    // Manual mode enable       31:30 CDB  [1:0] = 2'b00: 1-bit command (MOSI/MISO)
      break;
    case 16u:
    		// PRQA S 0303 1 //0303_SFR_32
      SFR32( RPC_SMENR  ) = 0x0000400Cu;    // Manual mode enable       31:30 CDB  [1:0] = 2'b00: 1-bit command (MOSI/MISO)
      break;
    default: // also 32
    		// PRQA S 0303 1 //0303_SFR_32
      SFR32( RPC_SMENR  ) = 0x0000400Fu;    // Manual mode enable       31:30 CDB  [1:0] = 2'b00: 1-bit command (MOSI/MISO)
      break;
    }
    //              17:16 SPIDB[1:0] = 2'b00: 1-bit data    (MOSI/MISO)
    //              15    DME        =    0 : dummy cycle disabled
    //              14    CDE        =    1 : Command enabled
    //              12    OCDE       =    0 : Option Command disabled
    //              11:8  ADE  [3:0] = 0000 : address disabled
    //              7:4   OPDE [3:0] = 0000 : option data disabled
    //              3:0   SPIDE[3:0] = 1000 : 8-bit data (1000 8-bit, 1100 16-bit, 1111 32-bit)
  }
  else
  {    		// PRQA S 0303 3 //0303_SFR_32
    SFR32( RPC_SMADR  ) = addr;
    SFR32( RPC_SMDMCR ) = (uint32)(sPAL_Qsf.DummyCycles-1uL);
    SFR32( RPC_SMENR  ) = 0x0000C708u;    // Manual mode enable       31:30 CDB  [1:0] = 2'b00: 1-bit command (MOSI/MISO)
    //              17:16 SPIDB[1:0] = 2'b00: 1-bit data    (MOSI/MISO)
    //              15    DME        =    1 : dummy cycle enabled
    //              14    CDE        =    1 : Command enabled
    //              12    OCDE       =    0 : Option Command disabled
    //              11:8  ADE  [3:0] = 0111 : 24-bit address
    //              7:4   OPDE [3:0] = 0000 : option data disabled
    //              3:0   SPIDE[3:0] = 1000 : 8-bit data (1000 8-bit, 1100 16-bit, 1111 32-bit)
  }
    		// PRQA S 0303 1 //0303_SFR_32
  SFR32( RPC_SMCR   ) = 0x00000005u;    // Manual mode control      SPIRE = 1: Data read enable;  SPIWE = 0: Data write disabled;  SPIE = 1: SPI transfer start

  status = u_Qsf_WaitTxEnd();
  if ( status == (Std_ReturnType)PAL_QSF_OK )
  {    		// PRQA S 0303 1 //0303_SFR_32
    data32 = SFR32( RPC_SMRDR0 );
    switch (size)
    {
    case 8u:
      *read = data32 & 0x000000FFu;
      break;
    case 16u:
      *read = data32 & 0x0000FFFFu;
      break;
    default: // also 32
      *read = data32;
      break;
    }
  }

  return((Std_ReturnType)status);
}

Std_ReturnType PAL_u_QsfIsBusy(void)
// Function: returns status of a write/erase action, and clears the error flags, if set
// Return values:
// PAL_QSF_OK       operation complete
// PAL_QSF_BUSY     erase/write operation still ongoing
// Qsf_FLAH_ERROR_TIMEOUT   operation timed out
// PAL_QSF_ERROR_ERASE  error during erase
// PAL_QSF_ERROR_WRITE  error during write
{
  Std_ReturnType    status;
  uint32    statusreg;

  // 2021-03-11; uie23485
  // Summary: Message(3:2982) This assignment is redundant. The value of this object is never used before being modified.
  // Reason:  This must have been done for debug purposes?
  // PRQA S 2982 ++
  status = u_Qsf_ReadCommandData ( &statusreg, 0x05u, 8u, 0u );   // cmd 0x05 read status reg
  if ( status == E_OK )
  {
    status = E_OK;
    if ( (statusreg & BIT0) == BIT0 )
    {
      status = (Std_ReturnType)PAL_QSF_BUSY;      // WIP   (Write In Progress)
    }
      e_PAL_SdfType type_res =  sPAL_Qsf.Type;
    if (type_res==PAL_SDF_TYPE_CYPRESS)
    {
      if ( (statusreg & BIT5) == BIT5 ) // Cypress bit5=E_ERR_D
      {
        status = (Std_ReturnType)PAL_QSF_ERROR_ERASE; // E_ERR (Erase Error)
      }

      if ( (statusreg & (uint32)0x60) != 0u ) // Cypress bit6=P_ERR_D and bit5=E_ERR_D
      {
        status = u_Qsf_WriteCommandData ( 0x30u, 0u, 0u, 0u ); // cmd 0x30 clear status reg only for Cypress
      }
    }
  }
  // PRQA S 2982 --
  return( status );
}

static Std_ReturnType u_Qsf_WaitWIP(uint32 timeout_us)
// Function: waits for a write/erase action to complete
// Return values:
// PAL_QSF_OK       OK, operation completed successfully or idle
// PAL_QSF_ERROR_TIMEOUT    operation timed out
// PAL_QSF_ERROR_ERASE  error during erase
// PAL_QSF_ERROR_WRITE  error during write
{
  Std_ReturnType status = (Std_ReturnType)PAL_QSF_ERROR_TIMEOUT;
  uint32 t, elapsed_t;

  t = SYS_TimeGetCount();
  do
  {
    elapsed_t = SYS_TimeGetSince( t );
    status = PAL_u_QsfIsBusy();
  } while (( elapsed_t < timeout_us )&&(status == (Std_ReturnType)PAL_QSF_BUSY));

  return(status);
}

Std_ReturnType u_Qsf_WriteEnable(void)
// Function: submits a "write enable" command to the flash device for a subsequent write operation
// Return values:
// PAL_QSF_OK       OK, operation completed successfully or idle
// PAL_QSF_ERROR_TIMEOUT    operation timed out
{
  return( u_Qsf_WriteCommandData ( 0x06u, 0u, 0u, 0u ) );
}

// 2021-03-10; uie23485
// Summary: Message(3:7010) HIS metrics check min no of test cases
// Reason:  Not critical, too many if/else statements
// PRQA S 7002 ++

static Std_ReturnType u_Qsf_StartRead(uint32 FlashAddr, uint32 SizeBytes)
// Function: prepares to read data from the QSPI flash
// Return values:
// PAL_QSF_OK       OK, data verified successfully (matches)
// PAL_QSF_ERROR_ALIGN  access crosses RPC window boundary
// PAL_QSF_ERROR_SIZE   accessing flash outside of bounds
{
  uint32    FlashAddrEnd;

  Std_ReturnType status = E_OK;
  uint32 DeviceSize_res = sPAL_Qsf.DeviceSize;
  if ( FlashAddr + SizeBytes > DeviceSize_res )
  {
    status = (Std_ReturnType)PAL_QSF_ERROR_SIZE;
  }

  if(status == E_OK)
  {
    FlashAddrEnd = FlashAddr + SizeBytes - 1u;
    if ( (FlashAddr & ~PAL_QSF_WINDOW_MASK) != (FlashAddrEnd & ~PAL_QSF_WINDOW_MASK) )    // window boundary crossing
    {
      status = (Std_ReturnType)PAL_QSF_ERROR_ALIGN;
    }
  }

  if(status == E_OK)
  {    		// PRQA S 0303 2 //0303_SFR_32
    SFR32( RPC_DREAR  ) = 0x00000001u | ( (uint32)(FlashAddr & ~PAL_QSF_WINDOW_MASK) >> 9u ); // Data read extended address   use 64MiB window [25:0]
    SFR32( RPC_DRCR   ) = 0x01000000u;    // Data read control        negate SSL in case still in continuous read mode

    PAL_v_QsfDDRCalibration();
    uint8 u_calibratedStrtimValue_res = u_calibratedStrtimValue;
    if(u_calibratedStrtimValue_res >= 0x08u)
    {
      sPAL_Qsf.Mode = PAL_QSPI_SDR_BURST;
    }
    else
    {
      // do nothing
    }

      e_PAL_QspiMode Mode_res =  sPAL_Qsf.Mode;
    if (Mode_res == PAL_SPI_SDR_BURST) // (single) SPI, SDR, burst access
    {    		// PRQA S 0303 7 //0303_SFR_32
      SFR32( RPC_OFFSET1  ) = 0x31511144u;    // PHY timing offset        set PHY read mode to SDR
      SFR32( RPC_PHYCNT   ) = 0x80000260u;    // PHY control          PHYMEM[1:0] = 2'b00: Serial Flash SDR;  CAL = 1: PHY calibration;  HS = 0: normal read response
      SFR32( RPC_CMNCR    ) = 0x01FFF300u;    // Common control       BSZ[1:0] = 2'b00: Serial Flash (1ch);  MD = 0: External address space mode
      SFR32( RPC_DRCR ) = 0x011F0300u;        // Data read control        RBURST = 5'h1F: read burst length is 32 units of 64-bit = 256 bytes;  RBE = 1: read bursts enabled;  RCF = 1: read cache flush
      SFR32( RPC_DRCMR    ) = 0x000B0000u;    // Data read command        CMD[7:0] = 8'h0B = FAST_READ
      SFR32( RPC_DRENR    ) = 0x0000C700u;    // Data read enable     CDB  [1:0] = 2'b00: 1-bit command (MOSI/MISO)
      //              ADB  [1:0] = 2'b00: 1-bit address (MOSI/MISO)
      //              SPIDB[1:0] = 2'b00: 1-bit data    (MOSI/MISO)
      //              DME        =    1 : dummy cycle enabled
      //              CDE        =    1 : Command enabled
      //              OCDE       =    0 : Option Command disabled
      //              ADE  [3:0] = 0111 : ADR[23:0] output
      //              OPDE [3:0] = 0000 : option data disabled
      // PRQA S 4461 2 // 4461_UL_Warning
      // PRQA S 0303 2 // 0303_SFR_32
      SFR32( RPC_DRDMCR   ) = 0x00000000uL + (uint32)(sPAL_Qsf.DummyCycles-1uL); // Data read dummy cycles   DMDB[1:0] = 2'b00: 1-bit (MOSI/MISO);  DMCYC[4:0] = 5'h07: 8 cycles
      SFR32( RPC_DRDRENR  ) = 0x00000000u;    // Data read DDR enable     HYPE = 3'b000: SPI Flash;  ADDRE = 0: address SDR transfer;  DRDRE = 0: data SDR transfer
    }    		// PRQA S 0303 1 //0303_SFR_32
    else if (Mode_res == PAL_QSPI_SDR_BURST) // QuadSPI, SDR, burst access
    {        
		  e_PAL_SdfType type_res =  sPAL_Qsf.Type;
 		  // PRQA S 0303 7 //0303_SFR_32
      SFR32( RPC_OFFSET1  ) = 0x31511144u;    // PHY timing offset        set PHY read mode to SDR
      SFR32( RPC_PHYCNT   ) = 0x80000260u;    // PHY control          PHYMEM[1:0] = 2'b00: Serial Flash SDR;  CAL = 1: PHY calibration;  HS = 0: normal read response
      SFR32( RPC_CMNCR    ) = 0x01557300u;    // Common control       BSZ[1:0] = 2'b00: Serial Flash (1ch);  MD = 0: External address space mode
      SFR32( RPC_DRCR ) = 0x011F0300u;        // Data read control        RBURST = 5'h1F: read burst length is 32 units of 64-bit = 256 bytes;  RBE = 1: read bursts enabled;  RCF = 1: read cache flush
      SFR32( RPC_DRCMR    ) = 0x00EC0000u;    // Data read command        CMD[7:0] = 8'hEC = 4QIOR = 4-byte address SDR Quad I/O Fast Read
      // PRQA S 4461 1 // 4461_UL_Warning
      SFR32( RPC_DRENR    ) = 0x0222CF00uL |
          (type_res==PAL_SDF_TYPE_CYPRESS ? 0x00200080uL : 0uL) | // option data bit size 4, OPD3 is output
          (type_res==PAL_SDF_TYPE_MACRONIX ? 0x00200080uL : 0uL); // option data bit size 4, OPD3 is output
      // Data read enable     CDB  [1:0] = 2'b00: 1-bit command (MOSI/MISO)
      //              ADB  [1:0] = 2'b10: 4-bit address (DIO[3:0])
      //              SPIDB[1:0] = 2'b10: 4-bit data    (DIO[3:0])
      //              DME        =    1 : dummy cycle enabled
      //              CDE        =    1 : Command enabled
      //              OCDE       =    0 : Option Command disabled
      //
      //              ADE  [3:0] = 1111 : ADR[31:0] output
      //              OPDE [3:0] = 0000 : option data disabled
      // PRQA S 4461 2 // 4461_UL_Warning
    	// PRQA S 0303 15 //0303_SFR_32
      SFR32( RPC_DRDMCR   ) = 0x00020000uL | (uint32)(sPAL_Qsf.DummyCyclesEC-1uL);
      // Data read dummy cycles   DMDB[1:0] = 2'b10: 4-bit (DIO[3:0]);  DMCYC[4:0] = 5'h09: 10 cycles
      SFR32( RPC_DRDRENR  ) = 0x00000000u;    // Data read DDR enable     HYPE = 3'b000: SPI Flash;  ADDRE = 0: address SDR transfer;  DRDRE = 0: data SDR transfer
    }
    else if (Mode_res == PAL_QSPI_DDR_SINGLE) // QuadSPI, DDR, single access (non-burst access)
    {
      e_PAL_SdfType type_res =  sPAL_Qsf.Type;
      SFR32( RPC_OFFSET1  ) = 0x21511144u;    // PHY timing offset        set PHY read mode to DDR
      SFR32( RPC_PHYCNT   ) = 0x80000261u;    // PHY control          PHYMEM[1:0] = 2'b01: Serial Flash DDR;  CAL = 1: PHY calibration;  HS = 0: normal read response
      SFR32( RPC_CMNCR    ) = 0x01557300u;    // Common control       BSZ[1:0] = 2'b00: Serial Flash (1ch);  MD = 0: External address space mode
      SFR32( RPC_DRCR     ) = 0x011F0200u;    // Data read control        RBE = 0: read bursts disabled
      SFR32( RPC_DRCMR    ) = 0x00EE0000u;    // Data read command        CMD[7:0] = 8'hEE = 4DDRQIOR = 4-byte address DDR Quad I/O Fast Read
      // PRQA S 4461 1 // 4461_UL_Warning
      SFR32( RPC_DRENR    ) = 0x0222CF00uL |
          (type_res==PAL_SDF_TYPE_CYPRESS ? 0x00200080uL : 0uL) | // option data bit size 4, OPD3 is output
          (type_res==PAL_SDF_TYPE_MACRONIX ? 0x00200080uL : 0uL); // option data bit size 4, OPD3 is output
      // Data read enable     CDB  [1:0] = 2'b00: 1-bit command (MOSI/MISO)
      //              ADB  [1:0] = 2'b10: 4-bit address (DIO[3:0])
      //              SPIDB[1:0] = 2'b10: 4-bit data    (DIO[3:0])
      //              DME        =    1 : dummy cycle enabled
      //              CDE        =    1 : Command enabled
      //              OCDE       =    0 : Option Command disabled
      //              ADE  [3:0] = 1111 : ADR[31:0] output
      //              OPDE [3:0] = 0000 : option data disabled
      // PRQA S 4461 2 // 4461_UL_Warning
    	// PRQA S 0303 15 //0303_SFR_32
      SFR32( RPC_DRDMCR   ) = 0x00020000uL | (sPAL_Qsf.DummyCyclesEE-1uL);
      // Data read dummy cycles   DMDB[1:0] = 2'b10: 4-bit (DIO[3:0]);  DMCYC[4:0] = 5'h07: 8 cycles
      SFR32( RPC_DRDRENR  ) = 0x00000111u;    // Data read DDR enable     HYPE = 3'b000: SPI Flash;  ADDRE = 1: address DDR transfer;  DRDRE = 1: data DDR transfer
    }
    else if (Mode_res == PAL_QSPI_DDR_BURST) // QuadSPI, DDR, burst access
    {
      e_PAL_SdfType type_res =  sPAL_Qsf.Type;
      SFR32( RPC_OFFSET1  ) = 0x21511144u;    // PHY timing offset        set PHY read mode to DDR
      SFR32( RPC_PHYCNT   ) = 0x00080261u | (uint32)((uint32)u_calibratedStrtimValue << 15u);
      SFR32( RPC_CMNCR    ) = 0x01557300u;    // Common control       BSZ[1:0] = 2'b00: Serial Flash (1ch);  MD = 0: External address space mode
      SFR32( RPC_DRCR     ) = 0x011F0301u;    // Data read control        RBURST = 5'h1F: read burst length is 32 units of 64-bit = 256 bytes;  RBE = 1: read bursts enabled;  RCF = 1: read cache flush;  SSLE = 1: continuous read
      SFR32( RPC_DRCMR    ) = 0x00EE0000u;    // Data read command        CMD[7:0] = 8'hEE = 4DDRQIOR = 4-byte address DDR Quad I/O Fast Read
      // PRQA S 4461 1 // 4461_UL_Warning
      SFR32( RPC_DRENR    ) = 0x0222CF00uL |
          ((type_res==PAL_SDF_TYPE_CYPRESS) ? 0x00200080uL : 0uL) | // option data DDR transfer of option data
          ((type_res==PAL_SDF_TYPE_MACRONIX) ? 0x00200000uL : 0uL); // option data DDR transfer of option data
      // Data read enable     CDB  [1:0] = 2'b00: 1-bit command (MOSI/MISO)
      //              ADB  [1:0] = 2'b10: 4-bit address (DIO[3:0])
      //              SPIDB[1:0] = 2'b10: 4-bit data    (DIO[3:0])
      //              DME        =    1 : dummy cycle enabled
      //              CDE        =    1 : Command enabled
      //              OCDE       =    0 : Option Command disabled
      //              ADE  [3:0] = 1111 : ADR[31:0] output
      //              OPDE [3:0] = 0000 : option data disabled
      // PRQA S 4461 2 // 4461_UL_Warning
      // PRQA S 0303 3 //0303_SFR_32
      SFR32( RPC_DRDMCR   ) = 0x00020000uL | (sPAL_Qsf.DummyCyclesEE-1uL);
      // Data read dummy cycles   DMDB[1:0] = 2'b10: 4-bit (DIO[3:0]);  DMCYC[4:0] = 5'h07: 8 cycles
      SFR32( RPC_DRDRENR  ) = 0x00000111u;    // Data read DDR enable     HYPE = 3'b000: SPI Flash;  ADDRE = 1: address DDR transfer;  DRDRE = 1: data DDR transfer
    }
    else
    {
      status = (Std_ReturnType)PAL_QSF_ERROR_NOTSUPPORTED;
    }
  }
  if(status == E_OK)
  {
    // PRQA S 4461 1 // 4461_UL_Warning
    v_Qsf_DCacheRegionInvalidate(PAL_QSF_WINDOW_START + (FlashAddr & PAL_QSF_WINDOW_MASK), SizeBytes);
  }

  return(status);
}
// PRQA S 7002 --

static Std_ReturnType u_Qsf_ReadRPCWindow(uint32 FlashAddr, uint8 *RAMptr, uint32 SizeBytes)
// Function: reads data from the QSPI flash (within the RPC window)
// Return values:
// PAL_QSF_OK       OK, data read successfully
// PAL_QSF_ERROR_SIZE   accessing flash outside of bounds
{
  Std_ReturnType status = E_OK;

  if ( SizeBytes != 0u )
  {
    status = u_Qsf_StartRead( FlashAddr, SizeBytes );
    if ( status == E_OK )
      {
      // 2021-03-10; uie23485
      // Summary: Message(3:0326) cast b/w pointer to void type and an integral type
      //          Message(3:0314) Implicit conversion from a pointer to object type to a pointer to void.
      //          Message(3:4461) UL is being converted to narrower unsigned type, uint32 on assignment.
      //          Message(3:1496) [I] Destination and source objects may have incompatible types.
      // Reason:  4461 is okay as uL is uint32, It is ensured that the copying does not overlap Proven in use.
      // PRQA S 0326,0314,1496,4461 11
#ifdef QSF_PRESBL_USE_CACHE
      /*PRQA S 2987 1*/ /* uic80947: 04.03.2021 The function call isn't redundant, it's necessary to synch the data in memory */
      v_Qsf_SyncMemories();
      (void)Qsf_memcpy((void*)RAMptr, (void*)(PAL_QSF_WINDOW_START + (FlashAddr & PAL_QSF_WINDOW_MASK)), SizeBytes);
      v_Cache_WbInvAll();
      /*PRQA S 2987 1*/ /* uic80947: 04.03.2021 The function call isn't redundant, it's necessary to synch the data in memory */
      v_Qsf_SyncMemories();
#else
      #ifndef SIM_SCT_CFG //Not Courage
      (void)Qsf_memcpy((void*)RAMptr, (void*)(PAL_QSF_WINDOW_START + (FlashAddr & PAL_QSF_WINDOW_MASK)), SizeBytes);
      #endif //SIM_SCT_CFG
#endif
      }
  }
  return( status );
}

Std_ReturnType PAL_u_QsfRead(uint32 FlashAddr, uint8 *RAMptr, uint32 SizeBytes)
// Function: reads data from the QSPI flash
// Return values:
// PAL_QSF_OK       OK, data read successfully
// PAL_QSF_ERROR_SIZE   accessing flash outside of bounds
{
  Std_ReturnType status = E_OK;
  uint8 *RAMptr8;
  uint32 FlashAddrEnd;
  uint32 SizeBytesRead;

  if(SizeBytes != 0u)
  {
    RAMptr8 = RAMptr;
    while ( SizeBytes != 0u)
    {
      // check for window boundary crossing
      FlashAddrEnd = FlashAddr + SizeBytes - 1u;
      if ( (FlashAddr & ~PAL_QSF_WINDOW_MASK) != (FlashAddrEnd & ~PAL_QSF_WINDOW_MASK) )
      {
        // PRQA S 4461 1 // 4461_UL_Warning
        FlashAddrEnd = FlashAddr | PAL_QSF_WINDOW_MASK;
      }
      SizeBytesRead = FlashAddrEnd - FlashAddr + 1u;

      // window read

      status = u_Qsf_ReadRPCWindow ( FlashAddr, RAMptr8, SizeBytesRead );

      if ( status == E_OK )
      {
        // prepare for next window
        SizeBytes -= SizeBytesRead;
        FlashAddr += SizeBytesRead;
        // PRQA S 0488 3 // 2017-10-25; uidv7790
        // summary: Msg(3:0488) Performing pointer arithmetic.
        // reason: yes, increase the pointer to uint8
        RAMptr8   += SizeBytesRead;
      }
      else
      {
        break;
      }
    }
  }

  return( status );
}

static Std_ReturnType u_Qsf_VerifyRPCWindow(uint32 FlashAddr, uint8 *RAMptr, uint32 SizeBytes)
// Function: reads data from the QSPI flash and verifies it to provided data (within the RPC window)
// Return values:
// PAL_QSF_OK       OK, data verified successfully (matches)
// PAL_QSF_ERROR_VERIFY data mismatch found during verify
// PAL_QSF_ERROR_SIZE   accessing flash outside of bounds
{
  Std_ReturnType status = E_OK;
  uint8 *RAMptr8;
  uint32 *RAMptr32;
  uint8 *Flashptr8;
  uint32 *Flashptr32;

  if ( SizeBytes != 0u )
  {
    status = (Std_ReturnType)u_Qsf_StartRead( FlashAddr, SizeBytes );
    if ( status == E_OK )
    {
      // PRQA S 0306 1 # date: <2023-11-22>, reviewer: uif85752, reason: Cast needed due to the generic interface. No risk. CCB Jira-ID: ARS540DP-30215
      if ( ((uint32)RAMptr & 3u) != 0u)
      {
        status = (Std_ReturnType)PAL_QSF_ERROR_ALIGN;
      }
    }

    if ( status == E_OK )
    {
      // PRQA S 0306 5 # date: <2023-11-22>, reviewer: uif85752, reason: Cast needed due to the generic interface. No risk. CCB Jira-ID: ARS540DP-30215
      // PRQA S 3305,0310 3 // 2017-10-25; uidv7790
      // summary: Msg(3:3305) Pointer cast to stricter alignment.
      // reason: Allowed due to previous alignment check.
      RAMptr32   = (uint32*)RAMptr;
      Flashptr32 = (uint32 *)(PAL_QSF_WINDOW_START + FlashAddr);
      while ( SizeBytes >= 4u )
      {
        if ( *RAMptr32 != *Flashptr32 )
        {
          status = (Std_ReturnType)PAL_QSF_ERROR_VERIFY;
          break;    // comment this out to prevent reverse-engineering by time measurement
        }
        // PRQA S 0489 4 // 2017-10-25; uidv7790
        // summary: Msg(3:0489) The integer value 1 is being added or subtracted from a pointer.
        // reason: intended and accepted
        RAMptr32++;
        Flashptr32++;
        SizeBytes -= 4u;
      }

      RAMptr8   = (uint8*)RAMptr32;
      Flashptr8 = (uint8*)Flashptr32;
      while ( SizeBytes != 0u)
      {
        if ( *RAMptr8 != *Flashptr8 )
        {
          status = (Std_ReturnType)PAL_QSF_ERROR_VERIFY;
          break;    // comment this out to prevent reverse-engineering by time measurement
        }
        // PRQA S 0489 4 // 2017-10-25; uidv7790
        // summary: Msg(3:0489) The integer value 1 is being added or subtracted from a pointer.
        // reason: intended and accepted
        RAMptr8++;
        Flashptr8++;
        SizeBytes--;
      }
    }
  }

  return( status );
}

Std_ReturnType PAL_u_QsfVerify(uint32 FlashAddr, uint8 *RAMptr, uint32 SizeBytes)
// Function: reads data from the QSPI flash and verifies it to provided data
// Return values:
// PAL_QSF_OK       OK, data verified successfully (matches)
// PAL_QSF_ERROR_VERIFY data mismatch found during verify
// PAL_QSF_ERROR_SIZE   accessing flash outside of bounds
{
  Std_ReturnType status = E_OK;
  uint8 *RAMptr8;
  uint32 FlashAddrEnd;
  uint32    SizeBytesRead;

  if ( SizeBytes != 0u)
  {
    RAMptr8 = RAMptr;
    while ( SizeBytes != 0u )
    {
      // check for window boundary crossing
      FlashAddrEnd = FlashAddr + SizeBytes - 1u;
      if ( (FlashAddr & ~PAL_QSF_WINDOW_MASK) != (FlashAddrEnd & ~PAL_QSF_WINDOW_MASK) )
      {
        // PRQA S 4461 1 // 4461_UL_Warning
        FlashAddrEnd = FlashAddr | PAL_QSF_WINDOW_MASK;
      }
      SizeBytesRead = FlashAddrEnd - FlashAddr + 1u;

      // window verify
      status = u_Qsf_VerifyRPCWindow ( FlashAddr, RAMptr8, SizeBytesRead );
      if ( status == E_OK )
      {
        // prepare for next window
        SizeBytes -= SizeBytesRead;
        FlashAddr += SizeBytesRead;
        // PRQA S 0488 3 // 2017-10-25; uidv7790
        // summary: Msg(3:0488) Performing pointer arithmetic.
        // reason: yes, increase the pointer to uint8
        RAMptr8   += SizeBytesRead;
      }
      else
      {
        break;
      }
    }
  }

  return(status);
}

static Std_ReturnType u_Qsf_BlankCheckRPCWindow(uint32 FlashAddr, uint32 SizeBytes)
// Function: checks if an area of the QSPI flash is blank (within the RPC window)
// Return values:
// PAL_QSF_OK       OK, area is empty
// PAL_QSF_ERROR_ERASE  area is not empty
// PAL_QSF_ERROR_SIZE   accessing flash outside of bounds
{
  Std_ReturnType status = E_OK;
  uint8 *Flashptr8;
  uint32    *Flashptr32;

  if ( SizeBytes != 0u )
  {
    status = u_Qsf_StartRead( FlashAddr, SizeBytes );
    if ( status == E_OK )
    {
      // PRQA S 0306 1 # date: <2023-11-22>, reviewer: uif85752, reason: Cast needed due to the generic interface. No risk. CCB Jira-ID: ARS540DP-30215
      Flashptr32 = (uint32 *)(PAL_QSF_WINDOW_START + FlashAddr);
      while ( SizeBytes >= 4u )
      {
        if ( *Flashptr32 != 0xFFFFFFFFuL )
        {
          status = (Std_ReturnType)PAL_QSF_ERROR_ERASE;
          break;    // comment this out to prevent reverse-engineering by time measurement
        }
        // PRQA S 0489 3 // 2017-10-25; uidv7790
        // summary: Msg(3:0489) The integer value 1 is being added or subtracted from a pointer.
        // reason: intended and accepted
        Flashptr32++;
        SizeBytes -= 4u;
      }

      Flashptr8 = (uint8*)Flashptr32;
      while ( SizeBytes != 0u )
      {
        if ( *Flashptr8 != 0xFFu )
        {
          status = (Std_ReturnType)PAL_QSF_ERROR_ERASE;
          break;    // comment this out to prevent reverse-engineering by time measurement
        }
        // PRQA S 0489 3 // 2017-10-25; uidv7790
        // summary: Msg(3:0489) The integer value 1 is being added or subtracted from a pointer.
        // reason: intended and accepted
        Flashptr8++;
        SizeBytes--;
      }
    }
  }
  return( status );
}

Std_ReturnType PAL_u_QsfBlankCheck(uint32 FlashAddr, uint32 SizeBytes)
// Function: checks if an area of the QSPI flash is blank
// Return values:
// PAL_QSF_OK       OK, area is empty
// PAL_QSF_ERROR_TIMEOUT    operation timed out
// PAL_QSF_ERROR_ERASE  area is not empty
// PAL_QSF_ERROR_SIZE   accessing flash outside of bounds
{
  Std_ReturnType status = E_OK;
  uint32 FlashAddrEnd;
  uint32 SizeBytesRead;

  if ( SizeBytes != 0u )
  {
    while ( SizeBytes != 0u )
    {
      // check for window boundary crossing
      FlashAddrEnd = FlashAddr + SizeBytes - 1u;
      if ( (FlashAddr & ~PAL_QSF_WINDOW_MASK) != (FlashAddrEnd & ~PAL_QSF_WINDOW_MASK) )
      {
        // PRQA S 4461 1 // 4461_UL_Warning
        FlashAddrEnd = FlashAddr | PAL_QSF_WINDOW_MASK;
      }
      SizeBytesRead = FlashAddrEnd - FlashAddr + 1u;

      // window black check
      status = u_Qsf_BlankCheckRPCWindow ( FlashAddr, SizeBytesRead );
      if ( status == E_OK )
      {
        // prepare for next window
        SizeBytes -= SizeBytesRead;
        FlashAddr += SizeBytesRead;
      }
      else
      {
        break;
      }
    }
  }

  return(status);
}

Std_ReturnType PAL_u_QsfEraseSector(uint32 FlashAddr)
// Function: initiates erasing of one sector of the QSPI flash
// Return values:
// PAL_QSF_OK       OK, sector erased
// PAL_QSF_ERROR_TIMEOUT    operation timed out
// PAL_QSF_ERROR_ALIGN  start address is not aligned with sector
// PAL_QSF_ERROR_SIZE   accessing flash outside of bounds
{
  Std_ReturnType status = E_OK;
  uint32 FlashAddr_res = (FlashAddr & SECTORMASK);
  uint32 DeviceSize_res = sPAL_Qsf.DeviceSize;
  if ( FlashAddr_res != 0u)
  {
    status = (Std_ReturnType)PAL_QSF_ERROR_ALIGN;
  }
  else if ( FlashAddr >= DeviceSize_res )
  {
    status = (Std_ReturnType)PAL_QSF_ERROR_SIZE;
  }
  else
  {
    status = u_Qsf_WriteEnable();
  }

  if ( status == E_OK )
  {    		// PRQA S 0303 2 //0303_SFR_32
    SFR32( RPC_PHYCNT ) = 0x80030260u;     // PHY control          PHYMEM[1:0] = 2'b00: Serial Flash SDR;  CAL = 1: PHY calibration
    SFR32( RPC_CMNCR  ) = 0x81FFF300u;     // Common control       BSZ[1:0] = 2'b00: Serial Flash (1ch);  MD = 1: Manual mode

    switch(sPAL_Qsf.SectorSize)
    {
      case 4u*KiB:
    		// PRQA S 0303 1 //0303_SFR_32
        SFR32( RPC_SMCMR  ) = 0x00210000u; // Manual mode command     CMD[7:0] = 8'h21 = 4SE: Sector Erase 4 byte addressing
        break;
      case 32u*KiB:
    		// PRQA S 0303 1 //0303_SFR_32
        SFR32( RPC_SMCMR  ) = 0x005C0000u; // Manual mode command     CMD[7:0] = 8'h5C = 32SE: Sector Erase 4 byte addressing
        break;
      case 64u*KiB:
      default:
    		// PRQA S 0303 1 //0303_SFR_32
        SFR32( RPC_SMCMR  ) = 0x00DC0000u; // Manual mode command     CMD[7:0] = 8'hDC = 64SE: Sector Erase 4 byte addressing
        break;
    }    		// PRQA S 0303 3 //0303_SFR_32
    SFR32( RPC_SMADR  ) = FlashAddr;  // Manual mode address      address
    SFR32( RPC_SMDRENR    ) = 0x00000000u;    // Manual mode DDR enable   HYPE = 3'b000: SPI Flash;  ADDRE = 0: address SDR transfer;  SPIDRE = 0: data SDR transfer
    SFR32( RPC_SMENR  ) = 0x00004F00u; // Manual mode enable   CDB  [1:0] = 2'b00: 1-bit command (MOSI/MISO)
    //        ADB  [1:0] = 2'b00: 1-bit address (MOSI/MISO)
    //        SPIDB[1:0] = 2'b00: 1-bit data    (MOSI/MISO)
    //        DME        =    0 : dummy cycle disabled
    //        CDE        =    1 : Command enabled
    //        OCDE       =    0 : Option Command disabled
    //        ADE  [3:0] = 1111 : ADR[31:0] output
    //        OPDE [3:0] = 0000 : option data disabled
    //        SPIDE[3:0] = 0000 : data disabled
    		// PRQA S 0303 1 //0303_SFR_32
    SFR32( RPC_SMCR   ) = 0x00000001u;    // Manual mode control      SPIRE = 0: Data read disabled;  SPIWE = 0: Data write disabled;  SPIE = 1: SPI transfer start
    status = u_Qsf_WaitTxEnd();
  }

  return status;
}

Std_ReturnType PAL_u_QsfEraseSector_4K(uint32 FlashAddr)
// Function: initiates erasing of one sector of the QSPI flash
// Return values:
// PAL_QSF_OK       OK, sector erased
// PAL_QSF_ERROR_TIMEOUT    operation timed out
// PAL_QSF_ERROR_ALIGN  start address is not aligned with sector
// PAL_QSF_ERROR_SIZE   accessing flash outside of bounds
{
  Std_ReturnType status = E_OK;
  uint32 DeviceSize_res = sPAL_Qsf.DeviceSize;
  uint32 FlashAddr_res = (FlashAddr & SECTORMASK);
  if (FlashAddr_res!= 0u )
  {
    status = (Std_ReturnType)PAL_QSF_ERROR_ALIGN;
  }

  else if ( FlashAddr >= DeviceSize_res)
  {
    status = (Std_ReturnType)PAL_QSF_ERROR_SIZE;
  }
  else
  {
    status = u_Qsf_WriteEnable();
    if ( status == E_OK )
    {    		// PRQA S 0303 6 //0303_SFR_32
      SFR32( RPC_PHYCNT ) = 0x80030260u;      // PHY control          PHYMEM[1:0] = 2'b00: Serial Flash SDR;  CAL = 1: PHY calibration
      SFR32( RPC_CMNCR  ) = 0x81FFF300u;      // Common control       BSZ[1:0] = 2'b00: Serial Flash (1ch);  MD = 1: Manual mode
      SFR32( RPC_SMCMR  ) = 0x00210000u;      // Manual mode command     CMD[7:0] = 8'h21 = 4SE: SubSector(4KB) Erase 4 byte addressing
      SFR32( RPC_SMADR  ) = FlashAddr;        // Manual mode address      address
      SFR32( RPC_SMDRENR    ) = 0x00000000u;  // Manual mode DDR enable   HYPE = 3'b000: SPI Flash;  ADDRE = 0: address SDR transfer;  SPIDRE = 0: data SDR transfer
      SFR32( RPC_SMENR  ) = 0x00004F00u;      // Manual mode enable   CDB  [1:0] = 2'b00: 1-bit command (MOSI/MISO)
      //        ADB  [1:0] = 2'b00: 1-bit address (MOSI/MISO)
      //        SPIDB[1:0] = 2'b00: 1-bit data    (MOSI/MISO)
      //        DME        =    0 : dummy cycle disabled
      //        CDE        =    1 : Command enabled
      //        OCDE       =    0 : Option Command disabled
      //        ADE  [3:0] = 1111 : ADR[31:0] output
      //        OPDE [3:0] = 0000 : option data disabled
      //        SPIDE[3:0] = 0000 : data disabled
    		// PRQA S 0303 1 //0303_SFR_32
      SFR32( RPC_SMCR   ) = 0x00000001u;    // Manual mode control      SPIRE = 0: Data read disabled;  SPIWE = 0: Data write disabled;  SPIE = 1: SPI transfer start
      status = u_Qsf_WaitTxEnd();
    }
  }

  return(status);
}

// 2021-03-10; uie23485
// Summary: Message(3:7002) HIS metrics check , no of cyclic paths
// Reason:  Not critical, too many test cases
// PRQA S 7002 ++
Std_ReturnType PAL_u_QsfProgramPage(uint32 FlashAddr, uint8 *RAMptr, uint32 SizeBytes)
// Function: writes (up to/within) one page of data to the QSPI flash, but does not wait for the write to complete
// Return values:
// PAL_QSF_OK       OK, data written successfully
// PAL_QSF_ERROR_TIMEOUT    operation timed out
// PAL_QSF_ERROR_ALIGN  programming range crosses page boundary, base address is not 16-byte aligned, or size is no multiple of 16
// PAL_QSF_ERROR_SIZE   accessing flash outside of bounds; or SizeBytes is larger than PageSize
{
  Std_ReturnType status = E_OK;
  uint32 FlashAddrEnd;
  uint32 *RAMptr32;
    		// PRQA S 0303 1 //0303_SFR_32
  volatile uint32 *BUFptr32 = (volatile uint32 *)RPC_WRBUF;
  volatile uint16 *RAMptr16;
  volatile uint8 *RAMptr8;
	uint32 DeviceSize_res = sPAL_Qsf.DeviceSize;
	uint32 PageSize_res = sPAL_Qsf.PageSize;
  if (    ( SizeBytes > RPC_WRBUF_SIZE ) // size of RPC buffer
      || (SizeBytes > PageSize_res)        // limit of device
      || ((FlashAddr + SizeBytes) > DeviceSize_res) )
  {
    status = (Std_ReturnType)PAL_QSF_ERROR_SIZE;
  }

  if( status == E_OK)
  {
    // 2021-03-10; uie23485
    // Summary: Message(3:0404)  More than one read access to volatile objects between sequence points.
    // Reason:  This is permitted
    // PRQA S 0404 2
    FlashAddrEnd = FlashAddr + SizeBytes - 1u;
	uint32 pagemask_res = (FlashAddr & ~PAGEMASK);
	uint32 flashaddr_res = (FlashAddrEnd & ~PAGEMASK);
    if (   ((FlashAddr & 15u) != 0u)                  // suggested minimum write size (for S25FS*)
        || ((SizeBytes & 15u) != 0u)                  // suggested minimum write size (for S25FS*)
        //    || (SizeBytes & 3)                      // transferred to device in 4-byte units
        || (pagemask_res != flashaddr_res ) )  // page boundary crossing
    {
      status = (Std_ReturnType)PAL_QSF_ERROR_ALIGN;
    }
  }

  if( status == E_OK)
  {
    status = u_Qsf_WriteEnable();
  }

  if( status == E_OK)
  {
    // switch RPC buffer (from read cache) to write buffer
    		// PRQA S 0303 2 //0303_SFR_32
    SFR32( RPC_DRCR   ) = 0x011F0300u;    // Data read control        RCF = 1: flush read cache;  negate SSL
    SFR32( RPC_PHYCNT ) = 0x80030274u;    // PHY control          PHYMEM[1:0] = 2'b00: Serial Flash SDR;  CAL = 1: PHY calibration;  WBUF/WBUF2 = 1: Write Buffer Enable

     // PRQA S 0306 2 # date: <2023-11-22>, reviewer: uif85752, reason: Cast needed due to the generic interface. No risk. CCB Jira-ID: ARS540DP-30215
    // fill write buffer with data
    switch ((uint32)RAMptr & 3u)
    {
    case 0u:
      // PRQA S 3305,0310,0562 5 // 2017-10-25; uidv7790
      // summary: Msg(3:3305) Pointer cast to stricter alignment.
      //          Msg(3:3310) Casting to different object pointer type.
      //          Msg(3:0562) Right hand operator points to a more heavily qualified type
      // reason:  Allowed due to previous alignment check.
      RAMptr32 = (volatile uint32*)RAMptr;
      while ( SizeBytes != 0u )
      {
        *BUFptr32 = *RAMptr32;
        // PRQA S 0489 4 // 2017-10-25; uidv7790
        // summary: Msg(3:0489) The integer value 1 is being added or subtracted from a pointer.
        // reason: intended and accepted
        BUFptr32++;
        RAMptr32++;
        SizeBytes -= 4u;
      }
      break;
    case 2u:
      // PRQA S 3305, 0310 5
      // 2017-10-25; uidv7790
      // summary: Msg(3:3305) Pointer cast to stricter alignment.
      //          Msg(3:0310) Casting to different object pointer type.
      // reason: Allowed due to previous alignment check.
      RAMptr16 = (volatile uint16*)RAMptr;
      while ( SizeBytes != 0u )
      {
        // 2021-03-10; uie23485
        // Summary: Message(3:0404)  More than one read access to volatile objects between sequence points.
        // Reason:  This is permitted
        // PRQA S 0404 1
        *BUFptr32 = ((uint32)RAMptr16[1u] << 16u) |
                      RAMptr16[0u];
        // PRQA S 0489 4 // 2017-10-25; uidv7790
        // summary: Msg(3:0489) The integer value 1 is being added or subtracted from a pointer.
        // reason: intended and accepted
        BUFptr32++;
        RAMptr16 += 2u;
        SizeBytes -= 4u;
      }
      break;
    default:
      RAMptr8 = (volatile uint8*)RAMptr;
      while ( SizeBytes != 0u )
      {
        // 2021-03-10; uie23485
        // Summary: Message(3:0404)  More than one read access to volatile objects between sequence points.
        // Reason:  This is permitted
        // PRQA S 0404 1
        *BUFptr32 = ((uint32)RAMptr8[3u] << 24u) |
            ((uint32)RAMptr8[2u] << 16u) |
            ((uint32)RAMptr8[1u] << 8u) |
            RAMptr8[0u];
        // PRQA S 0489 4 // 2017-10-25; uidv7790
        // summary: Msg(3:0489) The integer value 1 is being added or subtracted from a pointer.
        // reason: intended and accepted
        BUFptr32++;
        RAMptr8 += 4u;
        SizeBytes -= 4u;
      }
      break;
    }
    		// PRQA S 0303 5 //0303_SFR_32
    SFR32( RPC_CMNCR  ) = 0x81FFF300u;        // Common control       BSZ[1:0] = 2'b00: Serial Flash (1ch);  MD = 1: Manual mode
    SFR32( RPC_SMCMR  ) = 0x00120000u;        // Manual mode command     CMD[7:0] = 8'h12 = 4PP = Page program 4 byte addressing
    SFR32( RPC_SMADR  ) = FlashAddr;          // Manual mode address      address
    SFR32( RPC_SMDRENR    ) = 0x00000000u;    // Manual mode DDR enable   HYPE = 3'b000: SPI Flash;  ADDRE = 0: address SDR transfer;  SPIDRE = 0: data SDR transfer
    SFR32( RPC_SMENR  ) = 0x00004F0Fu;        // Manual mode enable   CDB  [1:0] = 2'b00: 1-bit command (MOSI/MISO)
    //        ADB  [1:0] = 2'b00: 1-bit address (MOSI/MISO)
    //        SPIDB[1:0] = 2'b00: 1-bit data    (MOSI/MISO)
    //        DME        =    0 : dummy cycle disabled
    //        CDE        =    1 : Command enabled
    //        OCDE       =    0 : Option Command disabled
    //        ADE  [3:0] = 1111 : ADR[31:0] output
    //        OPDE [3:0] = 0000 : option data disabled
    //        SPIDE[3:0] = 1111 : 32-bit data
    		// PRQA S 0303 1 //0303_SFR_32
    SFR32( RPC_SMCR   ) = 0x00000003u;    // Manual mode control      SPIRE = 0: Data read disabled;  SPIWE = 1: Data write enabled;  SPIE = 1: SPI transfer start

    status = u_Qsf_WaitTxEnd();
    if ( status == E_OK )
    {
      // switch RPC buffer back to read cache
    		// PRQA S 0303 2 //0303_SFR_32
      SFR32( RPC_PHYCNT ) = 0x00030270u;    // PHY control          PHYMEM[1:0] = 2'b00: Serial Flash SDR;  CAL = 0: no PHY calibration;  WBUF = 0: Write Buffer disable
      SFR32( RPC_DRCR   ) = 0x011F0300u;    // Data read control        RCF = 1: flush read cache;  negate SSL
    }

  }
  return( status );
}

// PRQA S 7002 --
// PRQA S 3006 3
// Date: 2023-11-22; Reviewer : uif85752; 
// Reason: code contains a mixture of in-line assembler and C due to design
static void v_Qsf_DCacheRegionInvalidate(const uint32 u_Start, const uint32 u_Len)
{
#if (CPU_TYPE == 32)
  uint32 u_Address = u_Start & CACHE_LINE_MASK;
  uint32 u_SCTLR;
  /* Read u_SCTLR (System Control Register) */
  // PRQA S 3112 3 // 2019-10-24; uidv7790
  // summary: Msg(3:3112) This statement has no side-effect - it can be removed.
  // reason: There is __asm code for reading 'Control Register' which is not recognized by static checker.
  #ifndef SIM_SCT_CFG
  __asm volatile("MRC p15, 0, %[result], c1, c0, 0" : [result] "=r" (u_SCTLR));
  #endif //SIM_SCT_CFG
  // 2021-03-11; uie23485
  // Summary: Message(3:2961) Using value of uninitialized automatic object u_SCTLR
  // Reason:  This is initialized/read using asm commands, not recognized by static checker
  // PRQA S 2961 1
  if (0u != (u_SCTLR & 4u) )
  {
    while (u_Start + u_Len > u_Address)
    {
      /* invalidate cache line by MVA */
      // PRQA S 3112 3 // 2019-10-24; uidv7790
      // summary: Msg(3:3112) This statement has no side-effect - it can be removed.
      // reason: There is __asm code which is not recognized by static checker.
     #ifndef SIM_SCT_CFG
      __asm volatile ("mcr p15, #0, %[input_u_Address], c7, c6, #1"
          : // empty output operand list
          : [input_u_Address] "r" (u_Address)
      );
     #endif //SIM_SCT_CFG

      u_Address += CACHE_LINE_LEN;
    }
    // PRQA S 3112 3 // 2017-10-25; uidv7790
    // summary: Msg(3:3112) This statement has no side-effect - it can be removed.
    // reason: There is __asm code which is not recognized by static checker.
    pDSB();
  }
#endif // (CPU_TYPE == 32)
}

uint32 PAL_u_QsfGetPageSize(void)
{
  return sPAL_Qsf.PageSize;
}

uint32 PAL_u_QsfGetSectorSize(void)
{
  return sPAL_Qsf.SectorSize;
}

uint32 PAL_u_QsfGetWordWriteTimeout_us(void)
{
  return sPAL_Qsf.TIMEOUT_WriteWord_us;
}

uint32 PAL_u_QsfGetSectorEraseTimeout_ms(void)
{
  return sPAL_Qsf.TIMEOUT_EraseSector_ms;
}

// 2021-03-10; uie23485
// Summary: Message(3:7002) HIS metrics check , no of cyclic paths
// Reason:  Not critical, too many test cases
// PRQA S 7002 ++
static void PAL_v_QsfDDRCalibration(void)
{

  uint8           u_statusArray[8]    = {0xFu, 0xFu, 0xFu, 0xFu, 0xFu, 0xFu, 0xFu, 0xFu};
  uint8           u_STRTIM            = 0x7u;
  uint8           u_strtimWindowsEnd  = 0xFu;
  uint8           u_strtimWindowStart = 0xFu;
  uint32          u_readData          = 0u;
  const uint32    u_FlashAddr         = PAL_QSF_CALIBRATION_ADDR;
  volatile uint32 u_TempRegister;

  uint32 u_TimeGetCount = SYS_TimeGetCount();
  if((0u == u_lastCalibrationTime_ticks) || ((u_TimeGetCount - u_lastCalibrationTime_ticks) > CALIBRATION_INTERVAL_TICKS))
  {
    // PRQA S 0303 8 //0303_SFR_32
    /*** Initial setting External Address Space Read Mode ***/
    SFR32( RPC_DREAR    ) = 0x00000001u;

    SFR32( RPC_OFFSET1  ) = 0x21511144u;    // PHY timing offset        set PHY read mode to DDR
    SFR32( RPC_CMNCR    ) = 0x01557300u;    // Common control       BSZ[1:0] = 2'b00: Serial Flash (1ch);  MD = 0: External address space mode
    SFR32( RPC_DRCR     ) = 0x011F0200u;        // Data read control        RBE = 0: read bursts disabled
    SFR32( RPC_DRCMR    ) = 0x00EE0000u;    // Data read command        CMD[7:0] = 8'hEE = 4DDRQIOR = 4-byte address DDR Quad I/O Fast Read
    SFR32( RPC_DRENR    ) = 0x0222CF00u;
    // Data read enable     CDB  [1:0] = 2'b00: 1-bit command (MOSI/MISO)
    //              ADB  [1:0] = 2'b10: 4-bit address (DIO[3:0])
    //              SPIDB[1:0] = 2'b10: 4-bit data    (DIO[3:0])
    //              DME        =    1 : dummy cycle enabled
    //              CDE        =    1 : Command enabled
    //              OCDE       =    0 : Option Command disabled
    //              ADE  [3:0] = 1111 : ADR[31:0] output
    //              OPDE [3:0] = 0000 : option data disabled
    // PRQA S 0303 3 //0303_SFR_32
    SFR32( RPC_DRDMCR   ) = 0x00020000u | (uint32)(sPAL_Qsf.DummyCyclesEE-1uL);
    // Data read dummy cycles   DMDB[1:0] = 2'b10: 4-bit (DIO[3:0]);  DMCYC[4:0] = 5'h07: 8 cycles
    SFR32( RPC_DRDRENR  ) = 0x00000111u;    // Data read DDR enable     HYPE = 3'b000: SPI Flash;  ADDRE = 1: address DDR transfer;  DRDRE = 1: data DDR transfer
    /***** Initial setting External Address Space Read Mode end *****/

    // go through all the tap positions and fill in u_statusArray with 0 (match) or 1 (doesn't match)
    for(u_STRTIM = 8u; u_STRTIM >= 1u; u_STRTIM--)
    {
      if(u_STRTIM == 8u) //First flow
      {
        // Note1: When PHYCNT.CAL = 1'b1, PHYCNT.STRTIM must be set to 3'b111
        // Specify DDRCAL = 1 in SW calibration flow
        // No need to change the register value in the 1st flow.
        // 0x800B8261u
        // 31      CAL         = 0b1          : Calibration is executed
        // 19      DDRCAL      = 0b1          : SW calibration for DDR transfer
        // 17-15   STRTIM[2:0] = 0b111        : The delay is smallest
        // 14-5    Reserved    = 0b0000010011 : Read-only
        // 1-0     PHYMEM,     = 0b01         : Serial flash in DDR mode
        // PRQA S 0303 1 //0303_SFR_32
        SFR32( RPC_PHYCNT   ) = 0x800B8261u;
      }
      else // Set different PHYCNT.STRTIM to change Strobe Timing Adjustment
      {
        // Note2: When set PHYCNT.STRTIM, PHYCNT.CAL must be set to 1'b0
        // PRQA S 0303 1 //0303_SFR_32
        u_TempRegister = SFR32( RPC_PHYCNT   );
        // Set STRTIM[2:0] bits 15, 16 and 17 to zero
        u_TempRegister &= ~((uint32)0b111u << 15u);
        // Change only STRTIM[2:0]
        u_TempRegister |= (uint32)(((uint32)u_STRTIM-1u) << 15u);
        // PRQA S 0303 1 //0303_SFR_32
        SFR32( RPC_PHYCNT   ) = u_TempRegister;
      }

      // Read from PAL_QSF_CALIBRATION_ADDR address in DDR mode to compare with expected pattern.
      // PRQA S 4461 1 // 4461_UL_Warning
      v_Qsf_DCacheRegionInvalidate(PAL_QSF_WINDOW_START + (u_FlashAddr & PAL_QSF_WINDOW_MASK), (uint32)sizeof(u_readData));

      // 2023-11-08; uif08910
      // Summary: Message(3:0326) cast b/w pointer to void type and an integral type
      //          Message(3:0314) Implicit conversion from a pointer to object type to a pointer to void.
      //          Message(3:4461) UL is being converted to narrower unsigned type, uint32 on assignment.
      //          Message(3:1496) [I] Destination and source objects may have incompatible types.
      // Reason:  4461 is okay as uL is uint32, It is ensured that the copying does not overlap Proven in use.
      // PRQA S 0326,0314,1496,4461 1
      (void)Qsf_memcpy((void*)&u_readData, (void*)(PAL_QSF_WINDOW_START + (u_FlashAddr & PAL_QSF_WINDOW_MASK)), sizeof(u_readData));

      // Compare the read data.
      if (u_readData == PAL_QSF_CALIBRATION_PATTERN)
      {
        u_statusArray[(u_STRTIM-1u)] = 0u;      // data matching
      }
      else
      {
        u_statusArray[(u_STRTIM-1u)] = 1u;      // data not matching
      }
    }

    // find middle tap position out of all good ones
    for(u_STRTIM = 7u; u_STRTIM >= 1u; u_STRTIM--)
    {
      if((0u == u_statusArray[u_STRTIM]) && (0xFu == u_strtimWindowsEnd))
      {
        u_strtimWindowsEnd = u_STRTIM;
      }
      if((0u == u_statusArray[u_STRTIM]) && ((1u == u_statusArray[u_STRTIM-1u]) ))
      {
        u_strtimWindowStart = u_STRTIM;
      }
    }
    if(0u == u_statusArray[0])
    {
      u_strtimWindowStart = 0u;
    }

    // Right shift by 1 bit is used instead division with 2
    u_calibratedStrtimValue = (u_strtimWindowsEnd + u_strtimWindowStart) >> 1u;  // get middle u_STRTIM tap position

    u_lastCalibrationTime_ticks = SYS_TimeGetCount();                           // get time of last calibration in ticks
  }
  else
  {

  }

}
// PRQA S 7002 --

#ifdef QSF_PRESBL_USE_CACHE

/* uic80947: Taken from ST layer, defined here to avoid dependency to ST layer*/
static void v_Qsf_SyncMemories(void)
{
  #ifndef SIM_SCT_CFG
  __asm volatile("dsb sy");
  __asm volatile("isb sy");
  #endif // SIM_SCT_CFG
}

#endif

#ifdef __cplusplus
}
#endif

// PRQA S 7013 3 // 2017-10-25; uidv7790
// summary: Msg(3:7013) Info: number of executable lines per file.
// reason: accepted, no split of file, please
