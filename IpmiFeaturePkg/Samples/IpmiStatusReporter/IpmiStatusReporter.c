/** @file
  Sample common code for sending EfiReportStatus to IPMI SEL.

  Copyright (c) Microsoft Corporation
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <IpmiStatusReporter.h>

EFI_STATUS
EFIAPI
IpmiReportStatusCode (
  IN EFI_STATUS_CODE_TYPE   CodeType,
  IN EFI_STATUS_CODE_VALUE  Value,
  IN UINT32                 Instance,
  IN EFI_GUID               *CallerId,
  IN EFI_STATUS_CODE_DATA   *Data
  )

{
  EFI_STATUS            Status;
  UINT16                RecordId;
  SEL_STATUS_CODE_DATA  EntryData;

  //
  // TODO: Platform specific logic for deciding what kind of events to be logged.
  // This structure use is intended as a sample and the platform may use a different
  // you specific structure and Record Type for the status code reported.
  //

  ZeroMem (&EntryData, sizeof (EntryData));
  EntryData.Type     = (UINT8)(CodeType | EFI_STATUS_CODE_TYPE_MASK);
  EntryData.Severity = (UINT8)((CodeType | EFI_STATUS_CODE_SEVERITY_MASK) >> 12);
  EntryData.Value    = Value;

  Status = SelAddOemEntry (
             &RecordId,
             0, // TODO: Platform specific Record Type code.
             (UINT8 *)&EntryData
             );

  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a: Failed to add status code entry\n", __FUNCTION__));
    return Status;
  }

  return Status;
}
