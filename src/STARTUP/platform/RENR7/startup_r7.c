// Copyright (C) Continental AG
// All Rights Reserved
// COMPONENT: STARTUP_R7
// VERSION:   $Revision: 1.42 $
/// @file     startup_r7.c
/// @brief    This file implements the pre main start up for the cores 1 and 2

// C-code part of startup code
#include <startup_r7.h>
//#include <Os_ARM_timer_pmu.h>
#include <memory_layout.h>
#include <IuSbl_part.h>
#include <PreSBL_Loading.h>
#include <sys_time.h>
#include <sys_mem.h>
#include <Fls.h>
#include <FS_Reset.h>
#include <PAL_Watchdog.h>
#include <Reg_RCarV3M.h>
#include <Mk_qmboard.h>
#include <PAL_McuDefs.h>
#include <Mk_ARM_exceptionhandling.h>

#ifdef __cplusplus
extern "C"
{
#endif

// typical memory mapped register access
// PRQA S 3453 5
// 2020-03-25; uidv7790
// Msg(2:3453) A function could probably be used instead of this function-like macro.
// reason: No, these macros provide lvalues which cannot be done with functions.
#define BOARD_REG_32(a) (*((volatile uint32*)(a)))
#define BOARD_REG_16(a) (*((volatile uint16*)(a)))

// Various registers
#define BOARD_CPGWPR  BOARD_REG_32(0xe6150900u)
#define BOARD_CPGSD0CKCR  BOARD_REG_32(0xE6150074u)
#define BOARD_CPGSMSTPCR9  BOARD_REG_32(0xE6150994u)
#define BOARD_CPGMSTPSR9  BOARD_REG_32(0xE61509A4u)
#define BOARD_CPGSMSTPCR3  BOARD_REG_32(0xE615013Cu)
#define BOARD_CPGMSTPSR3  BOARD_REG_32(0xE6150048u)

#define BOARD_I2C4ICCCR  BOARD_REG_32(0xE66D8018u)
#define BOARD_RMSTPCR1  BOARD_REG_32(0xe6150114u)
#define BOARD_TMUTCR3  BOARD_REG_16(0xE6FC0010u)
#define BOARD_TMUTCR3_CLEAR  (0x0u)
#define BOARD_RMSTPCR1_TMU4 0x00200000U
#define BOARD_RMSTPCR1_TMU3 0x00400000U
#define BOARD_RMSTPCR1_TMU2 0x00800000U
#define BOARD_RMSTPCR1_TMU1 0x01000000U
#define BOARD_RMSTPCR1_TMU0 0x02000000U
#define BOARD_RMSTPCR1_TMUx (BOARD_RMSTPCR1_TMU4|BOARD_RMSTPCR1_TMU3|BOARD_RMSTPCR1_TMU2|BOARD_RMSTPCR1_TMU1|BOARD_RMSTPCR1_TMU0)
#define BOARD_ARM_GLOBAL_TIMER_CTRL  BOARD_REG_32(0xf0000208u)

#define BOARD_CPGSD0CKCR_INIT_VAL  0x0000F046u
#define BOARD_I2C4ICCCR_INIT_VAL   0x00000026u


#define BIT04 (1uL << 4u)
#define BIT27 (1uL << 27u)
#define TIMEOUT100 (100u)

#define BOARD_RMSTPCR3 BOARD_REG_32(0xE615011Cu)
#define BOARD_RMSTPCR3_CMT1_MASK (0x00000004UL)

#define PPAR_SDF_START       SDF_PPAR_START  
#define PPAR_SDF_LEN         0x5000uL /* length of PPAR is 20kB which is fixed*/

#define PRR_BIT_SILVER 4u

#define LED_0_MASK 0x00000020uL
#define LED_1_MASK 0x00000040uL
#define LED_2_MASK 0x00000080uL

// memory allocation of 20kB for PPAR in .ppar_bu section
// PRQA S 3218 4
// 2019-1-17; uidw1725
// Msg(3:3218) File scope static, 'u_PPAR_SRAM', is only accessed in one function.
// reason: reserve global PPAR memory area
static uint8 u_PPAR_SRAM[PPAR_SDF_LEN] __attribute__((section(".ppar_bu")));

// holds check points for startup
volatile uint32 u_STARTUP_CheckPoint;

// Watchdog Check Point setting
static void v_CheckPoint(t_STARTUP_CheckPoint u_CheckPoint)
{
  u_STARTUP_CheckPoint = (uint32)u_CheckPoint;
}

// Indicate Init Dones
static void v_IndicateInitDone(void)
{
  v_CheckPoint(STARTUP_CP_INIT_DONE);
}

/// @brief    Calls the contructors of all static objects
///           Linker stores in special section INIT_ARRAY a table with all constructors
///           as function pointer. The function pointers are not absolute addresses but 
///           relative.
/// @pre      none
/// @post     none
///
/// @return   none
///
/// @globals  none
static void v_call_ctors(void)
{
  // Define constructor function pointer type
  typedef void (*ctor_func_t)(void);

// QAC does not support linker symbols with $$
#ifdef __asm
  #define DUMMY_LOCATION_FOR_QAC (0x10u)
  const sint32* init_array = (const sint32*)DUMMY_LOCATION_FOR_QAC;
  const uint32 size = 0x10u;
#else
  // Import init_array from linker
  extern const sint32 SHT$$INIT_ARRAY$$Base;
  extern const sint32 SHT$$INIT_ARRAY$$Limit;

  const sint32* init_array = &(SHT$$INIT_ARRAY$$Base);
  const uint32 size = &(SHT$$INIT_ARRAY$$Limit) - &(SHT$$INIT_ARRAY$$Base);
#endif

  ctor_func_t ctor;
  uint32 index;

  // Loop through init_array and execute each constructor until we hit the end of the list
  for (index = 0u; index < size; index++)
  {
    // PRQA S 0305 4
    // 2020-03-25; uidv7790
    // Msg(3:0305) [I] Cast between a pointer to function and an integral type.
    // reason: Constructor pointer must be calculated from the table.
    ctor = (ctor_func_t)(init_array[index] + (sint32)&init_array[index]);
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
/// The below implementation is commented out as this can be solved by having -fno-use-cxa-atexit compile flag.
/// __aeabi_atexit()
///
/// The destructor is call after the constructor (fx call_ctors()) by the runtime environment.
/// This function is used by the runtime environment to register C++ destructors for static
/// objects that have to be called when the application terminates. As our application never
/// "exits", we provide our own implementation of __aeabi_atexit() which does nothing.
/// This is required as the default implementation uses malloc() which should be avoided.
/// @pre                -
/// @post               -
/// @globals            None
/// @InOutCorrelation   None
/// @callsequence
/// @testmethod         module test callsequence or review
/// @traceability       L4 design specification in Doors <a href="N/A">N/A</a>

void __aeabi_atexit()
{

}

// Copy PPAR data from SDF to SRAM section .ppar_bu
void STARTUP_v_copyPPAR(void)
{

	MemIf_JobResultType u_JobStatus;

	//initialize FLS for loading
	Fls_Init(&s_QsfLoad);

	//Copy data from source to destination
	(void)Fls_Read((uint32)PPAR_SDF_START, u_PPAR_SRAM, PPAR_SDF_LEN);
	do
	{
		Fls_MainFunction();
		u_JobStatus = Fls_GetJobResult();
	} while (u_JobStatus == MEMIF_JOB_PENDING);

}


/// @brief                         Init clock tree registers which are not set elsewhere
///
/// @pre                          none
/// @post                         none
///
/// @return                       none
///
/// @globals                      none
///
/// @InOutCorrelation             TBD
/// @startuml
/// start
/// :Enable clock I2C_IF4;
/// :Enable clock TPU0;
/// stop
/// @enduml
static void v_InitClocks(void)
{
  uint32 u_Duration;
  uint32 u_Start;

  // Enable clock I2C_IF4
  BOARD_CPGSMSTPCR9 &= ~BIT27;
  u_Start = SYS_TimeGetCount();
  do
  {
    u_Duration = SYS_TimeGetSince(u_Start);
    if ( (BOARD_CPGMSTPSR9 & BIT27) == 0u)
    {
      break;
    }
  } while (u_Duration < TIMEOUT100);

  // Enable clock TPU0
  BOARD_CPGSMSTPCR3 &= ~BIT04;
  u_Start = SYS_TimeGetCount();
  do
  {
    u_Duration = SYS_TimeGetSince(u_Start);
    if ( (BOARD_CPGMSTPSR3 & BIT04) == 0u)
    {
      break;
    }
  } while (u_Duration < TIMEOUT100);

  BOARD_CPGSD0CKCR = BOARD_CPGSD0CKCR_INIT_VAL;
  BOARD_I2C4ICCCR  = BOARD_I2C4ICCCR_INIT_VAL;
}

// Start DPU_SBL
void STARTUP_v_StartDpuSbl(void)
{
  uint32 u_DpuSblEntry;

  // Enable Power Domains
  IuSbl_v_DPU_PowerOn();

  // load DPU_SBL
  u_DpuSblEntry = PRESBL_u_Load_BuildUnit((uint32)SDF_SBL_DPU_START);

  // Start 1st A53 core
  IuSbl_v_DPU_Start(COREID_CA53_0, u_DpuSblEntry);

  // now we can switch Fls config to NvM on infra side
  Fls_Init(&s_QsfApplNvm);
  return;
}

// gets called by assembler code part of startup
void STARTUP_v_PreMain(void)
{
  v_CheckPoint(STARTUP_CP_PREMAIN_ENTRY);

  // Initially trigger watchdog
  Pal_Wdg_v_TriggerInitial();

}


// MK_InitHardwareBeforeData() is called during startup before the .data and .bss sections
// have been initialised. The initial values of non-automatic variables must not be relied
// upon here. Values written to such variables will be lost after this function returns.
// External RAM, ROM, FLASH etc. can be set up here.
// PLL initialisation should go here so that the RAM initialisation runs at full speed.
//
void MK_InitHardwareBeforeData(void)
{
  // enable I$ and BTA$
  STARTUP_v_EnableICache();

  // set clock registers
  v_InitClocks();

  // copy exception table
  MK_ExceptionTable = STARTUP_ExceptionTable;
}


/// MK_InitHardwareAfterData() is called during startup after the .data and .bss sections
/// have been initialized (so global variables can
/// now be used). The BoardInit() can be used to start up drivers etc.
void MK_InitHardwareAfterData(void)
{
	  uint32 newval;

  v_CheckPoint(STARTUP_CP_DATA_INIT_DONE);
  

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


// Switch on LED2 and turn off LED0 and LED1
void STARTUP_v_SetLed(void)
{
	  // PRQA S 1281 6
	  // 2019-1-17; uidw1725
	  // Msg(3:1281) Integer literal constant is of an unsigned type but does not include a "U" suffix.
	  // reason: GPIO_OUTDT5 is defined in delivered Reg_RCarV3M.h as #define PRR 0xFFF00044 without "U" suffix
	REG32( GPIO_OUTDT5 ) &= ~LED_0_MASK;
	REG32( GPIO_OUTDT5 ) &= ~LED_1_MASK;
	REG32( GPIO_OUTDT5 ) |= LED_2_MASK;
}


// Identify the silicon version of the V3M.
uint8 STARTUP_u_GetSiliconVersion(void)
{
  uint8 silver;
  uint32 prr;

  // PRQA S 1281 4
  // 2019-1-17; uidw1725
  // Msg(3:1281) Integer literal constant is of an unsigned type but does not include a "U" suffix.
  // reason: PRR is defined in delivered Reg_RCarV3M.h as #define PRR 0xFFF00044 without "U" suffix
  prr = *(volatile uint32*)PRR;           // read out Product Register PRR
  prr = prr & BIT04;                      // mask out irrelevant bits
  silver = (uint8)(prr>>PRR_BIT_SILVER);  // shift relevant bit to bit position 0
  return silver;                          // 0:V3M cut 1.0 / 1: V3M cut 2.0
}

#ifdef __cplusplus
}
#endif

