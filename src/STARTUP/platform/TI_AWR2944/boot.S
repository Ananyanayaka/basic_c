//******************************************************************************
//* BOOT  v16.9.4                                                              *
//*                                                                            *
//* Copyright (c) 1996-2019 Texas Instruments Incorporated                     *
//* http://www.ti.com/                                                         *
//*                                                                            *
//*  Redistribution and  use in source  and binary forms, with  or without     *
//*  modification,  are permitted provided  that the  following conditions     *
//*  are met:                                                                  *
//*                                                                            *
//*     Redistributions  of source  code must  retain the  above copyright     *
//*     notice, this list of conditions and the following disclaimer.          *
//*                                                                            *
//*     Redistributions in binary form  must reproduce the above copyright     *
//*     notice, this  list of conditions  and the following  disclaimer in     *
//*     the  documentation  and/or   other  materials  provided  with  the     *
//*     distribution.                                                          *
//*                                                                            *
//*     Neither the  name of Texas Instruments Incorporated  nor the names     *
//*     of its  contributors may  be used to  endorse or  promote products     *
//*     derived  from   this  software  without   specific  prior  written     *
//*     permission.                                                            *
//*                                                                            *
//*  THIS SOFTWARE  IS PROVIDED BY THE COPYRIGHT  HOLDERS AND CONTRIBUTORS     *
//*  "AS IS"  AND ANY  EXPRESS OR IMPLIED  WARRANTIES, INCLUDING,  BUT NOT     *
//*  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR     *
//*  A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT     *
//*  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,     *
//*  SPECIAL,  EXEMPLARY,  OR CONSEQUENTIAL  DAMAGES  (INCLUDING, BUT  NOT     *
//*  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,     *
//*  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY     *
//*  THEORY OF  LIABILITY, WHETHER IN CONTRACT, STRICT  LIABILITY, OR TORT     *
//*  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE     *
//*  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.      *
//*                                                                            *
//******************************************************************************

//****************************************************************************
//* BOOT.ASM
//*
//* THIS IS THE INITIAL BOOT ROUTINE FOR TMS470 C++ PROGRAMS.
//* IT MUST BE LINKED AND LOADED WITH ALL C++ PROGRAMS.
//*
//* THIS MODULE PERFORMS THE FOLLOWING ACTIONS:
//*   1) ALLOCATES THE STACK AND INITIALIZES THE STACK POINTER
//*   2) CALLS AUTO-INITIALIZATION ROUTINE
//*   3) CALLS THE FUNCTION MAIN TO START THE C++ PROGRAM
//*   4) CALLS THE STANDARD EXIT ROUTINE
//*
//* THIS MODULE DEFINES THE FOLLOWING GLOBAL SYMBOLS:
//*   1) __stack     STACK MEMORY AREA
//*   2) _c_int00    BOOT ROUTINE
//*
//****************************************************************************

//****************************************************************************
// Accomodate lowerd names in EABI
//****************************************************************************
    .set	PRE_INIT_RTN,     system_pre_init
    .set	ARGS_MAIN_RTN,    main
    .set	EXIT_RTN,         exit

//****************************************************************************
//*  32 BIT STATE BOOT ROUTINE                                               *
//****************************************************************************
    .global __stack

//****************************************************************************
//*  STACK ADDRESSES                                               *
//****************************************************************************
    .sect   ".bootCode"
    .extern __IRQ_STACK_END
    .extern __FIQ_STACK_END
    .extern __ABORT_STACK_END
    .extern __UND_STACK_END
    .extern __SVC_STACK_END
	.extern __USER_STACK_END
    
IRQ_STACK_ADDR:
    .word __IRQ_STACK_END
FIQ_STACK_ADDR:
	.word __FIQ_STACK_END
ABORT_STACK_ADDR:
	.word __ABORT_STACK_END
UND_STACK_ADDR:
	.word __UND_STACK_END
SVC_STACK_ADDR:
	.word __SVC_STACK_END
USER_STACK_ADDR:
	.word __USER_STACK_END

    .global _c_int00
