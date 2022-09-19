/** @file
  NULL implementation of the SSIF SMBus library.

  Copyright (c) Microsoft Corporation
  PDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Library/BaseLib.h>
#include <PiPei.h>

/**
  Opens the SMBus connection if needed.

  @retval   EFI_SUCCESS           Them SMBus connection was successfully opened.
**/
EFI_STATUS
EFIAPI
SendPreBootSignaltoBmc (
  IN CONST EFI_PEI_SERVICES  **PeiServices
  )
{
  return EFI_SUCCESS;
}
