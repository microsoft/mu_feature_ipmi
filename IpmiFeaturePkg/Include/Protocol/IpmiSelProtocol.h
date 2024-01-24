/** @file

  Definitions for the IPMI SEL Protocol. This protocol give high level access to
  the IPMI System Event Log records. The intention of this protocol is to be used
  by less-priviledged users of the SEL such as a boot loader. Because of this,
  this protocol only allows the user access to create OEM records as system records
  should not be written by non-system binaries. Priviledged users should use
  the IpmiSelLib instead.

  Copyright (c) Microsoft Corporation.
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef SEL_PROTOCOL_H_
#define SEL_PROTOCOL_H_

#include <IndustryStandard/Ipmi.h>

// {5ecad598-c13a-48fb-be85-7198b6a4be38}
#define IPMI_SEL_PROTOCOL_GUID \
  { \
    0x5ecad598, 0xc13a, 0x48fb, { 0xbe, 0x85, 0x71, 0x98, 0xb6, 0xa4, 0xbe, 0x38 } \
  }

typedef struct _IPMI_SEL_PROTOCOL IPMI_SEL_PROTOCOL;

//
// Generic structure for SEL records.
//

#pragma pack(1)

typedef struct _SEL_RECORD {
  UINT16    RecordId;
  UINT8     RecordType;

  union {
    struct {
      UINT32    TimeStamp;
      UINT16    GeneratorId;
      UINT8     EvMRevision;
      UINT8     SensorType;
      UINT8     SensorNumber;
      UINT8     EventDirType;
      UINT8     Data[3];
    } System;

    struct {
      UINT32    TimeStamp;
      UINT8     ManufacturerId[3];
      UINT8     Data[6];
    } Oem;

    struct {
      UINT8    Data[13];
    } OemNonTimestamped;
  } Record;
} SEL_RECORD;

#pragma pack()

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
typedef
EFI_STATUS
(EFIAPI  *SEL_GET_RECORD_ENTRY)(
  IN  UINT16      RecordId,
  OUT SEL_RECORD  *Record,
  OUT UINT16      *NextRecordId OPTIONAL
  );

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
typedef
EFI_STATUS
(EFIAPI *SEL_ADD_RECORD_ENTRY)(
  OUT UINT16  *RecordId OPTIONAL,
  IN  UINT8   RecordType,
  IN  UINT8   ManufacturerId[3],
  IN  UINT8   Data[6]
  );

//
// IPMI TRANSPORT PROTOCOL
//
struct _IPMI_SEL_PROTOCOL {
  SEL_GET_RECORD_ENTRY    GetRecordEntry;
  SEL_ADD_RECORD_ENTRY    AddRecordEntry;
};

#endif
