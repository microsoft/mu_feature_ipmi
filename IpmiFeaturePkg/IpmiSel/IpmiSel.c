/** @file
  The DXE implementation of the IPMI SEL Protocol.

  Copyright (c) Microsoft Corporation.
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiBootServicesTableLib.h>

#include <Protocol/IpmiSelProtocol.h>
#include <Library/IpmiSelLib.h>

//
// Protocol function prototypes.
//

EFI_STATUS
EFIAPI
IpmiSelGetRecordEntry (
  IN  UINT16      RecordId,
  OUT SEL_RECORD  *Record,
  OUT UINT16      *NextRecordId OPTIONAL
  );

EFI_STATUS
EFIAPI
IpmiSelAddRecordEntry (
  OUT UINT16  *RecordId OPTIONAL,
  IN  UINT8   RecordType,
  IN  UINT8   ManufacturerId[3],
  IN  UINT8   Data[6]
  );

//
// Protocol definition.
//

IPMI_SEL_PROTOCOL  mIpmiSelProtocol = {
  IpmiSelGetRecordEntry,
  IpmiSelAddRecordEntry
};

/**
  Retrieves a record from the system event log.

  @param[in]  RecordId      The record ID to retrieve. 0x0000 will always retrieve
                            the first entry and 0xFFFF will always retrieve the
                            last entry.
  @param[out] Record        Receives the record entry if found.
  @param[out] NextRecordId  If provided, receives the next record ID. Will be set
                            to 0xFFFF if the retrieved record is the last entry.

  @retval   EFI_SUCCESS             The SEL entry was retrieved.
  @retval   EFI_INVALID_PARAMETER   Record pointer is NULL.
  @retval   Other                   An error was returned by IPMI.
**/
EFI_STATUS
EFIAPI
IpmiSelGetRecordEntry (
  IN  UINT16      RecordId,
  OUT SEL_RECORD  *Record,
  OUT UINT16      *NextRecordId OPTIONAL
  )
{
  return SelGetEntry (RecordId, Record, NextRecordId);
}

/**
  Adds an Event Record to the SEL. This only allows creation of "OEM Sel Record"
  types. System events cannot be created through this API.

  @param[out]  RecordId         If provided, will be set to the Record ID of the
                                created record.
  @param[in]   RecordType       SEL Record Type number. Must be 0xC0-0xDF.
  @param[in]   ManufacturerId   The manufacturer ID for the event. Must be 3 bytes.
  @param[in]   Data             The record data. Must be 6 bytes.

  @retval   EFI_SUCCESS             The record was successfully added to the SEL.
  @retval   EFI_INVALID_PARAMETER   The RecordType is invalid, ManufacturerId is
                                    NULL, or Data is NULL.
  @retval   Other                   An error was returned by IPMI.

**/
EFI_STATUS
EFIAPI
IpmiSelAddRecordEntry (
  OUT UINT16  *RecordId OPTIONAL,
  IN  UINT8   RecordType,
  IN  UINT8   ManufacturerId[3],
  IN  UINT8   Data[6]
  )
{
  return SelAddOemEntryEx (RecordId, RecordType, ManufacturerId, Data);
}

/**
  Entry point to the IPMI SEL Protocol module.

  @param[in]    ImageHandle   The handle for this module image.
  @param[in]    SystemTable   Pointer to the UEFI system table.

  @retval   EFI_SUCCESS   IPMI SEL Protocol successfully installed.
  @retval   Other         An error was returned by a subroutine.
**/
EFI_STATUS
EFIAPI
IpmiSelEntryPoint (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  return gBS->InstallMultipleProtocolInterfaces (
                &ImageHandle,
                &gIpmiSelProtocolGuid,
                &mIpmiSelProtocol,
                NULL
                );
}
