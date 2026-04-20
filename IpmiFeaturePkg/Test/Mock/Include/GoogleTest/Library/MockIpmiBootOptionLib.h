/** @file MockIpmiBootOptionLib.h
  Google Test mocks for IpmiBootOptionLib

  Copyright (c) Microsoft Corporation
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#ifndef MOCK_IPMI_BOOT_OPTION_LIB_H_
#define MOCK_IPMI_BOOT_OPTION_LIB_H_

#include <Library/GoogleTestLib.h>
#include <Library/FunctionMockLib.h>
extern "C" {
  #include <Uefi.h>
  #include <Library/IpmiBootOptionLib.h>
}

struct MockIpmiBootOptionLib {
  MOCK_INTERFACE_DECLARATION (MockIpmiBootOptionLib);

  MOCK_FUNCTION_DECLARATION (
    EFI_STATUS,
    IpmiGetBootDevice,
    (
     OUT IPMI_BOOT_OPTION_SELECTOR  *Selector
    )
    );

  MOCK_FUNCTION_DECLARATION (
    EFI_STATUS,
    IpmiGetCmosClearOption,
    (
     OUT BOOLEAN  *ClearCmos
    )
    );
};

#endif
