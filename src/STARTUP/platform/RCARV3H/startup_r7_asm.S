// Copyright (C) Continental AG
// All Rights Reserved
// COMPONENT: STARTUP_R7
// VERSION:   $Revision: 1.10 $

// Provides assembler code part for startup code ARM Cortex R7.
// Note:
// Assembler code files ending with .S (uper case S) are pre-processed.
// Means you can use preprocessor instructions like #include, #if ...
// If Assembler code files end with .s (lower case s) then it is not pre-processed.

.global OSVR_Entry							    /* entry point */
.global STARTUP_v_EnableICache
.global STARTUP_v_EnableCache
.global STARTUP_v_Error							/* endless loop as error handler */
.global boardFIQException
.global boardInvalidException
.global STARTUP_ExceptionTable

.section .reset, "ax"
OSVR_Entry:
  ;/* switch to SVC and disable interrupts */
  cpsid ai, #0x13
  msr cpsr_xsf, #0x100  ;/* initialize all bits of CPSR */


  ;/* set LED pattern to mark start of application */
  bl    STARTUP_v_SetLed

  /* Enable ECC and Lockstep */
  bl    v_EnableCoreECCLockstep

  ;/* enable the VFP */
  bl    EnableVfp

  ;/* initialize registers */
  bl    InitRegisters

  ;/* setup stack for startup */
	ldr		r13, =MK_kernelStackTop		    /* get base address of kernel stack */
	ldr		r13, [r13]						/* load core 0's initial stack pointer */

  /* check data cache status:
   * If disabled then perform an invalidate.
   * If enabled then perform a clean and invalidate and switch it off
   */
  mrc   p15, 0, r10, c1, c0, 0
  ands  r0, r10, #0x0004         /* L1 D cache */
  blne  v_CleanInvalidate
  bleq  v_Invalidate
  bic   r10, r10, #0x0004
  dsb   sy
  mcr   p15, 0, r10, c1, c0, 0
  isb   sy

  /* minimal Mpu setup */
  bl    STARTUP_v_SetupMpu

  ;/* call board specific early initialization */
  bl    STARTUP_v_PreMain

  // initialize RTDMAC before use in APPL (enable following line when RTDMAC is fully integrated)
#if 0
  bl    RTDMAC_v_GlobInit
#endif //#if 0

  ;/* start the microkernel */
  b     MK_Entry2


  /* it is an error if we return here from C code */
.type STARTUP_v_Error, "function"
STARTUP_v_Error:
boardFIQException:
boardInvalidException:
  b   STARTUP_v_Error         /* endless loop in case of error */


;/* EnableVfp
; *
; * Enable the VFPV16 Co-processor.
; */
.type EnableVfp, "function"
EnableVfp:
  mrc   p15, #0, r0, c1, c0, #2
  orr   r0, r0, #0xF00000
  mcr   p15, #0, r0, c1, c0, #2
  mov   r0, #0x40000000
  vmsr  FPEXC, r0
  bx    lr


;/* InitRegisters
; *
; * Initialize all registers with constant values. This way, the registers of
; * both cores will have identical values, and a lockstep error cannot result
; * from different (random) initial values for a register.
; */
.type InitRegisters, "function"
InitRegisters:
  ;/* initialize general purpose registers */
  mov   r0, #0
  mov   r1, r0
  mov   r2, r0
  mov   r3, r0
  mov   r4, r0
  mov   r5, r0
  mov   r6, r0
  mov   r7, r0
  mov   r8, r0
  mov   r9, r0
  mov   r10, r0
  mov   r11, r0
  mov   r12, r0

  ;/* lr is already initialized with the return address! */
  ;/* initialize spsr_svc */
  mov   sp, r0
  msr   spsr_cxsf, r0

#if 1
  ;/* switch to FIQ  */
  cps   #0x11

  ;/* initialize banked registers { r8_fiq - lr_fiq, spsr_fiq } */
  mov   r8, r0
  mov   r9, r0
  mov   r10, r0
  mov   r11, r0
  mov   r12, r0
  mov   lr, r0
  mov   sp, r0
  msr   spsr_cxsf, r0
