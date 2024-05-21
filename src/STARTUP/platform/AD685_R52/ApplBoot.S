#include "AmbaCortexR52.h"

/*-----------------------------------------------------------------------------------------------*\
 *   Mode, corresponds to bits 0-5 in CPSR
\*-----------------------------------------------------------------------------------------------*/
#define MODE_MASK   0x1F    /* Bit mask for mode bits in CPSR */
#define FIQ_DISABLE 0x40    /* ARM 1= disable FIQ */
#define IRQ_DISABLE 0x80    /* ARM 1= disable IRQ */

#define USR_MODE    0xD0    /* User mode */
#define FIQ_MODE    0xD1    /* Fast Interrupt Request mode */
#define IRQ_MODE    0xD2    /* Interrupt Request mode */
#define SVC_MODE    0xD3    /* Supervisor mode */
#define ABT_MODE    0xD7    /* Abort mode */
#define UND_MODE    0xDB    /* Undefined Instruction mode */
#define SYS_MODE    0xDF    /* System mode */

        .text
        .arm
        .global _start

        .align 5                            /* Vector Base Address shall be 32-byte alignment */
_start:                                     /* Make this a DATA label, so that stack usage */
                                            /* analysis does not consider it an uncalled function */

        B       Reset_Handler               /* 0x00: Reset (RESET: Supervisor) */
        B       Undefined_Handler           /* 0x04: Undefined intruction (UNDEF) */
        B       SWI_Handler                 /* 0x08: Software Interrupt (SWI: Supervisor) */
        B       Prefetch_Handler            /* 0x0c: Prefetch Abort (PABT) */
        B       DataAbort_Handler           /* 0x10: Data Abort (DABT) */
        B       .                           /* 0x14: Reserved */
        LDR     PC, IRQ_IsrAddr             /* 0x18: IRQ */
        LDR     PC, FIQ_IsrAddr             /* 0x1c: FIQ */

/*-----------------------------------------------------------------------------------------------*\
 *   Default exception handlers
\*-----------------------------------------------------------------------------------------------*/
.macro DumpRegisters
        CPSID   if                          @ Disable FIQ and IRQ interrupts
        STMDB   SP!, {R0 - R7}              @ Save r0 - r7
        MOV     R7, SP                      @ Use R7 as SP

        MOV     R1, LR                      @ Get preview mode PC
        MRS     R2, SPSR                    @ Read current SPSR
        MRS     R3, CPSR                    @ Read current CPSR
        ORR     R4, R2, #0xc0               @ Set FIQ and IRQ disable bit flags

        MSR     CPSR_c, R4                  @ Switch to previous mode
        STMDB   R7!, {R8 - R14}             @ Save r8 - r14

        MSR     CPSR_cxsf, R3               @ Switch back to current mode
        STMDB   R7!, {R1, R2}               @ Save preview mode PC, CPSR
        MOV     SP, R7                      @ Restore R7 to SP
.endm


Undefined_Handler:
        DumpRegisters
        MOV     R0, #0
        MOV     R1, SP
        LDR     PC, Excp_HndlAddr

SWI_Handler:
        B       SWI_Handler

Prefetch_Handler:
        DumpRegisters
        MOV     R0, #1
        MOV     R1, SP
        LDR     PC, Excp_HndlAddr

DataAbort_Handler:
        DumpRegisters
        MOV     R0, #2
        MOV     R1, SP
        LDR     PC, Excp_HndlAddr


/*-----------------------------------------------------------------------------------------------*\
 *   low-level system initialization code.
 *
 *   After a reset execution starts here, the mode is ARM, supervisor
 *   with interrupts disabled.
\*-----------------------------------------------------------------------------------------------*/
Reset_Handler:
/*---------------------------------------------------------------------------*\
 *  Initialise the processor
\*---------------------------------------------------------------------------*/

        MRC    p15, 0, R0, CR1, CR0, 0      /* read SCTLR */
        MOV    R1, #0x1004
        ORR    R0, R0, R1
        MCR    p15, 0, R0, c1, c0, 0        /* SCTLR */
        DSB
        ISB
        LDR    R0, =_start
        MCR    p15, 0, R0, c12, c0, 0       /* VBAR */

/*---------------------------------------------------------------------------*\
 *  R0 = CPSR, R1 = Current Running Core-ID, bit [1:0]
\*---------------------------------------------------------------------------*/
        MRS     R0, CPSR                    /* get current mode bits to R0 */
        BIC     R4, R0, #MODE_MASK          /* clear mode */

/*---------------------------------------------------------------------------*\
 *  enter FIQ mode and setup stack
\*---------------------------------------------------------------------------*/
        ORR     R0, R4, #FIQ_MODE           /* set to FIQ mode, disable FIR/IRQ interrupts */
        MSR     CPSR_c, R0                  /* enter FIQ mode */
        LDR     R2, =SysStackBaseFIQ    /* R2 = pointer to the End of Stack */
        LDR     SP, [R2]                /* set stack pointer to the End of Stack */

