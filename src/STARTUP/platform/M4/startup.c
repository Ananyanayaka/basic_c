// Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
// All Rights Reserved

// COMPONENT: StartUp_M4
/// \file startup_M4.c
/// \brief File contains start up code for Cortex M4
/// \addtogroup StartUp_M4 {

#include "Std_Types.h"
#include "startup_cfg.h"
#include "startup.h"

#ifdef __cplusplus
extern "C"
{
#endif

#if defined (FPU_USED)
/// \brief Enables the FPU coprocessors.
static void STARTUP_v_EnableFpu(void);
#endif

/// \brief initializes memory and disables global interrupts.
void STARTUP_v_PreMainStartup(void);

#if (ZERO_INIT_GROUPS_COUNT > 0u)
/// \brief Initializes bss memory segments.
static void STARTUP_v_MemoryZeroInit(const MemArea *memAreaSet);
#endif

#if (DATA_INIT_GROUPS_COUNT > 0u)
/// \brief Initializes data memory segments.
static void STARTUP_v_MemoryInit(const RamMemArea *memAreasSet);
#endif

/// \brief Disables the global system interrupt.
static void STARTUP_v_HwDisableInterruptAtPowerOn(void);


// Needed for BASEPRI analyzation within STARTUP_v_HwDisableInterruptAtPowerOn()
static uint8 DisIntPrioTmp;
// volatile required because of register access
static volatile uint8 DisIntITmp;


static inline void set_NVIC_IPR(uint8 idx, uint8 ui8_value);
static inline uint32 get_NVIC_ICTR(void);
static inline uint32* get_pui32PtrToReg(uint32 Addr);
// PRQA S 0605 ++
// 2022-10-24; uie43272
// message(3:0605) A declaration must declare a tag or an identifier.
// reason: Code is written using assembly instructions.

/* =========================================================================== */
/*                                                                             */
/* Description: Entry point for all cores                                      */
/*                                                                             */
/* =========================================================================== */
__as1(.section .Startup , "ax" )
// reset
___asm(.globl StartupEntry)
___asm(StartupEntry:)
___asm(B PreAsmStartupHook)


/* =========================================================================== */
/*                                                                             */
/* Description: Optional hook for platform specific tasks                      */
/*                                                                             */
/* =========================================================================== */
___asm(.globl PreAsmStartupHook)
___asm(PreAsmStartupHook:)
___asm(B _M4RegistersInit)

/* =========================================================================== */
/*                                                                             */
/* Description: Initialize general purpose registers                           */
/*                                                                             */
/* =========================================================================== */
___asm(.globl _M4RegistersInit)
___asm(_M4RegistersInit:)
// initialize general purpose registers
    __as1(MOV  R0, 0)
    __as1(MOV  R1, 0)
    __as1(MOV  R2, 0)
    __as1(MOV  R3, 0)
    __as1(MOV  R4, 0)
    __as1(MOV  R5, 0)
    __as1(MOV  R6, 0)
    __as1(MOV  R7, 0)
    __as1(MOV  R8, 0)
    __as1(MOV  R9, 0)
    __as1(MOV  R10, 0)
    __as1(MOV  R11, 0)
    __as1(MOV  R12, 0)

___asm(B StartupZeroInitLoop)

/* =========================================================================== */
/*                                                                             */
/* Description: Initialize memory blocks and areas with zero                   */
/*                                                                             */
/* =========================================================================== */
___asm(.globl StartupZeroInitLoop)
___asm(StartupZeroInitLoop:)

// Initialize stack area

// PRQA S 3462 1 # date: 2024-01-16, reviewer: uidg9094, CCBIssueId: ADASQMS-3493, reason: common usage in embedded development
__as1(LDR  R3, =_STACK_C0_START)   // start of stack area

// PRQA S 3462 1 # date: 2024-01-16, reviewer: uidg9094, CCBIssueId: ADASQMS-3493, reason: common usage in embedded development
__as1(LDR  R4, =_STACK_C0_LIMIT)   // end of stack area
__as1(MOV  R6, 0)                  // value to be filled

___asm(startup_area_zero_init_loop_start:)
__as1(STR  R6, [R3])    // must be an aligned memory access!
__as2(ADD  R3, R3, 4)
__as1(CMP  R3, R4) \
___asm(BEQ startup_area_zero_init_end) // If Start is same with End address-> Finished.

___asm(B startup_area_zero_init_loop_start)
___asm(startup_area_zero_init_end:)

___asm(B StackPointerInit_c0)


/* =========================================================================== */
/*                                                                             */
/* Description: Initialize stack pointer                                       */
/*                                                                             */
/* =========================================================================== */
___asm(.globl StackPointerInit_c0)
___asm(StackPointerInit_c0:)

// Initialization of the stack pointer
// PRQA S 3462 1 # date: 2024-01-16, reviewer: uidg9094, CCBIssueId: ADASQMS-3493, reason: common usage in embedded development
__as1(LDR  R0, =__section_stack_c0_end)
__as1(MOV  SP, R0)

#if defined (FPU_USED)
___asm(B STARTUP_v_EnableFpu)
#endif

/* =========================================================================== */
/*                                                                             */
/* Description: Jump to STARTUP_v_PreMainStartup()                             */
/*                                                                             */
/* =========================================================================== */
___asm(B STARTUP_v_PreMainStartup)

/* =========================================================================== */
/*                                                                             */
/* Description: Jump to STARTUP_v_ExceptionHandler() in case of an unexpected  */
/*              return from PreMain/main                                       */
/*                                                                             */
/* =========================================================================== */
___asm(B STARTUP_v_ExceptionHandler)

// PRQA S 0605 --

/* =========================================================================== */
/*                                                                             */
/* Description: Definition of functions                                        */
/*                                                                             */
/* =========================================================================== */

#if defined (FPU_USED)
/* Enable the FPU coprocessors, because some HwSpec functions and tests use floating point numbers. */
/* must be called from startup code as Os_Init expects the FPU registers to be setup by the customer. */
static void STARTUP_v_EnableFpu(void)
{
  HW_CPACR = HW_CPACR | (0xFUL << 20);
  HW_FPCCR = (HW_FPCCR & ~(0x1UL << 30)) | (0x1UL << 31); /* Set FPCCR.ASPEN, Reset FPCCR.LSPEN. */
}
#endif /*FPU_USED*/

// PRQA S 1503 5
// 2024-01-19; uidg9094
// message(3:1503) The function is defined but is not used within this project.
// reason: generic function
void STARTUP_v_PreMainStartup(void)
{
#if (ZERO_INIT_GROUPS_COUNT > 0u)
  // ZeroInit_Group contains groups which are configured with Init Policy "ZERO_INIT"
  STARTUP_v_MemoryZeroInit(ZeroInit_Group);
#endif

#if (DATA_INIT_GROUPS_COUNT > 0u)
  // DataInit_Group contains groups which are configured with Init Policy "INIT"
  STARTUP_v_MemoryInit(DataInit_Group);
#endif

  STARTUP_v_HwDisableInterruptAtPowerOn();

  (void)main();
}

#if (ZERO_INIT_GROUPS_COUNT > 0u)
static void STARTUP_v_MemoryZeroInit(const MemArea *memAreaSet)
{
  uint32    ui32_i;
  volatile uint32 *memPtrStart;
  volatile uint32 *memPtrEnd;

  for (ui32_i=0; ui32_i<ZERO_INIT_GROUPS_COUNT; ui32_i++)
  {
    memPtrStart = get_pui32PtrToReg(memAreaSet[ui32_i].Start);
    memPtrEnd = get_pui32PtrToReg(memAreaSet[ui32_i].End);
    while (memPtrStart < memPtrEnd)
    {
      *memPtrStart = 0u;
      memPtrStart++;
    }
  }
}
#endif

#if (DATA_INIT_GROUPS_COUNT > 0u)
static void STARTUP_v_MemoryInit(const RamMemArea *memAreasSet)
{
  uint32 *memPtrStart;
  uint32 *memPtrEnd;
  uint32 *romPtrStart;
  uint32 *romPtrEnd;
  uint32    ui32_i;

  for (ui32_i=0u; ui32_i<DATA_INIT_GROUPS_COUNT; ui32_i++)
  {
    memPtrStart = get_pui32PtrToReg(memAreasSet[ui32_i].Start);
    memPtrEnd = get_pui32PtrToReg(memAreasSet[ui32_i].End);
    romPtrStart = get_pui32PtrToReg(memAreasSet[ui32_i].Romstart);
    romPtrEnd = get_pui32PtrToReg(memAreasSet[ui32_i].Romend);
    if ((memPtrEnd - memPtrStart) != (romPtrEnd - romPtrStart))
    {
      // Defined size in rom does not match ram size
      STARTUP_v_ExceptionHandler();
    }

    while (memPtrStart < memPtrEnd)
    {
      *memPtrStart = *romPtrStart;
      memPtrStart++;
      romPtrStart++;
    }
  }
}
#endif

// PRQA S 3006 4
// 2023-12-20; uie43272
// message(3:3006)  This function contains a mixture of in-line assembler statements and C statements.
// reason: Code is written using assembly instructions.
static void STARTUP_v_HwDisableInterruptAtPowerOn(void)
{
  uint8 ui8_Idx;

  DisIntITmp = 0u; // prevent compiler warning

  /* get pointer to DisIntITmp */
  // PRQA S 3462 1 # date: 2024-01-16, reviewer: uidg9094, CCBIssueId: ADASQMS-3493, reason: common usage in embedded development
  __as1(LDR  R4, =DisIntITmp)
  /* analyze amount of supported bits in priority registers and BASEPRI
   * (BASEPRI bits not supported are "Read All Zero, Ignore Writes") */
   /* BASEPRI = 255 */
  __as1(MOV  R2, #255)
  __as1(MSR  basepri, R2)
  // DisIntITmp = BASEPRI
  __as1(MRS  R2, basepri)
  __as1(STRB R2, [R4])

  DisIntPrioTmp = 0XFFU;
  for (ui8_Idx = 0; ui8_Idx < 8u; ui8_Idx++)
  {
    if ((((uint8)(1u << ui8_Idx)) & DisIntITmp) != 0u)
    {
      DisIntPrioTmp = (uint8)(1u << ui8_Idx);
      break;
    }
  }
  if (DisIntPrioTmp == 0XFFU)
  {
    // BASEPRI not writable!
    while (1)
    {
      // do nothing
    }
  }

  // get pointer to DisIntPrioTmp
  // PRQA S 3462 1 # date: 2024-01-16, reviewer: uidg9094, CCBIssueId: ADASQMS-3493, reason: common usage in embedded development
  __as1(LDR  R4, =DisIntPrioTmp)
  __as1(LDRB  R2, [R4])
  // BASEPRI = DisIntPrioTmp
  __as1(MSR  basepri, R2)

  // evaluate amount of available NVIC_IPR regs within this Cortex-M* core
  DisIntITmp = (uint8)((get_NVIC_ICTR() & 0X0000000FU)+1u) << 5u;

  /* set all interrupt priorities to BASEPRI priority -> disable all interrupts
   * (if an interrupts shall be enabled -> set its NVIC_IPR reg to 0) */
  for (ui8_Idx = 0; ui8_Idx < DisIntITmp; ui8_Idx++)
  {
    set_NVIC_IPR(ui8_Idx, DisIntPrioTmp);
  }
}

static inline void set_NVIC_IPR(uint8 idx, uint8 ui8_value)
{
  //ToDo: get level 4 CCB approval
  (*((volatile uint8 *)(0xE000E400UL+(idx)))) = ui8_value;
}

static inline uint32 get_NVIC_ICTR(void)
{
  //ToDo: get level 4 CCB approval
  return (*((volatile uint32 *)(0xE000E004UL)));
}

static inline uint32* get_pui32PtrToReg(uint32 Addr)
{
  //ToDo: get level 4 CCB approval
  return (uint32*)Addr;
}

#ifdef __cplusplus
}
#endif

// Close the Doxygen group StartUp_M4
/// \}

