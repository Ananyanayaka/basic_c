/**
 * @file  startup.c
 *
 * @brief
 *  ARM R5 start-up code
 *
 *  \par
 *  ============================================================================
 *  @n   (C) Copyright 2017-2020, Texas Instruments, Inc.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *  Redistributions of source code must retain the above copyright
 *  notice, this list of conditions and the following disclaimer.
 *
 *  Redistributions in binary form must reproduce the above copyright
 *  notice, this list of conditions and the following disclaimer in the
 *  documentation and/or other materials provided with the
 *  distribution.
 *
 *  Neither the name of Texas Instruments Incorporated nor the names of
 *  its contributors may be used to endorse or promote products derived
 *  from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
*/
#include "startup.h"

static inline void REG32(const unsigned int address,const unsigned int value)
{
#ifdef __GTEST_SWITCH__ /* means GTEST */
  /* Gtest cannot access memory locations */
  unsigned int varRegister;
  varRegister = value;
#else
  // 2023-02-06; uif31319 , CCBIssueId: RAD600-53510
  // summary: Msg(4:0303) Cast between a pointer to volatile object and an integer type 
  // reason: memory mapped register needs to be accesed diretly via pointer
  // PRQA S 0303 1
  (*((volatile unsigned int*)(address))) = value;
#endif

}

#define W1TOCLEAR       0xFFFFFFFFU

/* Import init_array symbols from linker */
extern void (*__INIT_ARRAY_LIST__[])(void);
extern void (*__INIT_ARRAY_END__[])(void);


#define STARTUP_START_SEC_CONST_ASIL_B_UNSPECIFIED
#include "Startup_Memmap.h"
#undef STARTUP_MEMMAP_H

/*  Declarations  */
/**
 * \brief  TEX[2:0], C and B values.
 *     CSL_ArmR5MemAttr is used as index here.
 *     gMemAttr[x][0]: TEX[2:0] values
 *     gMemAttr[x][1]: C bit value
 *     gMemAttr[x][2]: B bit value
 */
static const unsigned int gMemAttr[CSL_ARM_R5_MEM_ATTR_MAX][3U] =
{
/*  TEX[2:0], C,   B bits */
  {   0x0U,   0x0U,  0x0U,}, /* Strongly-ordered.*/
  {   0x0U,   0x0U,  0x1U,}, /* Shareable Device.*/
  {   0x0U,   0x1U,  0x0U,}, /* Outer and Inner write-through, no write-allocate. */
  {   0x0U,   0x1U,  0x1U,}, /* Outer and Inner write-back, no write-allocate. */
  {   0x1U,   0x0U,  0x0U,}, /* Outer and Inner Non-cacheable. */
  {   0x1U,   0x1U,  0x1U,}, /* Outer and Inner write-back, write-allocate.*/
  {   0x2U,   0x0U,  0x0U,}, /* Non-shareable Device.*/
};

