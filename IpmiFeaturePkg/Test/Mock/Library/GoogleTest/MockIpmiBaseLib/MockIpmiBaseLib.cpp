/** @file MockIpmiBaseLib.cpp
  Google Test mocks for IpmiBaseLib

  Copyright (c) Microsoft Corporation
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#include <GoogleTest/Library/MockIpmiBaseLib.h>

MOCK_INTERFACE_DEFINITION (MockIpmiBaseLib);
MOCK_FUNCTION_DEFINITION (MockIpmiBaseLib, IpmiSubmitCommand, 6, EFIAPI);
MOCK_FUNCTION_DEFINITION (MockIpmiBaseLib, GetBmcStatus, 2, EFIAPI);