#endif

  ;/* switch to IRQ mode */
  cps   #0x12

  ;/* initialize banked registers { sp_irq, lr_irq, spsr_irq } */
  mov   lr, r0
  mov   sp, r0
  msr   spsr_cxsf, r0

  ;/* switch to undefined mode */
  cps   #0x1B

  ;/* initialize banked registers { sp_und, lr_und, spsr_und } */
  mov   lr, r0
  mov   sp, r0
  msr   spsr_cxsf, r0

  ;/* switch to abort mode */
  cps   #0x17

  ;/* initialize banked registers { sp_abt, lr_abt, spsr_abt } */
  mov   lr, r0
  mov   sp, r0
  msr   spsr_cxsf, r0

  ;/* switch to system mode */
  cps   #0x1F

  ;/* initialize banked registers { sp_usr, lr_usr } */
  mov   lr, r0
  mov   sp, r0

  ;/* switch back to SVC */
  cps   #0x13

  ;/* initialize vFPU registers of both cores */
#if 0 // already done
  vmov  d0, r1, r1
  vmov  d1, r1, r1
  vmov  d2, r1, r1
  vmov  d3, r1, r1
  vmov  d4, r1, r1
  vmov  d5, r1, r1
  vmov  d6, r1, r1
  vmov  d7, r1, r1
  vmov  d8, r1, r1
  vmov  d9, r1, r1
  vmov  d10, r1, r1
  vmov  d11, r1, r1
  vmov  d12, r1, r1
  vmov  d13, r1, r1
  vmov  d14, r1, r1
  vmov  d15, r1, r1
#endif
  ;/* initialize call buffer. Save LR first! */
  mov   r0, lr

  bl    InitRegisters_l1
.type InitRegisters_l1, "function"
InitRegisters_l1:
  bl    InitRegisters_l2
.type InitRegisters_l2, "function"
InitRegisters_l2:
  bl    InitRegisters_l3
.type InitRegisters_l3, "function"
InitRegisters_l3:
  bl    InitRegisters_l4
.type InitRegisters_l4, "function"
InitRegisters_l4:

  bx    r0

  /* setup MPU for startup */
.type STARTUP_v_SetupMpu, "function"
STARTUP_v_SetupMpu:
  /* first disable MPU */
  mrc   p15, 0, r0, c1, c0, 0
  bfc   r0, #0, #1
  dsb   sy
  mcr   p15, 0, r0, c1, c0, 0
  isb   sy

  /* Region 0: 0x40000000, 2MB, rwx/r_x, exec, wb/w-alloc */
  mov   r0, #0
  mcr   p15, 0, r0, c6, c2, 0     /* set region number */
  ldr   r0, =0x40000000
  mcr   p15, 0, r0, c6, c1, 0     /* set base address */
  mcr   p15, 0, r0, c6, c1, 1
  mov   r0, #0x29
  mcr   p15, 0, r0, c6, c1, 2     /* set size and enable */
  mcr   p15, 0, r0, c6, c1, 3
  movw  r0, #0x20f
  mcr   p15, 0, r0, c6, c1, 4     /* set region access control */
  mcr   p15, 0, r0, c6, c1, 5

  /* enable background region and Safety OS will reconfigure and enable the mpu, BR will disabled by Safety OS */
  mrc   p15, 0, r0, c1, c0, 0
  orr   r0, r0, #0x20000
  dsb   sy
  mcr   p15, 0, r0, c1, c0, 0
  isb   sy

  bx    lr

.type v_CleanInvalidate, "function"
v_CleanInvalidate:
  mov   r0, #0
  mcr   p15, 2, r0, c0, c0, 0     /* select D cache */
  mrc   p15, 1, r0, c0, c0, 0     /* read CCSIDR */
  mov   r0, r0, ASR #13
  ldr   r3, =0x3FFF
  and   r0, r0, r3
  cmp   r0, #0x7F
  moveq r12, #0x1000           /* 4KB * 4way = 16KB */
  beq   cleanInvDCache
  cmp   r0, #0xFF
  moveq r12, #0x2000           /* 32KB */
  beq   cleanInvDCache
  movne r12, #0x4000           /* 64KB */

.type cleanInvDCache, "function"
cleanInvDCache:
  mov   r0, #0x00000000
  mov   r1, #0x40000000
  mov   r2, #0x80000000
  mov   r3, #0xC0000000

