/** @file IpmiPowerRestorePolicy.c

  This file implementing a driver entrypoint that configure Power Restore Policy via IPMI command

  Copyright (c) Microsoft Corporation.
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <PiPei.h>
#include <Library/DebugLib.h>
#include <Library/IpmiCommandLib.h>
#include <Guid/PlatformPowerRestorePolicy.h>
#include <Library/PolicyLib.h>

#define  POWER_RESTORE_POLICY_UNKNOWN     0x03  // "Get Chassis Status"

/**
  Configure Power Restore Policy via IPMI

  @param[in]  FileHandle      Handle of the file
  @param[in]  PeiServices     List of possible PEI Services.

  @return EFI_SUCCESS         Power Restore Policy is configured to expected setting successfully.
  @return Others              Failed to configure Power Restore Policy
**/
EFI_STATUS
EFIAPI
IpmiPowerRestorePolicyEntry (
  IN       EFI_PEI_FILE_HANDLE  FileHandle,
  IN CONST EFI_PEI_SERVICES     **PeiServices
  )
{
  EFI_STATUS                              Status;
  IPMI_GET_CHASSIS_STATUS_RESPONSE        ChassisStatusRespData;
  UINT8                                   CurrentPowerRestorePolicy;
  PLATFORM_POWER_RESTORE_POLICY           PlatformPowerRestorePolicy;
  IPMI_SET_POWER_RESTORE_POLICY_REQUEST   SetRestorePolicyRequest;
  IPMI_SET_POWER_RESTORE_POLICY_RESPONSE  SetRestorePolicyResponse;
  UINT16                                  PolicySize = sizeof (PLATFORM_POWER_RESTORE_POLICY);
  //
  // Get current power restore policy setting
  //
  Status = IpmiGetChassisStatus (&ChassisStatusRespData);

  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "[%a] - IpmiGetChassisStatus: %r\n", __FUNCTION__, Status));
    CurrentPowerRestorePolicy = POWER_RESTORE_POLICY_UNKNOWN;
  } else {
    CurrentPowerRestorePolicy = (ChassisStatusRespData.CurrentPowerState >> 5) & 0x3; // [6:5] - power restore policy
  }

  DEBUG ((DEBUG_VERBOSE, "[%a] - CurrentPowerRestorePolicy: 0x%x\n", __FUNCTION__, CurrentPowerRestorePolicy));

  //
  // Get platform power restore policy setting
  //
  Status     = GetPolicy (&gPlatformPowerRestorePolicyGuid, NULL, &PlatformPowerRestorePolicy, &PolicySize);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a: Failed to get platform power restore policy! %r\n", __FUNCTION__, Status));
    return Status;
  }

  DEBUG ((DEBUG_VERBOSE, "[%a] - PlatformPowerRestorePolicy: 0x%x\n", __FUNCTION__, PlatformPowerRestorePolicy));

  if ( PlatformPowerRestorePolicy.PolicyValue == CurrentPowerRestorePolicy) {
    // Just return if CurrentPowerRestorePolicy the same with PlatformPowerRestorePolicy
    DEBUG ((DEBUG_VERBOSE, "[%a] - Current Power Restore Policy is consistent with platform setting\n", __FUNCTION__));
    return EFI_SUCCESS;
  }

  //
  // If platform setting is not POWER_RESTORE_POLICY_NO_CHANGE, then configure the power restore policy based on PlatformPowerRestorePolicy
  //
  if (PlatformPowerRestorePolicy.PolicyValue != PowerRestorePolicyNoChange) {
    SetRestorePolicyRequest.PowerRestorePolicy.Bits.PowerRestorePolicy = (UINT8)PlatformPowerRestorePolicy.PolicyValue;
    SetRestorePolicyRequest.PowerRestorePolicy.Bits.Reserved           = 0;
    Status                                                             = IpmiSetPowerRestorePolicy (&SetRestorePolicyRequest, &SetRestorePolicyResponse);
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_ERROR, "[%a] - IpmiSetPowerRestorePolicy status: %r\n", __FUNCTION__, Status));
      return Status;
    } else if (SetRestorePolicyResponse.CompletionCode != 0) {
      DEBUG ((DEBUG_VERBOSE, "[%a] - IpmiSetPowerRestorePolicy completion code: %r\n", __FUNCTION__, SetRestorePolicyResponse.CompletionCode));
      return EFI_UNSUPPORTED;
    }
  }

  return EFI_SUCCESS;
}
