#ifndef CORE_CM7_CACHE_H
#define CORE_CM7_CACHE_H

#if defined (__clang__)
  /* PRQA S 3116 ++ *//* Unrecognized #pragma arguments 'clang system_header' This #pragma directive has been ignored. *//* Pragma required for header inclusion. */
  #pragma clang system_header   /* treat file as system include file */
#endif

#ifdef __cplusplus
 extern "C" {
#endif

#include "sys_init.h"
#include "system.h"

/* PRQA S 3006 ++ *//* "This function contains a mixture of in-line assembler statements and C statements." *//* Yes, required for memory barrier handling */

// 2024-02-20; uid95356
// summary: Msg(0306) [I] Cast between a pointer to object and an integral type.
// Reason : Pointer needed to access  fixed address memory mapped register. It has no impact.  
/* PRQA S 0306 ++ *//* CCBIssueID: ADASQMS-3232 */

/// @brief    M7 core Enable Instruction cache
/// @pre      none
/// @post     Instruction cache enabled
/// @param    none
/// @return   none
/// @globals  none
/// @InOutCorrelation
///   Invalidates and enables the instruction cache
/// @callsequence
///   @image html SCB_EnableICache.svg Enable instruction cache
/// @testmethod ECU test required, unit test not feasible due to HW dependency
/// @traceability 
/// @id{L3_SWD_OS_MAIN_TSK}, @satisfies{893315}
__attribute__((always_inline)) static inline void SCB_EnableICache (void)
{
    if ( ( SCB->CCR & SCB_CCR_IC_Msk ) != 0u ) return;  /* return if ICache is already enabled */

    __asm volatile ("dsb 0xF":::"memory");
    __asm volatile ("isb 0xF":::"memory");
    SCB->ICIALLU = 0U;                      /* invalidate I-Cache */
    __asm volatile ("dsb 0xF":::"memory");
    __asm volatile ("isb 0xF":::"memory");
    SCB->CCR |=  (uint32_t)SCB_CCR_IC_Msk;  /* enable I-Cache */
    __asm volatile ("dsb 0xF":::"memory");
    __asm volatile ("isb 0xF":::"memory");
}


/// @brief    M7 core Enable Data cache
/// @pre      none
/// @post     Data cache enabled
/// @param    none
/// @return   none
/// @globals  none
/// @InOutCorrelation
///   Invalidates and enables the data cache
/// @callsequence
///   @image html SCB_EnableDCache.svg Enable data cache
/// @testmethod ECU test required, unit test not feasible due to HW dependency
/// @traceability 
/// @id{L3_SWD_OS_MAIN_TSK}, @satisfies{893315}
__attribute__((always_inline)) static inline void SCB_EnableDCache (void)
{
    uint32_t ccsidr;
    uint32_t sets;
    uint32_t ways;

    if ( ( SCB->CCR & SCB_CCR_DC_Msk ) != 0u ) return;  /* return if DCache is already enabled */

    SCB->CSSELR = 0U;                       /* select Level 1 data cache */
    __asm volatile ("dsb 0xF":::"memory");

    ccsidr = SCB->CCSIDR;

    /* invalidate D-Cache */
    sets = (uint32_t)(CCSIDR_SETS(ccsidr) + 1U );
    while ( sets > 0u ) 
    {
      ways = (uint32_t)(CCSIDR_WAYS(ccsidr) + 1U );
      while ( ways > 0u ) 
      {
        SCB->DCISW = ((((sets - 1u) << SCB_DCISW_SET_Pos) & (uint32_t)SCB_DCISW_SET_Msk) |
                      (((ways - 1u) << SCB_DCISW_WAY_Pos) & (uint32_t)SCB_DCISW_WAY_Msk)  );
        ways--;
      }
      sets--;
    }
    __asm volatile ("dsb 0xF":::"memory");

    SCB->CCR |=  (uint32_t)SCB_CCR_DC_Msk;  /* enable D-Cache */

    __asm volatile ("dsb 0xF":::"memory");
    __asm volatile ("isb 0xF":::"memory");
}

/* PRQA S 3006,0306,3116 -- */

#ifdef __cplusplus
}
#endif

#endif /* CORE_CM7_CACHE_H */
