/** @file
  Mock implementation for the IPMI watchdog timer.

  Copyright (c) Microsoft Corporation
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include "MockIpmi.h"

IPMI_GET_WATCHDOG_TIMER_RESPONSE  mWatchdog;

/**
  Mocks the result of IPMI_APP_GET_WATCHDOG_TIMER.

  @param[in]       Data           The IPMI request data.
  @param[in]       DataSize       The size of the IPMI request data.
  @param[out]      Response       The response data buffer.
  @param[in, out]  ResponseSize   On input, the available size of buffer.
                                  On output, the size of written data in the buffer.
**/
VOID
MockIpmiGetWatchdog (
  IN VOID       *Data,
  IN UINT8      DataSize,
  OUT VOID      *Response,
  IN OUT UINT8  *ResponseSize
  )
{
  IPMI_GET_WATCHDOG_TIMER_RESPONSE  *Watchdog;

  ASSERT (*ResponseSize >= sizeof (IPMI_GET_WATCHDOG_TIMER_RESPONSE));

  //
  // Decrement the counter for the sake of realism.
  //

  if (mWatchdog.TimerUse.Bits.TimerRunning && (mWatchdog.PresentCountdownValue > 0)) {
    mWatchdog.PresentCountdownValue--;
  }

  Watchdog                 = Response;
  *Watchdog                = mWatchdog;
  Watchdog->CompletionCode = IPMI_COMP_CODE_NORMAL;

  *ResponseSize = sizeof (IPMI_GET_WATCHDOG_TIMER_RESPONSE);
}

/**
  Mocks the result of IPMI_APP_SET_WATCHDOG_TIMER.

  @param[in]       Data           The IPMI request data.
  @param[in]       DataSize       The size of the IPMI request data.
  @param[out]      Response       The response data buffer.
  @param[in, out]  ResponseSize   On input, the available size of buffer.
                                  On output, the size of written data in the buffer.
**/
VOID
MockIpmiSetWatchdog (
  IN VOID       *Data,
  IN UINT8      DataSize,
  OUT VOID      *Response,
  IN OUT UINT8  *ResponseSize
  )
{
  IPMI_SET_WATCHDOG_TIMER_REQUEST  *SetWatchdog;
  UINT8                            *CompletionCode;

  ASSERT (DataSize >= sizeof (IPMI_SET_WATCHDOG_TIMER_REQUEST));
  ASSERT (*ResponseSize >= sizeof (*CompletionCode));

  SetWatchdog                               = Data;
  mWatchdog.TimerUse.Bits.TimerUse          = SetWatchdog->TimerUse.Bits.TimerUse;
  mWatchdog.TimerActions.Bits.TimeoutAction = SetWatchdog->TimerActions.Bits.TimeoutAction;
  mWatchdog.TimerUseExpirationFlagsClear   &= ~SetWatchdog->TimerUseExpirationFlagsClear;
  mWatchdog.InitialCountdownValue           = SetWatchdog->InitialCountdownValue;
  mWatchdog.PresentCountdownValue           = SetWatchdog->InitialCountdownValue;

  DEBUG ((
    DEBUG_INFO,
    "Mock IPMI watchdog set. Use: %d Action: %d ExpiredFlags: 0x%x Timer: 0x%x\n",
    mWatchdog.TimerUse.Bits.TimerUse,
    mWatchdog.TimerActions.Bits.TimeoutAction,
    mWatchdog.TimerUseExpirationFlagsClear,
    mWatchdog.InitialCountdownValue
    ));

  CompletionCode  = Response;
  *CompletionCode = IPMI_COMP_CODE_NORMAL;

  *ResponseSize = sizeof (*CompletionCode);
}

/**
  Mocks the result of IPMI_APP_RESET_WATCHDOG_TIMER.

  @param[in]       Data           The IPMI request data.
  @param[in]       DataSize       The size of the IPMI request data.
  @param[out]      Response       The response data buffer.
  @param[in, out]  ResponseSize   On input, the available size of buffer.
                                  On output, the size of written data in the buffer.
**/
VOID
MockIpmiResetWatchdog (
  IN VOID       *Data,
  IN UINT8      DataSize,
  OUT VOID      *Response,
  IN OUT UINT8  *ResponseSize
  )
{
  UINT8  *CompletionCode;

  ASSERT (*ResponseSize >= sizeof (*CompletionCode));

  mWatchdog.TimerUse.Bits.TimerRunning = 1;
  mWatchdog.PresentCountdownValue      = mWatchdog.InitialCountdownValue;

  DEBUG ((DEBUG_INFO, "Mock IPMI watchdog started.\n"));

  CompletionCode  = Response;
  *CompletionCode = IPMI_COMP_CODE_NORMAL;

  *ResponseSize = sizeof (*CompletionCode);
}
