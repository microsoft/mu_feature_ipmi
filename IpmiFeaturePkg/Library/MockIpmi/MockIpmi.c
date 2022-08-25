/** @file
  A mock implementation of a BMC. This allow of minimal use of the IPMI
  functionality without an actual BMC. This is intended for use in testing
  changes to the IPMI feature package as well as components that depend on it.

  Copyright (c) Microsoft Corporation
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include "MockIpmi.h"

BOOLEAN             gResponseValid;
IPMI_RESPONSE_DATA  gResponse;
UINT8               gResponseSize;

/**
  Mocks the result of IPMI_APP_GET_SELFTEST_RESULTS.

  @param[in]       Data           The IPMI request data.
  @param[in]       DataSize       The size of the IPMI request data.
  @param[out]      Response       The response data buffer.
  @param[in, out]  ResponseSize   On input, the available size of buffer.
                                  On output, the size of written data in the buffer.
**/
VOID
MockIpmiGetSelfTest (
  IN VOID       *Data,
  IN UINT8      DataSize,
  OUT VOID      *Response,
  IN OUT UINT8  *ResponseSize
  )
{
  IPMI_SELF_TEST_RESULT_RESPONSE  *SelfTest;

  ASSERT (*ResponseSize >= sizeof (IPMI_SELF_TEST_RESULT_RESPONSE));

  SelfTest                 = Response;
  SelfTest->CompletionCode = IPMI_COMP_CODE_NORMAL;
  SelfTest->Result         = IPMI_APP_SELFTEST_NOT_IMPLEMENTED;
  SelfTest->Param          = 0;

  *ResponseSize = sizeof (IPMI_SELF_TEST_RESULT_RESPONSE);
}

/**
  Mocks the result of IPMI_APP_GET_DEVICE_ID.

  @param[in]       Data           The IPMI request data.
  @param[in]       DataSize       The size of the IPMI request data.
  @param[out]      Response       The response data buffer.
  @param[in, out]  ResponseSize   On input, the available size of buffer.
                                  On output, the size of written data in the buffer.
**/
VOID
MockIpmiGetDeviceId (
  IN VOID       *Data,
  IN UINT8      DataSize,
  OUT VOID      *Response,
  IN OUT UINT8  *ResponseSize
  )
{
  IPMI_GET_DEVICE_ID_RESPONSE  *DeviceId;

  ASSERT (*ResponseSize >= sizeof (IPMI_GET_DEVICE_ID_RESPONSE));

  DeviceId                       = Response;
  DeviceId->CompletionCode       = IPMI_COMP_CODE_NORMAL;
  DeviceId->DeviceId             = 0xAB;
  DeviceId->DeviceRevision.Uint8 = 0;
  DeviceId->FirmwareRev1.Uint8   = 0x0;
  DeviceId->MinorFirmwareRev     = 0;
  DeviceId->SpecificationVersion = 2;
  DeviceId->DeviceSupport.Uint8  = 0;
  DeviceId->ManufacturerId[0]    = 0xB;
  DeviceId->ManufacturerId[1]    = 0xA;
  DeviceId->ManufacturerId[2]    = 0xD;
  DeviceId->ProductId            = 1;
  DeviceId->AuxFirmwareRevInfo   = 0;

  *ResponseSize = sizeof (IPMI_GET_DEVICE_ID_RESPONSE);
}

/**
  Mocks the result of IPMI_STORAGE_GET_SEL_INFO.

  @param[in]       Data           The IPMI request data.
  @param[in]       DataSize       The size of the IPMI request data.
  @param[out]      Response       The response data buffer.
  @param[in, out]  ResponseSize   On input, the available size of buffer.
                                  On output, the size of written data in the buffer.
**/
VOID
MockIpmiSelGetInfo (
  IN VOID       *Data,
  IN UINT8      DataSize,
  OUT VOID      *Response,
  IN OUT UINT8  *ResponseSize
  )
{
  IPMI_GET_SEL_INFO_RESPONSE  *SelInfo;

  ASSERT (*ResponseSize >= sizeof (IPMI_GET_SEL_INFO_RESPONSE));

  SelInfo                       = Response;
  SelInfo->CompletionCode       = IPMI_COMP_CODE_NORMAL;
  SelInfo->Version              = 0x51; // Per IPMI v2
  SelInfo->NoOfEntries          = 0;
  SelInfo->FreeSpace            = SIZE_16KB;
  SelInfo->RecentAddTimeStamp   = 0;
  SelInfo->RecentEraseTimeStamp = 0;
  SelInfo->OperationSupport     = 0;

  *ResponseSize = sizeof (IPMI_GET_SEL_INFO_RESPONSE);
}