/*---------------------------------------------------------------------------*\
 *  enter IRQ mode and setup stack
\*---------------------------------------------------------------------------*/
        ORR     R0, R4, #IRQ_MODE           /* set to IRQ mode, disable FIR/IRQ interrupts */
        MSR     CPSR_c, R0                  /* enter IRQ mode */
        LDR     R2, =SysStackBaseIRQ    /* R2 = pointer to the End of Stack */
        LDR     SP, [R2]                /* set stack pointer to the End of Stack */

/*---------------------------------------------------------------------------*\
 *  enter ABT mode and setup stack
\*---------------------------------------------------------------------------*/
        ORR     R0, R4, #ABT_MODE           /* set to ABT mode, disable FIR/IRQ interrupts */
        MSR     CPSR_c, R0                  /* enter ABT mode */
        LDR     R2, =SysStackBaseABT    /* R2 = pointer to the End of Stack */
        LDR     SP, [R2]                /* set stack pointer to the End of Stack */

/*---------------------------------------------------------------------------*\
 *  enter UND mode and setup stack
\*---------------------------------------------------------------------------*/
        ORR     R0, R4, #UND_MODE           /* set to UND mode, disable FIR/IRQ interrupts */
        MSR     CPSR_c, R0                  /* enter UND mode */
        LDR     R2, =SysStackBaseUND    /* R2 = pointer to the End of Stack */
        LDR     SP, [R2]                /* set stack pointer to the End of Stack */

/*---------------------------------------------------------------------------*\
 *  enter system (USER) mode and setup stack
\*---------------------------------------------------------------------------*/
        ORR     R0, R4, #SYS_MODE           /* set to SYS mode, disable FIR/IRQ interrupts */
        MSR     CPSR_c, R0                  /* enter SVC mode */
        LDR     R2, =SysStackBaseSYS    /* R2 = pointer to the End of Stack */
        LDR     SP, [R2]                /* set stack pointer to the End of Stack */

/*---------------------------------------------------------------------------*\
 *  enter SVC mode and setup stack
\*---------------------------------------------------------------------------*/
        ORR     R0, R4, #SVC_MODE           /* set to SVC mode, disable FIR/IRQ interrupts */
        MSR     CPSR_c, R0                  /* enter SVC mode */
        LDR     R2, =SysStackBaseSVC    /* R2 = pointer to the End of Stack */
        LDR     SP, [R2]                /* set stack pointer to the End of Stack */

/*---------------------------------------------------------------------------*\
 *  Initialize Architectural Features (Floating-point, Advanced SIMD).
\*---------------------------------------------------------------------------*/
        MRC     p15, 0, R0, c1, c0, 2       /* Read CPACR */
        ORR     R0, R0, #0xf00000           /* CPACR[23:22] = Full access CP11, CPACR[21:20] = Full access CP10 */
        MCR     p15, 0, R0, c1, c0, 2
        ISB
        MOV     R0, #0x40000000             /* FPEXC[30] = Enable Advanced SIMD and VFP extensions */
        VMSR    FPEXC, R0
        MOV     R0, #0x3000000              /* FPSCR[26] = IEEE 754, FPSCR[25] = Default NaN, FPSCR[24] = Flush-to-zero, FPSCR[23:22] = Round to nearest */
        VMSR    FPSCR, R0

RtosCoreInit:
/*---------------------------------------------------------------------------*\
 *  Add more initialization here
\*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*\
 *  C initialization here
\*---------------------------------------------------------------------------*/
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
        STMIA   R0!, {R3}
        SUBS    R2, R2, #4
        BGT     BssZeroInitRemain

BssInitDone:
@ Enable IRQ and FIQ interrupts
        CPSIE   if
/*---------------------------------------------------------------------------*\
 *  Continue to __cmain for C-level initialization
\*---------------------------------------------------------------------------*/
        B       main

/*---------------------------------------------------------------------------*\
 *  Get CPSR (Current Program Status Register)
\*---------------------------------------------------------------------------*/
.global get_CPSR
get_CPSR:
        MRS     R0, CPSR
        BX      LR

/*---------------------------------------------------------------------------*\
 *  Set CPSR (Current Program Status Register)
\*---------------------------------------------------------------------------*/
.global set_CPSR
set_CPSR:
        MSR     CPSR_cxsf, R0
        BX      LR

/*---------------------------------------------------------------------------*\
 *  Hypervisor Call ( HVC )
\*---------------------------------------------------------------------------*/
.global AmbaHvcCall
AmbaHvcCall:
        HVC     #0
        BX      LR

