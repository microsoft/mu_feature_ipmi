/** @file MockApi.c

  Copyright (c) Microsoft Corporation.
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <stdint.h>
#include <cmocka.h>

#include <PiPei.h>
#include <Library/DebugLib.h>
#include <Library/IpmiCommandLib.h>
#include <Library/PolicyLib.h>
#include <Library/BaseMemoryLib.h>

/**
 * @brief Mock version of GetPolicy
 *
 */
EFI_STATUS
EFIAPI
GetPolicy (
  IN CONST EFI_GUID  *PolicyGuid,
  OUT UINT64         *Attributes OPTIONAL,
  OUT VOID           *Policy,
  IN OUT UINT16      *PolicySize
  )
{
  EFI_STATUS  Status;

  // Check that this is the right guid being used
  check_expected_ptr (PolicyGuid);

  Status = mock_type (EFI_STATUS);

  if (!EFI_ERROR (Status)) {
    // Set Attributes, Policy and PolicySize
    *PolicySize = (UINT16)mock ();
    if (Policy != NULL) {
      CopyMem ((VOID *)Policy, (VOID *)mock (), *PolicySize);
    }
  }

  return Status;
}

EFI_STATUS
EFIAPI
IpmiGetChassisStatus (
  OUT IPMI_GET_CHASSIS_STATUS_RESPONSE  *GetChassisStatusResponse
  )
{
  GetChassisStatusResponse->CurrentPowerState = mock_type (UINT8);

  return mock_type (UINTN);
}

EFI_STATUS
EFIAPI
IpmiSetPowerRestorePolicy (
  IN  IPMI_SET_POWER_RESTORE_POLICY_REQUEST   *ChassisControlRequest,
  OUT IPMI_SET_POWER_RESTORE_POLICY_RESPONSE  *ChassisControlResponse
  )
{
  function_called ();

  check_expected (ChassisControlRequest->PowerRestorePolicy.Uint8);

  ChassisControlResponse->CompletionCode            = mock_type (UINT8);
  ChassisControlResponse->PowerRestorePolicySupport =  ChassisControlRequest->PowerRestorePolicy.Bits.PowerRestorePolicy;

  return mock_type (UINTN);
}