/**
  Mocks the result of IPMI_STORAGE_ADD_SEL_ENTRY.

  @param[in]       Data           The IPMI request data.
  @param[in]       DataSize       The size of the IPMI request data.
  @param[out]      Response       The response data buffer.
  @param[in, out]  ResponseSize   On input, the available size of buffer.
                                  On output, the size of written data in the buffer.
**/
VOID
MockIpmiSelAddEntry (
  IN VOID       *Data,
  IN UINT8      DataSize,
  OUT VOID      *Response,
  IN OUT UINT8  *ResponseSize
  )
{
  IPMI_ADD_SEL_ENTRY_REQUEST   *SelEntry;
  IPMI_ADD_SEL_ENTRY_RESPONSE  *SelResponse;
  STATIC UINT16                RecordId = 1;

  ASSERT (DataSize >= sizeof (IPMI_ADD_SEL_ENTRY_REQUEST));
  ASSERT (*ResponseSize >= sizeof (IPMI_ADD_SEL_ENTRY_RESPONSE));

  SelEntry = Data;
  DEBUG ((DEBUG_INFO, "[SEL ENTRY] RecordType: 0x%x\n", SelEntry->RecordData.RecordType));

  SelResponse                 = Response;
  SelResponse->CompletionCode = IPMI_COMP_CODE_NORMAL;
  SelResponse->RecordId       = RecordId++;

  *ResponseSize = sizeof (IPMI_ADD_SEL_ENTRY_RESPONSE);
}

/**
  Mocks the result of IPMI_STORAGE_GET_SEL_TIME.

  @param[in]       Data           The IPMI request data.
  @param[in]       DataSize       The size of the IPMI request data.
  @param[out]      Response       The response data buffer.
  @param[in, out]  ResponseSize   On input, the available size of buffer.
                                  On output, the size of written data in the buffer.
**/
VOID
MockIpmiSelGetTime (
  IN VOID       *Data,
  IN UINT8      DataSize,
  OUT VOID      *Response,
  IN OUT UINT8  *ResponseSize
  )
{
  IPMI_GET_SEL_TIME_RESPONSE  *TimeResponse;

  ASSERT (*ResponseSize >= sizeof (IPMI_GET_SEL_TIME_RESPONSE));

  TimeResponse                 = Response;
  TimeResponse->CompletionCode = IPMI_COMP_CODE_NORMAL;
  TimeResponse->Timestamp      = 0;

  *ResponseSize = sizeof (IPMI_GET_SEL_TIME_RESPONSE);
}

/**
  Mocks the result of IPMI_STORAGE_SET_SEL_TIME.

  @param[in]       Data           The IPMI request data.
  @param[in]       DataSize       The size of the IPMI request data.
  @param[out]      Response       The response data buffer.
  @param[in, out]  ResponseSize   On input, the available size of buffer.
                                  On output, the size of written data in the buffer.
**/
VOID
MockIpmiSelSetTime (
  IN VOID       *Data,
  IN UINT8      DataSize,
  OUT VOID      *Response,
  IN OUT UINT8  *ResponseSize
  )
{
  IPMI_SET_SEL_TIME_REQUEST  *SetTime;
  UINT8                      *CompletionCode;

  ASSERT (DataSize >= sizeof (IPMI_SET_SEL_TIME_REQUEST));
  ASSERT (*ResponseSize >= sizeof (*CompletionCode));

  SetTime = Data;

  CompletionCode  = Response;
  *CompletionCode = IPMI_COMP_CODE_NORMAL;

  *ResponseSize = sizeof (*CompletionCode);
}

/**
  Mocks the result of IPMI_STORAGE_CLEAR_SEL.

  @param[in]       Data           The IPMI request data.
  @param[in]       DataSize       The size of the IPMI request data.
  @param[out]      Response       The response data buffer.
  @param[in, out]  ResponseSize   On input, the available size of buffer.
                                  On output, the size of written data in the buffer.
**/
VOID
MockIpmiSelClear (
  IN VOID       *Data,
  IN UINT8      DataSize,
  OUT VOID      *Response,
  IN OUT UINT8  *ResponseSize
  )
{
  IPMI_CLEAR_SEL_REQUEST   *ClearRequest;
  IPMI_CLEAR_SEL_RESPONSE  *ClearResponse;

  ASSERT (DataSize >= sizeof (IPMI_CLEAR_SEL_REQUEST));
  ASSERT (*ResponseSize >= sizeof (IPMI_CLEAR_SEL_RESPONSE));

  ClearRequest = Data;
  ASSERT (ClearRequest->AscC == 'C');
  ASSERT (ClearRequest->AscL == 'L');
  ASSERT (ClearRequest->AscR == 'R');

  ClearResponse                 = Response;
  ClearResponse->CompletionCode = IPMI_COMP_CODE_NORMAL;
  if (ClearRequest->Erase == IPMI_CLEAR_SEL_REQUEST_INITIALIZE_ERASE) {
    ClearResponse->ErasureProgress = IPMI_CLEAR_SEL_RESPONSE_ERASURE_IN_PROGRESS;
  } else if (ClearRequest->Erase == IPMI_CLEAR_SEL_REQUEST_GET_ERASE_STATUS) {
    ClearResponse->ErasureProgress = IPMI_CLEAR_SEL_RESPONSE_ERASURE_COMPLETED;
  } else {
    ASSERT (FALSE);
  }

  *ResponseSize = sizeof (IPMI_CLEAR_SEL_RESPONSE);
}

