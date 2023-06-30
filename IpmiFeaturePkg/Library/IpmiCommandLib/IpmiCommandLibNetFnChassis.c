/** @file
  IPMI Command - NetFnChassis.

  Copyright (c) 2018 - 2021, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#include <PiPei.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/IpmiBaseLib.h>

#include <IndustryStandard/Ipmi.h>

EFI_STATUS
EFIAPI
IpmiGetChassisCapabilities (
  OUT IPMI_GET_CHASSIS_CAPABILITIES_RESPONSE  *GetChassisCapabilitiesResponse
  )
{
  EFI_STATUS  Status;
  UINT32      DataSize;

  DataSize = sizeof (*GetChassisCapabilitiesResponse);
  Status   = IpmiSubmitCommand (
               IPMI_NETFN_CHASSIS,
               IPMI_CHASSIS_GET_CAPABILITIES,
               NULL,
               0,
               (VOID *)GetChassisCapabilitiesResponse,
               &DataSize
               );
  return Status;
}

EFI_STATUS
EFIAPI
IpmiGetChassisStatus (
  OUT IPMI_GET_CHASSIS_STATUS_RESPONSE  *GetChassisStatusResponse
  )
{
  EFI_STATUS  Status;
  UINT32      DataSize;

  DataSize = sizeof (*GetChassisStatusResponse);
  Status   = IpmiSubmitCommand (
               IPMI_NETFN_CHASSIS,
               IPMI_CHASSIS_GET_STATUS,
               NULL,
               0,
               (VOID *)GetChassisStatusResponse,
               &DataSize
               );
  return Status;
}

EFI_STATUS
EFIAPI
IpmiChassisControl (
  IN IPMI_CHASSIS_CONTROL_REQUEST  *ChassisControlRequest,
  OUT UINT8                        *CompletionCode
  )
{
  EFI_STATUS  Status;
  UINT32      DataSize;

  DataSize = sizeof (*CompletionCode);
  Status   = IpmiSubmitCommand (
               IPMI_NETFN_CHASSIS,
               IPMI_CHASSIS_CONTROL,
               (VOID *)ChassisControlRequest,
               sizeof (*ChassisControlRequest),
               (VOID *)CompletionCode,
               &DataSize
               );
  return Status;
}

/**
  This function sets power restore policy.

  @param[in]  SetPowerRestireRequest    The set power restore policy control
                                        command request.
  @param[out] SetPowerRestireResponse   The response of power restore policy.

  @retval EFI_SUCCESS            Command is sent successfully.
  @retval EFI_NOT_AVAILABLE_YET  Transport interface is not ready yet.
  @retval Other                  Failure.

**/
EFI_STATUS
EFIAPI
IpmiSetPowerRestorePolicy (
  IN  IPMI_SET_POWER_RESTORE_POLICY_REQUEST   *SetPowerRestireRequest,
  OUT IPMI_SET_POWER_RESTORE_POLICY_RESPONSE  *SetPowerRestireResponse
  )
{
  EFI_STATUS  Status;
  UINT32      DataSize;

  DataSize = sizeof (*SetPowerRestireResponse);
  Status   = IpmiSubmitCommand (
               IPMI_NETFN_CHASSIS,
               IPMI_CHASSIS_SET_POWER_RESTORE_POLICY,
               (VOID *)SetPowerRestireRequest,
               sizeof (*SetPowerRestireRequest),
               (VOID *)SetPowerRestireResponse,
               &DataSize
               );
  return Status;
}

EFI_STATUS
EFIAPI
IpmiSetSystemBootOptions (
  IN  IPMI_SET_BOOT_OPTIONS_REQUEST   *BootOptionsRequest,
  OUT IPMI_SET_BOOT_OPTIONS_RESPONSE  *BootOptionsResponse
  )
{
  EFI_STATUS  Status;
  UINT32      DataSize;

  DataSize = sizeof (*BootOptionsResponse);
  Status   = IpmiSubmitCommand (
               IPMI_NETFN_CHASSIS,
               IPMI_CHASSIS_SET_SYSTEM_BOOT_OPTIONS,
               (VOID *)BootOptionsRequest,
               sizeof (*BootOptionsRequest),
               (VOID *)BootOptionsResponse,
               &DataSize
               );
  return Status;
}

EFI_STATUS
EFIAPI
IpmiGetSystemBootOptions (
  IN  IPMI_GET_BOOT_OPTIONS_REQUEST   *BootOptionsRequest,
  OUT IPMI_GET_BOOT_OPTIONS_RESPONSE  *BootOptionsResponse
  )
{
  EFI_STATUS  Status;
  UINT32      DataSize;

  DataSize = sizeof (*BootOptionsResponse);
  Status   = IpmiSubmitCommand (
               IPMI_NETFN_CHASSIS,
               IPMI_CHASSIS_GET_SYSTEM_BOOT_OPTIONS,
               (VOID *)BootOptionsRequest,
               sizeof (*BootOptionsRequest),
               (VOID *)BootOptionsResponse,
               &DataSize
               );
  return Status;
}