/// @brief Function initlizes 
/// @id{L3_SWD_STARTUP_CINIT}, @satisfies{907532}, @satisfies{821669}, @satisfies{821670}
/// @impl{L3_SWD_STARTUP_CINIT}
//***************************************************************
//* FUNCTION DEF: _c_int00
//***************************************************************
_c_int00:
    //*------------------------------------------------------
    //* SETUP PRIVILEGED AND USER MODE ACCESS TO COPROCESSORS
    //* 10 AND 11, REQUIRED TO ENABLE NEON/VFP
    //* COPROCESSOR ACCESS CONTROL REG (CPACR)
    //* BITS [23:22] - CP11, [21:20] - CP10
    //* SET TO 0b11 TO ENABLE USER AND PRIV MODE ACCESS
    //*------------------------------------------------------
    MRC      p15,#0x0,r0,c1,c0,#2
    MOV      r3,#0xf00000
    ORR      r0,r0,r3
    MCR      p15,#0x0,r0,c1,c0,#2

    //*------------------------------------------------------
    //* ENABLE ECC FOR TCMA AND TCMB ON ACTLR
    //* AUXILIARY CONTROL REG (ACTLR)
    //* BITS [27:25] - B1TCMPCEN, B0TCMPCEN, ATCMPCEN
    //*------------------------------------------------------
    MRC      p15,#0x0,r0,c1,c0,#1
    MOV      r3,#0xe000000
    ORR      r0,r0,r3
    MCR      p15,#0x0,r0,c1,c0,#1

    //*------------------------------------------------------
    // SET THE EN BIT, FPEXC[30] TO ENABLE NEON AND VFP
    //*------------------------------------------------------
    MOV      r0,#0x40000000
    FMXR     FPEXC,r0

    //*------------------------------------------------------
    //* SET TO IRQ MODE
    //*------------------------------------------------------
    MRS     r0, cpsr
    BIC     r0, r0, #0x1F  // CLEAR MODES
    ORR     r0, r0, #0x12  // SET IRQ MODE
    MSR     cpsr_cf, r0

    //*------------------------------------------------------
    //* INITIALIZE THE IRQ MODE STACK
    //*------------------------------------------------------
    LDR     sp, IRQ_STACK_ADDR

    //*------------------------------------------------------
    //* SET TO FIQ MODE
    //*------------------------------------------------------
    MRS     r0, cpsr
    BIC     r0, r0, #0x1F  // CLEAR MODES
    ORR     r0, r0, #0x11  // SET FRQ MODE
    MSR     cpsr_cf, r0

    //*------------------------------------------------------
    //* INITIALIZE THE FIQ MODE STACK
    //*------------------------------------------------------
    LDR     sp, FIQ_STACK_ADDR

    //*------------------------------------------------------
    //* SET TO ABORT MODE
    //*------------------------------------------------------
    MRS     r0, cpsr
    BIC     r0, r0, #0x1F  // CLEAR MODES
    ORR     r0, r0, #0x17  // SET ABORT MODE
    MSR     cpsr_cf, r0

    //*------------------------------------------------------
    //* INITIALIZE THE ABORT MODE STACK
    //*------------------------------------------------------
    LDR     sp, ABORT_STACK_ADDR

    //*------------------------------------------------------
    //* SET TO UNDEFINED MODE
    //*------------------------------------------------------
    MRS     r0, cpsr
    BIC     r0, r0, #0x1F  // CLEAR MODES
    ORR     r0, r0, #0x1B  // SET UNDEFINED MODE
    MSR     cpsr_cf, r0

    //*------------------------------------------------------
    //* INITIALIZE THE UNDEFINED MODE STACK
    //*------------------------------------------------------
    LDR     sp, UND_STACK_ADDR

    //*------------------------------------------------------
    //* SET TO SUPERVISOR MODE
    //*------------------------------------------------------
    MRS     r0, cpsr
    BIC     r0, r0, #0x1F  // CLEAR MODES
    ORR     r0, r0, #0x13  // SET SUPERVISOR MODE
    MSR     cpsr_cf, r0

    //*------------------------------------------------------
    //* INITIALIZE THE SUPERVISOR MODE STACK
    //*------------------------------------------------------
    LDR     sp, SVC_STACK_ADDR

    //*------------------------------------------------------
    //* SET TO SYSTEM MODE
    //*------------------------------------------------------
    MRS     r0, cpsr
    BIC     r0, r0, #0x1F  // CLEAR MODES
    ORR     r0, r0, #0x1F  // SET SYSTEM MODE
    MSR     cpsr_cf, r0

    //*------------------------------------------------------
    //* INITIALIZE THE USER MODE STACK
    //*------------------------------------------------------
	LDR		sp, USER_STACK_ADDR
	
    //*-----------------------------------------------------
    //* ALIGN THE STACK TO 64-BITS (EABI)
    //*-----------------------------------------------------
    BIC     sp, sp, #0x07  // Clear upper 3 bits for 64-bit alignment.

    BL      platform_init

    //*------------------------------------------------------
    //* SET TO SYSTEM MODE
    //*------------------------------------------------------
    MRS     r0, cpsr
    BIC     r0, r0, #0x1F  // CLEAR MODES
    ORR     r0, r0, #0x1F  // SET SYSTEM MODE
    MSR     cpsr_cf, r0

    //*------------------------------------------------------
    //* Perform all the required initializations:
    //*   - Clear BSS and exception stack memory areas
    //*   - Call global constructors
    //*------------------------------------------------------
    BL      PRE_INIT_RTN

    //*------------------------------------------------------
    //* CALL APPLICATION
    //*------------------------------------------------------
    LDR     PC, jumptomain
