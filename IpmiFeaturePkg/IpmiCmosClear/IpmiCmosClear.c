/** @file
  The DXE implementation of the IPMI CMOS clear module.

  Copyright (c) Microsoft Corporation
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>

#include <Library/IpmiBootOptionLib.h>
#include <Library/PlatformCmosClearLib.h>

/**
  Entry point to the IPMI CMOS Clear DXE module.

  @param[in]    ImageHandle   The handle for this module image.
  @param[in]    SystemTable   Pointer to the UEFI system table.

  @retval   EFI_SUCCESS           The watchdog timer and callbacks were properly setup.
  @retval   EFI_PROTOCOL_ERROR    An unexpected Completion Code was returned by IPMI.
  @retval   Other                 An error was returned by a subroutine.
**/
EFI_STATUS
EFIAPI
IpmiCmosEntryPoint (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS  Status;
  BOOLEAN     ClearCmos;
  BOOLEAN     RebootRequired;

  Status = IpmiGetCmosClearOption (&ClearCmos);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a: Failed to query CMOS clear bit in IPMI boot options! %r\n", __FUNCTION__, Status));
    return Status;
  }

  DEBUG ((DEBUG_INFO, "CMOS Clear: %d\n", ClearCmos));

  if (ClearCmos) {
    RebootRequired = FALSE;
    Status         = PlatformClearCmos (&RebootRequired);
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_ERROR, "%a: Platform CMOS clear failed! %r\n", __FUNCTION__, Status));
      return Status;
    }

    DEBUG ((DEBUG_INFO, "CMOS Cleared. RebootRequired: %x\n", RebootRequired));
    if (RebootRequired) {
      gRT->ResetSystem (EfiResetCold, EFI_SUCCESS, 0, NULL);
    }
  }

  return EFI_SUCCESS;
}
