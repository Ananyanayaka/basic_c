// Copyright (C) Continental AG
// All Rights Reserved
// COMPONENT: STARTUP_R7
// VERSION:   $Revision: 1.38 $
/// @file     startup_r7.c
/// @brief    This file implements the pre main start up for the cores 1 and 2

//Note:Reference information for 0303_SFR_32 warnings supression
// Date: 2023-11-20, uif85752
// Reason: Memory mapped register needs to be accessed directly via pointer.CCB Jira-ID: ADASQMS-3232
// Msg(4:0303) [I] Cast between a pointer to volatile object and an integral type.
// Msg(4:0306) [I] Cast between a pointer to volatile object and an integral type.

// C-code part of startup code
#include <startup_r7.h>
#include <memory_layout.h>
#include <IuSbl_part.h>
#include <PreSBL_Loading.h>
#include <image_loader.h>
#include <ImageLoader_Cfg.h>
#include <Csm_Cfg.h>
#include <Csm.h>
#include <Crypto_1_HW_TRG_KeyManagement.h>
#include <sys_time.h>
#include <sys_mem.h>
#include <Fls.h>
#include <FS_Reset.h>
#include <PAL_Watchdog.h>
#include <Reg_RCarV3M.h>
#include <PAL_McuDefs.h>
#include <Mk_ARM_exceptionhandling.h>
#include <Std_Types.h>
#include <xpar_Loading.h>
#include <SDF_VUFBL_Standard.h>
#include <PAL_Reset.h>

