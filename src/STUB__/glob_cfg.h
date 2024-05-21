// COMPANY:   Continental Automotive
// COMPONENT: Types
// VERSION:   $Revision: 1.3 $
/// @file  00_Projects\MFC510\common\GLOB\glob_cfg.h
/// @brief Global types and definitions MFC510

#ifndef GLOB_CFG_H
#define GLOB_CFG_H
#include "Reg_RCarV3H.h"
#ifdef __cplusplus
extern "C"
{
// PRQA S ALL ++
// 2021-05-21; uie23485
// Summary:
// Message(4:1020) Avoid Macros
// Message(4:1025) Macros may be used as literal
// Message(3:5053) Lower case letter in suffix
// Message(3:5174) Macro definition of standard library macro 'offsetof'
// Message(3:5181) Reuse of definition of standard library macro 'offsetof'
// Reason:         QACPP tool doesn't recognize the extern C command
#endif //-- __cplusplus

/// Description:  usage of memchr() from SYS component \n
/// Range:        1 (STD_ON on Infra), 0 (STD_OFF on Infra) \n
/// Resolution:   - \n
/// Unit:         -
#define SYS_MEMCHR_ENABLED  0u

/// Description: Nominal battery voltage \n
/// Nominal battery voltage in millivolt (float) \n
/// Range       Any floating point number \n
/// Resolution  1 \n
/// Unit        mV
#ifndef GLOB_BATTERY_VOLTAGE
#define GLOB_BATTERY_VOLTAGE  12000.0f
#endif


/// Description:  Switch for enabling/disabling New MTSI \n
///               0 or is NOT defined => Legacy MTS is used \n
///               1                   => New MTSI is used \n
/// Range:        {0, 1} \n
/// Resolution:   Boolean \n
/// Unit:         -
#define FEAT_MTSI_OPTIMIZEDVERSION_USED 1u


/// Description: provide macro offsetof
/// @param[in]   s : structure type
/// @param[in]   memb : a member of the structure s
/// @return      the offset in bytes of the member memb in the structure s
#ifndef _MSC_VER
#ifndef offsetof
  #define offsetof(s, memb) ((uint32)((uint8 *)&((s *)0u)->memb - (uint8 *)0u))
#endif
#endif

#ifdef __cplusplus
}
// PRQA S ALL --
#endif //-- __cplusplus
#define CMT1_CMCNT0                     0xE6130014                  // R/W 32  Compare match timer counter 0
#endif //-- GLOB_CFG_H