static const CSL_ArmR5MpuRegionCfg gCslR5MpuCfg[CSL_ARM_R5F_MPU_REGIONS_MAX] =
{
  {
  // PRQA S 1053      ++ //date:2022-10-08, reviewer:uif31319, reason: ADAS C standard is C99, but QAC configurations always seem to assume C90
  /* Region 0 configuration: complete 32 bit address space = 4Gbits */
    .regionId         = 0U,
    .enable           = 1U,
    .baseAddr         = 0x0U,
    .size             = CSL_ARM_R5_MPU_REGION_SIZE_4GB,
    .subRegionEnable  = CSL_ARM_R5_MPU_SUB_REGION_ENABLE_ALL,
    .exeNeverControl  = 1U,
    .accessPermission = CSL_ARM_R5_ACC_PERM_PRIV_USR_RD_WR,
    .shareable        = 0U,
    .cacheable        = 0U,
    .cachePolicy      = 0U,
    .memAttr          = 0U,
  },
  {
    /* Region 1 configuration: 1MB L3 */
    .regionId         = 1U,
    .enable           = 1U,
    .baseAddr         = 0x88000000U,
    .size             = CSL_ARM_R5_MPU_REGION_SIZE_4MB,
    .subRegionEnable  = CSL_ARM_R5_MPU_SUB_REGION_ENABLE_ALL,
    .exeNeverControl  = 0U,
    .accessPermission = CSL_ARM_R5_ACC_PERM_PRIV_USR_RD_WR,
    .shareable        = 0U,
    .cacheable        = 1U,
    .cachePolicy      = CSL_ARM_R5_CACHE_POLICY_WB_WA,
    .memAttr          = 0U,
  },
  {
    /* Region 2 configuration: 64KB L3 uncached */
    .regionId         = 2U,
    .enable           = 1U,
    .baseAddr         = 0x88100000U,
    .size             = CSL_ARM_R5_MPU_REGION_SIZE_64KB,
    .subRegionEnable  = CSL_ARM_R5_MPU_SUB_REGION_ENABLE_ALL,
    .exeNeverControl  = 0U,
    .accessPermission = CSL_ARM_R5_ACC_PERM_PRIV_USR_RD_WR,
    .shareable        = 1U,
    .cacheable        = 0U,
    .cachePolicy      = CSL_ARM_R5_CACHE_POLICY_NON_CACHEABLE,
    .memAttr          = CSL_ARM_R5_MEM_ATTR_STRONGLY_NON_CACHED,
  },
  {
    /* Region 3 configuration: 32 KB TCMA */
    .regionId         = 3U,
    .enable           = 1U,
    .baseAddr         = 0x00000000,
    .size             = CSL_ARM_R5_MPU_REGION_SIZE_32KB,
    .subRegionEnable  = CSL_ARM_R5_MPU_SUB_REGION_ENABLE_ALL,
    .exeNeverControl  = 0U,
    .accessPermission = CSL_ARM_R5_ACC_PERM_PRIV_USR_RD_WR,
    .shareable        = 0U,
    .cacheable        = 1U,
    .cachePolicy      = CSL_ARM_R5_CACHE_POLICY_WB_WA,
    .memAttr          = 0U,
  },
  {
    /* Region 4 configuration: 32 KB  TCMB */
    .regionId         = 4U,
    .enable           = 1U,
    .baseAddr         = 0x00080000,
    .size             = CSL_ARM_R5_MPU_REGION_SIZE_32KB,
    .subRegionEnable  = CSL_ARM_R5_MPU_SUB_REGION_ENABLE_ALL,
    .exeNeverControl  = 0U,
    .accessPermission = CSL_ARM_R5_ACC_PERM_PRIV_USR_RD_WR,
    .shareable        = 0U,
    .cacheable        = 1U,
    .cachePolicy      = CSL_ARM_R5_CACHE_POLICY_WB_WA,
    .memAttr          = 0U,
  },
  {
    /* Region 5 configuration: 1 MB L2 RAM */
    .regionId         = 5U,
    .enable           = 1U,
    .baseAddr         = 0x10200000,
    .size             = CSL_ARM_R5_MPU_REGION_SIZE_1MB,
    .subRegionEnable  = CSL_ARM_R5_MPU_SUB_REGION_ENABLE_ALL,
    .exeNeverControl  = 0U,
    .accessPermission = CSL_ARM_R5_ACC_PERM_PRIV_USR_RD_WR,
    .shareable        = 0U,
    .cacheable        = 1U,
    .cachePolicy      = CSL_ARM_R5_CACHE_POLICY_WB_WA,
    .memAttr          = 0U,
  },
  {
    /* Region 6 configuration: 64KB L2 uncached*/
    .regionId         = 6U,
    .enable           = 1U,
    .baseAddr         = 0x10200000,
    .size             = CSL_ARM_R5_MPU_REGION_SIZE_64KB,
    .subRegionEnable  = CSL_ARM_R5_MPU_SUB_REGION_ENABLE_ALL,
    .exeNeverControl  = 0U,
    .accessPermission = CSL_ARM_R5_ACC_PERM_PRIV_USR_RD_WR,
    .shareable        = 1U,
    .cacheable        = 0U,
    .cachePolicy      = CSL_ARM_R5_CACHE_POLICY_NON_CACHEABLE,
    .memAttr          = CSL_ARM_R5_MEM_ATTR_STRONGLY_NON_CACHED,
  },
  {
    /* Region 7 configuration: 64KB L3 */
    .regionId         = 7U,
    .enable           = 1U,
    .baseAddr         = 0x88180000U,
    .size             = CSL_ARM_R5_MPU_REGION_SIZE_64KB,
    .subRegionEnable  = CSL_ARM_R5_MPU_SUB_REGION_ENABLE_ALL,
    .exeNeverControl  = 0U,
    .accessPermission = CSL_ARM_R5_ACC_PERM_PRIV_USR_RD_WR,
    .shareable        = 0U,
    .cacheable        = 0U,
    .cachePolicy      = CSL_ARM_R5_CACHE_POLICY_NON_CACHEABLE,
    .memAttr          = CSL_ARM_R5_MEM_ATTR_STRONGLY_NON_CACHED,
  }
  // PRQA S 1053 --
  // PRQA S 0686 2 //date:2022-08-12, reviewer:uif31319,
  // Not a problem. The array size is configured for maximum size(MPU registers) The memory used as per the MAP file is only for 8 elements
};

