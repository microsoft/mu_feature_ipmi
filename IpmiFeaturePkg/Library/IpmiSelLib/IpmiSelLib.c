/** @file
  Implements the SEL library. This library simplifies using the IPMI sel
  functionality.

  Copyright (c) Microsoft Corporation
  PDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Uefi.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/TimerLib.h>

#include <IndustryStandard/Ipmi.h>
#include <Library/IpmiBaseLib.h>
#include <Library/IpmiSelLib.h>

//
// All SEL entries are the same size and exactly 16 bytes.
//

#define SEL_ENTRY_SIZE  (16)
STATIC_ASSERT (
  sizeof (IPMI_SEL_EVENT_RECORD_DATA) == SEL_ENTRY_SIZE,
  "Unexpected SEL entry size!"
  );

STATIC_ASSERT (
  sizeof (IPMI_TIMESTAMPED_OEM_SEL_RECORD_DATA) == SEL_ENTRY_SIZE,
  "Unexpected SEL entry size!"
  );

STATIC_ASSERT (
  sizeof (IPMI_NON_TIMESTAMPED_OEM_SEL_RECORD_DATA) == SEL_ENTRY_SIZE,
  "Unexpected SEL entry size!"
  );

/**
  Converts a IPMI completion code to a EFI status code.

  @param[in]  CompCode    The IPMI completion code to convert.

  @retval     EFI_STATUS best representing the completion code.
**/
EFI_STATUS
EFIAPI
IpmiCompCodeToEfiStatus (
  IN UINT8  CompCode
  )
{
  switch (CompCode) {
    case IPMI_COMP_CODE_NORMAL:
      return EFI_SUCCESS;

    case IPMI_COMP_CODE_INVALID_COMMAND:
      return EFI_UNSUPPORTED;

    case IPMI_COMP_CODE_INVALID_FOR_GIVEN_LUN:
    case IPMI_COMP_CODE_INVALID_DATA_FIELD:
    case IPMI_COMP_CODE_COMMAND_ILLEGAL:
      return EFI_INVALID_PARAMETER;

    case IPMI_COMP_CODE_INVALID_REQUEST_DATA_LENGTH:
      return EFI_BAD_BUFFER_SIZE;

    case IPMI_COMP_CODE_NOT_PRESENT:
    case IPMI_COMP_CODE_OUT_OF_RANGE:
      return EFI_NOT_FOUND;

    case IPMI_COMP_CODE_TIMEOUT:
      return EFI_TIMEOUT;

    case IPMI_COMP_CODE_OUT_OF_SPACE:
      return EFI_OUT_OF_RESOURCES;

    case IPMI_COMP_CODE_BMC_INIT_IN_PROGRESS:
      return EFI_NOT_READY;

    case IPMI_COMP_CODE_INSUFFICIENT_PRIVILEGE:
      return EFI_SECURITY_VIOLATION;

    default:
      return EFI_PROTOCOL_ERROR;
  }
}