.type CleanInvDCacheBySet, "function"
CleanInvDCacheBySet:
  mcr   p15, 0, r0, c7, c14, 2     /* clean and invalidate dcache on way 0 */
  mcr   p15, 0, r1, c7, c14, 2     /* clean and invalidate dcache on way 1 */
  mcr   p15, 0, r2, c7, c14, 2     /* clean and invalidate dcache on way 2 */
  mcr   p15, 0, r3, c7, c14, 2     /* clean and invalidate dcache on way 3 */
  add   r0, r0, #0x20
  add   r1, r1, #0x20
  add   r2, r2, #0x20
  add   r3, r3, #0x20

  cmp   r0, r12
  bne   CleanInvDCacheBySet

  bx    lr

.type v_Invalidate, "function"
v_Invalidate:
  mov   r0, #0
  mcr   p15, 2, r0, c0, c0, 0     /* select D cache */
  mrc   p15, 1, r0, c0, c0, 0     /* read CCSIDR */
  mov   r0, r0, ASR #13
  ldr   r3, =0x3FFF
  and   r0, r0, r3
  cmp   r0, #0x7F
  moveq r12, #0x1000           /* 4KB * 4way = 16KB */
  beq   invDCache
  cmp   r0, #0xFF
  moveq r12, #0x2000           /* 32KB */
  beq   invDCache
  movne r12, #0x4000           /* 64KB */

.type invDCache, "function"
invDCache:
  mov   r0, #0x00000000
  mov   r1, #0x40000000
  mov   r2, #0x80000000
  mov   r3, #0xC0000000

.type InvDCacheBySet, "function"
InvDCacheBySet:
  mcr   p15, 0, r0, c7, c6, 2     /* invalidate dcache on way 0 */
  mcr   p15, 0, r1, c7, c6, 2     /* invalidate dcache on way 1 */
  mcr   p15, 0, r2, c7, c6, 2     /* invalidate dcache on way 2 */
  mcr   p15, 0, r3, c7, c6, 2     /* invalidate dcache on way 3 */
  add   r0, r0, #0x20
  add   r1, r1, #0x20
  add   r2, r2, #0x20
  add   r3, r3, #0x20

  cmp   r0, r12
  bne   InvDCacheBySet

  bx    lr


.type v_EnableCoreECCLockstep, "function"
v_EnableCoreECCLockstep:

  /* Requirement L3_SW_4724 */

  /* part (1) */
  /* Enable Lockstep error reporting*/
  ldr   r1, =0xE6260280          /* Address of MFIERRTGTR0 */
  ldr   r2, [r1]
  ldr   r3, =0x00082000          /* bit 13 and 19 */
  orr   r3, r3, r2
  str   r3, [r1]

  ldr   r1, =0xE6260200          /* Address of MFIERRCTLR0 */
  ldr   r2, [r1]
  ldr   r3, =0x00082000          /* bit 13 and 19 */
  orr   r3, r3, r2
  str   r3, [r1]

  /* part (2) */
  /* Initialize un-reset registers */
  MOV r0,#0

// SPSR
  CPS #0x17 ;// switch to Abort mode
  MSR SPSR_fsxc,r0
  CPS #0x1b ;// switch to Undefined mode
  MSR SPSR_fsxc,r0
  CPS #0x12 ;// switch to IRQ mode
  MSR SPSR_fsxc,r0
  CPS #0x11 ;// switch to FIQ mode
  MSR SPSR_fsxc,r0
  CPS #0x13 ;// switch back to Supervisor mode
  MSR SPSR_fsxc,r0

// FPU
// enable fpu coproc access
  MRC p15,0,r2,c1,c0,2 ;// read coproc access
  ORR r1,r2,#0xF<<20
  MCR p15,0,r1,c1,c0,2 ;// write coproc access

// check if fpu is present
  MRC p15,0,r1,c1,c0,2 ;// read coproc access to check if FPU is present
  TST r1,#0xF<<20
  BEQ skip_fpu_init

