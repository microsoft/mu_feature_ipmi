/** @file
  Implements the SEL library. This library simplifies using the IPMI sel
  functionality.

  Copyright (c) Microsoft Corporation
  PDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Uefi.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <IndustryStandard/Ipmi.h>
#include <Library/IpmiBaseLib.h>
#include <Library/IpmiBootOptionLib.h>

//
// Direct definitions of the expected structures for accurate structure sizes.
//

#pragma pack(1)

typedef struct _IPMI_GET_BOOT_OPTIONS_RESPONSE_HDR {
  UINT8                                      CompletionCode;
  IPMI_GET_BOOT_OPTIONS_PARAMETER_VERSION    ParameterVersion;
  IPMI_GET_BOOT_OPTIONS_PARAMETER_VALID      ParameterValid;
} IPMI_GET_BOOT_OPTIONS_RESPONSE_HDR;

typedef struct _IPMI_GET_BOOT_OPTIONS_RESPONSE_0 {
  IPMI_GET_BOOT_OPTIONS_RESPONSE_HDR        Header;
  IPMI_BOOT_OPTIONS_RESPONSE_PARAMETER_0    Data;
} IPMI_GET_BOOT_OPTIONS_RESPONSE_0;

typedef struct _IPMI_GET_BOOT_OPTIONS_RESPONSE_5 {
  IPMI_GET_BOOT_OPTIONS_RESPONSE_HDR        Header;
  IPMI_BOOT_OPTIONS_RESPONSE_PARAMETER_5    Data;
} IPMI_GET_BOOT_OPTIONS_RESPONSE_5;

typedef struct _IPMI_SET_BOOT_OPTIONS_REQUEST_5 {
  IPMI_SET_BOOT_OPTIONS_PARAMETER_VALID     ParameterValid;
  IPMI_BOOT_OPTIONS_RESPONSE_PARAMETER_5    Data;
} IPMI_SET_BOOT_OPTIONS_REQUEST_5;

typedef struct _IPMI_SET_BOOT_OPTIONS_REQUEST_4 {
  IPMI_SET_BOOT_OPTIONS_PARAMETER_VALID     ParameterValid;
  IPMI_BOOT_OPTIONS_RESPONSE_PARAMETER_4    Data;
} IPMI_SET_BOOT_OPTIONS_REQUEST_4;

#pragma pack()

/**
  Retrieves the specified boot options parameter data.

  @param[in]  ParameterSelector   The boot option parameter number.
  @param[in]  ResponseSize        The expected data response size.
  @param[out] Response            The response data from the BMC.

  @retval   EFI_SUCCESS           Boot option paramater was successfully retrieved.
  @retval   EFI_INVALID_PARAMETER Response pointer or size is invalid.
  @retval   EFI_BAD_BUFFER_SIZE   The buffer returned did not match expected size.
  @retval   EFI_PROTOCOL_ERROR    The BMC returned a failing completion code.
  @retval   Other                 An error was returned by a subroutine.
**/
EFI_STATUS
EFIAPI
IpmiGetBootOption (
  IN UINT8                                ParameterSelector,
  IN UINT32                               ResponseSize,
  OUT IPMI_GET_BOOT_OPTIONS_RESPONSE_HDR  *Response
  )

