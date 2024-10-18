/** @file MockIpmiSelLib.cpp
  Google Test mocks for IpmiSelLib

  Copyright (c) Microsoft Corporation
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#include <GoogleTest/Library/MockIpmiSelLib.h>

MOCK_INTERFACE_DEFINITION (MockIpmiSelLib);
MOCK_FUNCTION_DEFINITION (MockIpmiSelLib, SelAddSystemEntry, 7, EFIAPI);
MOCK_FUNCTION_DEFINITION (MockIpmiSelLib, SelAddOemEntry, 3, EFIAPI);
MOCK_FUNCTION_DEFINITION (MockIpmiSelLib, SelAddOemEntryEx, 4, EFIAPI);
MOCK_FUNCTION_DEFINITION (MockIpmiSelLib, SelAddOemEntryNoTimestamp, 3, EFIAPI);
MOCK_FUNCTION_DEFINITION (MockIpmiSelLib, SelClear, 1, EFIAPI);
MOCK_FUNCTION_DEFINITION (MockIpmiSelLib, SelGetTime, 1, EFIAPI);
MOCK_FUNCTION_DEFINITION (MockIpmiSelLib, SelSetTime, 1, EFIAPI);
MOCK_FUNCTION_DEFINITION (MockIpmiSelLib, SelGetInfo, 1, EFIAPI);
MOCK_FUNCTION_DEFINITION (MockIpmiSelLib, SelGetEntry, 3, EFIAPI);
