/** @file
  Implements the IPMI base lib for the mock BMC.

  Copyright (c) Microsoft Corporation
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include "MockIpmi.h"
#include <Library/IpmiBaseLib.h>

#define MOCK_BASE_IPMI_BUFFER_SIZE  (MAX_UINT8)

STATIC UINT8  RequestBuffer[MOCK_BASE_IPMI_BUFFER_SIZE];
STATIC UINT8  ResponseBuffer[MOCK_BASE_IPMI_BUFFER_SIZE];

/**
  Initialize the global varible with the pointer of IpmiTransport Protocol.

  @retval EFI_SUCCESS - Always return success

**/
EFI_STATUS
InitializeIpmiBase (
  VOID
  )
{
  return EFI_SUCCESS;
}

/**
  Routine to send commands to BMC.

  @param NetFunction       - Net function of the command
  @param Command           - IPMI Command
  @param CommandData       - Command Data
  @param CommandDataSize   - Size of CommandData
  @param ResponseData      - Response Data
  @param ResponseDataSize  - Response Data Size

  @retval EFI_NOT_AVAILABLE_YET - IpmiTransport Protocol is not installed yet

**/
EFI_STATUS
IpmiSubmitCommand (
  IN UINT8    NetFunction,
  IN UINT8    Command,
  IN UINT8    *CommandData,
  IN UINT32   CommandDataSize,
  OUT UINT8   *ResponseData,
  OUT UINT32  *ResponseDataSize
  )
{
  IPMI_COMMAND   *Request;
  IPMI_RESPONSE  *Response;
  UINT8          ResponseSize;
  EFI_STATUS     Status;

  ASSERT (CommandDataSize <= MOCK_BASE_IPMI_BUFFER_SIZE - sizeof (IPMI_COMMAND));

  ZeroMem (&RequestBuffer[0], MOCK_BASE_IPMI_BUFFER_SIZE);
  ZeroMem (&ResponseBuffer[0], MOCK_BASE_IPMI_BUFFER_SIZE);
  Request      = (VOID *)&RequestBuffer[0];
  Response     = (VOID *)&ResponseBuffer[0];
  ResponseSize = MOCK_BASE_IPMI_BUFFER_SIZE;

  Request->Lun         = 0;
  Request->NetFunction = NetFunction;
  Request->Command     = Command;
  CopyMem (Request + 1, CommandData, CommandDataSize);

  Status = MockIpmiCommand (Request, (UINT8)(CommandDataSize + sizeof (IPMI_COMMAND)));
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = MockIpmiResponse (Response, &ResponseSize);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  ASSERT (ResponseSize - sizeof (IPMI_RESPONSE) <= *ResponseDataSize);
  *ResponseDataSize = ResponseSize - sizeof (IPMI_RESPONSE);
  CopyMem (ResponseData, Response + 1, *ResponseDataSize);
  return EFI_SUCCESS;
}
