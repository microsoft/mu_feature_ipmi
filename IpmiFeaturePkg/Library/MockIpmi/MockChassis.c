/** @file
  Mock implementations for Chassis messages.

  Copyright (c) Microsoft Corporation
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include "MockIpmi.h"

IPMI_BOOT_OPTIONS_RESPONSE_PARAMETER_5_DATA_5  mBootFlags;

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
  IPMI_SET_BOOT_OPTIONS_REQUEST   *SetOptions;
  IPMI_SET_BOOT_OPTIONS_RESPONSE  *SetOptionsResponse;

  ASSERT (DataSize >= sizeof (IPMI_SET_BOOT_OPTIONS_REQUEST));
  ASSERT (*ResponseSize >= sizeof (IPMI_SET_BOOT_OPTIONS_RESPONSE));

  SetOptions                         = Data;
  SetOptionsResponse                 = Response;
  *ResponseSize                      = sizeof (*SetOptionsResponse);
  SetOptionsResponse->CompletionCode = IPMI_COMP_CODE_NORMAL;
  if (SetOptions->ParameterValid.Bits.ParameterSelector != IPMI_BOOT_OPTIONS_PARAMETER_BOOT_FLAGS) {
    DEBUG ((DEBUG_ERROR, "%a: Mock boot options only support boot flags!\n", __FUNCTION__));
    SetOptionsResponse->CompletionCode = IPMI_COMP_CODE_INVALID_DATA_FIELD;
    return;
  }

  ASSERT (
    DataSize - sizeof (IPMI_SET_BOOT_OPTIONS_REQUEST) >=
    sizeof (IPMI_BOOT_OPTIONS_RESPONSE_PARAMETER_5)
    );

  CopyMem (&mBootFlags, SetOptions + 1, sizeof (IPMI_BOOT_OPTIONS_RESPONSE_PARAMETER_5));
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
  IPMI_SET_BOOT_OPTIONS_REQUEST   *SetOptions;
  IPMI_SET_BOOT_OPTIONS_RESPONSE  *SetOptionsResponse;

  ASSERT (DataSize >= sizeof (IPMI_SET_BOOT_OPTIONS_REQUEST));
  ASSERT (*ResponseSize >= sizeof (IPMI_SET_BOOT_OPTIONS_RESPONSE));

  SetOptions                         = Data;
  SetOptionsResponse                 = Response;
  *ResponseSize                      = sizeof (*SetOptionsResponse);
  SetOptionsResponse->CompletionCode = IPMI_COMP_CODE_NORMAL;
  if (SetOptions->ParameterValid.Bits.ParameterSelector != IPMI_BOOT_OPTIONS_PARAMETER_BOOT_FLAGS) {
    DEBUG ((DEBUG_ERROR, "%a: Mock boot options only support boot flags!\n", __FUNCTION__));
    SetOptionsResponse->CompletionCode = IPMI_COMP_CODE_INVALID_DATA_FIELD;
    return;
  }

  ASSERT (
    DataSize - sizeof (IPMI_SET_BOOT_OPTIONS_REQUEST) >=
    sizeof (IPMI_BOOT_OPTIONS_RESPONSE_PARAMETER_5)
    );

  CopyMem (&mBootFlags, SetOptions + 1, sizeof (IPMI_BOOT_OPTIONS_RESPONSE_PARAMETER_5));
}
