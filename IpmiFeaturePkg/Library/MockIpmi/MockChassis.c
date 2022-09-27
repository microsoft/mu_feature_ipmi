/** @file
  Mock implementations for Chassis messages.

  Copyright (c) Microsoft Corporation
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include "MockIpmi.h"

IPMI_BOOT_OPTIONS_RESPONSE_PARAMETER_5  mBootFlags;
UINT8                                   mBootOptionAcks = 0xFF;

/**
  Mocks the result of IPMI_CHASSIS_GET_SYSTEM_BOOT_OPTIONS.

  @param[in]       Data           The IPMI request data.
  @param[in]       DataSize       The size of the IPMI request data.
  @param[out]      Response       The response data buffer.
  @param[in, out]  ResponseSize   On input, the available size of buffer.
                                  On output, the size of written data in the buffer.
**/
VOID
MockIpmiGetSystemBootOptions (
  IN VOID       *Data,
  IN UINT8      DataSize,
  OUT VOID      *Response,
  IN OUT UINT8  *ResponseSize
  )
{
  IPMI_GET_BOOT_OPTIONS_REQUEST   *GetOptionRequest;
  IPMI_GET_BOOT_OPTIONS_RESPONSE  *OptionResponse;

  ASSERT (DataSize >= sizeof (IPMI_GET_BOOT_OPTIONS_REQUEST));
  ASSERT (*ResponseSize >= sizeof (IPMI_GET_BOOT_OPTIONS_RESPONSE));

  GetOptionRequest = Data;

  OptionResponse                                        = Response;
  OptionResponse->CompletionCode                        = IPMI_COMP_CODE_NORMAL;
  OptionResponse->ParameterValid.Bits.ParameterSelector = GetOptionRequest->ParameterSelector.Bits.ParameterSelector;

  ASSERT (GetOptionRequest->BlockSelector == 0);
  ASSERT (GetOptionRequest->SetSelector == 0);

  if (GetOptionRequest->ParameterSelector.Bits.ParameterSelector == IPMI_BOOT_OPTIONS_PARAMETER_BOOT_FLAGS) {
    ASSERT (
      *ResponseSize - sizeof (IPMI_GET_BOOT_OPTIONS_RESPONSE) >=
      sizeof (IPMI_BOOT_OPTIONS_RESPONSE_PARAMETER_5)
      );

    CopyMem (OptionResponse + 1, &mBootFlags, sizeof (IPMI_BOOT_OPTIONS_RESPONSE_PARAMETER_5));
    *ResponseSize = sizeof (IPMI_GET_BOOT_OPTIONS_RESPONSE) +
                    sizeof (IPMI_BOOT_OPTIONS_RESPONSE_PARAMETER_5);
  } else {
    DEBUG ((DEBUG_ERROR, "%a: Mock boot options only support boot flags!\n", __FUNCTION__));
    OptionResponse->CompletionCode = 0x80; // Spec defined response for unsupported parameter.
    *ResponseSize                  = sizeof (*OptionResponse);
  }
}

/**
  Mocks the result of IPMI_CHASSIS_SET_SYSTEM_BOOT_OPTIONS.

  @param[in]       Data           The IPMI request data.
  @param[in]       DataSize       The size of the IPMI request data.
  @param[out]      Response       The response data buffer.
  @param[in, out]  ResponseSize   On input, the available size of buffer.
                                  On output, the size of written data in the buffer.
**/
VOID
MockIpmiSetSystemBootOptions (
  IN VOID       *Data,
  IN UINT8      DataSize,
  OUT VOID      *Response,
  IN OUT UINT8  *ResponseSize
  )
{
  IPMI_SET_BOOT_OPTIONS_REQUEST           *SetOptions;
  IPMI_SET_BOOT_OPTIONS_RESPONSE          *SetOptionsResponse;
  IPMI_BOOT_OPTIONS_RESPONSE_PARAMETER_4  *BootOptionAcks;

  ASSERT (DataSize >= sizeof (IPMI_SET_BOOT_OPTIONS_REQUEST));
  ASSERT (*ResponseSize >= sizeof (IPMI_SET_BOOT_OPTIONS_RESPONSE));

  SetOptions                         = Data;
  SetOptionsResponse                 = Response;
  *ResponseSize                      = sizeof (*SetOptionsResponse);
  SetOptionsResponse->CompletionCode = IPMI_COMP_CODE_NORMAL;
  if (SetOptions->ParameterValid.Bits.ParameterSelector == IPMI_BOOT_OPTIONS_PARAMETER_BOOT_FLAGS) {
    ASSERT (
      DataSize - sizeof (IPMI_SET_BOOT_OPTIONS_REQUEST) >=
      sizeof (IPMI_BOOT_OPTIONS_RESPONSE_PARAMETER_5)
      );

    CopyMem (&mBootFlags, SetOptions + 1, sizeof (IPMI_BOOT_OPTIONS_RESPONSE_PARAMETER_5));
  } else if (SetOptions->ParameterValid.Bits.ParameterSelector == IPMI_BOOT_OPTIONS_PARAMETER_BOOT_INFO_ACK) {
    ASSERT (
      DataSize - sizeof (IPMI_SET_BOOT_OPTIONS_REQUEST) >=
      sizeof (IPMI_BOOT_OPTIONS_RESPONSE_PARAMETER_4)
      );

    BootOptionAcks   = (IPMI_BOOT_OPTIONS_RESPONSE_PARAMETER_4 *)(SetOptions + 1);
    mBootOptionAcks &= ~BootOptionAcks->WriteMask;
    mBootOptionAcks |= (BootOptionAcks->WriteMask & BootOptionAcks->BootInitiatorAcknowledgeData);
  } else {
    DEBUG ((DEBUG_ERROR, "%a: Mock boot options only support boot flags!\n", __FUNCTION__));
    SetOptionsResponse->CompletionCode = 0x80; // Spec defined response for unsupported parameter.
  }
}
