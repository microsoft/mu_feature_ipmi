/** @file
  Common code for handling IPMI Watch Dog Timers.

  Copyright (c) Microsoft Corporation
  Copyright (c) 2018 - 2019, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Uefi.h>

#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/PcdLib.h>
#include <Library/IpmiCommandLib.h>

#include <IndustryStandard/Ipmi.h>

#include <IpmiWatchdog.h>

/**
  Enables the IPMI watchdog timer with the provided configurations.

  @param[in]  TimerUse        The current use of the IPMI watchdog timer.
  @param[in]  TimerAction     The requested action of the BMC if the time expires.
  @param[in]  FlagsClear      The watchdog expired flags to clear.
  @param[in]  CountdownValue  The initial time of the watchdog timer in seconds.

  @retval   EFI_SUCCESS         Watchdog timer was successfully started.
  @retval   EFI_PROTOCOL_ERROR  Unexpected completion code was returned.
  @retval   Other               An error was returned by the IPMI command lib.
**/
EFI_STATUS
EFIAPI
IpmiEnableWatchdogTimer (
  IN CONST UINT8   TimerUse,
  IN CONST UINT8   TimerAction,
  IN CONST UINT8   FlagsClear,
  IN CONST UINT16  CountdownValue
  )
{
  EFI_STATUS                        Status;
  IPMI_SET_WATCHDOG_TIMER_REQUEST   WatchdogTimer;
  IPMI_GET_WATCHDOG_TIMER_RESPONSE  GetWatchdogTimer;
  UINT8                             CompletionCode;

  DEBUG ((
    DEBUG_INFO,
    "Setting IPMI watchdog timer. Timer: 0x%x Action: 0x%x ClearFlags: 0x%x Countdown: 0x%x\n",
    TimerUse,
    TimerAction,
    FlagsClear,
    CountdownValue
    ));

  ASSERT (TimerUse != 0);
  ASSERT (CountdownValue != 0);

  Status = IpmiGetWatchdogTimer (&GetWatchdogTimer);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a: Failed to get watchdog information.\n", __FUNCTION__));
    return Status;
  }

  //
  // Initialize the settings.
  //
  ZeroMem (&WatchdogTimer, sizeof (WatchdogTimer));
  WatchdogTimer.TimerUse.Bits.TimerUse          = TimerUse;
  WatchdogTimer.TimerActions.Bits.TimeoutAction = TimerAction;
  WatchdogTimer.TimerUseExpirationFlagsClear    = FlagsClear;
  WatchdogTimer.InitialCountdownValue           = CountdownValue;

  //
  // Set the watchdog information.
  //
  Status = IpmiSetWatchdogTimer (&WatchdogTimer, &CompletionCode);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a: Failed to set the watchdog timer. %r\n", __FUNCTION__, Status));
    return Status;
  } else if (CompletionCode != IPMI_COMP_CODE_NORMAL) {
    DEBUG ((DEBUG_ERROR, "%a: Failed to set the watchdog timer. Completion Code 0x%x\n", __FUNCTION__, CompletionCode));
    return EFI_PROTOCOL_ERROR;
  }

  //
  // Start the watchdog timer.
  //
  Status = IpmiResetWatchdogTimer (&CompletionCode);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a: Failed to set the watchdog timer. %r\n", __FUNCTION__, Status));
    return Status;
  } else if (CompletionCode != IPMI_COMP_CODE_NORMAL) {
    DEBUG ((DEBUG_ERROR, "%a: Failed to set the watchdog timer. Completion Code 0x%x\n", __FUNCTION__, CompletionCode));
    return EFI_PROTOCOL_ERROR;
  }

  return Status;
}

/**
  Disables the IPMI watchdog timer.

  @param[in]  TimerUse        The current use of the IPMI watchdog timer.
  @param[in]  FlagsClear      The watchdog expired flags to clear.

  @retval   EFI_SUCCESS         Watchdog timer was successfully stopped.
  @retval   EFI_PROTOCOL_ERROR  Unexpected completion code was returned.
  @retval   Other               An error was returned by the IPMI command lib.
**/
EFI_STATUS
EFIAPI
IpmiDisableWatchdogTimer (
  IN CONST UINT8  TimerUse,
  IN CONST UINT8  FlagsClear
  )
{
  EFI_STATUS                        Status;
  IPMI_SET_WATCHDOG_TIMER_REQUEST   SetWatchdogTimer;
  UINT8                             CompletionCode;
  IPMI_GET_WATCHDOG_TIMER_RESPONSE  GetWatchdogTimer;

  Status = IpmiGetWatchdogTimer (&GetWatchdogTimer);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a: Failed to get watchdog information.\n", __FUNCTION__));
    return Status;
  }

  //
  // Check if timer is still running, if not abort disable routine.
  //
  if (GetWatchdogTimer.TimerUse.Bits.TimerRunning == 0) {
    DEBUG ((DEBUG_WARN, "%a: Watchdog timer is not currently running.\n", __FUNCTION__));
    return EFI_ABORTED;
  }

  //
  // Set the timer configuration.
  //

  ZeroMem (&SetWatchdogTimer, sizeof (SetWatchdogTimer));
  SetWatchdogTimer.TimerUse.Bits.TimerRunning   = 0;
  SetWatchdogTimer.TimerUse.Bits.TimerUse       = TimerUse;
  SetWatchdogTimer.TimerUseExpirationFlagsClear = FlagsClear;

  //
  // Set the timer, this will also stop the current timer.
  //

  Status = IpmiSetWatchdogTimer (&SetWatchdogTimer, &CompletionCode);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a: Failed to clear the watchdog timer. %r\n", __FUNCTION__, Status));
    return Status;
  } else if (CompletionCode != IPMI_COMP_CODE_NORMAL) {
    DEBUG ((DEBUG_ERROR, "%a: Failed to clear the watchdog timer. Completion Code 0x%x\n", __FUNCTION__, CompletionCode));
    return EFI_PROTOCOL_ERROR;
  }

  return Status;
}
