/** @file
  Definitions for the IPMI platform library. This library allows
  the platform to customize IPMI initialization and provide device specific
  implementation details.

  Copyright (c) Microsoft Corporation
  Copyright (c) 2018, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef _IPMI_PLATFORM_LIB_H_
#define _IPMI_PLATFORM_LIB_H_

#include <Uefi.h>
#include <IndustryStandard/Ipmi.h>
#include <IpmiInterface.h>

/**
  Sets a platform IO range for use in the IPMI stack.

  @param[in]    IpmiIoBase      The IO port range to set.

  @retval       EFI_SUCCESS     The port was successfully set.
**/
EFI_STATUS
EFIAPI
PlatformIpmiIoRangeSet (
  IN UINT16  IpmiIoBase
  );

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
  );

/**
  Performs any platform specific initialization needed for the BMC or IPMI
  stack.

  @retval   EFI_SUCCESS   The platform initialization was successfully completed.
**/
EFI_STATUS
EFIAPI
PlatformIpmiInitialize (
  VOID
  );

#endif
