/** @file
  Driver for handling IPMI Watch Dog Timers.

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

EFI_STATUS
EnableFrb2WatchDogTimer (
  )
{
  EFI_STATUS                        Status;
  IPMI_SET_WATCHDOG_TIMER_REQUEST   FrbTimer;
  IPMI_GET_WATCHDOG_TIMER_RESPONSE  GetWatchdogTimer;
  UINT8                             CompletionCode;

  ZeroMem (&FrbTimer, sizeof (FrbTimer));
  FrbTimer.TimerUse.Bits.TimerUse        = IPMI_WATCHDOG_TIMER_BIOS_FRB2;
  FrbTimer.TimerActions.Uint8            = 0;
  FrbTimer.PretimeoutInterval            = 0;
  FrbTimer.TimerUseExpirationFlagsClear |= BIT1;    // set Frb2ExpirationFlag
  FrbTimer.InitialCountdownValue         = PcdGet16 (PcdFrb2TimeoutSeconds);

  // Set BMC watchdog timer
  Status = IpmiSetWatchdogTimer (&FrbTimer, &CompletionCode);
  Status = IpmiResetWatchdogTimer (&CompletionCode);
}

EFI_STATUS
EfiDisableFrb (
  VOID
  )
{
  EFI_STATUS                        Status;
  IPMI_SET_WATCHDOG_TIMER_REQUEST   SetWatchdogTimer;
  UINT8                             CompletionCode;
  IPMI_GET_WATCHDOG_TIMER_RESPONSE  GetWatchdogTimer;

  Status = IpmiGetWatchdogTimer (&GetWatchdogTimer);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // Check if timer is still running, if not abort disable routine.
  //
  if (GetWatchdogTimer.TimerUse.Bits.TimerRunning == 0) {
    return EFI_ABORTED;
  }

  ZeroMem (&SetWatchdogTimer, sizeof (SetWatchdogTimer));
  //
  // Just flip the Timer Use bit. This should release the timer.
  //
  SetWatchdogTimer.TimerUse.Bits.TimerRunning    = 0;
  SetWatchdogTimer.TimerUse.Bits.TimerUse        = IPMI_WATCHDOG_TIMER_BIOS_FRB2;
  SetWatchdogTimer.TimerUseExpirationFlagsClear &= ~BIT2;
  SetWatchdogTimer.TimerUseExpirationFlagsClear |= BIT1 | BIT4;

  Status = IpmiSetWatchdogTimer (&SetWatchdogTimer, &CompletionCode);
  return Status;
}
