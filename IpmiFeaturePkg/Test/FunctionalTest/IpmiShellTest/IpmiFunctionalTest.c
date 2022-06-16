/** @file
  Functional shell test for IPMI communication. This test is designed to
  exercise some common IPMI commands to test a platforms IPMI stack. It is not
  intended to be a test of the BMC's implementation of IPMI or related features.

  Copyright (c) Microsoft Corporation
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Uefi.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/UnitTestLib.h>
#include <Library/IpmiCommandLib.h>

#define UNIT_TEST_NAME     "IPMI Functional Test"
#define UNIT_TEST_VERSION  "1.0"

//
// Command Tests
//

/**
  Tests the GetDeviceId IPMI function.

  @param[in]  Context             UNUSED

  @retval  UNIT_TEST_PASSED             The Unit test has completed and the test
                                        case was successful.
  @retval  UNIT_TEST_ERROR_TEST_FAILED  A test case assertion has failed.
**/
UNIT_TEST_STATUS
EFIAPI
GetDeviceIdTest (
  IN UNIT_TEST_CONTEXT  Context
  )
{
  EFI_STATUS                   Status;
  IPMI_GET_DEVICE_ID_RESPONSE  DeviceId;

  Status = IpmiGetDeviceId (&DeviceId);
  UT_ASSERT_NOT_EFI_ERROR (Status);
  UT_ASSERT_EQUAL (DeviceId.CompletionCode, 0);

  DEBUG ((DEBUG_INFO, "************ DEVICE ID **************\n"));
  DEBUG ((
    DEBUG_INFO,
    "DeviceID: %d\nSpecVersion: %d\nManufacturerId %c%c%c\nProductId: %d\n",
    DeviceId.DeviceId,
    DeviceId.SpecificationVersion,
    DeviceId.ManufacturerId[0],
    DeviceId.ManufacturerId[1],
    DeviceId.ManufacturerId[2],
    DeviceId.ProductId
    ));

  DEBUG ((DEBUG_INFO, "*************************************\n"));

  return UNIT_TEST_PASSED;
}

/**
  Tests basic watchdog timer functionality

  @param[in]  Context             UNUSED

  @retval  UNIT_TEST_PASSED             The Unit test has completed and the test
                                        case was successful.
  @retval  UNIT_TEST_ERROR_TEST_FAILED  A test case assertion has failed.
**/
UNIT_TEST_STATUS
EFIAPI
WatchdogTimerTest (
  IN UNIT_TEST_CONTEXT  Context
  )
{
  EFI_STATUS                       Status;
  UINT8                            CompletionCode;
  IPMI_SET_WATCHDOG_TIMER_REQUEST  TimerReq = { 0 };

  TimerReq.TimerUse.Bits.TimerRunning            = 0; // Force the timer to stop.
  TimerReq.TimerUse.Bits.TimerUse                = IPMI_WATCHDOG_TIMER_BIOS_POST;
  TimerReq.TimerActions.Bits.TimeoutAction       = IPMI_WATCHDOG_TIMER_ACTION_NO_ACTION;
  TimerReq.TimerActions.Bits.PreTimeoutInterrupt = IPMI_WATCHDOG_PRE_TIMEOUT_INTERRUPT_NONE;
  TimerReq.TimerUseExpirationFlagsClear          = IPMI_WATCHDOG_TIMER_EXPIRATION_FLAG_BIOS_POST;
  TimerReq.InitialCountdownValue                 = (10 * 10); // 100ms * (10 * 10) = 10 seconds

  Status = IpmiSetWatchdogTimer (&TimerReq, &CompletionCode);
  UT_ASSERT_NOT_EFI_ERROR (Status);
  UT_ASSERT_EQUAL (CompletionCode, 0);

  return UNIT_TEST_PASSED;
}

//
// SEL Tests
//

/**
  Tests getting the SEL time.

  @param[in]  Context             UNUSED

  @retval  UNIT_TEST_PASSED             The Unit test has completed and the test
                                        case was successful.
  @retval  UNIT_TEST_ERROR_TEST_FAILED  A test case assertion has failed.
**/
UNIT_TEST_STATUS
EFIAPI
SelGetTimeTest (
  IN UNIT_TEST_CONTEXT  Context
  )
{
  EFI_STATUS                  Status;
  IPMI_GET_SEL_TIME_RESPONSE  SelTime;

  Status = IpmiGetSelTime (&SelTime);
  UT_ASSERT_NOT_EFI_ERROR (Status);
  UT_ASSERT_EQUAL (SelTime.CompletionCode, 0);
  UT_ASSERT_NOT_EQUAL (SelTime.Timestamp, 0);
  DEBUG ((DEBUG_INFO, "SEL Time: 0x%x\n", SelTime.Timestamp));
  return UNIT_TEST_PASSED;
}

