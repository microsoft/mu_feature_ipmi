/** @file
    IpmiPowerRestorePolicy.c

    Copyright (c) Microsoft Corporation.
**/

//
// Common header files
//
#include <PiPei.h>
#include <Library/DebugLib.h>
#include <Library/IpmiCommandLib.h>
#include <Library/PlatformPowerRestorePolicyConfigurationLib.h>

/**
  Synchronize BMC power restore policy setting with config data.

  @param  FileHandle      Handle of the file
  @param  PeiServices     List of possible PEI Services.

  @return EFI_SUCCESS     Synchronize successfully or policy is consistent between BMC and platform.
  @return Others          Synchronize failed
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
  UINT8                                   PlatformPowerRestorePolicy;
  IPMI_SET_POWER_RESTORE_POLICY_REQUEST   SetRestorePolicyRequest;
  IPMI_SET_POWER_RESTORE_POLICY_RESPONSE  SetRestorePolicyResponse;

  //
  // Get current power restore policy
  //
  Status = IpmiGetChassisStatus (&ChassisStatusRespData);

  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "[%a] - IpmiGetChassisStatus: %r\n", __FUNCTION__, Status));
    CurrentPowerRestorePolicy = POWER_RESTORE_POLICY_UNKNOWN;
  } else {
    CurrentPowerRestorePolicy = (ChassisStatusRespData.CurrentPowerState >> 5) & 0x3; // [6:5] - power restore policy
  }

  DEBUG ((DEBUG_INFO, "[%a] - CurrentPowerRestorePolicy: 0x%x\n", __FUNCTION__, CurrentPowerRestorePolicy));

  //
  // Get platform power restore policy
  //
  Status = GetPowerRestorePolicy (&PlatformPowerRestorePolicy);

  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "[%a] - GetPowerRestorePolicy: %r\n", __FUNCTION__, Status));
    return Status;
  }

  DEBUG ((DEBUG_INFO, "[%a] - PlatformPowerRestorePolicy: 0x%x\n", __FUNCTION__, PlatformPowerRestorePolicy));

  if ( PlatformPowerRestorePolicy == CurrentPowerRestorePolicy) {
    // Just return if policy is consistent between BMC and platform.
    DEBUG ((DEBUG_ERROR, "[%a] - Current Power Restore Policy is consistent with platform setting\n", __FUNCTION__));
    return EFI_SUCCESS;
  }

  //
  // If platform setting is not POWER_RESTORE_POLICY_NO_CHANGE, then sync BMC
  //
  if (PlatformPowerRestorePolicy != POWER_RESTORE_POLICY_NO_CHANGE) {
    SetRestorePolicyRequest.PowerRestorePolicy.Bits.PowerRestorePolicy = PlatformPowerRestorePolicy;
    // Set reserved bits to 0
    SetRestorePolicyRequest.PowerRestorePolicy.Bits.Reserved = 0;
    // Update IPMI power restore policy
    Status = IpmiSetPowerRestorePolicy (&SetRestorePolicyRequest, &SetRestorePolicyResponse);
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_ERROR, "[%a] - IpmiSetPowerRestorePolicy status: %r\n", __FUNCTION__, Status));
      return Status;
    } else if (SetRestorePolicyResponse.CompletionCode != 0) {
      DEBUG ((DEBUG_INFO, "[%a] - IpmiSetPowerRestorePolicy completion code: %r\n", __FUNCTION__, SetRestorePolicyResponse.CompletionCode));
      return EFI_UNSUPPORTED; // per spec
    }
  }

  return EFI_SUCCESS;
}
