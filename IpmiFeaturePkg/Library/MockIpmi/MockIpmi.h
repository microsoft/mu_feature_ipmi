/** @file
  Definitions for the Mock IPMI implementation.

  Copyright (c) Microsoft Corporation
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef _MOCK_IPMI_H_
#define _MOCK_IPMI_H_

#include <Uefi.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <IndustryStandard/Ipmi.h>

#define MOCK_IPMI_BUFFER_SIZE  250

#pragma pack(1)

//
// Structure of IPMI Command buffer
//
typedef struct _IPMI_COMMAND {
  UINT8    Lun         : 2;
  UINT8    NetFunction : 6;
  UINT8    Command;
} IPMI_COMMAND;

//
// Structure of IPMI Command response buffer
//
typedef struct _IPMI_RESPONSE {
  UINT8    Lun         : 2;
  UINT8    NetFunction : 6;
  UINT8    Command;
} IPMI_RESPONSE;

typedef struct _IPMI_RESPONSE_DATA {
  IPMI_RESPONSE    Header;
  UINT8            ResponseData[MOCK_IPMI_BUFFER_SIZE];
} IPMI_RESPONSE_DATA;

#pragma pack()

//
// Definitions for registering new handlers.
//

typedef
VOID
(*MOCK_IPMI_RESPONSE)(
  IN VOID       *Data,
  IN UINT8      DataSize,
  OUT VOID      *Response,
  IN OUT UINT8  *ResponseSize
  );

typedef struct _MOCK_IPMI_HANDLER_ENTRY {
  UINT8                 NetFunction;
  UINT8                 Command;
  MOCK_IPMI_RESPONSE    Handler;
} MOCK_IPMI_HANDLER_ENTRY;

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
  );

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
  );

//
// Mock IPMI routines.
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
  );

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
  );

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
  );

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
  );

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
  );

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
  );

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
  );

/**
  Mocks the result of IPMI_STORAGE_GET_SEL_ENTRY.

  @param[in]       Data           The IPMI request data.
  @param[in]       DataSize       The size of the IPMI request data.
  @param[out]      Response       The response data buffer.
  @param[in, out]  ResponseSize   On input, the available size of buffer.
                                  On output, the size of written data in the buffer.
**/
VOID
MockIpmiSelGetEntry (
  IN VOID       *Data,
  IN UINT8      DataSize,
  OUT VOID      *Response,
  IN OUT UINT8  *ResponseSize
  );

/**
  Mocks the result of IPMI_APP_GET_WATCHDOG_TIMER.

  @param[in]       Data           The IPMI request data.
  @param[in]       DataSize       The size of the IPMI request data.
  @param[out]      Response       The response data buffer.
  @param[in, out]  ResponseSize   On input, the available size of buffer.
                                  On output, the size of written data in the buffer.
**/
VOID
MockIpmiGetWatchdog (
  IN VOID       *Data,
  IN UINT8      DataSize,
  OUT VOID      *Response,
  IN OUT UINT8  *ResponseSize
  );

/**
  Mocks the result of IPMI_APP_SET_WATCHDOG_TIMER.

  @param[in]       Data           The IPMI request data.
  @param[in]       DataSize       The size of the IPMI request data.
  @param[out]      Response       The response data buffer.
  @param[in, out]  ResponseSize   On input, the available size of buffer.
                                  On output, the size of written data in the buffer.
**/
VOID
MockIpmiSetWatchdog (
  IN VOID       *Data,
  IN UINT8      DataSize,
  OUT VOID      *Response,
  IN OUT UINT8  *ResponseSize
  );

/**
  Mocks the result of IPMI_APP_RESET_WATCHDOG_TIMER.

  @param[in]       Data           The IPMI request data.
  @param[in]       DataSize       The size of the IPMI request data.
  @param[out]      Response       The response data buffer.
  @param[in, out]  ResponseSize   On input, the available size of buffer.
                                  On output, the size of written data in the buffer.
**/
VOID
MockIpmiResetWatchdog (
  IN VOID       *Data,
  IN UINT8      DataSize,
  OUT VOID      *Response,
  IN OUT UINT8  *ResponseSize
  );

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
  );

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
  );

#endif
