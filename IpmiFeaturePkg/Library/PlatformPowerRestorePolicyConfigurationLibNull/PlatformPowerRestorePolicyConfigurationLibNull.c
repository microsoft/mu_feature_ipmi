/** @file
  PlatformPowerRestorePolicyConfigurationLibNull.h

  Copyright (c) Microsoft Corporation.

**/
#include <Uefi.h>
#include <Library/PlatformPowerRestorePolicyConfigurationLib.h>

/**
  Get PowerRestorePolicy

  @param[out] PowerRestorePolicy  The PowerRestorePolicy setting of the platform

  @retval EFI_INVALID_PARAMETER      The input parameter is invalid.
  @retval EFI_SUCCESS                Get PowerRestorePolicy successfully.
**/
EFI_STATUS
EFIAPI
GetPowerRestorePolicy (
  OUT UINT8  *PowerRestorePolicy
  )
{
  if (PowerRestorePolicy == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  *PowerRestorePolicy = POWER_RESTORE_POLICY_NO_CHANGE;

  return EFI_SUCCESS;
}