{
  IPMI_GET_BOOT_OPTIONS_REQUEST  Request;
  UINT32                         DataSize;
  EFI_STATUS                     Status;

  if ((Response == NULL) ||
      (ResponseSize < sizeof (IPMI_GET_BOOT_OPTIONS_RESPONSE_HDR)))
  {
    ASSERT (FALSE);
    return EFI_INVALID_PARAMETER;
  }

  ZeroMem (&Request, sizeof (Request));
  ZeroMem (Response, ResponseSize);
  Request.ParameterSelector.Bits.ParameterSelector = ParameterSelector;

  DataSize = ResponseSize;
  Status   = IpmiSubmitCommand (
               IPMI_NETFN_CHASSIS,
               IPMI_CHASSIS_GET_SYSTEM_BOOT_OPTIONS,
               (VOID *)&Request,
               sizeof (Request),
               (VOID *)Response,
               &DataSize
               );

  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a: Failed to get IPMI boot options (%d). %r\n", __FUNCTION__, ParameterSelector, Status));
    return Status;
  }

  if (DataSize < sizeof (Response->CompletionCode)) {
    DEBUG ((DEBUG_ERROR, "%a: Response too small for completion code! 0x%x\n", __FUNCTION__, DataSize));
    return EFI_BAD_BUFFER_SIZE;
  }

  if (Response->CompletionCode != IPMI_COMP_CODE_NORMAL) {
    DEBUG ((
      DEBUG_ERROR,
      "%a: Get system boot options returned failing completion code. 0x%x\n",
      __FUNCTION__,
      Response->CompletionCode
      ));

    return EFI_PROTOCOL_ERROR;
  }

  if (DataSize < ResponseSize) {
    DEBUG ((DEBUG_ERROR, "%a: Unexpected response size! 0x%x\n", __FUNCTION__, DataSize));
    return EFI_BAD_BUFFER_SIZE;
  }

  return EFI_SUCCESS;
}

/**
  Checks if the set in progress is active for IPMI boot options.

  @param[out]   SetInProgress   TRUE if set in progress is active, FALSE otherwise.

  @retval   EFI_SUCCESS     Successfully checked the set in progress bit.
  @retval   Other           Subroutine returned an error.
**/
EFI_STATUS
EFIAPI
IpmiCheckSetInProgress (
  OUT BOOLEAN  *SetInProgress
  )
{
  IPMI_GET_BOOT_OPTIONS_RESPONSE_0  Response;
  EFI_STATUS                        Status;

  ZeroMem (&Response, sizeof (Response));
  Status = IpmiGetBootOption (
             IPMI_BOOT_OPTIONS_PARAMETER_SELECTOR_SET_IN_PROGRESS,
             sizeof (Response),
             &Response.Header
             );

  if (EFI_ERROR (Status)) {
    return Status;
  }

  if (Response.Header.ParameterValid.Bits.ParameterValid != 0) {
    DEBUG ((DEBUG_INFO, "%a: Boot options parameter 0 invalid.\n", __FUNCTION__));
    *SetInProgress = FALSE;
  } else {
    *SetInProgress = Response.Data.Bits.SetInProgress & BIT0;
  }

  return EFI_SUCCESS;
}

/**
  Acknowledges the IPMI boot options.

**/
VOID
IpmiAcknowledgeBootOption (
  VOID
  )
{
  IPMI_SET_BOOT_OPTIONS_REQUEST_4  Request;
  IPMI_SET_BOOT_OPTIONS_RESPONSE   Response;
  EFI_STATUS                       Status;
  UINT32                           DataSize;

  ZeroMem (&Request, sizeof (Request));
  ZeroMem (&Response, sizeof (Response));
  Request.ParameterValid.Bits.ParameterSelector    = IPMI_BOOT_OPTIONS_PARAMETER_BOOT_INFO_ACK;
  Request.ParameterValid.Bits.MarkParameterInvalid = 0;
  Request.Data.WriteMask                           = BOOT_OPTION_HANDLED_BY_BIOS;
  Request.Data.BootInitiatorAcknowledgeData        = 0;
  DataSize                                         = sizeof (Response);

  Status = IpmiSubmitCommand (
             IPMI_NETFN_CHASSIS,
             IPMI_CHASSIS_SET_SYSTEM_BOOT_OPTIONS,
             (VOID *)&Request,
             sizeof (Request),
             (VOID *)&Response,
             &DataSize
             );

  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a: Failed to acknowledge IPMI boot options! %r\n", __FUNCTION__, Status));
  } else if (DataSize < sizeof (IPMI_SET_BOOT_OPTIONS_RESPONSE)) {
    DEBUG ((DEBUG_ERROR, "%a: Unexpected message size! 0x%x\n", __FUNCTION__, DataSize));
  } else if (Response.CompletionCode != IPMI_COMP_CODE_NORMAL) {
    DEBUG ((
      DEBUG_ERROR,
      "%a: Acknowledge IPMI boot option returned failing completion code! 0x%x\n",
      __FUNCTION__,
      Response.CompletionCode
      ));
  }
}