BSS_START:      .word   __bss_start
BSS_END:        .word   __bss_end
IRQ_IsrAddr:    .word   __tx_irq_handler
FIQ_IsrAddr:    .word   __tx_fiq_handler
Excp_HndlAddr:  .word   __amba_assert

/*
 *  Assertion trap
 */
.global __amba_assert
__amba_assert:
        CPSID   if
        SEV
        WFE
        WFE
        B       __amba_assert


@VOID   __tx_irq_handler(VOID)
@{
    .global  __tx_irq_handler
    .global  __tx_irq_processing_return
__tx_irq_handler:
@
@    /* Jump to context save to save system context.  */
    B       _thread_context_save
__tx_irq_processing_return:
@
@    /* Enable FIQ.  */
    CPSIE   f
@
@    /* At this point execution is still in the IRQ mode.  The CPSR, point of
@       interrupt, and all C scratch registers are available for use.  In
@       addition, IRQ interrupts may be re-enabled - with certain restrictions -
@       if nested IRQ interrupts are desired.  Interrupts may be re-enabled over
@       small code sequences where lr is saved before enabling interrupts and
@       restored after interrupts are again disabled.  */
@
@
@    /* Application IRQ handlers can be called here!  */
    BL      irq_handler                  @ Ambarella C-ISR for IRQ
@
@    /* Jump to context restore to restore system context.  */
    B       _thread_context_restore
@}

@VOID   __tx_fiq_handler(VOID)
@{
    .global  __tx_fiq_handler
    .global  __tx_fiq_processing_return
__tx_fiq_handler:
@
@    /* Jump to fiq context save to save system context.  */
    B       _thread_fiq_context_save
__tx_fiq_processing_return:
@
@    /* At this point execution is still in the FIQ mode.  The CPSR, point of
@       interrupt, and all C scratch registers are available for use.  */
@
@    /* Application FIQ handlers can be called here!  */
    BL      firq_handler                  @ Ambarella C-ISR for FIQ
@
@    /* Jump to fiq context restore to restore system context.  */
    B       _thread_fiq_context_restore
@}

@VOID   _thread_context_save(VOID)
@{
    .global  _thread_context_save
_thread_context_save:
@
@    /* Upon entry to this routine, it is assumed that IRQ interrupts are locked
@       out, we are in IRQ mode, and all registers are intact.  */
@
    STMDB   sp!, {r0-r3}                        @ Save some working registers
@
@   /* Save the rest of the scratch registers on the stack and return to the
@      calling ISR.  */
@
    MRS     r0, SPSR                            @ Pickup saved SPSR
    SUB     lr, lr, #4                          @ Adjust point of interrupt
    STMDB   sp!, {r0, r10, r12, lr}             @ Store other registers
@
    CPSID   if                                  @ Disable FIQ interrupts
@
@   MOV     r10, #0                             @ Clear stack limit
    B       __tx_irq_processing_return          @ Continue IRQ processing
@}

@VOID   _thread_context_restore(VOID)
@{
    .global  _thread_context_restore
_thread_context_restore:
@
@    /* Lockout interrupts.  */
@
    CPSID   if                                  @ Disable IRQ and FIQ interrupts
@
@    /* Restore interrupted thread or ISR.  */
@   /* Recover the saved context and return to the point of interrupt.  */
@
    LDMIA   sp!, {r0, r10, r12, lr}             @ Recover SPSR, POI, and scratch regs
    MSR     SPSR_cxsf, r0                       @ Put SPSR back
    LDMIA   sp!, {r0-r3}                        @ Recover r0-r3
    MOVS    pc, lr                              @ Return to point of interrupt
@
@}

@ VOID   _thread_fiq_context_save(VOID)
@{
    .global  _thread_fiq_context_save
_thread_fiq_context_save:
    STMDB   sp!, {r0-r3}                        @ Save some working registers
@
@    /* Save the rest of the scratch registers on the stack and return to the
@       calling ISR.  */
@
    MRS     r0, SPSR                            @ Pickup saved SPSR
    SUB     lr, lr, #4                          @ Adjust point of interrupt
    STMDB   sp!, {r0, r10, r12, lr}             @ Store other registers

    B       __tx_fiq_processing_return          @ Continue FIQ processing
@}

@VOID   _thread_fiq_context_restore(VOID)
@{
    .global  _thread_fiq_context_restore
_thread_fiq_context_restore:
@
@    /* Lockout interrupts.  */
@
    CPSID   if                                  @ Disable IRQ and FIQ interrupts
@
@
    LDMIA   sp!, {r0, r10, r12, lr}             @ Recover SPSR, POI, and scratch regs
    MSR     SPSR_cxsf, r0                       @ Put SPSR back
    LDMIA   sp!, {r0-r3}                        @ Recover r0-r3
    MOVS    pc, lr                              @ Return to point of interrupt
@}
