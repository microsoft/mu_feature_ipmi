/** @file
  Definitions for the IPMI watchdog timers policy.

  Copyright (c) Microsoft Corporation
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef _IPMI_WATCHDOG_POLICY_H_
#define _IPMI_WATCHDOG_POLICY_H_

#include <uefi.h>

typedef struct _IPMI_WATCHDOG_POLICY {
  BOOLEAN    Frb2Enabled;
  UINT8      Frb2TimeoutAction;
  UINT16     Frb2TimeoutSeconds;
  BOOLEAN    OsWatchdogEnabled;
  UINT8      OsTimeoutAction;
  UINT16     OsTimeoutSeconds;
} IPMI_WATCHDOG_POLICY;

#endif
