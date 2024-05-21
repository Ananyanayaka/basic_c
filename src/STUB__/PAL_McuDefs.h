/* *************************************************************************************************
  COMPANY:     Continental Automotive

  PROJECT:     MFC5xx / R7

  CPU:         Renesas V3X

  MODNAME:     PAL_McuDefs.h

  VERSION:     $Revision: 1.31 $

  CHANGES:
  ---*/ /*---

   * ************************************************************************************************/
/** ************************************************************************************************
 *  \file
 *
 *  \brief  Hardware definitions for the Renesas V3X
 *
 *  \date       2017-04-12
 *  \author     Michael Schaeffler (uid41789)
 *  \copyright  Continental Automotive 2017
 *
 *  Hardware definitions for the Renesas V3X (Types, Bitmask, Defines, etc.)
 */
/* PRQA S 1011 EOF */ /* uid41789: 2018-01-10 Fix // comments */
#ifndef PAL_McuDefs_H
#define PAL_McuDefs_H

/* ============================================================================================== */
/*                                         Include Files                                          */
/* ============================================================================================== */
// #include <Std_Types.h>
// #include <GLOB_McuDefs.h>
// #include <platform_cfg.h>
// #if (CURRENT_CHIPSET != CHIPSET_REN_V3H)
// #include <Reg_RCarV3M.h>
// #else
#include <Reg_RCarV3H.h>
// #endif

/* ============================================================================================== */
/*                                        Defines & Macros                                        */
/* ============================================================================================== */

/** **********************************************************************************************
 *  \brief  Access Registers with these Macros
 *
 *  Access hardware registers with these macros.
 *
 *  \pre  None
 *  \post None
 *
 *  \param [in]    address   Register Address
 *
 *  \return Register Value
 */
/** @{ */ /* Start grouping */
/* PRQA S 3453,3472 4 */  /* uid41789: 2018-01-10 convenient implementation uia58668: 2021-02-02 function like macro replacing with equivalent function call */
#define REG64( address )  (*((volatile uint64 *) (address) ))
#define REG32( address )  (*((volatile uint32 *) (address) ))
#define REG16( address )  (*((volatile uint16 *) (address) ))
#define REG8( address )   (*((volatile uint8  *) (address) ))
/** @} */ /* End grouping */

/** **********************************************************************************************
 *  \brief  AXI-Bus EDC status Register Mask
 *
 *  AXI-Bus EDC status Register Mask
 *
 *  \range       Fixed
 *  \resolution  1
 *  \unit        None
 *
 */
/** @{ */ /* Start grouping */
#define AXI_EDCST1_DRAM_MLT       0x40000000uL
#define AXI_EDCST1_DRAM_SGL       0x20000000uL
/** @} */ /* End grouping */

/** **********************************************************************************************
 *  \brief  MFIS Error Status register defines
 *
 *  MFIS Error Status register defines
 *
 *  \range       Fixed
 *  \resolution  1
 *  \unit        None
 *
 */
/** @{ */ /* Start grouping */
#define MFIS_MFIERRSTSR0_CPG_ACCESS             0x40000000uL  ///< CPG safety access protection error
#define MFIS_MFIERRSTSR0_CLOCKMON_ERROR         0x20000000uL  ///< CPG Clock monitor error bit
#define MFIS_MFIERRSTSR0_CLOCKMON_HW_RED_ERR    0x10000000uL  ///< CPG Clock monitor HW redundancy error bit
#define MFIS_MFIERRSTSR0_R7_LCK_STP_OR          0x00080000uL  ///< Cortex-R7 lock-step operation error
#define MFIS_MFIERRSTSR0_R7_WRT_BUF             0x00040000uL  ///< Cortex-R7 Write Buffer external bus error
#define MFIS_MFIERRSTSR0_R7_ECC_ITCM_BNK        0x00020000uL  ///< Cortex-R7 ECC error bank empty for instruction TCM
#define MFIS_MFIERRSTSR0_R7_ECC_DTCM_BNK        0x00010000uL  ///< Cortex-R7 ECC error bank empty for data TCM
#define MFIS_MFIERRSTSR0_R7_ECC_ICACHE_BNK      0x00008000uL  ///< Cortex-R7 ECC error bank empty for instruction cache
#define MFIS_MFIERRSTSR0_R7_ECC_DCACHE_BNK      0x00004000uL  ///< Cortex-R7 ECC error bank empty for data cache
#define MFIS_MFIERRSTSR0_R7_LCK_STP_CMP         0x00002000uL  ///< Cortex-R7 lock-step comparator comparison error
#define MFIS_MFIERRSTSR0_R7_ECC_RAM_FATAL       0x00001000uL  ///< Cortex-R7 RAM ECC 2-Bit Errors
#define MFIS_MFIERRSTSR0_R7_ECC_RAM             0x00000800uL  ///< Cortex-R7 RAM ECC 1-Bit Errors
#define MFIS_MFIERRSTSR0_AXCCAXI_BUS_EDC_FATAL  0x00000200uL  ///< AXCC AXI bus EDC error
#define MFIS_MFIERRSTSR0_ISP_ERR_CH1            0x00000010uL  ///< Image Signal Processor (ISP) core error channel 1

