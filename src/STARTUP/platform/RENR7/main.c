// Copyright (C) Continental AG
// All Rights Reserved
// COMPONENT: STARTUP_R7
// VERSION:   $Revision: 1.8 $

#include <Os.h>
#include <startup_r7.h>

#ifdef __cplusplus
extern "C"
{
#endif

// Variables have already been initialized at this point
int main(void)
{
  /* enable D$ */
  STARTUP_v_EnableCache();

  (void)ChainTask(TSK_WdtST);

  return 0;
}

#ifdef __cplusplus
}
#endif

