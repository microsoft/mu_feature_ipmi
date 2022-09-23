/** @file
  NULL implementation of the IPMI platform library.

  Copyright (c) Microsoft Corporation
  PDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/IpmiPlatformLib.h>

/**
  Checks for the results of device specific self test codes.

  @param[in]  Result      The self-test result to be checked by the platform.
  @param[out] BmcStatus   A pointer that receives the BMC status change if needed.
**/
VOID
EFIAPI
GetDeviceSpecificTestResults (
  IN  IPMI_SELF_TEST_RESULT_RESPONSE  *Result,
  OUT BMC_STATUS                      *BmcStatus
  )
{
  *BmcStatus = BMC_HARDFAIL;
  return;
}

/**
  Sets a platform IO range for use in the IPMI stack.

  @param[in]    IpmiIoBase      The IO port range to set.

  @retval       EFI_SUCCESS     The port was successfully set.
**/
EFI_STATUS
EFIAPI
PlatformIpmiIoRangeSet (
  IN UINT16  IpmiIoBase
  )
{
  return EFI_SUCCESS;
}

/**
  Performs any platform specific initialization needed for the BMC or IPMI
  stack.

  @retval   EFI_SUCCESS   The platform initialization was successfully completed.
**/
EFI_STATUS
EFIAPI
PlatformIpmiInitialize (
  VOID
  )
{
  return EFI_SUCCESS;
}

/**
  Performs any platform specific initialization needed for the BMC or IPMI
  stack.

  @retval   EFI_SUCCESS   The platform initialization was successfully completed.
**/
EFI_STATUS
EFIAPI
PlatformIpmiGetFrb2Settings (
  VOID
  )
{
  return EFI_SUCCESS;
}
