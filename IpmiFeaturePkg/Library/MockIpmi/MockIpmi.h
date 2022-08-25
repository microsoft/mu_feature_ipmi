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

#endif
