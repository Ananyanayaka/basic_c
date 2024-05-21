// Copyright (C) Continental AG
// All Rights Reserved
// COMPONENT: IuSbl
// VERSION:   $Revision: 1.8 $
/// @file     IuSbl_part.c
/// @brief    This file implements the DPU startup and power functionality

//Note:Reference information for 0303_BOARD_REG_32 warnings supression
// Date: 2023-11-22, uif85752
// Reason: Memory mapped register needs to be accessed directly via pointer.CCB Jira-ID: ADASQMS-3232
// Msg(4:0303) [I] Cast between a pointer to volatile object and an integral type.

#include <IuSbl_part.h>

// Register access
#ifndef __COURAGE_TEST_SWITCH__

/// Generic 32bit IO accesses of a certain register given as parameter. \n
/// Register Address is input parameter. \n
/// Returns content from desired register.
#define BOARD_REG_32(a) (*((volatile uint32*)(a)))

// used registers

/// Accesses the CA53CPUCMCR value for IO. \n
/// Returns content from CA53CPUCMCR register.
#define BOARD_CPG_CA53CPUCMCR  BOARD_REG_32(0xE6151184u)

/// Accesses the CA53WUPCR value for IO. \n
/// Returns content from CA53WUPCR register.
#define BOARD_CPG_CA53WUPCR    BOARD_REG_32(0xE6151010u)

/// Accesses the CA53RESCNT value for IO. \n
/// Returns content from CA53RESCNT register.
#define BOARD_RST_CA53RESCNT   BOARD_REG_32(0xE6160044u)

/// Accesses the CA53CPU0BARL value for IO. \n
/// Returns content from CA53CPU0BARL register.
#define BOARD_RST_CA53CPU0BARL BOARD_REG_32(0xE6160084u)

/// Accesses the CA53CPU1BARL value for IO. \n
/// Returns content from CA53CPU1BARL register.
#define BOARD_RST_CA53CPU1BARL BOARD_REG_32(0xE6160094u)

/// Accesses the CA53CPU2BARL value for IO. \n
/// Returns content from CA53CPU2BARL register.
#define BOARD_RST_CA53CPU2BARL BOARD_REG_32(0xE61600A4u)

/// Accesses the CA53CPU3BARL value for IO. \n
/// Returns content from CA53CPU3BARL register.
#define BOARD_RST_CA53CPU3BARL BOARD_REG_32(0xE61600B4u)

/// Accesses the SYSCSR value for IO. \n
/// Returns content from SYSCSR register.
#define BOARD_SYSC_SYSCSR      BOARD_REG_32(0xE6180000u)

/// Accesses the PWRONCR3 value for IO. \n
/// Returns content from PWRONCR3 register.
#define BOARD_SYSC_PWRONCR3    BOARD_REG_32(0xE618014Cu)

/// Accesses the PWRER3 value for IO. \n
/// Returns content from PWRER3 register.
#define BOARD_SYSC_PWRER3      BOARD_REG_32(0xE6180154u)

/// Accesses the PWRONSR3 value for IO. \n
/// Returns content from PWRONSR3 register.
#define BOARD_SYSC_PWRONSR3    BOARD_REG_32(0xE6180150u)

/// Accesses the PWRSR3 value for IO. \n
/// Returns content from PWRSR3 register.
#define BOARD_SYSC_PWRSR3      BOARD_REG_32(0xE6180140u)

// additional defines
/// Macro for bit 0 (LSB).
#define BIT0 (1uL << 0u)

/// Macro for bit 1 (0x2).
#define BIT1 (1uL << 1u)

/// Macro for bit 3 (0x8).
#define BIT3 (1uL << 3u)

/// Macro for bit 4 (0x10).
#define BIT4 (1uL << 4u)

/// BIT RST_CODE_VALUE set to 0x5A5A0000u.
#define RST_CODE_VALUE (0x5A5A0000u)

/// Mask used for detecting entry points which are not aligned on 4-bytes in memory
#define MEM_ALIGNMENT_MASK (0x00000003u)
#endif

// HIS metrics check
// PRQA S 7010,2889 32
// 2021-1-6; uie38801
// Message(3:7010) HIS metric : no of exit points (too many return statements)
// Message(2:2889) HIS metric : This function has more than one 'return' path.
// Reason: non critical, if request is denied return, also return after non shut-off state is necessary

