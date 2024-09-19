/** @file
  The DXE implementation of the IPMI watchdog module.

  Copyright (c) Microsoft Corporation
  Copyright (c) 2018 - 2019, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/IpmiCommandLib.h>
#include <Library/PolicyLib.h>

#include <IndustryStandard/Ipmi.h>
#include <Library/IpmiWatchdogLib.h>
#include <Guid/IpmiWatchdogPolicy.h>

EFI_EVENT             mExitBootServicesEvent;
EFI_EVENT             mReadyToBootEvent;
IPMI_WATCHDOG_POLICY  mWatchdogPolicy;

/**
  Disables the FRB2 watchdog timer if it is running. This is intended to be
  called at ReadyToBoot.

  @param[in]  Event     The event triggering this routine.
  @param[in]  Context   Unused.
**/
VOID
EFIAPI
DisableFrb2WatchdogTimer (
  IN EFI_EVENT  Event,
  IN VOID       *Context
  )
{
  EFI_STATUS  Status;

  DEBUG ((DEBUG_INFO, "Disabling IPMI FBR2 watchdog timer.\n"));

  Status = IpmiDisableWatchdogTimer (
             IPMI_WATCHDOG_TIMER_BIOS_FRB2,
             IPMI_WATCHDOG_TIMER_EXPIRATION_FLAG_BIOS_FRB2
             );

  //
  // Make sure this doesn't get called again if it succeeded.
  //

  if (!EFI_ERROR (Status)) {
    gBS->CloseEvent (Event);
  }
}

/**
  Enabled the OS IPMI watchdog timer if it is enabled.

  @param[in]  Event     The event triggering this routine.
  @param[in]  Context   Unused.
**/
VOID
EFIAPI
EnableOsWatchdogTimer (
  IN EFI_EVENT  Event,
  IN VOID       *Context
  )
{
  if (mWatchdogPolicy.OsWatchdogEnabled) {
    DEBUG ((DEBUG_INFO, "Enabling IPMI OS watchdog timer.\n"));

    IpmiEnableWatchdogTimer (
      IPMI_WATCHDOG_TIMER_OS_LOADER,
      mWatchdogPolicy.OsTimeoutAction,
      IPMI_WATCHDOG_TIMER_EXPIRATION_FLAG_OS_LOAD,
      mWatchdogPolicy.OsTimeoutSeconds
      );
  }

  gBS->CloseEvent (Event);
}

/**
  Entry point to the IPMI watchdog DXE module. Will check the status of the
  watchdog timer and setup callbacks to handle various watchdog timer changes.

  @param[in]    ImageHandle   The handle for this module image.
  @param[in]    SystemTable   Pointer to the UEFI system table.

  @retval   EFI_SUCCESS           The watchdog timer and callbacks were properly setup.
  @retval   EFI_PROTOCOL_ERROR    An unexpected Completion Code was returned by IPMI.
  @retval   Other                 An error was returned by a subroutine.
**/
EFI_STATUS
EFIAPI
IpmiWatchdogDxeEntryPoint (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS                        Status;
  IPMI_GET_WATCHDOG_TIMER_RESPONSE  WatchdogTimer;
  UINT16                            PolicySize;

  PolicySize = sizeof (IPMI_WATCHDOG_POLICY);
  Status     = GetPolicy (&gIpmiWatchdogPolicyGuid, NULL, &mWatchdogPolicy, &PolicySize);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a: Failed to get watchdog policy! %r\n", __FUNCTION__, Status));
    ASSERT (FALSE);
    return Status;
  }

  //
  // Retrieve the current status of the watchdog timer.
  //
  Status = IpmiGetWatchdogTimer (&WatchdogTimer);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_INFO, "%a: Failed to get Watchdog Timer.\n", __FUNCTION__));
    return Status;
  }

  DEBUG ((
    DEBUG_INFO,
    "IPMI Watchdog timer status: %a\n",
    WatchdogTimer.TimerUse.Bits.TimerRunning ? "Running" : "Stopped"
    ));

  //
  // For BIOS not having PEI phase, enable IPMI FRB2 watchdog timer here.
  //
  if ((mWatchdogPolicy.Frb2Enabled) && (WatchdogTimer.TimerUse.Bits.TimerRunning == 0)) {
    IpmiEnableWatchdogTimer (
      IPMI_WATCHDOG_TIMER_BIOS_FRB2,
      mWatchdogPolicy.Frb2TimeoutAction,
      IPMI_WATCHDOG_TIMER_EXPIRATION_FLAG_BIOS_FRB2,
      mWatchdogPolicy.Frb2TimeoutSeconds
      );
  }

  //
  // Retrieve the current status of the watchdog timer again.
  //
  Status = IpmiGetWatchdogTimer (&WatchdogTimer);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_INFO, "%a: Failed to get Watchdog Timer.\n", __FUNCTION__));
    return Status;
  } else if (WatchdogTimer.CompletionCode != IPMI_COMP_CODE_NORMAL) {
    DEBUG ((
      DEBUG_ERROR,
      "%a: Failed to set the watchdog timer. Completion Code 0x%x\n",
      __FUNCTION__,
      WatchdogTimer.CompletionCode
      ));

    return EFI_PROTOCOL_ERROR;
  }

  DEBUG ((
    DEBUG_INFO,
    "IPMI Watchdog timer status: %a Type: %d Action: %d\n",
    WatchdogTimer.TimerUse.Bits.TimerRunning ? "Running" : "Stopped",
    WatchdogTimer.TimerUse.Bits.TimerUse,
    WatchdogTimer.TimerActions.Bits.TimeoutAction
    ));

  //
  // Set up callbacks for the FRB2 and OS watchdog timers.
  //

  Status = EfiCreateEventReadyToBootEx (
             TPL_NOTIFY,
             DisableFrb2WatchdogTimer,
             NULL,
             &mReadyToBootEvent
             );

  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a: Failed to create ReadyToBoot event for FRB2! %r\n", __FUNCTION__, Status));
    return Status;
  }

  Status = gBS->CreateEvent (
                  EVT_SIGNAL_EXIT_BOOT_SERVICES,
                  TPL_NOTIFY,
                  EnableOsWatchdogTimer,
                  NULL,
                  &mExitBootServicesEvent
                  );

  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a: Failed to create ExitBootServices event for OS! %r\n", __FUNCTION__, Status));
    return Status;
  }

  return Status;
}
