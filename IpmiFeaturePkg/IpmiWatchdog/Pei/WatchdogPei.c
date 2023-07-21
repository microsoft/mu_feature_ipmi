/** @file
  The PEI implementation of the IPMI watchdog module.

  Copyright (c) Microsoft Corporation
  Copyright (c) 2018 - 2019, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <PiPei.h>
#include <Library/DebugLib.h>
#include <Library/PolicyLib.h>

#include <IndustryStandard/Ipmi.h>
#include <Library/IpmiWatchdogLib.h>
#include <Guid/IpmiWatchdogPolicy.h>

/**
  Entry for the IPMI watchdog PEIM. Initialized the FRB2 watchdog timer if
  requested.

  @param[in]  FileHandle      Unused.
  @param[in]  PeiServices     Unused.

  @retval   EFI_SUCCESS       Always.
**/
EFI_STATUS
EFIAPI
InitializeWatchdogPei (
  IN       EFI_PEI_FILE_HANDLE  FileHandle,
  IN CONST EFI_PEI_SERVICES     **PeiServices
  )
{
  EFI_STATUS            Status;
  IPMI_WATCHDOG_POLICY  Policy;
  UINT16                PolicySize;

  PolicySize = sizeof (IPMI_WATCHDOG_POLICY);
  Status     = GetPolicy (&gIpmiWatchdogPolicyGuid, NULL, &Policy, &PolicySize);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a: Failed to get watchdog policy! %r\n", __FUNCTION__, Status));
    return Status;
  }

  if (Policy.Frb2Enabled) {
    IpmiEnableWatchdogTimer (
      IPMI_WATCHDOG_TIMER_BIOS_FRB2,
      Policy.Frb2TimeoutAction,
      IPMI_WATCHDOG_TIMER_EXPIRATION_FLAG_BIOS_FRB2,
      Policy.Frb2TimeoutSeconds
      );
  }

  return EFI_SUCCESS;
}
