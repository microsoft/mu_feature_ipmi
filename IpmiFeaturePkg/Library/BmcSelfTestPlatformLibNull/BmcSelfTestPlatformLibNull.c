/** @file
  NULL implementation of the BMC self-test platform library.

  Copyright (c) Microsoft Corporation
  PDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/BmcSelfTestPlatformLib.h>

/**
  Checks for the results of device specific self test codes.

  @param[in]  Result      The self-test result to be checked by the platform.
  @param[out] BmcStatus   A pointer that receives the BMC status change if needed.
**/
VOID
GetDeviceSpecificTestResults (
  IN  IPMI_SELF_TEST_RESULT_RESPONSE  *Result,
  OUT BMC_STATUS                      *BmcStatus
  )
{
  *BmcStatus = BMC_HARDFAIL;
  return;
}