//
// Registered handlers.
//

MOCK_IPMI_HANDLER_ENTRY  MockHandlers[] =
{
  { IPMI_NETFN_APP,     IPMI_APP_GET_DEVICE_ID,        MockIpmiGetDeviceId },
  { IPMI_NETFN_APP,     IPMI_APP_GET_SELFTEST_RESULTS, MockIpmiGetSelfTest },
  { IPMI_NETFN_STORAGE, IPMI_STORAGE_GET_SEL_INFO,     MockIpmiSelGetInfo  },
  { IPMI_NETFN_STORAGE, IPMI_STORAGE_ADD_SEL_ENTRY,    MockIpmiSelAddEntry },
  { IPMI_NETFN_STORAGE, IPMI_STORAGE_GET_SEL_TIME,     MockIpmiSelGetTime  },
  { IPMI_NETFN_STORAGE, IPMI_STORAGE_SET_SEL_TIME,     MockIpmiSelSetTime  },
  { IPMI_NETFN_STORAGE, IPMI_STORAGE_CLEAR_SEL,        MockIpmiSelClear    },
};

//
// Generic routines for handling top level IPMI commands and responses.
//

/**
  Processes a IPMI request and prepared the next response.

  @param[in]  Command       The IPMI request to mock.
  @param[in]  Size          The size of the IPMI request.

  @retval   EFI_SUCCESS     Always
**/
EFI_STATUS
MockIpmiCommand (
  IN IPMI_COMMAND  *Command,
  IN UINT8         Size
  )
{
  UINT32  Index;
  UINT8   ResponseDataSize;

  ASSERT (Command->Lun == 0);
  ASSERT (Command != NULL);
  ASSERT (Size >= sizeof (IPMI_COMMAND));

  //
  // Generic response handling. Add the 0 bit value to indicate this is a
  // response.
  //

  gResponse.Header.NetFunction = Command->NetFunction | 0x1;
  gResponse.Header.Command     = Command->Command;
  gResponse.ResponseData[0]    = IPMI_COMP_CODE_INVALID_COMMAND;
  gResponseSize                = sizeof (IPMI_RESPONSE);

  //
  // Search for a specific handler.
  //

  for (Index = 0; Index < (sizeof (MockHandlers) / sizeof (MockHandlers[0])); Index++) {
    if ((MockHandlers[Index].NetFunction == Command->NetFunction) &&
        (MockHandlers[Index].Command == Command->Command))
    {
      ResponseDataSize = sizeof (gResponse.ResponseData);
      MockHandlers[Index].Handler (
                            (VOID *)((&Command) + 1),
                            (Size - sizeof (IPMI_COMMAND)),
                            &gResponse.ResponseData[0],
                            &ResponseDataSize
                            );

      gResponseSize  = sizeof (IPMI_RESPONSE) + ResponseDataSize;
      gResponseValid = TRUE;
      return EFI_SUCCESS;
    }
  }

  //
  // No handler was found, responded that this is not supported.
  //

  gResponse.ResponseData[0] = IPMI_COMP_CODE_INVALID_COMMAND;
  gResponseSize             = sizeof (IPMI_RESPONSE) + sizeof (gResponse.ResponseData[0]);
  gResponseValid            = TRUE;
  return EFI_SUCCESS;
}

/**
  Provides the prepared response to the last IPMI request.

  @param[out]     Response    The mocked response.
  @param[in,out]  Size        The size of the mocked response.

  @retval   EFI_SUCCESS       Always.
**/
EFI_STATUS
MockIpmiResponse (
  OUT IPMI_RESPONSE  *Response,
  IN OUT UINT8       *Size
  )
{
  ASSERT (gResponseValid);
  ASSERT (gResponseSize != 0);
  ASSERT (gResponseSize <= sizeof (IPMI_RESPONSE_DATA));
  ASSERT (*Size > gResponseSize);

  CopyMem (Response, &gResponse, gResponseSize);
  *Size = gResponseSize;

  ZeroMem (&gResponse, sizeof (gResponse));
  gResponseValid = FALSE;

  return EFI_SUCCESS;
}
