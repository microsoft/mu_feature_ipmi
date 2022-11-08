/** @file
  Definitions for the platform CMOS clear library.

  Copyright (c) Microsoft Corporation
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef PLATFORM_CMOS_CLEAR_LIB_H_
#define PLATFORM_CMOS_CLEAR_LIB_H_

#include <Uefi.h>

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
  );

#endif
