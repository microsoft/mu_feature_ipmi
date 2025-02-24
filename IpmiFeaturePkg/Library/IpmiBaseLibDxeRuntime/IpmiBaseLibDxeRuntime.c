/** @file
  IpmiBaseLibDxeRuntime.c

  Copyright (c) Microsoft Corporation.
  Your use of this software is governed by the terms of the Microsoft agreement under which you obtained the software.
**/

#include <PiDxe.h>
#include <Protocol/IpmiTransportProtocol.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiRuntimeLib.h>

static IPMI_TRANSPORT  *mIpmiTransport;
extern BOOLEAN         mEfiAtRuntime;

/**
  The event of EFI Virtual Address Change of this library.
  Convert the physical pointer to virtual ones for runtime to use.

  @param[in]  Event     This event.
  @param[in]  *Context  The context buffer of this event.

**/
VOID
EFIAPI
ConvertIpmiTransportPointer (
  IN EFI_EVENT  Event,
  IN VOID       *Context
  )
{
  if (mIpmiTransport != NULL) {
    EfiConvertPointer (0, (VOID **)&mIpmiTransport->IpmiSubmitCommand);
    EfiConvertPointer (0, (VOID **)&mIpmiTransport->GetBmcStatus);
    EfiConvertPointer (0, (VOID **)&mIpmiTransport);
  }

  return;
}

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
  @retval   EFI_NOT_FOUND           Ipmi interface is not installed yet.
**/
EFI_STATUS
EFIAPI
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
    if (mEfiAtRuntime) {
      return EFI_NOT_FOUND;
    } else {
      Status = gBS->LocateProtocol (&gIpmiTransportProtocolGuid, NULL, (VOID **)&mIpmiTransport);
      if (EFI_ERROR (Status)) {
        DEBUG ((DEBUG_ERROR, "[%a]Failed to locate IPMI protocol(%r)\n", __FUNCTION__, Status));
        return Status;
      }
    }
  }

  return mIpmiTransport->IpmiSubmitCommand (mIpmiTransport, NetFunction, 0, Command, CommandData, CommandDataSize, ResponseData, ResponseDataSize);
}

/**
  Gets the current status of the BMC from the IPMI module.

  @param[out]   BmcStatus     The current status of the BMC.
  @param[out]   ComAddress    The address of the BMC.

  @retval   EFI_SUCCESS             Successfully retrieved BMC status
  @retval   EFI_NOT_AVAILABLE_YET   Ipmi interface is not installed yet.
**/
EFI_STATUS
EFIAPI
GetBmcStatus (
  OUT BMC_STATUS      *BmcStatus,
  OUT SM_COM_ADDRESS  *ComAddress
  )
{
  EFI_STATUS  Status;

  if (mIpmiTransport == NULL) {
    if (mEfiAtRuntime) {
      return EFI_NOT_FOUND;
    } else {
      Status = gBS->LocateProtocol (&gIpmiTransportProtocolGuid, NULL, (VOID **)&mIpmiTransport);
      if (EFI_ERROR (Status)) {
        DEBUG ((DEBUG_ERROR, "[%a]Failed to locate IPMI protocol(%r)\n", __FUNCTION__, Status));
        return Status;
      }
    }
  }

  return mIpmiTransport->GetBmcStatus (mIpmiTransport, BmcStatus, ComAddress);
}

/**
  Register callback to get IPMI transport protocol.

  @param[in]  Event                 Event whose notification function is being invoked.
  @param[in]  Context               The pointer to the notification function's context, which is
                                    implementation-dependent.
**/
VOID
EFIAPI
GetIpmiTransportProtocol (
  IN  EFI_EVENT  Event,
  IN  VOID       *Context
  )
{
  EFI_STATUS  Status = EFI_NOT_STARTED;

  DEBUG ((DEBUG_INFO, "[%a] \n", __func__));

  Status = gBS->LocateProtocol (&gIpmiTransportProtocolGuid, NULL, (VOID **)&mIpmiTransport);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "[%a]Failed to locate IPMI protocol(%r)\n", __func__, Status));
  } else {
    if (Event != NULL) {
      gBS->CloseEvent (Event);
    }
  }

  return;
}

/**
  The constructor function for IPMI base DXE runtime library.

  @param[in]  ImageHandle  The firmware allocated handle for the EFI image.
  @param[in]  SystemTable  A pointer to the EFI System Table.

  @return EFI_STATUS Success or failure status.

**/
EFI_STATUS
EFIAPI
IpmiBaseLibDxeRuntimeConstructor (
  IN  EFI_HANDLE        ImageHandle,
  IN  EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS  Status        = EFI_NOT_STARTED;
  VOID        *Registration = NULL;
  EFI_EVENT   Event         = NULL;

  DEBUG ((DEBUG_INFO, "[%a] \n", __func__));

  EfiCreateProtocolNotifyEvent (&gIpmiTransportProtocolGuid, TPL_CALLBACK, GetIpmiTransportProtocol, NULL, &Registration);

  Status = gBS->CreateEvent (EVT_SIGNAL_VIRTUAL_ADDRESS_CHANGE, TPL_NOTIFY, ConvertIpmiTransportPointer, NULL, &Event);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "[%a] Register event address change failed(%r)\n", __func__, Status));
  }

  return EFI_SUCCESS;
}
