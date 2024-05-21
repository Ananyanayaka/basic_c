// Copyright (C) Continental AG
// All Rights Reserved
// COMPONENT: Types
// VERSION:   $Revision: 1.2 $
/// @file     01_Source_Code\common\GLOB\GLOB_McuDefs.h
/// @brief    common MCU definitions

#ifndef GLOB_MCUDEFS_H
#define GLOB_MCUDEFS_H

#ifdef __cplusplus
extern "C"
{
#endif //-- __cplusplus

/// Description: Racerunner ID (used for PAL_MCU) \n
/// Range:       1 (constant) \n
/// Resolution:  1 \n
/// Unit:        none
#define PAL_MCU_RACERUNNER 1

/// Description: Racerunner Ultra ID (used for PAL_MCU) \n
/// Range:       2 (constant) \n
/// Resolution:  1 \n
/// Unit:        none
#define PAL_MCU_RACERUNNER_ULTRA 2

/// Description: Racerunner Skinny ID (used for PAL_MCU) \n
/// Range:       3 (constant) \n
/// Resolution:  1 \n
/// Unit:        none
#define PAL_MCU_RACERUNNER_SKINNY 3

/// Description: Vision Low ID (used for PAL_MCU) \n
/// Range:       4 (constant) \n
/// Resolution:  1 \n
/// Unit:        none
#define PAL_MCU_VISION_LOW 4

/// Description: Renesas RH850 ID (used for PAL_MCU) \n
/// Range:       5 (constant) \n
/// Resolution:  1 \n
/// Unit:        none
#define PAL_MCU_RENESAS 5

/// Description: R-Car V3M Cortex R7 ID (used for PAL_MCU) \n
/// Range:       6 (constant) \n
/// Resolution:  1 \n
/// Unit:        none
#define PAL_MCU_RENR7 6

/// Description: ARM Cortex-R4F, TI AR1642 (used for PAL_MCU) \n
/// Range:       7 (constant) \n
/// Resolution:  1 \n
/// Unit:        none
#define PAL_MCU_TIR4F 7

/// Description: R-Car V3M Cortex A53 ID (used for PAL_MCU) \n
/// Range:       8 (constant) \n
/// Resolution:  1 \n
/// Unit:        none
#define PAL_MCU_RENA53 8

/// Description: XAZU4EV Cortex R5 ID (used for PAL_MCU) \n
/// Range:       9 (constant) \n
/// Resolution:  1 \n
/// Unit:        none
#define PAL_MCU_XILR5 9

/// Description: XAZU4EV Cortex A53 ID (used for PAL_MCU) \n
/// Range:       10 (constant) \n
/// Resolution:  1 \n
/// Unit:        none
#define PAL_MCU_XILA53 10

// configure project
// PRQA S 0810, 0388 14
// 2018-10-26; uidw1725
// summary: Msg(3:0810) [L] '#include causes nesting to exceed 8 levels - program does not conform strictly to ISO:C90.
// reason:  used compiler supports nested comment level.
#include <GLOB_McuDefs_Cfg.h>

#ifdef __cplusplus
}
#endif //-- __cplusplus

#endif //-- GLOB_MCUDEFS_H