// enable fpu access
  VMRS r3,FPEXC
  ORR r1,r3,#1<<30
  VMSR FPEXC,r1
  VMOV d0,r0,r0
  VMOV d1,r0,r0
  VMOV d2,r0,r0
  VMOV d3,r0,r0
  VMOV d4,r0,r0
  VMOV d5,r0,r0
  VMOV d6,r0,r0
  VMOV d7,r0,r0
  VMOV d8,r0,r0
  VMOV d9,r0,r0
  VMOV d10,r0,r0
  VMOV d11,r0,r0
  VMOV d12,r0,r0
  VMOV d13,r0,r0
  VMOV d14,r0,r0
  VMOV d15,r0,r0

// restore previous value
  VMSR FPEXC,r3

skip_fpu_init:
// restore previous value
  MCR p15,0,r2,c1,c0,2

// MPU
// read MPU type register
  MRC p15,0,r1,c0,c0,4
  UBFX r1,r1,#8,#8 ;// r1 contains the number of regions
  SUB r1,r1,#1
  SUB r2,r1,#8 ;// Note: we must not set regions 0 to 7

clear_mpu_regions:
  MCR p15,0,r1,c6,c2,0 ;// set region number
  MCR p15,0,r0,c6,c1,0 ;// base addr
  MCR p15,0,r0,c6,c1,2 ;// size & enable
  MCR p15,0,r0,c6,c1,4 ;// access control
  SUB r1,r1,#1
  SUBS r2,r2,#1
  BGE clear_mpu_regions

  MCR p15,0,r0,c6,c2,0 ;// region number

// CP15
  MCR p15,2,r0,c0,c0,0 ;// cache size sel
  MCR p15,0,r0,c5,c0,0 ;// DFSR
  MCR p15,0,r0,c5,c0,1 ;// IFSR
  MCR p15,0,r0,c6,c0,0 ;// DFAR
  MCR p15,0,r0,c6,c0,2 ;// IFAR
  MCR p15,0,r0,c13,c0,1 ;// context id
  MCR p15,0,r0,c13,c0,2 ;// thread_id2
  MCR p15,0,r0,c13,c0,3 ;// thread_id3
  MCR p15,0,r0,c13,c0,4 ;// thread_id4
  MCR p15,0,r0,c15,c1,1 ;// ram access data0
  MCR p15,0,r0,c15,c1,2 ;// ram access data1
  MCR p15,0,r0,c15,c1,3 ;// ram access ecc

// ECC error bank
#if 0 // is done later
  MCR p15,0,r0,c15,c2,0 ;// dside ECC error bank #0
  MCR p15,0,r0,c15,c2,1 ;// dside ECC error bank #1
  MCR p15,0,r0,c15,c2,2 ;// dside ECC error bank #2
  MCR p15,0,r0,c15,c3,0 ;// iside ECC error bank #0
  MCR p15,0,r0,c15,c3,1 ;// iside ECC error bank #1
  MCR p15,0,r0,c15,c3,2 ;// iside ECC error bank #2
  MCR p15,0,r0,c15,c4,0 ;// dtcm ECC error bank
  MCR p15,0,r0,c15,c5,0 ;// itcm ECC error bank
#endif
// PMU
  MCR p15,0,r0,c9,c13,0 ;// cycle count
// read PMCR
  MRC p15,0,r1,c9,c12,0
  UBFX r1,r1,#11,#5 ;// r1 contains the number of counters
  SUB r1,r1,#1

clear_pmu_counters:
  MCR p15,0,r1,c9,c12,5 ;// set counter number
  MCR p15,0,r0,c9,c13,1 ;// event sel
  MCR p15,0,r0,c9,c13,2 ;// counter value
  SUBS r1,r1,#1
  BGE clear_pmu_counters

// CP14
  MRC p14, 0, r1, c0, c1, 0 ;// get DBGDSCR
  tst r1,#0xc000
  bne skip_debug_regs ;// if debugger is used