#define MFIS_MFIERRSTSR1_ECC_RTSRAM             0x00000100uL  ///< RT-SRAM ecc single-bit error
#define MFIS_MFIERRSTSR1_ECC_RTSRAM_REDUNDANT   0x20000000uL  ///< RT-SRAM ecc redundant error
#define MFIS_MFIERRSTSR1_RFSO6_TIMEOUT_ERROR    0x00040000uL  ///< RFSO time-out detection timer error (ch6)
#define MFIS_MFIERRSTSR1_RFSO5_TIMEOUT_ERROR    0x00020000uL  ///< RFSO time-out detection timer error (ch7)
#define MFIS_MFIERRSTSR1_RFSO4_TIMEOUT_ERROR    0x00010000uL  ///< RFSO time-out detection timer error (ch4)
#define MFIS_MFIERRSTSR1_RFSO3_TIMEOUT_ERROR    0x00008000uL  ///< RFSO time-out detection timer error (ch3)
#define MFIS_MFIERRSTSR1_RFSO2_TIMEOUT_ERROR    0x00004000uL  ///< RFSO time-out detection timer error (ch2)
#define MFIS_MFIERRSTSR1_RFSO1_TIMEOUT_ERROR    0x00002000uL  ///< RFSO time-out detection timer error (ch1)
#define MFIS_MFIERRSTSR1_RFSO0_TIMEOUT_ERROR    0x00001000uL  ///< RFSO time-out detection timer error (ch0)

#define MFIS_MFIERRSTSR4_AXI_BUS_EDC_FATAL      0x01000000uL  ///< AXI BUS Fatal EDC Error
#define MFIS_MFIERRSTSR4_AXI_ACCESS_PROTECTION_DRAM      0x00800000uL  ///< AXI BUS Fatal EDC Error
#define MFIS_MFIERRSTSR12_AXI_ACCESS_PROTECTION_RT_SRAM  0x00004000uL  ///< AXI BUS Fatal EDC Error

#define MFIS_MFIERRSTSR7_A53_ECC_L2_FATAL       0x02000000uL  ///< Cortex-A53 L2 cache RAM ECC fatal error
#define MFIS_MFIERRSTSR7_A53_ECC_L2             0x01000000uL  ///< Cortex-A53 L2 cache RAM ECC correctable error
#define MFIS_MFIERRSTSR7_A53_ECC_1_RAM_FATAL    0x00080000uL  ///< Cortex-A53 Core1 RAM ECC fatal error
#define MFIS_MFIERRSTSR7_A53_ECC_1_RAM          0x00040000uL  ///< Cortex-A53 Core1 RAM ECC correctable error
#define MFIS_MFIERRSTSR7_A53_ECC_0_RAM_FATAL    0x00020000uL  ///< Cortex-A53 Core0 RAM ECC fatal error
#define MFIS_MFIERRSTSR7_A53_ECC_0_RAM          0x00010000uL  ///< Cortex-A53 Core0 RAM ECC correctable error
#define MFIS_MFIERRSTSR7_RFSO10_TIMEOUT_ERROR   0x40000000uL  ///< RFSO time-out detection timer error (ch10)
#define MFIS_MFIERRSTSR7_RFSO9_TIMEOUT_ERROR    0x20000000uL  ///< RFSO time-out detection timer error (ch9)
#define MFIS_MFIERRSTSR7_RFSO8_TIMEOUT_ERROR    0x10000000uL  ///< RFSO time-out detection timer error (ch8)
#define MFIS_MFIERRSTSR7_RFSO7_TIMEOUT_ERROR    0x08000000uL  ///< RFSO time-out detection timer error (ch7)

#define MFIS_MFIERRSTSR8_ECC_RAM_FATAL          0x01000000uL  ///< RAM ECC 2-Bit Error
#define MFIS_MFIERRSTSR8_ECC_RAM                0x00800000uL  ///< RAM ECC 1-Bit Error
#define MFIS_MFIERRSTSR8_WWDT1_ERROR            0x00400000uL  ///< Window Watchdog CH1 Error
#define MFIS_MFIERRSTSR8_WWDT0_ERROR            0x00200000uL  ///< Window Watchdog CH0 Error
#define MFIS_MFIERRSTSR8_ICUMXA_WDT_ERROR       0x00100000uL  ///< ICUMXA WDT overflow error
#define MFIS_MFIERRSTSR8_ECC_RTSRAM_FATAL       0x00000001uL  ///< RT-SRAM ECC multi-bit error

#define MFIS_MFIERRSTSR9_SW_GENERATED_ERROR_1   0x02000000uL  ///< Software Generated Error (ID1)
#define MFIS_MFIERRSTSR9_SW_GENERATED_ERROR_2   0x04000000uL  ///< Software Generated Error (ID2)
/** @} */ /* End grouping */

/** **********************************************************************************************
 *  \brief  Clock register defines
 *
 *  Clock register defines
 *
 *  \range       Fixed
 *  \resolution  1
 *  \unit        None
 *
 */
