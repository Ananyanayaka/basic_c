#include "AmbaCortexR52.h"

        .text

        .global _start
        .global _AmbaResetHandler
        .global WaitBoot
        .global BldStop
        .global InitWithDbse

_start:
//----------------------------------------------------------------
// Entry point for the Reset handler
//----------------------------------------------------------------

    .global Start
    .globl Start, $function

Start:
//----------------------------------------------------------------
// EL2 Exception Vector Table
//----------------------------------------------------------------
// Note: LDR PC instructions are used here, though branch (B) instructions
// could also be used, unless the exception handlers are >32MB away.

EL2_Vectors:
        LDR PC, EL2_Reset_Addr
        LDR PC, EL2_Undefined_Addr
        LDR PC, EL2_HVC_Addr
        LDR PC, EL2_Prefetch_Addr
        LDR PC, EL2_Abort_Addr
        LDR PC, EL2_HypModeEntry_Addr
        LDR PC, EL2_IRQ_Addr
        LDR PC, EL2_FIQ_Addr


EL2_Reset_Addr:         .word    EL2_Reset_Handler
EL2_Undefined_Addr:     .word    EL2_Undefined_Handler
EL2_HVC_Addr:           .word    EL2_HVC_Handler
EL2_Prefetch_Addr:      .word    EL2_Prefetch_Handler
EL2_Abort_Addr:         .word    EL2_Abort_Handler
EL2_HypModeEntry_Addr:  .word    EL2_HypModeEntry_Handler
EL2_IRQ_Addr:           .word    EL2_IRQ_Handler
EL2_FIQ_Addr:           .word    EL2_FIQ_Handler


//----------------------------------------------------------------
// EL2 Exception Handlers
//----------------------------------------------------------------

.globl EL2_Undefined_Handler, $function
EL2_Undefined_Handler:
        B   EL2_Undefined_Handler
.globl EL2_HVC_Handler, $function
EL2_HVC_Handler:
        B   EL2_HVC_Handler
.globl EL2_Prefetch_Handler, $function
EL2_Prefetch_Handler:
        B   EL2_Prefetch_Handler
.globl EL2_Abort_Handler, $function
EL2_Abort_Handler:
        B   EL2_Abort_Handler
.globl EL2_HypModeEntry_Handler, $function
EL2_HypModeEntry_Handler:
        B   EL2_HypModeEntry_Handler
.globl EL2_IRQ_Handler, $function
EL2_IRQ_Handler:
        B   EL2_IRQ_Handler
.globl EL2_FIQ_Handler, $function
EL2_FIQ_Handler:
        B   EL2_FIQ_Handler

//----------------------------------------------------------------
// EL1 Exception Vector Table
//----------------------------------------------------------------
// Note: LDR PC instructions are used here, though branch (B) instructions
// could also be used, unless the exception handlers are >32MB away.

    .align 5
EL1_Vectors:
        LDR PC, EL1_Reset_Addr
        LDR PC, EL1_Undefined_Addr
        LDR PC, EL1_SVC_Addr
        LDR PC, EL1_Prefetch_Addr
        LDR PC, EL1_Abort_Addr
        LDR PC, EL1_Reserved_Addr
        LDR PC, EL1_IRQ_Addr
        LDR PC, EL1_FIQ_Addr


EL1_Reset_Addr:     .word    EL1_Reset_Handler
EL1_Undefined_Addr: .word    EL1_Undefined_Handler
EL1_SVC_Addr:       .word    EL1_SVC_Handler
EL1_Prefetch_Addr:  .word    EL1_Prefetch_Handler
EL1_Abort_Addr:     .word    EL1_Abort_Handler
EL1_Reserved_Addr:  .word    EL1_Reserved
EL1_IRQ_Addr:       .word    EL1_IRQ_Handler
EL1_FIQ_Addr:       .word    EL1_FIQ_Handler


//----------------------------------------------------------------
// EL1 Exception Handlers
//----------------------------------------------------------------

.globl EL1_Undefined_Handler, $function
EL1_Undefined_Handler:
        B   EL1_Undefined_Handler
.globl EL1_SVC_Handler, $function
EL1_SVC_Handler:
        B   EL1_SVC_Handler