/**
  Adds a generic entry to the SEL.

  @param[in]      Entry       The entry to be added to the SEL.
  @param[in]      EntrySize   The size of the entry in bytes.
  @param[in,out]  RecordId    If provided, receives the record ID of the entry.

  @retval   EFI_SUCCESS         The entry was successfully added.
  @retval   EFI_PROTOCOL_ERROR  Unexpected result size.
  @retval   Other               The IPMI base library returned an error.
**/
EFI_STATUS
EFIAPI
IpmiAddSelEntry (
  IN VOID        *Entry,
  IN UINT32      EntrySize,
  IN OUT UINT16  *RecordId OPTIONAL
  )
{
  EFI_STATUS                   Status;
  UINT32                       DataSize;
  IPMI_ADD_SEL_ENTRY_RESPONSE  Response;

  ASSERT (EntrySize == SEL_ENTRY_SIZE);

  DataSize                = sizeof (Response);
  Response.CompletionCode = IPMI_COMP_CODE_UNSPECIFIED;

  Status = IpmiSubmitCommand (
             IPMI_NETFN_STORAGE,
             IPMI_STORAGE_ADD_SEL_ENTRY,
             Entry,
             EntrySize,
             (VOID *)&Response,
             &DataSize
             );

  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a: Add SEL entry failed. %r\n", __FUNCTION__, Status));
    return Status;
  }

  if (DataSize < sizeof (Response.CompletionCode)) {
    DEBUG ((DEBUG_ERROR, "%a: Response too small for completion code!\n", __FUNCTION__));
    return EFI_PROTOCOL_ERROR;
  }

  Status = IpmiCompCodeToEfiStatus (Response.CompletionCode);
  if (EFI_ERROR (Status)) {
    DEBUG ((
      DEBUG_ERROR,
      "%a: Add SEL event returned failing completion code. (0x%x) %r\n",
      __FUNCTION__,
      Response.CompletionCode,
      Status
      ));

    return Status;
  }

  if (DataSize < sizeof (Response)) {
    DEBUG ((DEBUG_ERROR, "%a: Response too small for add SEL entry response.\n", __FUNCTION__));
    return EFI_PROTOCOL_ERROR;
  }

  if (RecordId != NULL) {
    *RecordId = Response.RecordId;
  }

  return Status;
}

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
  )
{
  IPMI_SEL_EVENT_RECORD_DATA  Entry;

  Entry.RecordId     = 0;
  Entry.RecordType   = IPMI_SEL_SYSTEM_RECORD;
  Entry.TimeStamp    = 0;
  Entry.GeneratorId  = IPMI_SOFTWARE_ID;
  Entry.EvMRevision  = IPMI_EVM_REVISION;
  Entry.SensorType   = SensorType;
  Entry.SensorNumber = SensorNumber;
  Entry.EventDirType = EventDirType;
  Entry.OEMEvData1   = Data0;
  Entry.OEMEvData2   = Data1;
  Entry.OEMEvData3   = Data2;

  return IpmiAddSelEntry (&Entry, sizeof (Entry), RecordId);
}

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
  )

