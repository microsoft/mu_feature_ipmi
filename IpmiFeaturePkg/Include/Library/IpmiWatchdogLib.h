/** @file
  Definitions for the generic IPMI watchdog module.

  Copyright (c) Microsoft Corporation
  Copyright (c) 2018 - 2019, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef IPMI_WATCHDOG_LIB_H_
#define IPMI_WATCHDOG_LIB_H_

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
  );

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
  );

#endif
