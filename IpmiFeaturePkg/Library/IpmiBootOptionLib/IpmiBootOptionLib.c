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

typedef struct _IPMI_GET_BOOT_OPTIONS_RESPONSE_5 {
  UINT8                                      CompletionCode;
  IPMI_GET_BOOT_OPTIONS_PARAMETER_VERSION    ParameterVersion;
  IPMI_GET_BOOT_OPTIONS_PARAMETER_VALID      ParameterValid;
  IPMI_BOOT_OPTIONS_RESPONSE_PARAMETER_5     Data;
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
  Clears the IPMI boot options boot flags.

**/
VOID
IpmiClearBootFlags (
  VOID
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
    DEBUG ((DEBUG_ERROR, "%a: Failed to clear IPMI boot flags! %r\n", __FUNCTION__, Status));
  } else if (DataSize < sizeof (IPMI_SET_BOOT_OPTIONS_RESPONSE)) {
    DEBUG ((DEBUG_ERROR, "%a: Unexpected message size! 0x%x\n", __FUNCTION__, DataSize));
  } else if (Response.CompletionCode != IPMI_COMP_CODE_NORMAL) {
    DEBUG ((
      DEBUG_ERROR,
      "%a: Clear IPMI boot flags returned failing completion code! 0x%x\n",
      __FUNCTION__,
      Response.CompletionCode
      ));
  }
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
IpmiGetBootOption (
  OUT IPMI_BOOT_OPTION_SELECTOR  *Selector
  )
{
  IPMI_GET_BOOT_OPTIONS_REQUEST     Request;
  IPMI_GET_BOOT_OPTIONS_RESPONSE_5  Response;
  EFI_STATUS                        Status;
  UINT32                            DataSize;

  if (Selector == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  *Selector = BootNone;
  ZeroMem (&Request, sizeof (Request));
  ZeroMem (&Response, sizeof (Response));
  Request.ParameterSelector.Bits.ParameterSelector = IPMI_BOOT_OPTIONS_PARAMETER_BOOT_FLAGS;
  Request.BlockSelector                            = 0;

  DataSize = sizeof (Response);
  Status   = IpmiSubmitCommand (
               IPMI_NETFN_CHASSIS,
               IPMI_CHASSIS_GET_SYSTEM_BOOT_OPTIONS,
               (VOID *)&Request,
               sizeof (Request),
               (VOID *)&Response,
               &DataSize
               );

  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a: Failed to get IPMI boot options. %r\n", __FUNCTION__, Status));
    return Status;
  }

  if (DataSize < sizeof (Response.CompletionCode)) {
    DEBUG ((DEBUG_ERROR, "%a: Response too small for completion code! 0x%x\n", __FUNCTION__, DataSize));
    return EFI_BAD_BUFFER_SIZE;
  }

  if (Response.CompletionCode != IPMI_COMP_CODE_NORMAL) {
    DEBUG ((
      DEBUG_ERROR,
      "%a: Get system boot options returned failing completion code. 0x%x\n",
      __FUNCTION__,
      Response.CompletionCode
      ));

    return EFI_PROTOCOL_ERROR;
  }

  if (DataSize < sizeof (Response)) {
    DEBUG ((DEBUG_ERROR, "%a: Unexpected response size! 0x%x\n", __FUNCTION__, DataSize));
    return EFI_BAD_BUFFER_SIZE;
  }

  if (Response.ParameterValid.Bits.ParameterValid != 0) {
    DEBUG ((DEBUG_INFO, "%a: Boot options parameter 5 invalid.\n", __FUNCTION__));
    goto Exit;
  }

  if (Response.Data.Data1.Bits.BootFlagValid == 0) {
    DEBUG ((DEBUG_INFO, "%a: Boot options flags not valid.\n", __FUNCTION__));
    goto Exit;
  }

  *Selector =  Response.Data.Data2.Bits.BootDeviceSelector;
  DEBUG ((
    DEBUG_INFO,
    "%a: Boot device override 0x%x. Persistence: %d \n",
    __FUNCTION__,
    Response.Data.Data2.Bits.BootDeviceSelector,
    Response.Data.Data1.Bits.PersistentOptions
    ));

  //
  // Clear the boot flags once this has been read. If the persistence option is
  // set then this should be skipped.
  //

  if (Response.Data.Data1.Bits.PersistentOptions == 0) {
    IpmiClearBootFlags ();
  }

Exit:
  //
  // Acknowledge the boot options.
  //

  IpmiAcknowledgeBootOption ();
  return EFI_SUCCESS;
}
