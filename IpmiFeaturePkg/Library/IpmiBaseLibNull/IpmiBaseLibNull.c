/** @file
  A Null Library to support all BMC access via IPMI command.

  @copyright
  Copyright 2011 - 2021 Intel Corporation. <BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#include <Library/BaseLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/DxeServicesLib.h>
#include <Library/DebugLib.h>
#include <Library/IpmiBaseLib.h>
#include <Ppi/IpmiTransportPpi.h>

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
  return EFI_SUCCESS;
}
