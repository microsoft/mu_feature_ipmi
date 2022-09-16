/** @file
  Host based unit tests for the IPMI watchdog modules.

  Copyright (c) Microsoft Corporation
  PDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Uefi.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/UnitTestLib.h>
#include <IndustryStandard/Ipmi.h>

#include <IpmiWatchdog.h>

#define UNIT_TEST_NAME     "IPMI Watchdog Unit Test"
#define UNIT_TEST_VERSION  "1.0"

//
// Hooks into the mock IPMI watchdog timer to check tests.
//

extern IPMI_GET_WATCHDOG_TIMER_RESPONSE  mWatchdog;

/**
  Clears the state of the test libraries.

  @param[in]  Context    UNUSED
**/
VOID
EFIAPI
ResetTestState (
  IN UNIT_TEST_CONTEXT  Context
  )
{
  ZeroMem (&mWatchdog, sizeof (mWatchdog));
}

/**
  Tests starting a watchdog timer.

  @param[in]  Context             UNUSED

  @retval  UNIT_TEST_PASSED             The Unit test has completed and the test
                                        case was successful.
  @retval  UNIT_TEST_ERROR_TEST_FAILED  A test case assertion has failed.
**/
UNIT_TEST_STATUS
EFIAPI
TestStartWatchdog (
  IN UNIT_TEST_CONTEXT  Context
  )

{
  EFI_STATUS  Status;

  Status = IpmiEnableWatchdogTimer (
             IPMI_WATCHDOG_TIMER_BIOS_FRB2,
             IPMI_WATCHDOG_TIMER_ACTION_HARD_RESET,
             IPMI_WATCHDOG_TIMER_EXPIRATION_FLAG_BIOS_FRB2,
             600
             );

  UT_ASSERT_STATUS_EQUAL (Status, EFI_SUCCESS);
  UT_ASSERT_STATUS_EQUAL (mWatchdog.InitialCountdownValue, 6000); // This is in 100ms, not seconds.
  UT_ASSERT_STATUS_EQUAL (mWatchdog.TimerActions.Bits.TimeoutAction, IPMI_WATCHDOG_TIMER_ACTION_HARD_RESET);
  UT_ASSERT_STATUS_EQUAL (mWatchdog.TimerUse.Bits.TimerRunning, 1);

  return UNIT_TEST_PASSED;
}

/**
  Tests stopping a watchdog timer.

  @param[in]  Context             UNUSED

  @retval  UNIT_TEST_PASSED             The Unit test has completed and the test
                                        case was successful.
  @retval  UNIT_TEST_ERROR_TEST_FAILED  A test case assertion has failed.
**/
UNIT_TEST_STATUS
EFIAPI
TestStopWatchdog (
  IN UNIT_TEST_CONTEXT  Context
  )

{
  EFI_STATUS  Status;

  Status = IpmiEnableWatchdogTimer (
             IPMI_WATCHDOG_TIMER_BIOS_FRB2,
             IPMI_WATCHDOG_TIMER_ACTION_HARD_RESET,
             IPMI_WATCHDOG_TIMER_EXPIRATION_FLAG_BIOS_FRB2,
             600
             );

  UT_ASSERT_STATUS_EQUAL (Status, EFI_SUCCESS);
  UT_ASSERT_STATUS_EQUAL (mWatchdog.TimerUse.Bits.TimerRunning, 1);

  IpmiDisableWatchdogTimer (IPMI_WATCHDOG_TIMER_BIOS_FRB2, IPMI_WATCHDOG_TIMER_ACTION_HARD_RESET);

  UT_ASSERT_STATUS_EQUAL (Status, EFI_SUCCESS);
  UT_ASSERT_STATUS_EQUAL (mWatchdog.TimerUse.Bits.TimerRunning, 0);

  return UNIT_TEST_PASSED;
}

/**
  Initializes and configures the IPMI watchdog module tests.

  @retval  EFI_SUCCESS           All test cases were dispatched.
  @retval  EFI_OUT_OF_RESOURCES  There are not enough resources available to
                                 initialize the unit tests.
**/
EFI_STATUS
EFIAPI
IpmiWatchdogTestMain (
  VOID
  )
{
  EFI_STATUS                  Status;
  UNIT_TEST_FRAMEWORK_HANDLE  Framework;
  UNIT_TEST_SUITE_HANDLE      WatchdogTests;

  Framework = NULL;

  DEBUG ((DEBUG_INFO, "%a v%a\n", UNIT_TEST_NAME, UNIT_TEST_VERSION));

  //
  // Start setting up the test framework for running the tests.
  //
  Status = InitUnitTestFramework (&Framework, UNIT_TEST_NAME, gEfiCallerBaseName, UNIT_TEST_VERSION);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "Failed in InitUnitTestFramework. Status = %r\n", Status));
    goto EXIT;
  }

  //
  // Populate the SEL Unit Test Suite.
  //
  Status = CreateUnitTestSuite (&WatchdogTests, Framework, "IPMI Watchdog Module Tests", "IPMI.WATCHDOG", NULL, NULL);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "Failed in CreateUnitTestSuite for WatchdogTests\n"));
    Status = EFI_OUT_OF_RESOURCES;
    goto EXIT;
  }

  AddTestCase (WatchdogTests, "Tests starting a watchdog timer", "TestStartWatchdog", TestStartWatchdog, NULL, ResetTestState, NULL);
  AddTestCase (WatchdogTests, "Tests stopping a watchdog timer", "TestStopWatchdog", TestStopWatchdog, NULL, ResetTestState, NULL);

  Status = RunAllTestSuites (Framework);

EXIT:
  if (Framework) {
    FreeUnitTestFramework (Framework);
  }

  return Status;
}

/**
  Standard POSIX C entry point for host based unit test execution.
**/
int
main (
  int   argc,
  char  *argv[]
  )
{
  return IpmiWatchdogTestMain ();
}
