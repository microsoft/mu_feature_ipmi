/** @file
  Common code for sending EfiReportStatus to IPMI SEL.

  Copyright (c) Microsoft Corporation
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef IPMI_STATUS_REPORTER_H_
#define IPMI_STATUS_REPORTER_H_

#include <Uefi.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/ReportStatusCodeLib.h>
#include <Library/PcdLib.h>
#include <IndustryStandard/Ipmi.h>
#include <Library/IpmiSelLib.h>

#pragma pack(1)

//
// Sample format for a EFI status code. The platform may choose a different format
// for its specific constraints. The platform may choose to not use a generic structure
// and use contextually specific events for particular failure codes.
//

typedef struct _SEL_STATUS_CODE_DATA {
  UINT8                    Type;
  UINT8                    Severity;
  EFI_STATUS_CODE_VALUE    Value;
} SEL_STATUS_CODE_DATA;

//
// Ensure this structure fits into the 6 bytes allowed for SEL OEM entry.
//

STATIC_ASSERT (sizeof (SEL_STATUS_CODE_DATA) == 6, "Incorrect size of SEL data!");

#pragma pack()

/**
  Sample implementation for logging a Status Code event to the System Event Log (SEL).

  @param[in]  CodeType   The status code type.
  @param[in]  Value      The status code value.
  @param[in]  Instance   The status code instance number.
  @param[in]  CallerId   Pointer to a GUID that identifies the caller of this
                         function. This is an optional parameter that may be NULL.
  @param[in]  Data       Pointer to the extended data buffer. This is an optional
                         parameter that may be NULL.

  @retval     EFI Status Code.
**/
EFI_STATUS
EFIAPI
IpmiReportStatusCode (
  IN EFI_STATUS_CODE_TYPE   CodeType,
  IN EFI_STATUS_CODE_VALUE  Value,
  IN UINT32                 Instance,
  IN EFI_GUID               *CallerId,
  IN EFI_STATUS_CODE_DATA   *Data
  );

#endif
