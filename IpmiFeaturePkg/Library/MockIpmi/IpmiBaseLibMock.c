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
  Sends a IPMI command to the BMC and returns the response.

  @param[in]      NetFunction       Net function of the command.
  @param[in]      Command           IPMI command number.
  @param[in]      CommandData       Command data buffer.
  @param[in]      CommandDataSize   Size of command data.
  @param[out]     ResponseData      Response Data buffer.
  @param[in,out]  ResponseDataSize  Response data buffer size on input, size of
                                    read data or required size on return.

  @retval   EFI_SUCCESS             Successfully send IPMI command.
  @retval   EFI_NOT_AVAILABLE_YET   Ipmi interface is not installed yet.
**/
EFI_STATUS
IpmiSubmitCommand (
  IN UINT8       NetFunction,
  IN UINT8       Command,
  IN UINT8       *CommandData,
  IN UINT32      CommandDataSize,
  OUT UINT8      *ResponseData,
  IN OUT UINT32  *ResponseDataSize
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

/**
  Gets the current status of the BMC from the IPMI module.

  @param[out]   BmcStatus     The current status of the BMC.
  @param[out]   ComAddress    The address of the BMC.

  @retval   EFI_SUCCESS             Successfully retrieved BMC status
  @retval   EFI_NOT_AVAILABLE_YET   Ipmi interface is not installed yet.
**/
EFI_STATUS
GetBmcStatus (
  OUT BMC_STATUS      *BmcStatus,
  OUT SM_COM_ADDRESS  *ComAddress
  )
{
  *BmcStatus = BMC_OK;
  ZeroMem (ComAddress, sizeof (*ComAddress));
}
