/** @file MockIpmiBootOptionLib.cpp
  Google Test mocks for IpmiBootOptionLib

  Copyright (c) Microsoft Corporation
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#include <GoogleTest/Library/MockIpmiBootOptionLib.h>

MOCK_INTERFACE_DEFINITION (MockIpmiBootOptionLib);
MOCK_FUNCTION_DEFINITION (MockIpmiBootOptionLib, IpmiGetBootDevice, 1, EFIAPI);
MOCK_FUNCTION_DEFINITION (MockIpmiBootOptionLib, IpmiGetCmosClearOption, 1, EFIAPI);