#define STARTUP_STOP_SEC_CONST_ASIL_B_UNSPECIFIED
#include "Startup_Memmap.h"
#undef STARTUP_MEMMAP_H

extern unsigned char __BSS_START;
extern unsigned char __BSS_END;
extern unsigned char __IRQ_STACK_START;
extern unsigned char __IRQ_STACK_END;
extern unsigned char __FIQ_STACK_START;
extern unsigned char __FIQ_STACK_END;
extern unsigned char __ABORT_STACK_START;
extern unsigned char __ABORT_STACK_END;
extern unsigned char __UND_STACK_START;
extern unsigned char __UND_STACK_END;
extern unsigned char __SVC_STACK_START;
extern unsigned char __SVC_STACK_END;
extern unsigned char __USER_STACK_START;
extern unsigned char __USER_STACK_END;

extern void CSL_armR5StartupMpuEnable(unsigned int enable);
extern void CSL_armR5StartupBREnable(unsigned int enable);
extern void CSL_armR5StartupMpuCfgRegion(unsigned int regionNum, unsigned int baseAddrRegVal, unsigned int sizeRegVal, unsigned int accessCtrlRegVal);

#define STARTUP_START_SEC_CODE_ASIL_B
#include "Startup_Memmap.h"
#undef STARTUP_MEMMAP_H

#ifndef __UNIT_TEST_SWITCH__
__attribute__((weak)) void platform_init(void);
#else
void platform_init(void);
#endif
/// @brief             Clear BSS and exception stack memory areas and initialize constructors
/// @pre               C environment has been set up
/// @post              main() shall be called
/// @param             none
/// @return            void
/// @globals           none
/// @callsequence
///   @image           html system_pre_init.png system_pre_init
/// @InOutCorrelation  none
/// @traceability
/// @id{L3_SWD_STARTUP_SYSTEMPREINIT},  @satisfies{816608},@satisfies{907536} \n
void system_pre_init(void);
static void CSL_armR5MPUCfg(void);


static void enable_cache(void)
{
  CSL_armR5StartupCacheEnableAllCache( 0 );    /* Disable I/D caches */
  CSL_armR5StartupCacheEnableForceWrThru( 0 ); /* Disable force write-thru */
  CSL_armR5StartupCacheInvalidateAllCache();   /* Invalidate I/D caches */
  CSL_armR5StartupCacheEnableAllCache( 1 );    /* Enable I/D caches */
}

/****************************************************************************************/
/* \brief platform_init() - platform_init() is called in the C/C++ startup routine,     */
/* _c_int00(), and provides a mechanism for tailoring cache/FPU/interrupt by device     */
/* prior to calling main().                                                             */
/*                                                                                      */
/****************************************************************************************/
/* Usage notes:
 * On entry to this function from boot, R5F must be in System (privileged) mode.
 */


