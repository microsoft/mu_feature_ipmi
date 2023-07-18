/** @file    MockApi.c

  Copyright (c) Microsoft Corporation. All rights reserved.

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
#include <Library/PlatformPowerRestorePolicyConfigurationLib.h>

/**
  @brief Mock this api locally in the test Get the Platform Power Restore Policy Setting Value object

  @param PowerRestorePolicy
  @retval EFI_STATUS
**/
EFI_STATUS
EFIAPI
GetPowerRestorePolicy (
  OUT UINT8  *PowerRestorePolicy
  )
{
  *PowerRestorePolicy = mock_type (UINT8);
  return mock_type (UINTN);
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
