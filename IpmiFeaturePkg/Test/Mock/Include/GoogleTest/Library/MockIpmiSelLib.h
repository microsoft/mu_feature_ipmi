/** @file MockIpmiSelLib.h
  Google Test mocks for IpmiSelLib

  Copyright (c) Microsoft Corporation
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#ifndef MOCK_IPMI_SEL_LIB_H_
#define MOCK_IPMI_SEL_LIB_H_

#include <Library/GoogleTestLib.h>
#include <Library/FunctionMockLib.h>
extern "C" {
  #include <Uefi.h>
  #include <Library/IpmiSelLib.h>
}

struct MockIpmiSelLib {
  MOCK_INTERFACE_DECLARATION (MockIpmiSelLib);

  MOCK_FUNCTION_DECLARATION (
    EFI_STATUS,
    SelAddSystemEntry,
    (
     IN OUT UINT16  *RecordId OPTIONAL,
     IN UINT8       SensorType,
     IN UINT8       SensorNumber,
     IN UINT8       EventDirType,
     IN UINT8       Data0,
     IN UINT8       Data1,
     IN UINT8       Data2
    )
    );

  MOCK_FUNCTION_DECLARATION (
    EFI_STATUS,
    SelAddOemEntry,
    (
     IN OUT UINT16  *RecordId OPTIONAL,
     IN UINT8       RecordType,
     IN UINT8       Data[6]
    )
    );

  MOCK_FUNCTION_DECLARATION (
    EFI_STATUS,
    SelAddOemEntryEx,
    (
     IN OUT UINT16  *RecordId OPTIONAL,
     IN UINT8       RecordType,
     IN UINT8       ManufacturerId[3],
     IN UINT8       Data[6]
    )
    );

  MOCK_FUNCTION_DECLARATION (
    EFI_STATUS,
    SelAddOemEntryNoTimestamp,
    (
     IN OUT UINT16  *RecordId OPTIONAL,
     IN UINT8       RecordType,
     IN UINT8       Data[13]
    )
    );

  MOCK_FUNCTION_DECLARATION (
    EFI_STATUS,
    SelClear,
    (
     BOOLEAN  AwaitClear
    )
    );

  MOCK_FUNCTION_DECLARATION (
    EFI_STATUS,
    SelGetTime,
    (
     OUT UINT32  *Time
    )
    );

  MOCK_FUNCTION_DECLARATION (
    EFI_STATUS,
    SelSetTime,
    (
     IN UINT32  Time
    )
    );

  MOCK_FUNCTION_DECLARATION (
    EFI_STATUS,
    SelGetInfo,
    (
     OUT SEL_INFO  *SelInfo
    )
    );

  MOCK_FUNCTION_DECLARATION (
    EFI_STATUS,
    SelGetEntry,
    (
     IN UINT16       RecordId,
     OUT SEL_RECORD  *Record,
     OUT UINT16      *NextRecordId OPTIONAL
    )
    );
};

#endif
