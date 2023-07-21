/** @file
  Definitions for the IPMI watchdog timers policy.

  Copyright (c) Microsoft Corporation
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef IPMI_WATCHDOG_POLICY_H_
#define IPMI_WATCHDOG_POLICY_H_

#include <Uefi.h>

#define IPMI_WATCHDOG_POLICY_GUID  {0x6b53a598, 0x4ff5, 0x43c5, {0x81, 0x83, 0x74, 0xd0, 0xe6, 0x34, 0xcd, 0x66}}

#pragma pack(1)

typedef struct _IPMI_WATCHDOG_POLICY {
  // Indicates that the Frb2 watchdog timer should be enabled during PEI.
  BOOLEAN    Frb2Enabled;

  // The action that should be taken if the Frb2 watchdog timer expires as mentioned
  // in section 27.6 of the IPMI specification.
  //    0 - No action
  //    1 - Hard reset
  //    2 - Power down
  //    3 - Power cycle
  UINT8      Frb2TimeoutAction;

  // The number of seconds to set the Frb2 watchdog timeout to.
  UINT16     Frb2TimeoutSeconds;

  // Indicates the OS watchdog timer should be enabled during ExitBootServices.
  BOOLEAN    OsWatchdogEnabled;

  // The action that should be taken if the OS watchdog timer expires. See Frb2TimeoutAction
  // for values.
  UINT8      OsTimeoutAction;

  // The number of seconds to set the OS watchdog timeout to.
  UINT16     OsTimeoutSeconds;
} IPMI_WATCHDOG_POLICY;

#pragma pack()

#endif
