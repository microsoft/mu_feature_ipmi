/** @file
  Definitions for the BMC self-test platform library which allows a platform
  to check for device specific self-test results.

  Copyright (c) Microsoft Corporation
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <IndustryStandard/IpmiNetFnApp.h>
#include <ServerManagement.h>

/**
  Checks for the results of device specific self test codes.

  @param[in]  Result      The self-test result to be checked by the platform.
  @param[out] BmcStatus   A pointer that receives the BMC status change if needed.

  @retval   EFI_SUCCESS                 <Description>
**/
VOID
GetDeviceSpecificTestResults (
  IN  IPMI_SELF_TEST_RESULT_RESPONSE  *Result,
  OUT BMC_STATUS                      *BmcStatus
  );