/** @{ */ /* Start grouping */
#define CPG_FSCLKCSR_CMRS_DIV8      0x00100000uL  ///< Select the division ratio of internal RCLK(1/8)
#define CPG_FSCLKCSR_CMRS_DIV16     0x00200000uL  ///< Select the division ratio of internal RCLK(1/16)
#define CPG_FSCLKCSR_CMRS_DIV32     0x00300000uL  ///< Select the division ratio of internal RCLK(1/32)
#define CPG_FSCLKCSR_CMRS_DIV128    0x00400000uL  ///< Select the division ratio of internal RCLK(1/128)
#define CPG_FSCLKCSR_CMRS_DIV256    0x00500000uL  ///< Select the division ratio of internal RCLK(1/256)
#define CPG_FSCLKCSR_CMRS_DIV512    0x00600000uL  ///< Select the division ratio of internal RCLK(1/512)
#define CPG_FSCLKCSR_CMRS_DIV1024   0x00700000uL  ///< Select the division ratio of internal RCLK(1/1024)

#define CPG_FSCLKCSR_EXRS_DIV8      0x00010000uL  ///< Select the division ratio of internal RCLK(1/8)
#define CPG_FSCLKCSR_EXRS_DIV16     0x00020000uL  ///< Select the division ratio of internal RCLK(1/16)
#define CPG_FSCLKCSR_EXRS_DIV32     0x00030000uL  ///< Select the division ratio of internal RCLK(1/32)
#define CPG_FSCLKCSR_EXRS_DIV128    0x00040000uL  ///< Select the division ratio of internal RCLK(1/128)
#define CPG_FSCLKCSR_EXRS_DIV256    0x00050000uL  ///< Select the division ratio of internal RCLK(1/256)
#define CPG_FSCLKCSR_EXRS_DIV512    0x00060000uL  ///< Select the division ratio of internal RCLK(1/512)
#define CPG_FSCLKCSR_EXRS_DIV1024   0x00070000uL  ///< Select the division ratio of internal RCLK(1/1024)

#define CPG_FSCLKCSR_CHKE_ENABLE    0x00008000uL  ///< Enable checking clock frequency
#define CPG_FSCLKCSR_CHKE_DISABLE   0x00000000uL  ///< Disable checking clock frequency

#define CPG_FSCLKCSR_MSKINT_ENABLE  0x00000200uL  ///< Issue a clock oscillation stop interrupt
#define CPG_FSCLKCSR_MSKINT_DISABLE 0x00000000uL  ///< Mask a clock oscillation stop interrupt

#define CPG_FSCLKCSR_ERCE_ENABLE    0x00000040uL  ///< Enable the oscillation check of EXTALR(OCO)
#define CPG_FSCLKCSR_ERCE_DISABLE   0x00000000uL  ///< Disable the oscillation check of EXTALR(OCO)

#define CPG_FSCLKCSR_CPCE_ENABLE    0x00000020uL  ///< Enable the oscillation check of CP
#define CPG_FSCLKCSR_CPCE_DISABLE   0x00000000uL  ///< Disable the oscillation check of CP

#define CPG_FSCLKCSR_P4CE_ENABLE    0x00000010uL  ///< Enable the oscillation check of PLL4
#define CPG_FSCLKCSR_P4CE_DISABLE   0x00000000uL  ///< disable the oscillation check of PLL4

#define CPG_FSCLKCSR_P3CE_ENABLE    0x00000008uL  ///< Enable the oscillation check of PLL3
#define CPG_FSCLKCSR_P3CE_DISABLE   0x00000000uL  ///< disable the oscillation check of PLL3

#define CPG_FSCLKCSR_P2CE_ENABLE    0x00000004uL  ///< Enable the oscillation check of PLL2
#define CPG_FSCLKCSR_P2CE_DISABLE   0x00000000uL  ///< disable the oscillation check of PLL2

#define CPG_FSCLKCSR_P1CE_ENABLE    0x00000002uL  ///< Enable the oscillation check of PLL1
#define CPG_FSCLKCSR_P1CE_DISABLE   0x00000000uL  ///< disable the oscillation check of PLL1

#define CPG_FSCLKCSR_P0CE_ENABLE    0x00000001uL  ///< Enable the oscillation check of PLL0
#define CPG_FSCLKCSR_P0CE_DISABLE   0x00000000uL  ///< disable the oscillation check of PLL0

#define CPG_DUMYMFIS                0xE6150770uL  ///< Dummy Error Set for error request to MFIS Register
/** @} */ /* End grouping */

/** **********************************************************************************************
 *  \brief  Backup Buffer Access Register
 *
 *  Register for the V3M CUT2 Register
 *
 *  \range       Fixed
 *  \resolution  1
 *  \unit        None
 *
 */
#define BKBAPR            0xE6150278uL          ///< Back-up buffer access protect register

/** **********************************************************************************************
 *  \brief  CPG PPL2 Control Register
 *
 *  Configuration Register for CPG PPL2
 *
 *  \range       Fixed
 *  \resolution  1
 *  \unit        None
 *
 */
#define CPG_PLL2CR 0xE615002CuL

#endif