/********************************************************************************************/
/// @impl{L3_SWD_STARTUP_PLATFORMINIT}
void platform_init(void) // PRQA S 1503 1 //date:2022-08-16, reviewer:uif31319, reason: functions are used in appropriate Projects.
{
  CSL_armR5MPUCfg();               /* Enable a default configuration for the MPU till OS overrides it */
  enable_cache();                  /* Enable all caches */
  CSL_armR5StartupFpuEnable(1);    /* Enable FPU */
  CSL_armR5StartupIntrEnableVic(SCTLR_VE_BIT);  /* Enable/Disable Vectored Interrupts, as required by the OS */

  /* Disable all interrupts on VIM before enabling CPU Interrupts */
  REG32(VIM_INTENCLR   , W1TOCLEAR);
  REG32(VIM_INTENCLR_1 , W1TOCLEAR);
  REG32(VIM_INTENCLR_2 , W1TOCLEAR);
  REG32(VIM_INTENCLR_3 , W1TOCLEAR);
  REG32(VIM_INTENCLR_4 , W1TOCLEAR);
  REG32(VIM_INTENCLR_5 , W1TOCLEAR);
  REG32(VIM_INTENCLR_6 , W1TOCLEAR);
  REG32(VIM_INTENCLR_7 , W1TOCLEAR);

  /* Invalidate any pending interrupts on VIM */
  REG32(VIM_STS   , W1TOCLEAR);
  REG32(VIM_STS_1 , W1TOCLEAR);
  REG32(VIM_STS_2 , W1TOCLEAR);
  REG32(VIM_STS_3 , W1TOCLEAR);
  REG32(VIM_STS_4 , W1TOCLEAR);
  REG32(VIM_STS_5 , W1TOCLEAR);
  REG32(VIM_STS_6 , W1TOCLEAR);
  REG32(VIM_STS_7 , W1TOCLEAR);

  /*
   * Disable the OS counters on RTIA (they may be running after a soft reset)
   * Set to suspend in debug mode and use NTU0 if external clock is configured (default)
   * The other RTI timers are not altered at this point since they may be used
   * for different purposes that require continuity of the timer
   */
  REG32(RTIA_CTRL_BASE , 0U);

  /* Disable all RTI interrupts before enabling CPU Interrupts */
  REG32(RTIA_CLRINTEN , W1TOCLEAR);
  REG32(RTIB_CLRINTEN , W1TOCLEAR);

  /* Invalidate any pending interrupts on the RTI peripheral */
  REG32(RTIA_INTFLAGS , W1TOCLEAR);
  REG32(RTIB_INTFLAGS , W1TOCLEAR);

  /* Write 0 to the IRQ and FIQ Vector registers to invalidate the active IRQ/FIQ */
  REG32(VIM_IRQVEC , 0U);
  REG32(VIM_FIQVEC , 0U);

  /***/
  CSL_armR5StartupIntrEnableFiq(0);  /* Disable FIQ */
  CSL_armR5StartupIntrEnableIrq(1);  /* Enable IRQ */
}

/*********************************************************************************************/
/// @brief             Call the constructors that are required by the CPP files running on IUC
/// @pre               Linker provides __INIT_ARRAY_LIST__ that holds all active constructors
/// @post              C++ constructors are set up
/// @param             none
/// @return            void
/// @globals           none
/// @callsequence
///   @image           html call_ctors.png call_ctors
/// @InOutCorrelation  none
/// @traceability @ref system_pre_init
static void call_ctors(void)
{
  // 2023-02-06; uif31319 , CCBIssueId: RAD600-53510
  // summary: Msg(4:0306) Cast between a pointer to object and an integer type 
  // reason: Linker symbol can only be assesed via typecast
  // PRQA S 0306 1
  const unsigned int u_noofctors = ((unsigned int)__INIT_ARRAY_END__ - (unsigned int)__INIT_ARRAY_LIST__) / sizeof(unsigned int);
  const unsigned int u_firstctorindex = 1U;
  unsigned int     i;

  /*
   * Loop through init_array and execute each constructor until end of list is hit.
   * Please note that the first and the last entries that are from ctordtor.o are skipped
   * because those are not relevant.
   */
  for (i=u_firstctorindex; i < (u_noofctors - u_firstctorindex); i++)
  {
    if (__INIT_ARRAY_LIST__[i] != (void*)0U)
    {
      __INIT_ARRAY_LIST__[i]();  // Call constructor
    }
  }
}


/// @impl{L3_SWD_STARTUP_SYSTEMPREINIT}
void system_pre_init(void) // PRQA S 1503 1 //date:2022-08-16, reviewer:uif31319, reason: functions are used in appropriate Projects.
{
  unsigned int i = 0U;
  unsigned char *dst;

  /* Clear BSS section */
  dst = &__BSS_START;
  // PRQA S 2771 2 //date:2022-10-08, reviewer:uif31319,
  //reason: Comparing well defined address by the linker this is a platform specific init sequence implementation(Non portable).
  for (i=0; &(dst[i]) < &__BSS_END; i++)
  {
    dst[i] = 0U;
  }

  /*
   * Call the constructors of all global C++ variables
   * according to the initialization table generated by the linker.
   */
  call_ctors();
  // PRQA S 7002 1 //date:2022-10-08, reviewer:uif31319, reason: Reason: Initialization of different memory sections unavoidable.
}