// deletes debugger breakpoints
  MCR p14,0,r0,c0,c0,4 ;// bvr0
  MCR p14,0,r0,c0,c1,4 ;// bvr1
  MCR p14,0,r0,c0,c2,4 ;// bvr2
  MCR p14,0,r0,c0,c3,4 ;// bvr3
  MCR p14,0,r0,c0,c4,4 ;// bvr4
  MCR p14,0,r0,c0,c5,4 ;// bvr5
  MCR p14,0,r0,c0,c0,5 ;// bcr0
  MCR p14,0,r0,c0,c1,5 ;// bcr1
  MCR p14,0,r0,c0,c2,5 ;// bcr2
  MCR p14,0,r0,c0,c3,5 ;// bcr3
  MCR p14,0,r0,c0,c4,5 ;// bcr4
  MCR p14,0,r0,c0,c5,5 ;// bcr5
  MCR p14,0,r0,c0,c0,6 ;// wvr0
  MCR p14,0,r0,c0,c1,6 ;// wvr1
  MCR p14,0,r0,c0,c2,6 ;// wvr2
  MCR p14,0,r0,c0,c3,6 ;// wvr3
  MCR p14,0,r0,c0,c0,7 ;// wcr0
  MCR p14,0,r0,c0,c1,7 ;// wcr1
  MCR p14,0,r0,c0,c2,7 ;// wcr2
  MCR p14,0,r0,c0,c3,7 ;// wcr3
  MCR p14,0,r0,c0,c6,0 ;// wfar

skip_debug_regs:
#if 0 // done later
  ISB
  DSB
#endif

  /*Configure D-TCM to base Address 0x00008000*/
  mrc   p15, 0, r0, c9, c1, 0    /*Read DTCM Region Register*/
  ldr   r3, =0x0FFF
  and   r0, r0, r3 
  orr   r0, r0, #0x8000 
  mcr   p15, 0, r0, c9, c1, 0    /*Write DTCM Region Register*/
  
  /* Init ITCM */
  mov   r0, #0x80
  mov   r2, #0x0               /* ITCM Address*/
  ldr   r3, =0x8000            /* ITCM Size 32KB  */

  /* Don't Overwrite the first 0x80 bytes */
  /* Exception Table */
  ldm   r2, {r4-r11}
  stm   r2!, {r4-r11}
  ldm   r2, {r4-r11}
  stm   r2!, {r4-r11}
  ldm   r2, {r4-r11}
  stm   r2!, {r4-r11}
  ldm   r2, {r4-r11}
  stm   r2!, {r4-r11}

  mov   r4, #0
  mov   r5, #0
  mov   r6, #0
  mov   r7, #0
  mov   r8, #0
  mov   r9, #0
  mov   r10, #0
  mov   r11, #0

  /* Init DTCM */
  mov   r0, #0
  ldr   r2, =0x00008000        /* DTCM Address  */
  ldr   r3, =0x8000            /* DTCM Size 32KB  */
1:
  stm   r2!, {r4-r11}
  add   r0, r0, #32
  cmp   r0, r3
  bne   1b

  /* Clear Error Bank Registers */
  mov   r0, #0

  mcr   p15,0,r0,c15,c2,0 /* DEER ECC error bank #0 */
  mcr   p15,0,r0,c15,c2,1 /* DEER ECC error bank #1 */
  mcr   p15,0,r0,c15,c2,2 /* DEER ECC error bank #2 */
  mcr   p15,0,r0,c15,c3,0 /* IEER ECC error bank #0 */
  mcr   p15,0,r0,c15,c3,1 /* IEER ECC error bank #1 */
  mcr   p15,0,r0,c15,c3,2 /* IEER ECC error bank #2 */
  mcr   p15,0,r0,c15,c4,0 /* DTCM ECC error bank */

  /* Enable Cache ECC */
  mrc   p15, 0, r0, c1, c0, 1    /* Read ACTLR */
  orr   r0, r0, #0x0200          /* Enable DTCM and DCACHE ECC */
  mcr   p15, 0, r0, c1, c0, 1    /* Read ACTLR */

  /* Stack initialization */
  ldr r1, =Image$$data_MK_c0_kernelStack$$Base     /* load kernel base stack address in r1 */
  ldr r2, =Image$$bss_MK_c0_kernelStack$$ZI$$Limit /* load kernel top stack address in r2 */
  ldr r3, =0xEBEBEBEB                              /* load stack init pattern in r3 */
  Stack_init_loop_core_0:                          /* stack init loop */
  str r3, [r1]                                     /* store init pattern at current address */
  add r1, r1, #4                                   /* move to next address */
  cmp r1, r2                                       /* compare current address with top stack address */
  bne Stack_init_loop_core_0                       /* if it is not the same repeat stack init loop*/

