/** @file PlatformPowerRestorePolicyDefault.c

  This driver provides default implementation of platform power restore policy.
  Platform can choose to either use this policy or produce policy in their platform code.

  Copyright (c) Microsoft Corporation.
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <PiPei.h>
#include <Library/DebugLib.h>
#include <Library/IpmiCommandLib.h>
#include <Guid/PlatformPowerRestorePolicy.h>
#include <Library/PolicyLib.h>

/**
  Produce the default implementation of platform power restore policy.

  @param[in]  FileHandle      Handle of the file
  @param[in]  PeiServices     List of possible PEI Services.

  @return EFI_SUCCESS         Power Restore Policy is configured to expected setting successfully.
  @return Others              Failed to configure Power Restore Policy
**/
EFI_STATUS
EFIAPI
PlatformPowerRestorePolicyDefaultEntry (
  IN       EFI_PEI_FILE_HANDLE  FileHandle,
  IN CONST EFI_PEI_SERVICES     **PeiServices
  )
{
  PLATFORM_POWER_RESTORE_POLICY  PlatformPowerRestorePolicy;
  EFI_STATUS                     Status;

  //
  // Set PlatformConfigPolicy
  //
  PlatformPowerRestorePolicy.PolicyValue = PowerRestorePolicyNoChange;

  Status = SetPolicy (&gPlatformPowerRestorePolicyGuid, POLICY_ATTRIBUTE_FINALIZED, &PlatformPowerRestorePolicy, sizeof (PLATFORM_POWER_RESTORE_POLICY));
  ASSERT (Status == EFI_SUCCESS);

  return EFI_SUCCESS;
}
