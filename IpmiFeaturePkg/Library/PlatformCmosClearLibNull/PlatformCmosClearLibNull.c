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

  @param[out]   RebootRequired    Indicates the platform should be rebooted after
                                  CMOS clear. If the routine is successful and
                                  RebootRequired is TRUE then the caller will
                                  reset the system.

  @retval   EFI_SUCCESS     CMOS was successfully cleared.
**/
EFI_STATUS
EFIAPI
PlatformClearCmos (
  OUT BOOLEAN  *RebootRequired
  )
{
  *RebootRequired = FALSE;
  return EFI_UNSUPPORTED;
}