{
  IPMI_TIMESTAMPED_OEM_SEL_RECORD_DATA  Entry;

  if ((RecordType < IPMI_SEL_OEM_TIME_STAMP_RECORD_START) ||
      (RecordType > IPMI_SEL_OEM_TIME_STAMP_RECORD_END))
  {
    DEBUG ((
      DEBUG_ERROR,
      "%a: Invalid Record ID for OEM entry! 0x%x\n",
      __FUNCTION__,
      RecordType
      ));

    return EFI_INVALID_PARAMETER;
  }

  Entry.RecordId          = 0;
  Entry.RecordType        = RecordType;
  Entry.TimeStamp         = 0;
  Entry.ManufacturerId[0] = PcdGet8 (PcdIpmiSelOemManufacturerId0);
  Entry.ManufacturerId[1] = PcdGet8 (PcdIpmiSelOemManufacturerId1);
  Entry.ManufacturerId[2] = PcdGet8 (PcdIpmiSelOemManufacturerId2);
  CopyMem (&Entry.OEMDefined[0], &Data[0], sizeof (Entry.OEMDefined));

  return IpmiAddSelEntry (&Entry, sizeof (Entry), RecordId);
}

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
  )
{
  IPMI_CLEAR_SEL_REQUEST   Request;
  IPMI_CLEAR_SEL_RESPONSE  Response;
  EFI_STATUS               Status;
  UINT32                   DataSize;

  ZeroMem (&Request, sizeof (Request));
  Request.AscC  = 'C';
  Request.AscL  = 'L';
  Request.AscR  = 'R';
  Request.Erase = IPMI_CLEAR_SEL_REQUEST_INITIALIZE_ERASE;
  DEBUG ((DEBUG_INFO, "Clearing SEL.\n"));

  while (TRUE) {
    ZeroMem (&Response, sizeof (Response));
    DataSize = sizeof (Response);

    Status = IpmiSubmitCommand (
               IPMI_NETFN_STORAGE,
               IPMI_STORAGE_CLEAR_SEL,
               (VOID *)&Request,
               sizeof (Request),
               (VOID *)&Response,
               &DataSize
               );

    if (EFI_ERROR (Status)) {
      DEBUG ((
        DEBUG_ERROR,
        "%a: Failed to send SEL clear command. %r\n",
        __FUNCTION__,
        Status
        ));

      return Status;
    }

    Status = IpmiCompCodeToEfiStatus (Response.CompletionCode);
    if (EFI_ERROR (Status)) {
      DEBUG ((
        DEBUG_ERROR,
        "%a: SEL clear returned failing completion code. (0x%x) %r\n",
        __FUNCTION__,
        Response.CompletionCode,
        Status
        ));

      return Status;
    }

    if (!AwaitClear ||
        (Response.ErasureProgress == IPMI_CLEAR_SEL_RESPONSE_ERASURE_COMPLETED))
    {
      break;
    }

    //
    // Delay 10 milliseconds and try again.
    //

    Request.Erase = IPMI_CLEAR_SEL_REQUEST_GET_ERASE_STATUS;
    DEBUG ((DEBUG_INFO, "Waiting for SEL clear.\n"));
    MicroSecondDelay (10 * 1000);
  }

  return Status;
}

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
  )
{
  IPMI_GET_SEL_TIME_RESPONSE  Response;
  EFI_STATUS                  Status;
  UINT32                      DataSize;

  if (Time == NULL) {
    ASSERT (FALSE);
    return EFI_INVALID_PARAMETER;
  }

  DataSize = sizeof (Response);
  Status   = IpmiSubmitCommand (
               IPMI_NETFN_STORAGE,
               IPMI_STORAGE_GET_SEL_TIME,
               NULL,
               0,
               (VOID *)&Response,
               &DataSize
               );

  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a: Failed to send get SEL time command. %r\n", __FUNCTION__, Status));
    return Status;
  }

  Status = IpmiCompCodeToEfiStatus (Response.CompletionCode);
  if (EFI_ERROR (Status)) {
    DEBUG ((
      DEBUG_ERROR,
      "%a: Get SEL time returned failing completion code. (0x%x) %r\n",
      __FUNCTION__,
      Response.CompletionCode,
      Status
      ));

    return Status;
  }

  *Time = Response.Timestamp;
  return Status;
}

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
  )
{
  IPMI_SET_SEL_TIME_REQUEST  Request;
  UINT8                      CompletionCode;
  EFI_STATUS                 Status;
  UINT32                     DataSize;

  Request.Timestamp = Time;
  DataSize          = sizeof (CompletionCode);

  Status = IpmiSubmitCommand (
             IPMI_NETFN_STORAGE,
             IPMI_STORAGE_SET_SEL_TIME,
             (VOID *)&Request,
             sizeof (Request),
             (VOID *)&CompletionCode,
             &DataSize
             );

  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a: Failed to send set SEL time command. %r\n", __FUNCTION__, Status));
    return Status;
  }

  Status = IpmiCompCodeToEfiStatus (CompletionCode);
  if (EFI_ERROR (Status)) {
    DEBUG ((
      DEBUG_ERROR,
      "%a: Get SEL time returned failing completion code. (0x%x) %r\n",
      __FUNCTION__,
      CompletionCode,
      Status
      ));

    return Status;
  }

  return Status;
}

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
  )
{
  IPMI_GET_SEL_INFO_RESPONSE  IpmiSelInfo;
  EFI_STATUS                  Status;
  UINT32                      DataSize;

  if (SelInfo == NULL) {
    DEBUG ((DEBUG_ERROR, "%a: SelInfo parameter is NULL!\n", __FUNCTION__));
    ASSERT (FALSE);
    return EFI_INVALID_PARAMETER;
  }

  DataSize = sizeof (IpmiSelInfo);
  Status   = IpmiSubmitCommand (
               IPMI_NETFN_STORAGE,
               IPMI_STORAGE_GET_SEL_INFO,
               NULL,
               0,
               (VOID *)&IpmiSelInfo,
               &DataSize
               );

  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a: Failed to send get SEL info command. %r\n", __FUNCTION__, Status));
    return Status;
  }

  Status = IpmiCompCodeToEfiStatus (IpmiSelInfo.CompletionCode);
  if (EFI_ERROR (Status)) {
    DEBUG ((
      DEBUG_ERROR,
      "%a: Get SEL info returned failing completion code. (0x%x) %r\n",
      __FUNCTION__,
      IpmiSelInfo.CompletionCode,
      Status
      ));

    return Status;
  }

  SelInfo->Version                    = IpmiSelInfo.Version;
  SelInfo->NumberOfEntries            = IpmiSelInfo.NoOfEntries;
  SelInfo->FreeSpace                  = IpmiSelInfo.FreeSpace;
  SelInfo->LastAddTimeStamp           = IpmiSelInfo.RecentAddTimeStamp;
  SelInfo->LastEraseTimeStamp         = IpmiSelInfo.RecentEraseTimeStamp;
  SelInfo->OperationSupported.AsUint8 = IpmiSelInfo.OperationSupport;

  return Status;
}
