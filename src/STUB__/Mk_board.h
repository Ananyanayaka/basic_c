/* Mk_board.h - board specific header file
 *
 * This file contains example implementations for those definitions which shall
 * be developed according to safety standards. When documented in the Safety
 * Manual as being safety-relevant, they have to be replaced by project-specific
 * implementations which have to be assessed to the required ASIL level.
 *
 * Warning: This file has not been developed in accordance with a safety
 * standard (no ASIL)!
 *
 * (c) Elektrobit Automotive Software
 *
 * $Id: Mk_board.h 1.25 2022/04/04 15:38:21CEST Singh, Vishal03 (uidd7889) none  $
*/

/* MISRA-C:2004 Deviation List
 *
 * MISRA-1) Deviated Rule: 6.3 (advisory)
 *  Typedefs that indicate size and signedness should be used in place
 *  of the basic types.
 *
 * Reason:
 *  By convention the signature of main returns a plain integer.
*/
// #ifndef MK_BOARD_H
// #define MK_BOARD_H

/* V3M uses 2.0.34 mk os where as V3H uses 2.0.153 mk os */
/* This needs to be updated once V3M EB os is updated */

// PRQA S 2000,2300,1020,1021,1025,1026 ++
// 2020-04-12; uidg3207; summary: Msg(3:1020) Avoid macros. , Msg(3:2000,2300) The function/object   '%1s' is in the global scope.
// 2020-04-12; uidg3207; summary: Msg(4:1021,1025,1026) Macro as literal ...
// reason:  implementation is in C, fix of C++ qac warnings not feasible! See CCB Request Issue 1557905
// #if defined(RCAR_V3H)
// /* Verify that the version of the board files matches the version of the microkernel */
// #include <Mk_Version.h>
// #include <Mk_board_version_check.h>
// #endif

// #include <public/Mk_timeconversion64.h>
// #include <public/Mk_public_types.h>
// #include <private/Mk_memoryprotection.h>
// #include <private/Mk_panic.h>

// #ifndef MK_ASM

/* Define page-table heap sizes for each core.
 * Type 0 heap is used for generating the flat partition map, count must be 1.
 * Type 1 heap is used for page tables, the count defines the number of tables
 * (4096 bytes each).
*/
// #define MK_BOARD_C0_HEAPTYPE0_COUNT		1U
// #define MK_BOARD_C0_HEAPTYPE1_COUNT		1024U
// #define MK_BOARD_C1_HEAPTYPE0_COUNT		1U
// #define MK_BOARD_C1_HEAPTYPE1_COUNT		1024U
// #ifdef RCAR_V3H
// #define MK_BOARD_C2_HEAPTYPE0_COUNT		1U
// #define MK_BOARD_C2_HEAPTYPE1_COUNT		1024U
// #define MK_BOARD_C3_HEAPTYPE0_COUNT		1U
// #define MK_BOARD_C3_HEAPTYPE1_COUNT		1024U
// #endif

/* Conversion macros for the timestamp.
 *
 * MK_TimestampNsToTicks() converts the parameter (time in nanoseconds) to ticks of the timebase/decrementer.
 * MK_TimestampTicksToNs(0 converts the parameter (no. of ticks) to nanoseconds. If the time would be larger
 * than can be represented by a 64-bit unsigned number, the maximum possible (2^64-1) is returned.
 *
 * Both of the above conversions are performed using integer arithmetic, so can be used to initialize constants.
 *
 * MK_TimestampNsToTicksF() and MK_TimestampTicksToNsF() perform the equivalent conversions using a call to a
 * dedicated multiplication/division function. The advantage is better accuracy, at the cost of slightly more
 * CPU time.
 *
 * The timestamp uses the virtual timer which is based on the system counter. It runs with the frequency
 * EXTAL/2 = 20 MHz/2 = 10 MHz.
*/
// #define MK_TimestampNsToTicks(ns)	MK_NsToTicks_10000000((ns))
// #define MK_TimestampNsToTicksF(ns)	MK_NsToTicksF_10000000((ns))
// #define MK_TimestampTicksToNs(tx)	MK_TicksToNs_10000000((tx))
// #define MK_TimestampTicksToNsF(tx)	MK_TicksToNsF_10000000((tx))

/* Conversion macros for the mk-internal ticker timers.
 *
 * MK_TickerNsToTicks() converts the parameter (time in nanoseconds) to ticks of the timebase/decrementer.
 * MK_TickerTicksToNs(0 converts the parameter (no. of ticks) to nanoseconds. If the time would be larger
 * than can be represented by a 64-bit unsigned number, the maximum possible (2^64-1) is returned.
 *
 * Both of the above conversions are performed using integer arithmetic, so can be used to initialize constants.
 *
 * MK_TickerNsToTicksF() and MK_TickerTicksToNsF() perform the equivalent conversions using a call to a
 * dedicated multiplication/division function. The advantage is better accuracy, at the cost of slightly more
 * CPU time.
 *
 * The default frequency depends on the TMU group. We use TMU group 1, 2 and 3 (channels 3-11) and for those,
 * the default frequency is S2D2/4 = 133 1/3 MHz/4 = 33 1/3 MHz
*/
// #define MK_TickerNsToTicks(ns)		MK_NsToTicks_33333333((ns))
// #define MK_TickerNsToTicksF(ns)		MK_NsToTicksF_33333333((ns))
// #define MK_TickerTicksToNs(tx)		MK_TicksToNs_33333333((tx))
// #define MK_TickerTicksToNsF(tx)		MK_TicksToNsF_33333333((tx))

