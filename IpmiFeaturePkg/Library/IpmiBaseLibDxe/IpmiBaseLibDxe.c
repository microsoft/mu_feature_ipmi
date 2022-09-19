/** @file
  A Library to support all BMC access via IPMI command during Dxe Phase.

  @copyright
  Copyright 1999 - 2021 Intel Corporation. <BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#include <PiDxe.h>
#include <Protocol/IpmiTransportProtocol.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/DebugLib.h>

STATIC IPMI_TRANSPORT  *mIpmiTransport = NULL;

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
  EFI_STATUS  Status;

  if (mIpmiTransport == NULL) {
    Status = gBS->LocateProtocol (&gIpmiTransportProtocolGuid, NULL, (VOID **)&mIpmiTransport);
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_ERROR, "%a: Failed to locate IPMI protocol. %r\n", __FUNCTION__, Status));
      return Status;
    }
  }

  Status = mIpmiTransport->IpmiSubmitCommand (
                             mIpmiTransport,
                             NetFunction,
                             0,
                             Command,
                             CommandData,
                             CommandDataSize,
                             ResponseData,
                             ResponseDataSize
                             );
  return Status;
}

/**
  Gets the current status of the BMC from the IPMI module.

  @param[out]   BmcStatus     The current status of the BMC.
  @param[out]   ComAddress    The address of the BMC.

  @retval   EFI_SUCCESS             Successfully retrieved BMC status
  @retval   EFI_NOT_AVAILABLE_YET   Ipmi interface is not installed yet.
**/
EFI_STATUS
IpmiGetBmcStatus (
  OUT BMC_STATUS      *BmcStatus,
  OUT SM_COM_ADDRESS  *ComAddress
  )
{
  EFI_STATUS  Status;

  if (mIpmiTransport == NULL) {
    Status = gBS->LocateProtocol (&gIpmiTransportProtocolGuid, NULL, (VOID **)&mIpmiTransport);
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_ERROR, "%a: Failed to locate IPMI protocol. %r\n", __FUNCTION__, Status));
      return Status;
    }
  }

  Status = mIpmiTransport->GetBmcStatus (
                             mIpmiTransport,
                             BmcStatus,
                             ComAddress
                             );
  return Status;
}