/**
  Clears the IPMI boot options boot flags.

  @param[in] BootFlags  The boot flags value to set.

  @retval   EFI_SUCCESS         Boot flags were successfully set.
  @retval   EFI_PROTOCOL_ERROR  IPMI set returned a failing completion code.
  @retval   Other               A subroutine returned a failing status.
**/
EFI_STATUS
EFIAPI
IpmiSetBootFlags (
  IN IPMI_BOOT_OPTIONS_RESPONSE_PARAMETER_5  *BootFlags
  )
{
  IPMI_SET_BOOT_OPTIONS_REQUEST_5  Request;
  IPMI_SET_BOOT_OPTIONS_RESPONSE   Response;
  EFI_STATUS                       Status;
  UINT32                           DataSize;

  ZeroMem (&Request, sizeof (Request));
  ZeroMem (&Response, sizeof (Response));
  Request.ParameterValid.Bits.ParameterSelector    = IPMI_BOOT_OPTIONS_PARAMETER_BOOT_FLAGS;
  Request.ParameterValid.Bits.MarkParameterInvalid = 0;
  CopyMem (&Request.Data, BootFlags, sizeof (IPMI_BOOT_OPTIONS_RESPONSE_PARAMETER_5));
  DataSize = sizeof (Response);

  Status = IpmiSubmitCommand (
             IPMI_NETFN_CHASSIS,
             IPMI_CHASSIS_SET_SYSTEM_BOOT_OPTIONS,
             (VOID *)&Request,
             sizeof (Request),
             (VOID *)&Response,
             &DataSize
             );

  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a: Failed to set IPMI boot flags! %r\n", __FUNCTION__, Status));
  } else if (DataSize < sizeof (IPMI_SET_BOOT_OPTIONS_RESPONSE)) {
    DEBUG ((DEBUG_ERROR, "%a: Unexpected message size! 0x%x\n", __FUNCTION__, DataSize));
  } else if (Response.CompletionCode != IPMI_COMP_CODE_NORMAL) {
    DEBUG ((
      DEBUG_ERROR,
      "%a: Setting IPMI boot flags returned failing completion code! 0x%x\n",
      __FUNCTION__,
      Response.CompletionCode
      ));

    Status = EFI_PROTOCOL_ERROR;
  }

  return Status;
}

