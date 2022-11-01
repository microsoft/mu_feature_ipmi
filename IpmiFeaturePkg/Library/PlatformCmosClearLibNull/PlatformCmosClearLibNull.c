/** @file
  NULL implementation of the IPMI platform library.

  Copyright (c) Microsoft Corporation
  PDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Uefi.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/PlatformCmosClearLib.h>

/**
  Implements platform specifics to clear the CMOS.

  @retval   EFI_SUCCESS     CMOS was successfully cleared.
**/
EFI_STATUS
EFIAPI
PlatformClearCmos (
  VOID
  )
{
  return EFI_UNSUPPORTED;
}
