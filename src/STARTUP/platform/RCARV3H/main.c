// Copyright (C) Continental AG
// All Rights Reserved
// COMPONENT: STARTUP_R7
// VERSION:   $Revision: 1.6 $
/// @file     main.c
/// @brief    This file is the entry point for the startup VH3 Module

#include <Os.h>
#include <startup_r7.h>

#ifdef __cplusplus
extern "C"
{
#endif

// Dynamic part of OS counters must be initialized early before usage of OS counter.
// Otherwise alarm of index 0 gets triggered unexpectedly.

/// External OS function for counters initialization.
void OS_InitCounters(void);



///   Startup entry point. \n
///   This function is the entry point for the startup VH3 Module. \n
///   All needed initialization are performed before calling this function. \n
///
///   @pre                  OS must be loaded.
///   @post                 ChainTask is running.
///
///   @param[in]            None.
///   @param[out]           None.
///
///   @return               Always return 0.
///
///   @globals
///                         None.
///
///   @InOutCorrelation
///     This function has no input. output is the return value which is always 0 (OK).\n
///     The dynamic behavior is described in the activity diagram below:\n
///     @image html main_Activity.png Activity Diagram
///     @startuml "main_Activity.png"
///     Start
///     :enable vCache;
///     :Init OS dynamic counters;
///     :start OS task WdtST;
///     Stop
///     @enduml
///
///   @callsequence
///     @image html main_seq.png Sequence Diagram
///     @startuml "main_seq.png"
///     participant EXT
///     participant STARTUP
///     participant OS
///     EXT -> STARTUP: main()
///     activate STARTUP
///     STARTUP -> STARTUP: STARTUP_v_EnableCache()
///     STARTUP -> OS: OS_InitCounters()
///     deactivate STARTUP
///     activate OS
///     OS --> STARTUP: void
///     deactivate OS
///     activate STARTUP
///     STARTUP --> EXT: 0
///     deactivate STARTUP
///     @enduml
///
///   @testmethod
///     1. TEST_RCARV3H_main_function_test()
///   @traceability
///   Link to L4 design specification in Doors:
///   <a href="doors://rbgs854a:40000/?version=2&prodID=0&view=00000007&urn=urn:telelogic::1-503e822e5ec3651e-O-422-0008a042">*Doors requirements L4_SW_422*</a> \n
///   doors://rbgs854a:40000/?version=2&prodID=0&view=00000007&urn=urn:telelogic::1-503e822e5ec3651e-O-422-0008a042
// PRQA S 3469 20
// 2021-1-6; uie38801
// Message(2:3469) This usage of a function-like macro looks like it could be replaced by an equivalent function call.
// Reason: Macro is a part of EB OS standard API.
int main(void)
{
  /* enable D$ */
  STARTUP_v_EnableCache();

  // ensure that dynamic part of OS counters is initialized before used
  OS_InitCounters();

  (void)ChainTask(TSK_WdtST);

  return 0;
}

#ifdef __cplusplus
}
#endif

