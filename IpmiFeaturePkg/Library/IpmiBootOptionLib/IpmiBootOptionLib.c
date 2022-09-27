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
#include <Library/IpmiBootOptionLib.h>

#pragma pack(1)

//
// Direct definitions of the expected structures for accurate structure sizes.
//

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
  Gets the boot device override provided by the BMC. This function will clear
  the IPMI flags on calling.

  @param[out]   Selector        The boot option device specified by BMC. BootNone
                                will be returned if no valid override exists.

  @retval       EFI_SUCCESS         The boot options were successfully queried.
  @retval       EFI_PROTOCOL_ERROR  A failing IPMI completion code was returned.
  @retval       Other               A failure was returned by the IPMI stack.
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

  if (Response.CompletionCode != IPMI_COMP_CODE_NORMAL) {
    DEBUG ((
      DEBUG_ERROR,
      "%a: Get system boot options returned failing completion code. 0x%x\n",
      __FUNCTION__,
      Response.CompletionCode
      ));

    return EFI_PROTOCOL_ERROR;
  }

  if (Response.ParameterValid.Bits.ParameterValid != 0) {
    DEBUG ((DEBUG_INFO, "%a: Boot options parameter 5 invalid.\n", __FUNCTION__));
    return EFI_SUCCESS;
  }

  if (Response.Data.Data1.Bits.BootFlagValid == 0) {
    DEBUG ((DEBUG_INFO, "%a: Boot options flags not valid.\n", __FUNCTION__));
    return EFI_SUCCESS;
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

  return EFI_SUCCESS;
}