static void CSL_armR5MPUCfg(void)
{
  unsigned int loopCnt = 0U;
  unsigned int baseAddrRegVal = 0U, sizeRegVal = 0U, accessCtrlRegVal = 0U, tex;

  CSL_armR5StartupCacheInvalidateAllCache();   /* Invalidate caches */
  CSL_armR5StartupCacheEnableDCache(0);        /* Disable D-cache */

  /* Disable MPU */
  CSL_armR5StartupMpuEnable(0U);

  /* Disable all MPU regions */
  for (loopCnt=0U ; loopCnt < CSL_ARM_R5F_MPU_REGIONS_MAX; loopCnt++)
  {
    CSL_armR5StartupMpuCfgRegion(loopCnt,
                   baseAddrRegVal,
                   sizeRegVal,
                   accessCtrlRegVal);
  }

  /* Disable Default (Background) MPU Regions */
  CSL_armR5StartupBREnable(0U);

  /* Configure MPU regions only for provided configuration */
  for (loopCnt=0U; loopCnt < CSL_ARM_R5F_MPU_REGIONS_MAX; loopCnt++)
  {
    if (CSL_ARM_R5_MPU_REGION_SIZE_32B <= gCslR5MpuCfg[loopCnt].size)
    {
      baseAddrRegVal   = ( gCslR5MpuCfg[loopCnt].baseAddr &
                  CSL_ARM_R5_MPU_REGION_BASE_ADDR_MASK);
      // PRQA S 2985 2 //date:2022-10-08, reviewer:uif31319, reason: redundant shift operation but increases the code readability.
      sizeRegVal     = ( gCslR5MpuCfg[loopCnt].enable <<
                  CSL_ARM_R5_MPU_REGION_SZEN_EN_SHIFT);
      sizeRegVal     |= ( gCslR5MpuCfg[loopCnt].size <<
                  CSL_ARM_R5_MPU_REGION_SZEN_SZ_SHIFT);
      sizeRegVal     |= ( gCslR5MpuCfg[loopCnt].subRegionEnable <<
                  CSL_ARM_R5_MPU_REGION_SZEN_SRD_SHIFT);
      accessCtrlRegVal = ( gCslR5MpuCfg[loopCnt].exeNeverControl <<
                  CSL_ARM_R5_MPU_REGION_AC_XN_SHIFT);
      accessCtrlRegVal |= ( gCslR5MpuCfg[loopCnt].accessPermission <<
                  CSL_ARM_R5_MPU_REGION_AC_AP_SHIFT);
      accessCtrlRegVal |= ( gCslR5MpuCfg[loopCnt].shareable <<
                  CSL_ARM_R5_MPU_REGION_AC_S_SHIFT);

      if (gCslR5MpuCfg[loopCnt].cacheable == 1U)
      {
        tex = 4U;
        tex |= (gCslR5MpuCfg[loopCnt].cachePolicy);
        accessCtrlRegVal |=
                ( tex << CSL_ARM_R5_MPU_REGION_AC_TEX_SHIFT);
        // PRQA S 2985 2 //date:2022-10-08, reviewer:uif31319, reason: redundant shift operation but increases the code readability.
        accessCtrlRegVal |= ( gCslR5MpuCfg[loopCnt].cachePolicy <<
                CSL_ARM_R5_MPU_REGION_AC_CB_SHIFT);
      }
      else
      {
        tex = gMemAttr[gCslR5MpuCfg[loopCnt].memAttr][0U];
        accessCtrlRegVal |=
                ( tex << CSL_ARM_R5_MPU_REGION_AC_TEX_SHIFT);
        // PRQA S 2985 3 //date:2022-10-08, reviewer:uif31319, reason: redundant shift operation but increases the code readability.
        accessCtrlRegVal |=
                ( gMemAttr[gCslR5MpuCfg[loopCnt].memAttr][1U] <<
                CSL_ARM_R5_MPU_REGION_AC_B_SHIFT);
        accessCtrlRegVal |=
                ( gMemAttr[gCslR5MpuCfg[loopCnt].memAttr][2U] <<
                CSL_ARM_R5_MPU_REGION_AC_C_SHIFT);
      }

      /* configure MPU region here */
      CSL_armR5StartupMpuCfgRegion(gCslR5MpuCfg[loopCnt].regionId,
                     baseAddrRegVal,
                     sizeRegVal,
                     accessCtrlRegVal);
    }
  }

  CSL_armR5StartupCacheInvalidateAllCache();   /* Invalidate caches */
  CSL_armR5StartupCacheEnableDCache(0);        /* Disable D-cache */

  /* Enable MPU */
  CSL_armR5StartupMpuEnable(1U);
}

#define STARTUP_STOP_SEC_CODE_ASIL_B
#include "Startup_Memmap.h"
#undef STARTUP_MEMMAP_H
