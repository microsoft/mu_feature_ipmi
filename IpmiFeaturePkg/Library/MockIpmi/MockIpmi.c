/** @file
  A mock implementation of a BMC. This allow of minimal use of the IPMI
  functionality without an actual BMC. This is intended for use in testing
  changes to the IPMI feature package as well as components that depend on it.

  Copyright (c) Microsoft Corporation
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include "MockIpmi.h"

//
// Registered handlers.
//

MOCK_IPMI_HANDLER_ENTRY  MockHandlers[] =
{
  { IPMI_NETFN_APP,     IPMI_APP_GET_DEVICE_ID,               MockIpmiGetDeviceId          },
  { IPMI_NETFN_APP,     IPMI_APP_GET_SELFTEST_RESULTS,        MockIpmiGetSelfTest          },
  { IPMI_NETFN_STORAGE, IPMI_STORAGE_GET_SEL_INFO,            MockIpmiSelGetInfo           },
  { IPMI_NETFN_STORAGE, IPMI_STORAGE_ADD_SEL_ENTRY,           MockIpmiSelAddEntry          },
  { IPMI_NETFN_STORAGE, IPMI_STORAGE_GET_SEL_TIME,            MockIpmiSelGetTime           },
  { IPMI_NETFN_STORAGE, IPMI_STORAGE_SET_SEL_TIME,            MockIpmiSelSetTime           },
  { IPMI_NETFN_STORAGE, IPMI_STORAGE_CLEAR_SEL,               MockIpmiSelClear             },
  { IPMI_NETFN_STORAGE, IPMI_STORAGE_GET_SEL_ENTRY,           MockIpmiSelGetEntry          },
  { IPMI_NETFN_APP,     IPMI_APP_GET_WATCHDOG_TIMER,          MockIpmiGetWatchdog          },
  { IPMI_NETFN_APP,     IPMI_APP_SET_WATCHDOG_TIMER,          MockIpmiSetWatchdog          },
  { IPMI_NETFN_APP,     IPMI_APP_RESET_WATCHDOG_TIMER,        MockIpmiResetWatchdog        },
  { IPMI_NETFN_CHASSIS, IPMI_CHASSIS_SET_SYSTEM_BOOT_OPTIONS, MockIpmiSetSystemBootOptions },
  { IPMI_NETFN_CHASSIS, IPMI_CHASSIS_GET_SYSTEM_BOOT_OPTIONS, MockIpmiGetSystemBootOptions },
};

//
// Internal globals used to track responses across calls.
//

STATIC BOOLEAN             mResponseValid;
STATIC IPMI_RESPONSE_DATA  mResponse;
STATIC UINT8               mResponseSize;

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

  mResponse.Header.NetFunction = Command->NetFunction | 0x1;
  mResponse.Header.Command     = Command->Command;
  mResponse.ResponseData[0]    = IPMI_COMP_CODE_INVALID_COMMAND;
  mResponseSize                = sizeof (IPMI_RESPONSE);

  //
  // Search for a specific handler.
  //

  for (Index = 0; Index < ARRAY_SIZE (MockHandlers); Index++) {
    if ((MockHandlers[Index].NetFunction == Command->NetFunction) &&
        (MockHandlers[Index].Command == Command->Command))
    {
      ResponseDataSize = sizeof (mResponse.ResponseData);
      MockHandlers[Index].Handler (
                            (VOID *)(Command + 1),
                            (Size - sizeof (IPMI_COMMAND)),
                            &mResponse.ResponseData[0],
                            &ResponseDataSize
                            );

      ASSERT (ResponseDataSize > 0);
      mResponseSize  = sizeof (IPMI_RESPONSE) + ResponseDataSize;
      mResponseValid = TRUE;
      return EFI_SUCCESS;
    }
  }

  //
  // No handler was found, responded that this is not supported.
  //

  mResponse.ResponseData[0] = IPMI_COMP_CODE_INVALID_COMMAND;
  mResponseSize             = sizeof (IPMI_RESPONSE) + sizeof (mResponse.ResponseData[0]);
  mResponseValid            = TRUE;
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
  ASSERT (mResponseValid);
  ASSERT (mResponseSize != 0);
  ASSERT (mResponseSize <= sizeof (IPMI_RESPONSE_DATA));
  ASSERT (*Size > mResponseSize);

  CopyMem (Response, &mResponse, mResponseSize);
  *Size = mResponseSize;

  ZeroMem (&mResponse, sizeof (mResponse));
  mResponseValid = FALSE;

  return EFI_SUCCESS;
}

//
// Basic mock handler routines.
//

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