// ensure all settings are finished before enabling Lockstep mode
  ISB
  DSB

  /* Requirement L3_SW_4724  part (3) */
  /* Enable Lockstep */
  ldr   r1, =0xE6271800          /* Address of COMPENABLE */
  ldr   r2, [r1]
  ldr   r3, =0x80000000
  orr   r3, r3, r2
  str   r3, [r1]

  bx    lr


.type STARTUP_v_EnableICache, "function"
STARTUP_v_EnableICache:
  /* initialize L1 caches */
  mov   r0, #0
  mcr   p15, 0, r0, c7, c5, 6     /* invalidate BTAC */
  mcr   p15, 0, r0, c7, c5, 0     /* invalidate I cache */

  /* enable I cache */
  mrc   p15, 0, r1, c1, c0, 0
  orr   r1, r1, #0x1800         /* L1 I and BAT cache */
  mcr   p15, 0, r1, c1, c0, 0

  isb   sy
  b   forwardI
forwardI:
  bx    lr

.section .text, "ax"

.type STARTUP_v_EnableCache, "function"
STARTUP_v_EnableCache:

  /* initialize L1 caches */
  mov   r0, #0
  mcr   p15, 0, r0, c7, c5, 6     /* invalidate BTAC */
  mcr   p15, 0, r0, c7, c5, 0     /* invalidate I cache */

  mov   r0, #0
  mcr   p15, 2, r0, c0, c0, 0     /* select D cache */
  mrc   p15, 1, r0, c0, c0, 0     /* read CCSIDR */
  mov   r0, r0, ASR #13
  ldr   r3, =0x3FFF
  and   r0, r0, r3
  cmp   r0, #0x7F
  moveq r12, #0x1000           /* 4KB * 4way = 16KB */
  beq   invalidateDCache
  cmp   r0, #0xFF
  moveq r12, #0x2000           /* 32KB */
  beq   invalidateDCache
  movne r12, #0x4000           /* 64KB */

invalidateDCache:
  mov   r0, #0x00000000
  mov   r1, #0x40000000
  mov   r2, #0x80000000
  mov   r3, #0xC0000000

invalidateDCacheBySet:
  mcr   p15, 0, r0, c7, c6, 2     /* invalidate dcache on way 0 */
  mcr   p15, 0, r1, c7, c6, 2     /* invalidate dcache on way 1 */
  mcr   p15, 0, r2, c7, c6, 2     /* invalidate dcache on way 2 */
  mcr   p15, 0, r3, c7, c6, 2     /* invalidate dcache on way 3 */
  add   r0, r0, #0x20
  add   r1, r1, #0x20
  add   r2, r2, #0x20
  add   r3, r3, #0x20

  cmp   r0, r12
  bne   invalidateDCacheBySet

  /* enable I and D cache */
  mrc   p15, 0, r1, c1, c0, 0
  orr   r1, r1, #0x1000         /* L1 I cache */
  orr   r1, r1, #0x0004         /* L1 D cache */
  orr   r1, r1, #0x0800         /* branch prediction on */
  mcr   p15, 0, r1, c1, c0, 0

  isb   sy
  b   forward
forward:
  bx    lr


/* The startup exception table.
*/
.section .startup_exceptiontable, "ax"

STARTUP_ExceptionTable:

  ldr   pc, trapStartup_reset
  ldr   pc, trapStartup_undef
  ldr   pc, trapStartup_sbreak
  ldr   pc, trapStartup_prefetch_abort
  ldr   pc, trapStartup_data_abort
  ldr   pc, trapStartup_reserved
  ldr   pc, trapStartup_irq
  ldr   pc, trapStartup_fiq

trapStartup_reset:
  .word   OSVR_Entry

trapStartup_undef:
  .word   MK_Ex_Startup_Undefined

trapStartup_sbreak:
  .word   MK_Ex_Startup_Svc

trapStartup_prefetch_abort:
  .word   MK_Ex_Startup_PrefetchAbort

trapStartup_data_abort:
  .word   MK_Ex_Startup_DataAbort

trapStartup_reserved:
  .word   MK_Ex_Startup_Undefined

trapStartup_irq:
  .word   MK_Ex_Startup_Irq

trapStartup_fiq:
  .word   MK_Ex_Startup_Fiq

Startup_ExceptionTable_End:
  .word   0