#include <PMC_DpuStartup.h>
#ifdef __cplusplus
extern "C"
{
#endif

// typical memory mapped register access
// PRQA S 3453 25
// 2020-03-25; uidv7790
// Msg(2:3453) A function could probably be used instead of this function-like macro.
// reason: No, these macros provide values which cannot be done with functions.
#ifndef __COURAGE_TEST_SWITCH__

/// Generic 32bit IO accesses of a certain register given as parameter. \n
/// Parameter a represents register address, return of this 'macro function' is content of desired register.
#ifdef BOARD_REG_32
#undef BOARD_REG_32
#endif
#define BOARD_REG_32(a) (*((volatile uint32*)(a)))

/// Generic 16bit IO accesses of a certain register given as parameter.
/// Parameter a represents register address, return of this 'macro function' is content of desired register.
#define BOARD_REG_16(a) (*((volatile uint16*)(a)))

// Various registers

/// Accesses the CPGWPR value for IO.
#define BOARD_CPGWPR  BOARD_REG_32(0xe6150900u)

/// Accesses the CPGSD0CKCR value for IO.
#define BOARD_CPGSD0CKCR  BOARD_REG_32(0xE6150074u)

/// Accesses the CPGSMSTPCR9 value for IO.
#define BOARD_CPGSMSTPCR9  BOARD_REG_32(0xE6150994u)

/// Accesses the CPGMSTPSR9 value for IO.
#define BOARD_CPGMSTPSR9  BOARD_REG_32(0xE61509A4u)

/// Accesses the CPGSMSTPCR3 value for IO.
#define BOARD_CPGSMSTPCR3  BOARD_REG_32(0xE615013Cu)

/// Accesses the CPGMSTPSR3 value for IO.
#define BOARD_CPGMSTPSR3  BOARD_REG_32(0xE6150048u)

/// Accesses the CPGFRQCRB value for IO.
#define BOARD_CPGFRQCRB  BOARD_REG_32(0xE6150004u)

/// Accesses the CPGCSI0CKCR value for IO.
#define BOARD_CPGCSI0CKCR  BOARD_REG_32(0xE615000Cu)

/// Accesses the SITSCR value for IO.
#define BOARD_SITSCR  BOARD_REG_16(0xE6EA0020u)

/// Accesses the I2C3ICCCR value for IO.
#define BOARD_I2C3ICCCR  BOARD_REG_32(0xE66D0018u)

/// Accesses the I2C4ICCCR value for IO.
#define BOARD_I2C4ICCCR  BOARD_REG_32(0xE66D8018u)

/// Accesses the I2C5ICCCR value for IO.
#define BOARD_I2C5ICCCR  BOARD_REG_32(0xE66E0018u)

/// Accesses the RMSTPCR1 value for IO.
#define BOARD_RMSTPCR1  BOARD_REG_32(0xe6150114u)

/// Accesses the BOARD_ARM_GLOBAL_TIMER_CTRL value for IO.
#define BOARD_ARM_GLOBAL_TIMER_CTRL  BOARD_REG_32(0xf0000208u)

/// CRC HW Clock Channel Selection Register 1
#define SMSTPCR3  *((volatile uint32*)0xE6150C2Cu)

/// CRC HW Clock Channel Selection Register 2
#define RMSTPCR3  *((volatile uint32*)0xE615011Cu)

/// CRC HW Configuration Register for Channel 0
#define DCRA0CTL  *((volatile uint32*)0xE6F00020u)

/// CRC HW Configuration Register for Channel 1
#define DCRA1CTL  *((volatile uint32*)0xE6F10020u)

/// Accesses the BOARD_RMSTPCR3 value for IO.
#define BOARD_RMSTPCR3 BOARD_REG_32(0xE615011Cu)  //ok
#endif

/// RMSTPCR1_TMU4 MASK. This is set to 0x00200000U.
#define BOARD_RMSTPCR1_TMU4 0x00200000U

/// BOARD_RMSTPCR1_TMU3 MASK. This is set to 0x00400000U.
#define BOARD_RMSTPCR1_TMU3 0x00400000U

/// BOARD_RMSTPCR1_TMU2 MASK. This is set to 0x00800000U.
#define BOARD_RMSTPCR1_TMU2 0x00800000U

/// BOARD_RMSTPCR1_TMU1 MASK. This is set to 0x01000000U.
#define BOARD_RMSTPCR1_TMU1 0x01000000U

/// BOARD_RMSTPCR1_TMU0 MASK. This is set to 0x02000000U.
#define BOARD_RMSTPCR1_TMU0 0x02000000U

/// BOARD_RMSTPCR1_TMUx MASK.\n
/// This is set to a combination of following flags:\n
/// BOARD_RMSTPCR1_TMU4 | BOARD_RMSTPCR1_TMU3 | BOARD_RMSTPCR1_TMU2 | BOARD_RMSTPCR1_TMU1 | BOARD_RMSTPCR1_TMU0 \n
#define BOARD_RMSTPCR1_TMUx (BOARD_RMSTPCR1_TMU4|BOARD_RMSTPCR1_TMU3|BOARD_RMSTPCR1_TMU2|BOARD_RMSTPCR1_TMU1|BOARD_RMSTPCR1_TMU0)

/// BOARD_CPGSD0CKCR_INIT_VAL MASK. This is set to 0x0000F046u.
#define BOARD_CPGSD0CKCR_INIT_VAL  0x0000F046u

/// BOARD_I2C4ICCCR initialization value. This is set to 0x00000026u.
#define BOARD_I2C4ICCCR_INIT_VAL   0x00000026u

/// BOARD_CPGFRQCRB initialization value. This is set to 0x00320235.
#define BOARD_CPGFRQCRB_INIT_VAL  0x00320235

/// BOARD_CPGCSI0CKCR initialization value. This is set to 0x0000011F.
#define BOARD_CPGCSI0CKCR_INIT_VAL  0x0000011F

/// BOARD_SITSCR initialization value. This is set to 0x1601.
#define BOARD_SITSCR_INIT_VAL  0x1601

/// BOARD_I2C3ICCCR initialization value. This is set to 0x00000026.
#define BOARD_I2C3ICCCR_INIT_VAL  0x00000026

/// BOARD_I2C5ICCCR  initialization value. This is set to 0x00000026.
#define BOARD_I2C5ICCCR_INIT_VAL  0x00000026

/// BIT 04 (0x10)
#define BIT04 (1uL << 4u)

/// BIT 08 (0x100)
#define BIT8  (1uL << 8u)

/// BIT 14 (0x4000)
#define BIT14 (1uL << 14u)

/// BIT 15 (0x8000)
#define BIT15 (1uL << 15u)

/// BIT 19 (0x0008 0000)
#define BIT19 (1uL << 19u)

/// BIT 27 (0x0800 0000)
#define BIT27 (1uL << 27u)

/// BIT 28 (0x1000 0000)
#define BIT28 (1uL << 28u)

/// BIT 31 (0x8000 0000)
#define BIT31 (1uL << 31u)

/// TIMEOUT is set to 100ms.
#define TIMEOUT100 (100u)

/// Mask for BOARD_RMSTPCR3_CMT1 is set to 0x00000004UL.
#define BOARD_RMSTPCR3_CMT1_MASK (0x00000004UL)

/// PPAR_SDF_START is assigned to SDF_PPAR_START.
#define PPAR_SDF_START       SDF_PPAR_START

/// PPAR_SDF_LEN is set to 0x5000uL.
#define PPAR_SDF_LEN         0x5000uL // length of PPAR is 20kB which is fixed

/// Position of bit in GPIO port 5 register which is used for control of LED0
#define LED_0_MASK 0x00000040uL

/// Position of bit in GPIO port 5 register which is used for control of LED1
#define LED_1_MASK 0x00000080uL

/// Position of bit in GPIO port 5 register which is used for control of LED2
#define LED_2_MASK 0x00000100uL

/// Initial value for RMSTPCR3 to Select Channel 0
#define RMSTPCR3_CRC0_VALUE 0xFFFFFF7Fu

/// Initial value for SMSTPCR3 to Select Channel 0
#define SMSTPCR3_CRC0_VALUE 0xFFFFFF7Fu

/// RC32 word (32bit) input MASK
#define CRC32_CTRL_REG_WORD (uint8)(0x00u)

/// CRC16 byte (8bit) input MASK
#define CRC16_CTRL_REG_BYTE (uint8)(0x05u)


// memory allocation of 20kB for PPAR in .ppar_bu section
// PRQA S 3218 9
// 2019-1-17; uidw1725
// Msg(3:3218) File scope static, 'u_PPAR_SRAM', is only accessed in one function.
// reason: reserve global PPAR memory area
#ifndef __COURAGE_TEST_SWITCH__

/// Memory allocation for PPAR.
static uint8 u_PPAR_SRAM[PPAR_SDF_LEN] __attribute__((section(".ppar_bu")));
#endif

// PRQA S 1514 5
// 2021-1-6; uie38801
// Message(2:1514) The object 'v_CheckPoint' is only referenced by function 'u_STARTUP_CheckPoint', in the translation unit where it is defined
// Reason: Component design
volatile uint32 u_STARTUP_CheckPoint;

static uint32 u_DpuSblEntry = 0U;
/// Reference Source file : startup_r7.c
///
/// @pre None
/// @post None
/// @param (const t_STARTUP_CheckPoint u_CheckPoint)
/// @return static void
/// @globals No global variables used
/// @InOutCorrelation None
/// @callsequence To be updated.
/// @InOutCorrelation Frequency at which the runnable is called: N/A
/// @startuml
/// (*) --> Call sequence for v_CheckPoint()
/// --> " u_STARTUP_CheckPoint is updated with (uint32)u_CheckPoint "
/// --> (*)
/// @enduml
///
/// @testmethod
///    Dynamic unit test method
///    - This function has to be tested for statement and branch coverage.
///    - For detailed unit test description follow the caller graph to the test function below.
///    - Test script: test_startup_r7.c
/// @traceability

static void v_CheckPoint(const t_STARTUP_CheckPoint u_CheckPoint);

/// Reference Source file : startup_r7.c
///
/// @pre None
/// @post None
/// @param None
/// @return static void
/// @globals No global variables used
/// @InOutCorrelation None
/// @callsequence To be updated.
/// @InOutCorrelation Frequency at which the runnable is called: N/A
/// @startuml "Call sequence for v_InitClocks()"
///
/// legend left
///    Task: Where the runnables are added, depending on the project (SchM, Com, etc.)
///    Task priority: n/a
///    Cycle time: N/A
/// endlegend
///
/// activate startup_r7.c
///
///    startup_r7.c -> sys_time.c: v_InitClocks() calls SYS_TimeGetCount()
///    note left
///       Result: None
///    end note
///    activate sys_time.c
///       sys_time.c -> startup_r7.c: done
///    deactivate sys_time.c
///    startup_r7.c -> sys_time.c: v_InitClocks() calls SYS_TimeGetSince()
///    note left
///       Parameters: u_Start
///       Result: None
///    end note
///    activate sys_time.c
///       sys_time.c -> startup_r7.c: done
///    deactivate sys_time.c
///    startup_r7.c -> sys_time.c: v_InitClocks() calls SYS_TimeGetCount()
///    note left
///       Result: None
///    end note
///    activate sys_time.c
///       sys_time.c -> startup_r7.c: done
///    deactivate sys_time.c
///    startup_r7.c -> sys_time.c: v_InitClocks() calls SYS_TimeGetSince()
///    note left
///       Call condition:  (BOARD_CPGMSTPSR9 & BIT27) == 0u
///       Parameters: u_Start
///       Result: None
///    end note
///    activate sys_time.c
///       sys_time.c -> startup_r7.c: done
///    deactivate sys_time.c
///    startup_r7.c -> sys_time.c: v_InitClocks() calls SYS_TimeGetCount()
///    note left
///       Result: None
///    end note
///    activate sys_time.c
///       sys_time.c -> startup_r7.c: done
///    deactivate sys_time.c
///    startup_r7.c -> sys_time.c: v_InitClocks() calls SYS_TimeGetSince()
///    note left
///       Call condition:  (BOARD_CPGMSTPSR3 & BIT04) == 0u
///       Parameters: u_Start
///       Result: None
///    end note
///    activate sys_time.c
///       sys_time.c -> startup_r7.c: done
///    deactivate sys_time.c
///    startup_r7.c -> sys_time.c: v_InitClocks() calls SYS_TimeGetCount()
///    note left
///       Result: None
///    end note
///    activate sys_time.c
///       sys_time.c -> startup_r7.c: done
///    deactivate sys_time.c
///    startup_r7.c -> sys_time.c: v_InitClocks() calls SYS_TimeGetSince()
///    note left
///       Call condition:  (BOARD_CPGCSI0CKCR & BIT8) == 0u
///       Parameters: u_Start
///       Result: None
///    end note
///    activate sys_time.c
///       sys_time.c -> startup_r7.c: done
///    deactivate sys_time.c
///    startup_r7.c -> sys_time.c: v_InitClocks() calls SYS_TimeGetCount()
///    note left
///       Result: None
///    end note
///    activate sys_time.c
///       sys_time.c -> startup_r7.c: done
///    deactivate sys_time.c
///    startup_r7.c -> sys_time.c: v_InitClocks() calls SYS_TimeGetSince()
///    note left
///       Call condition:  ((BOARD_SITSCR & BIT14) == 0u) && ((BOARD_SITSCR & BIT15) == 0u)
///       Parameters: u_Start
///       Result: None
///    end note
///    activate sys_time.c
///       sys_time.c -> startup_r7.c: done
///    deactivate sys_time.c
///    startup_r7.c -> sys_time.c: v_InitClocks() calls SYS_TimeGetCount()
///    note left
///       Result: None
///    end note
///    activate sys_time.c
///       sys_time.c -> startup_r7.c: done
///    deactivate sys_time.c
///    startup_r7.c -> sys_time.c: v_InitClocks() calls SYS_TimeGetSince()
///    note left
///       Call condition:  (BOARD_CPGMSTPSR9 & BIT28) == 0u
///       Parameters: u_Start
///       Result: None
///    end note
///    activate sys_time.c
///       sys_time.c -> startup_r7.c: done
///    deactivate sys_time.c
/// deactivate startup_r7.c
/// @enduml
///
/// @testmethod
///    Dynamic unit test method
///    - This function has to be tested for statement and branch coverage.
///    - For detailed unit test description follow the caller graph to the test function below.
///    - Test script: test_startup_r7.c
/// @traceability

static void v_InitClocks(void);

/// Watchdog Check Point setter \n \n
/// This function will set the current watchdog state machine position.\n
/// It can be one of the following items:\n
/// STARTUP_CP_PREMAIN_ENTRY,  Function entry of STARTUP_v_PreMain()\n
/// STARTUP_CP_DATA_INIT_DONE, Exception table is copied and BSS section is zeroed \n
/// STARTUP_CP_PER_INIT_DONE,  Peripheral initialization is done\n
static void v_CheckPoint(const t_STARTUP_CheckPoint u_CheckPoint)
{
  u_STARTUP_CheckPoint = (uint32)u_CheckPoint;
}

/// Reference Source file : startup_r7.c
///
/// @pre None
/// @post None
/// @param None
/// @return static void
/// @globals No global variables used
/// @InOutCorrelation None
/// @callsequence To be updated.
/// @InOutCorrelation Frequency at which the runnable is called: N/A
/// @startuml "Call sequence for v_IndicateInitDone()"
///
/// legend left
///    Task: Where the runnables are added, depending on the project (SchM, Com, etc.)
///    Task priority: n/a
///    Cycle time: N/A
/// endlegend
///
/// activate startup_r7.c
///
///    startup_r7.c -> startup_r7.c: v_IndicateInitDone() calls v_CheckPoint()
///    note left
///       Parameters: STARTUP_CP_INIT_DONE
///       Result: None
///    end note
///    activate startup_r7.c
///       startup_r7.c -> startup_r7.c: done
///    deactivate startup_r7.c
/// deactivate startup_r7.c
/// @enduml
///
/// @testmethod
///    Dynamic unit test method
///    - This function has to be tested for statement and branch coverage.
///    - For detailed unit test description follow the caller graph to the test function below.
///    - Test script: test_startup_r7.c
/// @traceability

/// Indicates Init Done \n
/// This function will switch the Pal Watchdog position to its final state (INIT_DONE).
static void v_IndicateInitDone(void)
{
  v_CheckPoint(STARTUP_CP_INIT_DONE);
}

/// Reference Source file : startup_r7.c
///
/// @pre None
/// @post None
/// @param None
/// @return void
/// @globals No global variables used
/// @InOutCorrelation None
/// @callsequence To be updated.
/// @InOutCorrelation Frequency at which the runnable is called: N/A
/// @startuml "Call sequence for v_call_ctors()"
///
/// legend left
///    Task: Where the runnables are added, depending on the project (SchM, Com, etc.)
///    Task priority: n/a
///    Cycle time: N/A
/// endlegend
///
/// activate startup_r7.c
/// activate FS_Reset.c
/// startup_r7.c -> FS_Reset.c : v_call_ctors() calls FS_Reset_v_Cat2Reaction()
/// FS_Reset.c -> startup_r7.c : done
/// deactivate FS_Reset.c
/// deactivate startup_r7.c
/// @enduml
///
/// @testmethod
///    Dynamic unit test method
///    - This function has to be tested for statement and branch coverage.
///    - For detailed unit test description follow the caller graph to the test function below.
///    - Test script: test_startup_r7.c
/// @traceability


/// This function is compiler specific. \n \n
/// Loop through init_array and execute each constructor until we hit the end of the list. \n
/// Calls the constructors of all static objects. \n
/// Linker stores in special section INIT_ARRAY a table with all constructors as function pointer. \n
/// The function pointers are not absolute addresses but relative to the base address of section INIT_ARRAY.
// PRQA S 2211 40
// 2021-1-6; uie38801
// Message(2:2211) '%s' is not aligned with the previously declared identifier.
// Reason: Alignment of identifiers.
#ifndef __COURAGE_TEST_SWITCH__
static void v_call_ctors(void)
{
  // Define constructor function pointer type
  typedef void  (*ctor_func_t)(void);//PRQA S 5004 
// Message(3:5004) This typedef is applied to a pointer type.
// Reason: Constructor function pointer type
// 2023-09-11 ; uif92156

  // QAC does not support linker symbols with $$
#ifdef __asm
#define DUMMY_LOCATION_FOR_QAC (0x10u)
  const sint32* init_array = (const sint32*)DUMMY_LOCATION_FOR_QAC; //PRQA S 0306
  const uint32  size = 0x10u;
#else
  // Import init_array from linker
  extern const sint32 SHT$$INIT_ARRAY$$Base;
  extern const sint32 SHT$$INIT_ARRAY$$Limit;

  const sint32* init_array = &(SHT$$INIT_ARRAY$$Base);
  const uint32  size = &(SHT$$INIT_ARRAY$$Limit) - &(SHT$$INIT_ARRAY$$Base);
#endif

  ctor_func_t   ctor;
  uint32        index;

  // Loop through init_array and execute each constructor until we hit the end of the list
  for (index = 0u; index < size; index++)
  {
    // PRQA S 0305 4
    // 2020-03-25; uidv7790
    // Msg(3:0305) [I] Cast between a pointer to function and an integral type.
    // reason: Constructor pointer must be calculated from the table.
    ctor = (ctor_func_t)(init_array[index] + (sint32)&init_array[index]); //PRQA S 0306
    if ( ctor == NULL)
    {
      // Report error...init array entry is NULL pointer
      FS_Reset_v_Cat2Reaction(FS_RESET_STARTUP_CONSTRUCTORS, NULL);
    }
    else
    {
      // Call constructor
      (ctor)();
    }
  }
}
#endif
// PRQA S 0602 14
// 2020-03-05; uie23485;
// summary: Msg(4:0602) The identifier '__cxa_atexit' is reserved for use by the library.
// reason:  Empty function, never called. Approved by QAC CCB (MKS issue 1376832).
void __cxa_atexit(void) //PRQA S 1503 
// Message(3:1503) The function '__cxa_atexit' is defined but is not used within this project.
// Reason: Function call is not in this file,but could be called in a different module.
// 2023-09-11 ; uif92156
{

}

void STARTUP_v_copyPPAR(void)
{
#ifdef CYBERSEC_ENABLED
  Std_ReturnType ret;

  ret = Load_xpar(PPAR_SDF_START, u_PPAR_SRAM, (uint32)PPAR_SDF_LEN);
  if(E_OK != ret)
  {
    // uif85744 19-1-2024
    // Msg(3:2214) Body of control statement is on the same line and is not enclosed within braces.
    // Reason : The 'while 'is dummy loop. It is no avoidable, no side effect.
    // PRQA S 2214 1
    while(1);
  }
#else
  MemIf_JobResultType u_JobStatus;

  // Initialize FLS for loading
  Fls_Init(&s_QsfLoad);

  // Copy PPAR data into RAM
  (void)Fls_Read((uint32)PPAR_SDF_START, u_PPAR_SRAM, (uint32)PPAR_SDF_LEN);
  do
  {
    Fls_MainFunction();
    u_JobStatus = Fls_GetJobResult();
  } while (u_JobStatus == MEMIF_JOB_PENDING);
#endif
}


// PRQA S 7002 ++
// 2021-02-17; uie23485
// Message(3:7002) Amount of non-cyclic paths, minimum number of necessary test cases
// Reason: not critical, as these non cyclic paths are necessary.

/// Init clock tree registers which are not set elsewhere. \n
/// This function will start following clocks:\n
/// I2C_IF4, TPU0, CSI0CKCR, I2C_IF3, I2C_IF5
static void v_InitClocks(void)
{
  uint32 u_Duration;
  uint32 u_Start;

  // Enable clock I2C_IF4
  BOARD_CPGSMSTPCR9 &= ~(uint32)BIT27; //PRQA S 0303 // 0303_REG_32
  u_Start = SYS_TimeGetCount();
  do
  {
    u_Duration = SYS_TimeGetSince(u_Start);
    if ( (BOARD_CPGMSTPSR9 & BIT27) == 0u) //PRQA S 0303 // 0303_REG_32
    {
      break;
    }
  } while (u_Duration < TIMEOUT100);

  // Enable clock TPU0
  BOARD_CPGSMSTPCR3 &= ~(uint32)BIT04; //PRQA S 0303 // 0303_REG_32
  u_Start = SYS_TimeGetCount();
  do
  {
    u_Duration = SYS_TimeGetSince(u_Start);
    if ( (BOARD_CPGMSTPSR3 & BIT04) == 0u) //PRQA S 0303 // 0303_REG_32
    {
      break;
    }
  } while (u_Duration < TIMEOUT100);

  // Enable clock CSI0CKCR
  BOARD_CPGCSI0CKCR &= ~(uint32)BIT8; //PRQA S 0303 // 0303_REG_32
  u_Start = SYS_TimeGetCount();
  do
  {
    u_Duration = SYS_TimeGetSince(u_Start);
    if ( (BOARD_CPGCSI0CKCR & BIT8) == 0u) //PRQA S 0303 // 0303_REG_32
    {
      break;
    }
  } while (u_Duration < TIMEOUT100);

  // SITSCR - Selects module clock as the source of master clock
  BOARD_SITSCR &= (uint16) ~BIT14; //PRQA S 0303 // 0303_REG_32
  BOARD_SITSCR &= (uint16) ~BIT15; //PRQA S 0303 // 0303_REG_32
  u_Start = SYS_TimeGetCount();
  do
  {
    u_Duration = SYS_TimeGetSince(u_Start);
    if ( ((BOARD_SITSCR & BIT14) == 0u) && ((BOARD_SITSCR & BIT15) == 0u)) //PRQA S 0303 // 0303_REG_32
    {
      break;
    }
  } while (u_Duration < TIMEOUT100);

  // Enable clock I2C_IF3
  BOARD_CPGSMSTPCR9 &= ~(uint32)BIT28; //PRQA S 0303 // 0303_REG_32
  u_Start = SYS_TimeGetCount();
  do
  {
    u_Duration = SYS_TimeGetSince(u_Start);
    if ( (BOARD_CPGMSTPSR9 & BIT28) == 0u) //PRQA S 0303 // 0303_REG_32
    {
      break;
    }
  } while (u_Duration < TIMEOUT100);

  // Enable clock I2C_IF5
  BOARD_CPGSMSTPCR9 &= ~(uint32)BIT19; //PRQA S 0303 // 0303_REG_32
  u_Start = SYS_TimeGetCount();
  do
  {
    u_Duration = SYS_TimeGetSince(u_Start);
    if ( (BOARD_CPGMSTPSR9 & BIT19) == 0u) //PRQA S 0303 // 0303_REG_32
    {
      break;
    }
  } while (u_Duration < TIMEOUT100);
  //PRQA S 0303 14 // 0303_REG_32
  BOARD_CPGSD0CKCR = BOARD_CPGSD0CKCR_INIT_VAL; 
  BOARD_I2C5ICCCR  = BOARD_I2C5ICCCR_INIT_VAL;
  BOARD_I2C4ICCCR  = BOARD_I2C4ICCCR_INIT_VAL;
  BOARD_I2C3ICCCR = BOARD_I2C3ICCCR_INIT_VAL;
  BOARD_SITSCR = BOARD_SITSCR_INIT_VAL;
  BOARD_CPGCSI0CKCR = BOARD_CPGCSI0CKCR_INIT_VAL;
  BOARD_CPGFRQCRB = BOARD_CPGFRQCRB_INIT_VAL;
  BOARD_CPGFRQCRB |=(uint32) BIT31; //Activates the FRQCRB, FRQCRC settings

  // Enable the HW CRC Clock and Initial Values
  RMSTPCR3 = RMSTPCR3_CRC0_VALUE;
  SMSTPCR3 = SMSTPCR3_CRC0_VALUE;
  DCRA0CTL = CRC32_CTRL_REG_WORD;
  DCRA1CTL = CRC16_CTRL_REG_BYTE;
}
// PRQA S 7002 --

void STARTUP_v_StartDpuSbl(void)
{
  uint32 u_DpuEntry = 0U;
  uint8  u_IsEvsDpu = FALSE;
  // Enable Power Domains
  IuSbl_v_DPU_PowerOn();

  // Key must always be set to valid to be usable within the Crypto HW Driver
  // HW key already stored in ICUMX
  (void)Csm_KeySetValid(SecureBootAuthKey);
  // load DPU_SBL
  u_DpuSblEntry = PRESBL_LoadImage(SDF_SBL_DPU_START, SBL_DPU_RBIN_NAME, 0U);
  
  if(u_DpuSblEntry == 0U)
  {	     
    // uif85744 19-01-2024
    // Msg(3:1295)  An integer constant of 'essentially unsigned' type is being converted to type _Bool on assignment.
    // Msg(3:4424)  An expression of 'essentially enum' type (InfoBlockStructure_e_InfoBlockIdExt) is being converted to unsigned type, 'unsigned char' on assignment.
    // Reason : The argument types is not avoidable.It won't cause any side effect.
    // PRQA S 1295,4424 1
    FBLSDF_u_b_NvmSetApplicationValidFlag(APPL_IBID_EXT, FALSE);
    Pal_v_Reset(PAL_RESET_FUNCTIONAL);
  }
  else
  {
    u_IsEvsDpu = PRESBL_v_IsEvsDpu();
    if(FALSE == u_IsEvsDpu)
    {
      u_DpuEntry = PRESBL_LoadImage(SDF_APPL_DPU_START, DPU_RBIN_NAME, 0U);
      if(0U == u_DpuEntry)
      {
        FBLSDF_u_b_NvmSetApplicationValidFlag(APPL_IBID_EXT, FALSE);
        Pal_v_Reset(PAL_RESET_FUNCTIONAL);
      }
      else
      {
        BAI_SetDpuEntrypoint(u_DpuEntry);
      }
    }
  }
  //Free the PMC lock
  PMC_v_FreeSDFLock(); 

  // now we can switch Fls config to NvM on infra side
  Fls_Init(&s_QsfApplNvm);
  STARTUP_v_RunDpuSbl();
  return;
}

void STARTUP_v_RunDpuSbl(void)
{
  if(0U != u_DpuSblEntry)
  {
  // Start 1st A53 core
  IuSbl_v_DPU_Start(COREID_CA53_0, u_DpuSblEntry);
  }
}


void STARTUP_v_PreMain(void)//PRQA S 1503 
// Message(3:1503) The function 'STARTUP_v_PreMain' is defined but is not used within this project.
// Reason: Function call is not in this file,but could be called in a different module.
// 2023-09-11 ; uif92156
{
  v_CheckPoint(STARTUP_CP_PREMAIN_ENTRY);

  // Initially trigger watchdog
  Pal_Wdg_v_TriggerInitial();

  // copy exception table
  MK_ExceptionTable = STARTUP_ExceptionTable;
}


void MK_InitHardwareBeforeData(void)    //PRQA S 1503 
// Message(3:1503) The function 'MK_InitHardwareBeforeData' is defined but is not used within this project.
// Reason: Function call is not in this file,but could be called in a different module.
// 2023-09-11 ; uif92156
{
  // enable I$ and BTA$
  STARTUP_v_EnableICache();

  // set clock registers
  v_InitClocks();

}

void MK_InitHardwareAfterData(void)   //PRQA S 1503 
// Message(3:1503) The function 'MK_InitHardwareAfterData' is defined but is not used within this project.
// Reason: Function call is not in this file,but could be called in a different module.
// 2023-09-11 ; uif92156
{
  uint32 newval;

  v_CheckPoint(STARTUP_CP_DATA_INIT_DONE);

  //PRQA S 0303 11 // 0303_REG_32
  // Release TMU[0-4] units from reset.
  newval = BOARD_RMSTPCR1 & ~BOARD_RMSTPCR1_TMUx;
  BOARD_CPGWPR = ~newval;
  BOARD_RMSTPCR1 = newval;

  // Realtime Module Stop Control Register
  // Enable supply of the clock signal to CMT1
  BOARD_RMSTPCR3 &=  (uint32)(~BOARD_RMSTPCR3_CMT1_MASK);

  // Start the ARM global timer
  BOARD_ARM_GLOBAL_TIMER_CTRL |= 0x1u;

  // Perform the initialization of the PMU to be used as QM-OS timer, that
  // requires to be run in privileged mode.

  OS_ARM_Timer_PmuInitPriv();

  v_call_ctors();

  v_CheckPoint(STARTUP_CP_PER_INIT_DONE);

  // Indicate that Init is done and next is handover to OS
  v_IndicateInitDone();

}

// PRQA S 3469 15
// 2021-1-6; uie38801
// Message(2:3469) This usage of a function-like macro looks like it could be replaced by an equivalent function call.
// Reason: Macro is very simple, it is used for accessing HW register.
void STARTUP_v_SetLed(void)   //PRQA S 1503 
// Message(3:1503) The function 'STARTUP_v_SetLed' is defined but is not used within this project.
// Reason: Function call is not in this file,but could be called in a different module.
// 2023-09-11 ; uif92156
{
  // PRQA S 1281 6
  // 2019-1-17; uidw1725
  // Msg(3:1281) Integer literal constant is of an unsigned type but does not include a "U" suffix.
  // reason: GPIO_OUTDT5 is defined in delivered Reg_RCarV3M.h as #define PRR 0xFFF00044 without "U" suffix
  REG32( GPIO_OUTDT5 ) |= (uint32)LED_0_MASK; //PRQA S 0303 // 0303_REG_32
  REG32( GPIO_OUTDT5 ) |= (uint32)LED_1_MASK; //PRQA S 0303 // 0303_REG_32
  REG32( GPIO_OUTDT5 ) &= ~(uint32)LED_2_MASK; //PRQA S 0303 // 0303_REG_32
}

#ifdef __cplusplus
}
#endif