void IuSbl_v_DPU_PowerOn(void)
{
  // set power mode
//PRQA S 0303 1 // 0303_BOARD_REG_32
  BOARD_CPG_CA53CPUCMCR = 0x00000000u;      // CA53 Common Power Control Register

  // power-up CA53 L2 (SCU)
//PRQA S 0303 1 // 0303_BOARD_REG_32
  while ( ( BOARD_SYSC_SYSCSR & BIT1) != BIT1)  // wait for SYSC to accept power-up requests
  {

  }
//PRQA S 0303 2 // 0303_BOARD_REG_32
  BOARD_SYSC_PWRONCR3 = BIT0;           // PoWeR resume Control Register: start power resume sequence
  if ( BOARD_SYSC_PWRER3 != 0u )            // check if request was denied
  {
    return;
  }
  while ( BOARD_SYSC_PWRONSR3 != 0u )   //PRQA S 0303  // 0303_BOARD_REG_32     // wait while PoWeRON sequence is being executed
  {

  }
  while ( BOARD_SYSC_PWRSR3 != BIT4 )   //PRQA S 0303 // 0303_BOARD_REG_32    // wait for module to be in the non-shutoff state
  {

  }

  return;
}

// PRQA S 7002,2024,7010,3227,2889 97
// 2021-1-6; uie38801
// Message(3:7002) Amount of non-cyclic paths, minimum number of necessary test cases
// Message(3:2024) jump statement in switch default
// Message(3:7010) lot of exit points
// Message(2:3227) The parameter 'u_CoreId' is never modified and so it could be declared with the 'const' qualifier.
// Message(2:3227) The parameter 'u_EntryPoint' is never modified and so it could be declared with the 'const' qualifier.
// Message(2:2889) This function has more than one 'return' path.
// Reason: not critical, as these non cyclic paths are necessary. There is no need for explicitly define input parameters as const.
// The function is provided to other modules, changing of interface may require chaning of many other modules.
void IuSbl_v_DPU_Start(uint32 u_CoreId, uint32 u_EntryPoint)
{
  uint32 u_rst;

  // check sanity of boot addresses
  if ( (u_EntryPoint & MEM_ALIGNMENT_MASK) != 0u)
  {
    return;
  }

  switch(u_CoreId)
  {
    case COREID_CA53_0:
      BOARD_RST_CA53CPU0BARL  = u_EntryPoint;//PRQA S 0303  // 0303_BOARD_REG_32
      break;

    case COREID_CA53_1:
      BOARD_RST_CA53CPU1BARL  = u_EntryPoint;//PRQA S 0303  // 0303_BOARD_REG_32
      break;

    case COREID_CA53_2:
      BOARD_RST_CA53CPU2BARL  = u_EntryPoint;//PRQA S 0303  // 0303_BOARD_REG_32
      break;

    case COREID_CA53_3:
      BOARD_RST_CA53CPU3BARL  = u_EntryPoint;//PRQA S 0303  // 0303_BOARD_REG_32
      break;

    default:
      return;
  }

  // wake up CA53 core
  BOARD_CPG_CA53WUPCR =(uint32) (BIT0 << u_CoreId);   // request wake-up //PRQA S 0303  // 0303_BOARD_REG_32
  while ( BOARD_CPG_CA53WUPCR != 0u ) //PRQA S 0303  // 0303_BOARD_REG_32 // wait until wake-up sequence is complete
  {

  }

  // check CA53 core reset state
  u_rst = BOARD_RST_CA53RESCNT;  // current reset bits//PRQA S 0303  // 0303_BOARD_REG_32
  if ( (u_rst & (BIT3 >> u_CoreId)) == 0x0u ) // already running ?
  {
    u_rst |= (uint32)BIT3 >> u_CoreId;
    BOARD_RST_CA53RESCNT = RST_CODE_VALUE | u_rst;  // assert reset //PRQA S 0303  // 0303_BOARD_REG_32
    while ( (BOARD_RST_CA53RESCNT & (BIT3 >> u_CoreId)) == 0u ) //PRQA S 0303  // 0303_BOARD_REG_32
    {

    }
  }

  // release CA53 core from reset
  u_rst &= ~(uint32)(BIT3 >> u_CoreId);  // clear bit to release reset
  BOARD_RST_CA53RESCNT = RST_CODE_VALUE | u_rst; //PRQA S 0303  // 0303_BOARD_REG_32
  while ( (BOARD_RST_CA53RESCNT & (BIT3 >> u_CoreId))!= 0u) //PRQA S 0303  // 0303_BOARD_REG_32
  {

  }

  return;
}
