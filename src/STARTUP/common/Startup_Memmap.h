// Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
// All Rights Reserved
// COMPONENT: Startup AWR294x
/// \file Startup_Memmap.h
/// \brief Memory mapping for Startup AWR294x

#ifndef STARTUP_MEMMAP_H
#define STARTUP_MEMMAP_H

#ifdef __cplusplus
extern "C"
{
#endif

#define MEMMAP_ERROR

// STARTUP SECTION Definition
#ifdef STARTUP_START_SEC_CODE_ASIL_B
  #undef STARTUP_START_SEC_CODE_ASIL_B
  #undef MEMMAP_ERROR
  #ifdef __DCC_LLVM__
    // QAC 3116: Unrecognized #pragma arguments '%s' This #pragma directive has been ignored.
    // Reason: QAC Helix doesn't recognize the compiler specific pragma intrinsic.
    // PRQA S 3116 1
    #pragma clang section text=".startupCode"
  #endif //#if defined( __DCC_LLVM__ )
#endif //#ifdef STARTUP_START_SEC_CODE_ASIL_B

#ifdef STARTUP_STOP_SEC_CODE_ASIL_B
  #undef STARTUP_STOP_SEC_CODE_ASIL_B
  #undef MEMMAP_ERROR
  #ifdef __DCC_LLVM__
    // QAC 3116: Unrecognized #pragma arguments '%s' This #pragma directive has been ignored.
    // Reason: QAC Helix doesn't recognize the compiler specific pragma intrinsic.
    // PRQA S 3116 1
    #pragma clang section text=""
  #endif //#if defined( __DCC_LLVM__ )
#endif //#ifdef STARTUP_STOP_SEC_CODE_ASIL_B

// STARTUP CONST SECTION Definition
#ifdef STARTUP_START_SEC_CONST_ASIL_B_UNSPECIFIED
  #undef STARTUP_START_SEC_CONST_ASIL_B_UNSPECIFIED
  #undef MEMMAP_ERROR
  #ifdef __DCC_LLVM__
    // QAC 3116: Unrecognized #pragma arguments '%s' This #pragma directive has been ignored.
    // Reason: QAC Helix doesn't recognize the compiler specific pragma intrinsic.
    // PRQA S 3116 1
    #pragma clang section rodata=".startuprodata"
  #endif //#if defined( __DCC_LLVM__ )
#endif //#ifdef STARTUP_START_SEC_CONST_ASIL_B_UNSPECIFIED

#ifdef STARTUP_STOP_SEC_CONST_ASIL_B_UNSPECIFIED
  #undef STARTUP_STOP_SEC_CONST_ASIL_B_UNSPECIFIED
  #undef MEMMAP_ERROR
  #ifdef __DCC_LLVM__
    // QAC 3116: Unrecognized #pragma arguments '%s' This #pragma directive has been ignored.
    // Reason: QAC Helix doesn't recognize the compiler specific pragma intrinsic.
    // PRQA S 3116 1
    #pragma clang section rodata=""
  #endif //#if defined( __DCC_LLVM__ )
#endif //#ifdef STARTUP_STOP_SEC_CONST_ASIL_B_UNSPECIFIED

#ifdef MEMMAP_ERROR
  #error No code MemMap section found in Startup_Memmap.h. Check your section define for validity.
#endif

#ifdef __cplusplus
}
#endif

#endif // #ifndef STARTUP_MEMMAP_H