.globl EL1_Prefetch_Handler, $function
EL1_Prefetch_Handler:
        B   EL1_Prefetch_Handler
.globl EL1_Abort_Handler, $function
EL1_Abort_Handler:
        B   EL1_Abort_Handler
EL1_Reserved:
        B   EL1_Reserved
.globl EL1_IRQ_Handler, $function
EL1_IRQ_Handler:
        B   EL1_IRQ_Handler
.globl EL1_FIQ_Handler, $function
EL1_FIQ_Handler:
        B   EL1_FIQ_Handler

//----------------------------------------------------------------
// EL2 Reset Handler
//----------------------------------------------------------------
/*
    .section  RESET,"ax"
    .align 3
*/
#ifdef __THUMB__
    .thumb2
#endif

.globl EL2_Reset_Handler, $function
EL2_Reset_Handler:
        //check current EL
        MRS R0, cpsr
        AND R0, R0, #0x1F
        CMP R0, #0x1A
        BNE EL1_Reset_Handler

        // init redistributor of gic in r52
        /* r9: ReDistributor Base Address of Current CPU */
        ldr r9, =AMBA_CR52_GIC_REDISTRIBUTOR_BASE_ADDR
        mov r5, #~0x2
        ldr r6, [r9, #0x14]
        and r6, r6, r5      /* Clear ProcessorSleep */
        str r6, [r9, #0x14]
        dsb st
        isb
    3:  ldr r5, [r9, #0x14]
        and r6, r5, #0x4
        cmp r6, #0x4
        beq 3b

        // Change EL2 exception base address
        LDR r0, =EL2_Vectors
        MCR p15, 4, r0, c12, c0, 0      //  Write to HVBAR

        // Init HSCTLR
        LDR r0, =0x30C50818             // See TRM for decoding
        MCR p15, 4, r0, c1, c0, 0       // Write to HSCTLR

        // Enable EL1 access to all IMP DEF registers
        LDR r0, =0x3781
        DSB
        MCR p15, 4, r0, c1, c0, 1       // Write to HACTLR
        ISB

        // Change EL1 exception base address
        LDR r0, =EL1_Vectors
        MCR p15, 0, r0, c12, c0, 0      // Write to VBAR

        // Go to SVC mode
        MRS r0, cpsr
        MOV r1, #0x13
        BFI r0, r1, #0, #5
#ifdef __THUMB__
        ORR r0, r0, #(0x1 << 5)         // Set T bit
#endif
        MSR SPSR, r0
        LDR r0, =EL1_Reset_Handler
        MSR ELR_hyp, r0
        DSB
        ISB
        ERET

    .global __use_two_region_memory

EL1_Reset_Handler:

//----------------------------------------------------------------
// Disable MPU and caches
//----------------------------------------------------------------
        MRC p15, 0, r0, c1, c0, 0       // Read System Control Register
        BIC r0, r0, #0x01               // Disable MPU (M bit) and data cache (C bit)
        MOV R1, #0x1004
        ORR R0, R0, R1
        DSB                             // Ensure all previous loads/stores have completed
        MCR p15, 0, r0, c1, c0, 0       // Write System Control Register
        ISB                             // Ensure subsequent insts execute wrt new MPU settings

//----------------------------------------------------------------
// Initialize Stacks using Linker symbol from scatter file.
// ABT, IRQ, FIQ, UNDEF size = STACKSIZE, SVC the rest.
// Stacks must be 8 byte aligned.
//----------------------------------------------------------------

        LDR R0, =SysStackBaseSYS           /* X0 = pointer to the Stack pointer array */
        LDR R2, [R0]                    /* X2 = pointer to the End of Stack */
        MOV SP, R2                      /* set stack pointer */

//----------------------------------------------------------------
// Cache invalidation. However Cortex-R52 provides CFG signals to
// invalidate cache automatically out of reset (CFGL1CACHEINVDISx)
//----------------------------------------------------------------

        DSB             // Complete all outstanding explicit memory operations

        MOV r0, #0
        MCR p15, 0, r0, c7, c5, 0       // Invalidate entire instruction cache

        // Invalidate Data/Unified Caches
        MRC     p15, 1, r0, c0, c0, 1      // Read CLIDR
        ANDS    r3, r0, #0x07000000        // Extract coherency level
        MOV     r3, r3, LSR #23            // Total cache levels << 1
        BEQ     Finished                   // If 0, no need to clean

        MOV     r10, #0                    // R10 holds current cache level << 1
Loop1:  ADD     r2, r10, r10, LSR #1       // R2 holds cache "Set" position
        MOV     r1, r0, LSR r2             // Bottom 3 bits are the Cache-type for this level
        AND     r1, r1, #7                 // Isolate those lower 3 bits
        CMP     r1, #2
        BLT     Skip                       // No cache or only instruction cache at this level

        MCR     p15, 2, r10, c0, c0, 0     // Write the Cache Size selection register
        ISB                                // ISB to sync the change to the CacheSizeID reg
        MRC     p15, 1, r1, c0, c0, 0      // Reads current Cache Size ID register
        AND     r2, r1, #7                 // Extract the line length field
        ADD     r2, r2, #4                 // Add 4 for the line length offset (log2 16 bytes)
        LDR     r4, =0x3FF
        ANDS    r4, r4, r1, LSR #3         // R4 is the max number on the way size (right aligned)
        CLZ     r5, r4                     // R5 is the bit position of the way size increment
        LDR     r7, =0x7FFF
        ANDS    r7, r7, r1, LSR #13        // R7 is the max number of the index size (right aligned)

Loop2:  MOV     r9, r4                     // R9 working copy of the max way size (right aligned)

#ifdef __THUMB__
Loop3:  LSL     r12, r9, r5
        ORR     r11, r10, r12              // Factor in the Way number and cache number into R11
        LSL     r12, r7, r2
        ORR     r11, r11, r12              // Factor in the Set number
#else
Loop3:  ORR     r11, r10, r9, LSL r5       // Factor in the Way number and cache number into R11
        ORR     r11, r11, r7, LSL r2       // Factor in the Set number
#endif
        MCR     p15, 0, r11, c7, c6, 2     // Invalidate by Set/Way
        SUBS    r9, r9, #1                 // Decrement the Way number
        BGE     Loop3
        SUBS    r7, r7, #1                 // Decrement the Set number
        BGE     Loop2
Skip:   ADD     r10, r10, #2               // Increment the cache number
        CMP     r3, r10
        BGT     Loop1

Finished:
        MRC     p15, 0, R0, c1, c0, 2       /* Read CPACR */
        ORR     R0, R0, #0xf00000           /* CPACR[23:22] = Full access CP11, CPACR[21:20] = Full access CP10 */
        MCR     p15, 0, R0, c1, c0, 2
        ISB

/**
 *  Add more initialization here
 */

/**
 *  C initialization here
 */
        LDR     R0, BSS_START
        LDR     R1, BSS_END
        BIC     R1, R1, #0x3
        ADD     R1, R1, #4

        MOV     R3, #0
        MOV     R4, #0
        MOV     R5, #0
        MOV     R6, #0
        MOV     R7, #0
        MOV     R8, #0
        MOV     R9, #0
        MOV     R10, #0
        MOV     R11, #0
        MOV     R12, #0
BssZeroInit:
        STMIA   R0!, {R3-R12}
        SUBS    R2, R1, R0
        CMP     R2, #40
        BGE     BssZeroInit

        CMP     R2, #0
        BEQ     BssInitDone

BssZeroInitRemain:
        STMIA  R0!, {R3}
        SUBS    R2, R2, #4
        BGT     BssZeroInitRemain

BssInitDone:

        MOV     R0, #0x40000000             /* FPEXC[30] = Enable Advanced SIMD and VFP extensions */
        VMSR    FPEXC, R0
        MOV     R0, #0x3000000              /* FPSCR[26] = IEEE 754, FPSCR[25] = Default NaN, FPSCR[24] = Flush-to-zero, FPSCR[23:22] = Round to nearest */
        VMSR    FPSCR, R0

        BL      main

/*
 *  BLD stop here
 */
.globl BldStop
BldStop:
        B       BldStop

BSS_START:          .word   __bss_start
BSS_END:            .word   __bss_end