/* Execution timer also uses the generic virtual timer.
*/
// #define MK_ExecutionNsToTicks(ns)	MK_TimestampNsToTicks(ns)

// #define MK_TIMESTAMPCLOCKFACTOR100U		1000U
// #define MK_TIMESTAMPCLOCKFACTOR10U		100U
// #define MK_TIMESTAMPCLOCKFACTOR1U		10U

// #define MK_HAS_TIMESTAMPCLOCKFACTOR100U		1
// #define MK_HAS_TIMESTAMPCLOCKFACTOR10U		1
// #define MK_HAS_TIMESTAMPCLOCKFACTOR1U		1

/*
 * Prototype for main()
 *
 * The microkernel will actually not use the returned value, but this prototype
 * was chosen, because it is also one of (hosted) C's standard prototypes.
*/
/* Deviation MISRA-1 */
// extern int main(void);// PRQA S 2427
// 2020-04-12; uidg3207; summary: Msg(4:2427) Direct use of fundamental type.
// reason:  by convention the signature of main returns a plain integer. See also CCB Request Issue 1557905

// extern void MK_BoardEarlyInit(const mk_objectid_t coreIndex);
// extern void MK_BoardSecureInit(mk_objectid_t coreIndex);


/* functions for page table initialization */
// extern mk_ptentry_t MK_BoardGenInitPageTable (const mk_flatmemregion_t* const regionMap, const mk_size_t nPages);
// extern mk_uint64_t * MK_BoardGetPTMemory(void);
// extern void MK_BoardInitPTMemory(void);

/* board heap size: number of pagetable memory blocks for initial pagetable hierarchy */
// #define MK_BOARD_N_INITPTBLOCKS 20U

/* heap memory for storing board pagetables */
// #define MK_BOARD_PTSIZE		4096U
// Conti change
// extern MK_ATTR_ALIGN(MK_BOARD_PTSIZE) mk_uint64_t MK_boardmem[MK_BOARD_N_INITPTBLOCKS][MK_BOARD_PTSIZE/sizeof(mk_uint64_t)];

/* SCTLR_EL1 value - microkernel will always set M, C and I bits during start-up, but
 * won't touch other bits
*/
// #if defined(RCAR_V3H)
// #define MK_BOARD_SCTLR_VALUE		( 0U	\
									// | (3U << 28)	/* reserved, RES1 */	\
									// | (1U << 26)	/* UCI: EL0 can do cache maintenance */	\
									// | (0U << 25)	/* EE: little endian data for EL1 */	\
									// | (0U << 24)	/* E0E: little endian data for EL0 */	\
									// | (3U << 22)	/* reserved, RES1 */	\
									// | (1U << 20)	/* reserved, RES1 */	\
									// | (0U << 19)	/* WXN: do not force write-xor-execute */	\
									// | (0U << 18)	/* nTWE: EL0 can't do WFE */	\
									// | (0U << 16)	/* nTWI: EL0 can't do WFI */	\
									// | (1U << 15)	/* UCT: EL0 may read CTR_EL0 */	\
									// | (1U << 14)	/* DZE: EL0 can clear cache */	\
									// | (1U << 12)	/* I: instruction caching allowed */	\
									// | (1U << 11)	/* reserved, RES1 */	\
									// | (0U << 9)		/* UMA: EL0 may not mask interrupts */	\
									// | (0U << 8)		/* SED: setend enabled for 32-bit EL0 */	\
									// | (0U << 7)		/* ITD: IT insns enabled for 32-bit EL0 */	\
									// | (0U << 5)		/* CP15BEN: CP15 disabled for 32-bit EL0 */	\
									// | (1U << 4)		/* SA0: EL0 stack alignment enforced */	\
									// | (1U << 3)		/* SA: EL1 stack alignment enforced */	\
									// | (1U << 2)		/* C: data caching allowed */	\
									// | (0U << 1)		/* A: alignment check disabled */	\
									// | (1U << 0)		/* M: MMU enabled */	\
									// )