/**
  Tests getting the SEL info.

  @param[in]  Context             UNUSED

  @retval  UNIT_TEST_PASSED             The Unit test has completed and the test
                                        case was successful.
  @retval  UNIT_TEST_ERROR_TEST_FAILED  A test case assertion has failed.
**/
UNIT_TEST_STATUS
EFIAPI
SelGetInfoTest (
  IN UNIT_TEST_CONTEXT  Context
  )
{
  EFI_STATUS                  Status;
  IPMI_GET_SEL_INFO_RESPONSE  SelInfo;

  Status = IpmiGetSelInfo (&SelInfo);
  UT_ASSERT_NOT_EFI_ERROR (Status);
  UT_ASSERT_EQUAL (SelInfo.CompletionCode, 0);

  DEBUG ((DEBUG_INFO, "************ SEL INFO ***************\n"));
  DEBUG ((
    DEBUG_INFO,
    "Version: %d\nNoOfEntries: %d\nFreeSpace %d\nOperationSupport: %d\n",
    SelInfo.Version,
    SelInfo.NoOfEntries,
    SelInfo.FreeSpace,
    SelInfo.OperationSupport
    ));

  DEBUG ((DEBUG_INFO, "*************************************\n"));

  return UNIT_TEST_PASSED;
}

/**
  Tests creating a SEL entry

  @param[in]  Context             UNUSED

  @retval  UNIT_TEST_PASSED             The Unit test has completed and the test
                                        case was successful.
  @retval  UNIT_TEST_ERROR_TEST_FAILED  A test case assertion has failed.
**/
UNIT_TEST_STATUS
EFIAPI
SelAddEntryTest (
  IN UNIT_TEST_CONTEXT  Context
  )
{
  EFI_STATUS                   Status;
  IPMI_ADD_SEL_ENTRY_REQUEST   SetRequest  = { 0 };
  IPMI_ADD_SEL_ENTRY_RESPONSE  SetResponse = { 0 };

  SetRequest.RecordData.RecordType   = 0xDC; // Random test type.
  SetRequest.RecordData.SensorType   = 0xA;
  SetRequest.RecordData.SensorNumber = 0xB;
  SetRequest.RecordData.OEMEvData1   = 0; // Leave unspecified for testing.
  SetRequest.RecordData.OEMEvData2   = 0xC;
  SetRequest.RecordData.OEMEvData3   = 0xD;

  Status = IpmiAddSelEntry (&SetRequest, &SetResponse);
  UT_ASSERT_NOT_EFI_ERROR (Status);
  UT_ASSERT_EQUAL (SetResponse.CompletionCode, 0);
  UT_ASSERT_NOT_EQUAL (SetResponse.RecordId, 0);
  UT_ASSERT_NOT_EQUAL (SetResponse.RecordId, 0xFFFF);
  DEBUG ((DEBUG_INFO, "Record ID: 0x%x\n", SetResponse.RecordId));

  return UNIT_TEST_PASSED;
}

//
// Test Orchestation
//

EFI_STATUS
EFIAPI
IpmiTestEntryPoint (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS                  Status;
  UNIT_TEST_FRAMEWORK_HANDLE  Framework;
  UNIT_TEST_SUITE_HANDLE      CommandTests;
  UNIT_TEST_SUITE_HANDLE      SelTests;

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
  // Populate the IPMI Command Test Suite.
  //
  Status = CreateUnitTestSuite (&CommandTests, Framework, "IPMI Command Tests", "IPMI.Commands", NULL, NULL);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "Failed in CreateUnitTestSuite for CommandTests\n"));
    Status = EFI_OUT_OF_RESOURCES;
    goto EXIT;
  }

  AddTestCase (CommandTests, "", "GetDeviceIdTest", GetDeviceIdTest, NULL, NULL, NULL);
  // AddTestCase (CommandTests, "", "WatchdogTimerTest", WatchdogTimerTest, NULL, NULL, NULL);

  //
  // Populate the SEL Test Suite.
  //
  Status = CreateUnitTestSuite (&SelTests, Framework, "IPMI SEL Tests", "IPMI.SEL", NULL, NULL);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "Failed in CreateUnitTestSuite for SelTests\n"));
    Status = EFI_OUT_OF_RESOURCES;
    goto EXIT;
  }

  AddTestCase (SelTests, "", "SelGetInfoTest", SelGetInfoTest, NULL, NULL, NULL);
  AddTestCase (SelTests, "", "SelGetTimeTest", SelGetTimeTest, NULL, NULL, NULL);
  AddTestCase (SelTests, "", "SelAddEntryTest", SelAddEntryTest, NULL, NULL, NULL);

  //
  // Execute the tests.
  //
  Status = RunAllTestSuites (Framework);

EXIT:
  if (Framework) {
    FreeUnitTestFramework (Framework);
  }

  return Status;
}
