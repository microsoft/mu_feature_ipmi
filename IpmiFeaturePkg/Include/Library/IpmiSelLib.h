/** @file
  Definitions for the SEL Library.

  Copyright (c) Microsoft Corporation
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef _IPMI_SEL_LIB_H
#define _IPMI_SEL_LIB_H

#pragma pack(1)

//
// Structure used for get info results.
//

typedef struct _SEL_INFO {
  UINT8     Version;
  UINT16    NumberOfEntries;
  UINT16    FreeSpace;
  UINT32    LastAddTimeStamp;
  UINT32    LastEraseTimeStamp;
  union {
    struct {
      UINT8    GetSelAllocationInfo : 1;
      UINT8    ReserveSel           : 1;
      UINT8    PartialAddSelEntry   : 1;
      UINT8    DeleteSel            : 1;
      UINT8    Reserved             : 3;
      UINT8    Overflow             : 1;
    } Bits;

    UINT8    AsUint8;
  } OperationSupported;
} SEL_INFO;

//
// Generic structure for SEL events.
//

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
  Adds a system event to the SEL.

  @param[in,out]  RecordId      If provided, receives the record ID of the entry.
  @param[in]      SensorType    The Sensor type for the event.
  @param[in]      SensorNumber  The sensor number for the event.
  @param[in]      EventDirType  The event Dir and Type values
  @param[in]      Data0         OEM defined data part 0.
  @param[in]      Data1         OEM defined data part 1.
  @param[in]      Data2         OEM defined data part 2.

  @retval   EFI_SUCCESS     Event was successfully added to the SEL.
  @retval   Other           And error was returned by IpmiAddSelEntry.
**/
EFI_STATUS
EFIAPI
SelAddSystemEntry (
  IN OUT UINT16  *RecordId OPTIONAL,
  IN UINT8       SensorType,
  IN UINT8       SensorNumber,
  IN UINT8       EventDirType,
  IN UINT8       Data0,
  IN UINT8       Data1,
  IN UINT8       Data2
  );

/**
  Adds an OEM timestamped event to the SEL.

  @param[in,out]  RecordId      If provided, receives the record ID of the entry.
  @param[in]      RecordType    The record type code. But be between 0xC0-0xDF.
  @param[in]      Data          Array of OEM defined event data.

  @retval   EFI_SUCCESS             Event was successfully added to the SEL.
  @retval   EFI_INVALID_PARAMETER   Invalid RecordType was given.
  @retval   Other                   And error was returned by IpmiAddSelEntry.
**/
EFI_STATUS
EFIAPI
SelAddOemEntry (
  IN OUT UINT16  *RecordId OPTIONAL,
  IN UINT8       RecordType,
  IN UINT8       Data[6]
  );

/**
  Clears the SEL.

  @param[in]  AwaitClear  Indicates the routine should wait for the SEL clear to
                          complete before returning.

  @retval   EFI_SUCCESS   The SEL clear command was successfully sent.
  @retval   Other         The IPMI base library returned an error.
**/
EFI_STATUS
EFIAPI
SelClear (
  BOOLEAN  AwaitClear
  );

/**
  Gets the SEL time.

  @param[out]   Time    Receives the SEL time.

  @retval   EFI_SUCCESS   The SEL time was retrieved.
  @retval   Other         The IPMI base library returned an error.
**/
EFI_STATUS
EFIAPI
SelGetTime (
  OUT UINT32  *Time
  );

/**
  Sets the SEL time.

  @param[in]   Time     The value to set the SEL time to.

  @retval   EFI_SUCCESS   The SEL time was set.
  @retval   Other         The IPMI base library returned an error.
**/
EFI_STATUS
EFIAPI
SelSetTime (
  IN UINT32  Time
  );

/**
  Gets information about the SEL.

  @param[out]   SelInfo     Receives information about the SEL.

  @retval   EFI_SUCCESS             The SEL information was retrieved.
  @retval   EFI_INVALID_PARAMETER   SelInfo pointer is NULL.
  @retval   Other                   The IPMI base library returned an error.
**/
EFI_STATUS
EFIAPI
SelGetInfo (
  OUT SEL_INFO  *SelInfo
  );

/**
  Gets information about the SEL.

  @param[in]    RecordId        The record ID of the entry to retrieve.
  @param[out]   Record          Receives the record if found.
  @param[out]   NextRecordId    If provided, receives the next record ID.

  @retval   EFI_SUCCESS             The SEL entry was retrieved.
  @retval   EFI_INVALID_PARAMETER   Record pointer is NULL.
  @retval   Other                   The IPMI base library returned an error.
**/
EFI_STATUS
EFIAPI
SelGetEntry (
  IN UINT16       RecordId,
  OUT SEL_RECORD  *Record,
  OUT UINT16      *NextRecordId OPTIONAL
  );

#endif
