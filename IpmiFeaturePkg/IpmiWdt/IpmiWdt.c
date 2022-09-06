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

VOID
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