/**
  Retrieves the IPMI boot option boot flags.

  @param[in] BootFlags    The boot flags.
  @param[in] FlagsValid   TRUE if the flags are valid, FALSE otherwise.

  @retval   EFI_SUCCESS         Boot flags were successfully retrieved.
  @retval   EFI_PROTOCOL_ERROR  IPMI get returned a failing completion code.
  @retval   Other               A subroutine returned a failing status.
**/
EFI_STATUS
EFIAPI
IpmiGetBootFlags (
  OUT IPMI_BOOT_OPTIONS_RESPONSE_PARAMETER_5  *BootFlags,
  OUT BOOLEAN                                 *FlagsValid
  )
{
  IPMI_GET_BOOT_OPTIONS_RESPONSE_5  Response;
  EFI_STATUS                        Status;

  if ((BootFlags == NULL) || (FlagsValid == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  *FlagsValid = FALSE;
  ZeroMem (BootFlags, sizeof (*BootFlags));
  ZeroMem (&Response, sizeof (Response));

  Status = IpmiGetBootOption (
             IPMI_BOOT_OPTIONS_PARAMETER_BOOT_FLAGS,
             sizeof (Response),
             &Response.Header
             );

  if (EFI_ERROR (Status)) {
    return Status;
  }

  if (Response.Header.ParameterValid.Bits.ParameterValid != 0) {
    DEBUG ((DEBUG_INFO, "%a: Boot options parameter 5 invalid.\n", __FUNCTION__));
    return EFI_SUCCESS;
  }

  if (Response.Data.Data1.Bits.BootFlagValid == 0) {
    DEBUG ((DEBUG_INFO, "%a: Boot options flags not valid.\n", __FUNCTION__));
    return EFI_SUCCESS;
  }

  CopyMem (BootFlags, &Response.Data, sizeof (Response.Data));
  *FlagsValid = TRUE;
  return EFI_SUCCESS;
}

/**
  Gets the boot device override provided by the BMC. This function will clear
  the IPMI flags on calling.

  @param[out]   Selector        The boot option device specified by BMC. BootNone
                                will be returned if no valid override exists.

  @retval       EFI_SUCCESS             The boot options were successfully queried.
  @retval       EFI_INVALID_PARAMETER   Selector is NULL.
  @retval       EFI_PROTOCOL_ERROR      A failing IPMI completion code was returned.
  @retval       Other                   A failure was returned by the IPMI stack.
**/
EFI_STATUS
EFIAPI
IpmiGetBootDevice (
  OUT IPMI_BOOT_OPTION_SELECTOR  *Selector
  )
{
  IPMI_BOOT_OPTIONS_RESPONSE_PARAMETER_5  FlagsData;
  BOOLEAN                                 FlagsValid;
  EFI_STATUS                              Status;
  BOOLEAN                                 SetInProgress;

  if (Selector == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  //
  // Check there is no set in progress.
  //

  SetInProgress = FALSE;
  Status        = IpmiCheckSetInProgress (&SetInProgress);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a: Failed to query set in progress! %r\n", __FUNCTION__, Status));
    return Status;
  }

  if (SetInProgress) {
    DEBUG ((DEBUG_ERROR, "%a: Boot option set in progress!\n", __FUNCTION__));
    return EFI_NOT_READY;
  }

  *Selector = BootNone;
  ZeroMem (&FlagsData, sizeof (FlagsData));
  FlagsValid = FALSE;

  Status = IpmiGetBootFlags (&FlagsData, &FlagsValid);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  if (FlagsValid) {
    *Selector =  FlagsData.Data2.Bits.BootDeviceSelector;
    DEBUG ((
      DEBUG_INFO,
      "%a: Boot device override 0x%x. Persistence: %d \n",
      __FUNCTION__,
      FlagsData.Data2.Bits.BootDeviceSelector,
      FlagsData.Data1.Bits.PersistentOptions
      ));

    //
    // Clear the boot flags once this has been read. If the persistence option is
    // set then this should be skipped.
    //

    if (FlagsData.Data1.Bits.PersistentOptions == 0) {
      ZeroMem (&FlagsData, sizeof (FlagsData));
      IpmiSetBootFlags (&FlagsData);
    }
  }

  //
  // Acknowledge the boot options.
  //

  IpmiAcknowledgeBootOption ();

  return EFI_SUCCESS;
}

/**
  Checks if the CMOS clear bit is set in the IPMI boot options.

  @param[out]  ClearCmos    TRUE if the CMOS clear bit is set, FALSE otherwise.

  @retval     EFI_SUCCESS   The CMOS bit was successfully checked.
  @retval     EFI_NOT_READY The boot option set in progress bit is set.
  @retval     Other         An error was returned by a subroutine.
**/
EFI_STATUS
EFIAPI
IpmiGetCmosClearOption (
  OUT BOOLEAN  *ClearCmos
  )

{
  EFI_STATUS                              Status;
  IPMI_BOOT_OPTIONS_RESPONSE_PARAMETER_5  FlagsData;
  BOOLEAN                                 FlagsValid;
  BOOLEAN                                 SetInProgress;

  *ClearCmos    = FALSE;
  SetInProgress = FALSE;
  //
  // Check there is no set in progress.
  //

  Status = IpmiCheckSetInProgress (&SetInProgress);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a: Failed to query set in progress! %r\n", __FUNCTION__, Status));
    return Status;
  }

  if (SetInProgress) {
    DEBUG ((DEBUG_ERROR, "%a: Boot option set in progress!\n", __FUNCTION__));
    return EFI_NOT_READY;
  }

  //
  // Check the CMOS clear option.
  //

  Status = IpmiGetBootFlags (&FlagsData, &FlagsValid);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  if (!FlagsValid) {
    return EFI_SUCCESS;
  }

  *ClearCmos = FlagsData.Data2.Bits.CmosClear;

  //
  // Clear the CMOS flag so that this doesn't occur next boot.
  //

  if (FlagsData.Data2.Bits.CmosClear) {
    FlagsData.Data2.Bits.CmosClear = 0;
    IpmiSetBootFlags (&FlagsData);
  }

  return EFI_SUCCESS;
}
