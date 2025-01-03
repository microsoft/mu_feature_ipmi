/** @file MockIpmiBaseLib.h
  Google Test mocks for IpmiBaseLib

  Copyright (c) Microsoft Corporation
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#ifndef MOCK_IPMI_BASE_LIB_H_
#define MOCK_IPMI_BASE_LIB_H_

#include <Library/GoogleTestLib.h>
#include <Library/FunctionMockLib.h>
extern "C" {
  #include <Uefi.h>
  #include <Library/IpmiBaseLib.h>
}

struct MockIpmiBaseLib {
  MOCK_INTERFACE_DECLARATION (MockIpmiBaseLib);

  MOCK_FUNCTION_DECLARATION (
    EFI_STATUS,
    IpmiSubmitCommand,
    (
     IN UINT8       NetFunction,
     IN UINT8       Command,
     IN UINT8       *CommandData,
     IN UINT32      CommandDataSize,
     OUT UINT8      *ResponseData,
     IN OUT UINT32  *ResponseDataSize
    )
    );

  MOCK_FUNCTION_DECLARATION (
    EFI_STATUS,
    GetBmcStatus,
    (
     OUT BMC_STATUS      *BmcStatus,
     OUT SM_COM_ADDRESS  *ComAddress
    )
    );
};

#endif