// #endif
// #if defined(RCAR_V3M)
// #define MK_BOARD_SCTLR_VALUE        ( 0U	\
									// | (3U << 28)	/* reserved, RES1 */	\
									// | (0U << 26)	/* UCI: EL0 can do cache maintenance */ \
									// | (0U << 25)	/* EE: little endian data for EL1 */	\
									// | (0U << 24)	/* E0E: little endian data for EL0 */	\
									// | (3U << 22)	/* reserved, RES1 */	\
									// | (1U << 20)	/* reserved, RES1 */	\
									// | (0U << 19)	/* WXN: do not force write-xor-execute */	\
									// | (0U << 18)	/* nTWE: EL0 can't do WFE */	\
									// | (0U << 16)	/* nTWI: EL0 can't do WFI */	\
									// | (0U << 15)	/* UCT: EL0 may read CTR_EL0 */	\
									// | (0U << 14)	/* DZE: EL0 can clear cache */	\
									// | (1U << 12)	/* I: instruction caching allowed */	\
									// | (1U << 11)	/* reserved, RES1 */	\
									// | (0U << 9)		/* UMA: EL0 may not mask interrupts */	\
									// | (0U << 8)		/* SED: setend enabled for 32-bit EL0 */	\
									// | (0U << 7)		/* ITD: IT insns enabled for 32-bit EL0 */	\
									// | (0U << 5)		/* CP15BEN: CP15 disabled for 32-bit EL0 */	\
									// | (1U << 4)		/* SA0: EL0 stack alignment enforced */	\
									// | (1U << 3)		/* SA: EL1 stack alignment enforced */	\
									// | (1U << 2)		/* C: data caching allowed */	\
									// | (0U << 1)		/* A: alignment check disabled */	\
									// | (1U << 0)		/* M: MMU enabled */	\
									// )
// #endif
// /* MAIR_EL1 value - microkernel uses attributes 0-1 (lower 32 bits) in MK_MAIR_VALUE.
 // *                  attributes 4-7 (upper 32 bits) can be set as needed.
// */
// #define MK_BOARD_MAIR_VALUE          MK_MAIR_VALUE \
                                   // | (0x44UL << 32)  /* attr 4: ONC, INC ; set up attr4 to non cached for use in user defined settings*/

// User define memory region types for non cahced memory regions on A53's
// Attribute for ram non cached (as it was provided as build in for former os versions)
// #define RAM_NON_CACHED ( MK_MMU_ATTR_NG     \
                        // | MK_MMU_ATTR_AF     \
                        // | MK_MMU_ATTR_SH_ISH \
                        // | MK_MMU_ATTR_AI(4) /* use attr 4 setting: non cached*/ \
                        // )

// #define MK_REGION_USER_MR_MDB_METADATA                  RAM_NON_CACHED
// #define MK_REGION_USER_MR_DPUAPPL_MDBData_ASILB_DPU2IU  RAM_NON_CACHED
// #define MK_REGION_USER_MR_DPUAPPL_MDBData_CEM_DPU2IU    RAM_NON_CACHED
// #define MK_REGION_USER_MR_DPUAPPL_MDBData_QM_DPU2IU     RAM_NON_CACHED
// #define MK_REGION_USER_MR_ST_SYNCH_DPU2IU               RAM_NON_CACHED
// #define MK_REGION_USER_MR_IUC_ICC_BUFFER                RAM_NON_CACHED
// #define MK_REGION_USER_MR_INFRA_MDB_IU2DPU              RAM_NON_CACHED
// #define MK_REGION_USER_MR_DPU_APPL_ICC_DATA_DPU2IU      RAM_NON_CACHED
// #if defined(RCAR_V3M)
// #define MK_REGION_USER_MR_DPU_ICC_DPU2IU      			RAM_NON_CACHED
// #endif
// #define MK_REGION_USER_MR_DPU_ICC_BUFFER                RAM_NON_CACHED
// #define MK_REGION_USER_MR_DPU_NoCache                   RAM_NON_CACHED
// #define MK_REGION_USER_MR_DPU_APPL_ICC_DATA_IU2DPU      RAM_NON_CACHED
// #if defined(RCAR_V3M)
// #define MK_REGION_USER_MR_DPU_ICC_IU2DPU			    RAM_NON_CACHED
// #endif
// #define MK_REGION_USER_MRG_DPUAPPL_ONCHIP_NOCACHE       RAM_NON_CACHED
// #define MK_REGION_USER_MRG_DPUAPPL_DEBUG_NOCACHE        RAM_NON_CACHED
// #define MK_REGION_USER_MRG_DPUAPPL_ASILB_NOCACHE        RAM_NON_CACHED
// #define MK_REGION_USER_MRG_DPUAPPL_QM_NOCACHE           RAM_NON_CACHED
// #ifdef DPUINFRA_MFC527 //defined in compiler option
// #define MK_REGION_USER_MRG_DPUAPPL_CEM_DF_NOCACHE       RAM_NON_CACHED
// #else
// #define MK_REGION_USER_MRG_DPUAPPL_CEM_NOCACHE          RAM_NON_CACHED
// #endif //DPUINFRA_MFC527
// #define MK_REGION_USER_MR_DPUAPPL_DebugData             RAM_NON_CACHED

// #endif
// PRQA S 2000,2300,1020,1021,1025,1026 --
// #endif

