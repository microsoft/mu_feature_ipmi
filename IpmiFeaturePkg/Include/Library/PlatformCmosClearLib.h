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

  @retval   EFI_SUCCESS     CMOS was successfully cleared.
**/
EFI_STATUS
EFIAPI
PlatformClearCmos (
  VOID
  );

#endif