jumptomain:
    .long ARGS_MAIN_RTN

    //*------------------------------------------------------
    //* IF APPLICATION DIDN'T CALL EXIT, CALL EXIT(1)
    //*------------------------------------------------------
    MOV     R0, #1
    BL      EXIT_RTN

    //*------------------------------------------------------
    //* DONE, LOOP FOREVER
    //*------------------------------------------------------
L1:B       L1

//***************************************************************
//* FUNCTION DEF: HF
//***************************************************************
    .global HF
HF:
L2: B   L2

//***************************************************************
//* CONSTANTS USED BY THIS MODULE
//***************************************************************


//******************************************************
//* UNDEFINED REFERENCES                               *
//******************************************************
    .global PRE_INIT_RTN
	.global ARGS_MAIN_RTN
	.global	EXIT_RTN
	.weak	platform_init
    .global	undefInstructionExptnHandler
    .global	swIntrExptnHandler
    .global	prefetchAbortExptnHandler
    .global	dataAbortExptnHandler
    .global	rsvdExptnHandler
    .global	irqExptnHandler
    .global	fiqExptnHandler

//****************************************************************************
// Setup reserved Handler
//****************************************************************************
    .global	_cslRsvdHandler
_cslRsvdHandler:
     b   _cslRsvdHandler
//****************************************************************************
// Setup Reset Vectors always in ARM mode
//****************************************************************************
    .arm
	.global	_resetvectors
    .sect   ".rstvectors"
_resetvectors:
        LDR pc, c_int00_addr        // Reset
        LDR pc, undefInst_addr      // Undefined Instruction
        LDR pc, swi_addr            // Software interrupt
        LDR pc, preAbort_addr       // Abort (prefetch)
        LDR pc, dataAbort_addr      // Abort (data)
        LDR pc, rsvd_addr           // rsvd
        LDR pc, irq_addr            // IRQ
        LDR pc, fiq_addr            // FIQ

c_int00_addr:
   .long _c_int00
undefInst_addr:
   .long undefInstructionExptnHandler
swi_addr:
   .long swIntrExptnHandler
preAbort_addr:
   .long prefetchAbortExptnHandler
dataAbort_addr:
   .long dataAbortExptnHandler
rsvd_addr:
   .long _cslRsvdHandler
irq_addr:
   .long irqExptnHandler
fiq_addr:
   .long fiqExptnHandler
.end
