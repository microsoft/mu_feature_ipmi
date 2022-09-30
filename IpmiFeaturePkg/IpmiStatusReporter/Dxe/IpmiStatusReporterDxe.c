/** @file


  Copyright (c) Microsoft Corporation
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <IpmiStatusReporter.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Protocol/ReportStatusCodeHandler.h>

STATIC EFI_EVENT                 mExitBootServicesEvent = NULL;
STATIC EFI_RSC_HANDLER_PROTOCOL  *mRscHandlerProtocol   = NULL;

/**
  Callback for ExitBootServices to unregister status callback.

  @param[in]  Event     The event triggering this routine.
  @param[in]  Context   Unused.
**/
VOID
EFIAPI
ExitBootServicesCallback (
  IN EFI_EVENT  Event,
  IN VOID       *Context
  )
{
  mRscHandlerProtocol->Unregister (IpmiReportStatusCode);
  gBS->CloseEvent (Event);
}

/**
  Entry point for IpmiStatusReporterDxe to intiialize the ReportStatusCode
  handler.

  @param[in]    ImageHandle   The handle for this module image.
  @param[in]    SystemTable   Pointer to the UEFI system table.

  @retval   EFI_SUCCESS           The watchdog timer and callbacks were properly setup.
  @retval   Other                 An error was returned by a subroutine.
**/
EFI_STATUS
EFIAPI
IpmiStatusReporterDxeEntryPoint (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS  Status;

  Status = gBS->LocateProtocol (&gEfiRscHandlerProtocolGuid, NULL, (VOID **)&mRscHandlerProtocol);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a: Failed to locate gEfiRscHandlerProtocolGuid! %r\n", __FUNCTION__, Status));
    ASSERT (FALSE);
    return Status;
  }

  Status = mRscHandlerProtocol->Register (IpmiReportStatusCode, TPL_HIGH_LEVEL);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a: Failed to register IPMI report status callback! %r\n", __FUNCTION__, Status));
    return Status;
  }

  //
  // Create an event to unregister at exit boot services.
  //

  Status = gBS->CreateEvent (
                  EVT_SIGNAL_EXIT_BOOT_SERVICES,
                  TPL_NOTIFY,
                  ExitBootServicesCallback,
                  NULL,
                  &mExitBootServicesEvent
                  );

  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a: Failed to create ExitBootServices callback! %r\n", __FUNCTION__, Status));
    return Status;
  }

  return Status;
}
