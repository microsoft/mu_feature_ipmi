/** @file
  The PEI implementation of the IPMI watchdog module.

  Copyright (c) Microsoft Corporation
  Copyright (c) 2018 - 2019, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <PiPei.h>
#include <Library/DebugLib.h>
#include <Library/PcdLib.h>

#include <IndustryStandard/Ipmi.h>
#include <IpmiWatchdog.h>

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
  if (PcdGetBool (PcdFrb2EnabledFlag)) {
    IpmiEnableWatchdogTimer (
      IPMI_WATCHDOG_TIMER_BIOS_FRB2,
      PcdGet8 (PcdFrb2TimeoutAction),
      IPMI_WATCHDOG_TIMER_EXPIRATION_FLAG_BIOS_FRB2,
      PcdGet16 (PcdFrb2TimeoutSeconds)
      );
  }

  return EFI_SUCCESS;
}
