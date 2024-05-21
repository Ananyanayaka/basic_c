// Copyright (C) Continental AG
// All Rights Reserved
// COMPONENT: IuSbl
// VERSION:   $Revision: 1.2 $


#include <IuSbl_part.h>

// Register access
#define BOARD_REG_32(a) (*((volatile uint32*)(a)))

// used registers
#define BOARD_CPG_CA53CPUCMCR  BOARD_REG_32(0xE6151184u)
#define BOARD_CPG_CA53WUPCR    BOARD_REG_32(0xE6151010u)

#define BOARD_RST_CA53RESCNT   BOARD_REG_32(0xE6160044u)
#define BOARD_RST_CA53CPU0BARL BOARD_REG_32(0xE6160084u)
#define BOARD_RST_CA53CPU1BARL BOARD_REG_32(0xE6160094u)

#define BOARD_SYSC_SYSCSR      BOARD_REG_32(0xE6180000u)
#define BOARD_SYSC_PWRONCR3    BOARD_REG_32(0xE618014Cu)
#define BOARD_SYSC_PWRER3      BOARD_REG_32(0xE6180154u)
#define BOARD_SYSC_PWRONSR3    BOARD_REG_32(0xE6180150u)
#define BOARD_SYSC_PWRSR3      BOARD_REG_32(0xE6180140u)

// additional defines
#define BIT0 (1u << 0u)
#define BIT1 (1u << 1u)
#define BIT2 (1u << 2u)
#define BIT3 (1u << 3u)
#define BIT4 (1u << 4u)
#define RST_CODE_VALUE (0x5A5A0000u)


// Enable Power Domains for DPU
void IuSbl_v_DPU_PowerOn(void)
{
  // set power mode
  BOARD_CPG_CA53CPUCMCR = 0x00000000u;	// CA53 Common Power Control Register

  // power-up CA53 L2 (SCU)
  while ( !( BOARD_SYSC_SYSCSR & BIT1) )		// wait for SYSC to accept power-up requests
  {

  }
  BOARD_SYSC_PWRONCR3 = BIT0;		// PoWeR resume Control Register: start power resume sequence
  if ( BOARD_SYSC_PWRER3 )			// check if request was denied
  {
    return;
  }
  while ( BOARD_SYSC_PWRONSR3 )			// wait while PoWeRON sequence is being executed
  {

  }
  while ( BOARD_SYSC_PWRSR3 != BIT4 )	// wait for module to be in the non-shutoff state
  {

  }

  return;
}

// Start DPU Core
void IuSbl_v_DPU_Start(uint32 u_CoreId, uint32 u_EntryPoint)
{
  uint32 u_rst;

  if (u_CoreId > COREID_CA53_1)
  {
    return;
  }

  // check sanity of boot addresses
  if ( u_EntryPoint & 0x00000003u)
  {
    return;
  }

  if (u_CoreId == COREID_CA53_0)
  {
    BOARD_RST_CA53CPU0BARL  = u_EntryPoint;	// CA53 CPU0 Boot Address register L [31:2]
  }
  else
  {
    BOARD_RST_CA53CPU1BARL  = u_EntryPoint;	// CA53 CPU1 Boot Address register L [31:2]
  }

  // wake up CA53 core
  BOARD_CPG_CA53WUPCR = (BIT0 << u_CoreId);   // request wake-up
  while ( BOARD_CPG_CA53WUPCR ) // wait until wake-up sequence is complete
  {

  }

  // check CA53 core reset state
  u_rst = BOARD_RST_CA53RESCNT;  // current reset bits
  if ( (u_rst & (BIT3 >> u_CoreId)) == 0x0u ) // already running ?
  {
    u_rst |= BIT3 >> u_CoreId;
    BOARD_RST_CA53RESCNT = RST_CODE_VALUE | u_rst;  // assert reset
    while ( (BOARD_RST_CA53RESCNT & (BIT3 >> u_CoreId)) == 0u )
    {

    }
  }

  // release CA53 core from reset
  u_rst &= ~(BIT3 >> u_CoreId);  // clear bit to release reset
  BOARD_RST_CA53RESCNT = RST_CODE_VALUE | u_rst;
  while ( BOARD_RST_CA53RESCNT & (BIT3 >> u_CoreId) )
  {

  }

  return;
}
