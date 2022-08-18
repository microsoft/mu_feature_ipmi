/** @file
  Implements the SEL library. This library simplifies using the IPMI sel
  functionality.

  Copyright (c) Microsoft Corporation
  PDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Uefi.h>
#include <Library/DebugLib.h>

#include <IndustryStandard/Ipmi.h>
#include <Library/IpmiCommandLib.h>
#include <Library/IpmiSelLib.h>

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
    case IPMI_COMP_CODE_INVALID_FOR_GIVEN_LUN:
      return EFI_INVALID_PARAMETER;
    case IPMI_COMP_CODE_TIMEOUT:
      return EFI_TIMEOUT;
    case IPMI_COMP_CODE_OUT_OF_SPACE:
      return EFI_OUT_OF_RESOURCES;
    case IPMI_COMP_CODE_INSUFFICIENT_PRIVILEGE:
      return EFI_SECURITY_VIOLATION;
    default:
      return EFI_PROTOCOL_ERROR;
  }
}

EFI_STATUS
EFIAPI
IpmiAddSelEntry (
  IN VOID                 *Entry,
  IN UINT32               *RequestSize,
  IN OUT OPTIONAL UINT16  *RecordId
  )
{
  EFI_STATUS                   Status;
  UINT32                       DataSize;
  IPMI_ADD_SEL_ENTRY_RESPONSE  Response;

  DataSize                = sizeof (Response);
  Response.CompletionCode = IPMI_COMP_CODE_UNSPECIFIED;

  Status = IpmiSubmitCommand (
             IPMI_NETFN_STORAGE,
             IPMI_STORAGE_ADD_SEL_ENTRY,
             Entry,
             RequestSize,
             (VOID *)&Response,
             &DataSize
             );

  if (EFI_ERROR (Status)) {
    return Status;
  }

  return IpmiCompCodeToEfiStatus (Response.CompletionCode);
}

EFI_STATUS
EFIAPI
SelAddEventEntry (

  )

EFI_STATUS
EFIAPI
SelAddOemEntry (

  )

EFI_STATUS
EFIAPI
SelAddTimestampedOemEntry (

  )
