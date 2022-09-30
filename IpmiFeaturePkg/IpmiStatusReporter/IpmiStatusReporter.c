/** @file
  Common code for sending EfiReportStatus to IPMI SEL.

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
  UINT32                TypeMask;
  EFI_STATUS            Status;
  UINT16                RecordId;
  SEL_STATUS_CODE_DATA  EntryData;

  TypeMask = PcdGet32 (PcdIpmiStatusCodeTypeMask);
  if ((TypeMask & CodeType) != TypeMask) {
    return EFI_UNSUPPORTED;
  }

  ZeroMem (&Data, sizeof (Data));
  EntryData.Type     = (UINT8)(CodeType | EFI_STATUS_CODE_TYPE_MASK);
  EntryData.Severity = (UINT8)((CodeType | EFI_STATUS_CODE_SEVERITY_MASK) >> 6);
  EntryData.Value    = Value;

  Status = SelAddOemEntry (
             &RecordId,
             PcdGet8 (PcdIpmiStatusCodeOemRecordType),
             (UINT8 *)&EntryData
             );

  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a: Failed to add status code entry\n", __FUNCTION__));
    return Status;
  }

  return Status;
}
