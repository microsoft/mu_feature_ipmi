/** @file TestIpmiPowerRestorePolicyHost.c

  Copyright (c) Microsoft Corporation.
  PDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>

#include <Uefi.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/UnitTestLib.h>

//
// Common header files
//
#include <PiPei.h>
#include <Library/DebugLib.h>
#include <Library/IpmiCommandLib.h>
#include <Library/PlatformPowerRestorePolicyConfigurationLib.h>

#define UNIT_TEST_NAME     "TestIpmiPowerRestorePolicyHost"
#define UNIT_TEST_VERSION  "1.0"

//
// Power Restore Policy configuration.
//
#define  POWER_RESTORE_POLICY_POWER_OFF   0x00
#define  POWER_RESTORE_POLICY_LAST_STATE  0x01
#define  POWER_RESTORE_POLICY_POWER_ON    0x02
#define  POWER_RESTORE_POLICY_NO_CHANGE   0x03  // "Set Power Restore Policy"
#define  POWER_RESTORE_POLICY_UNKNOWN     0x03  // "Get Chassis Status"

///
/// Global variables used in unit tests
///

extern
EFI_STATUS
EFIAPI
IpmiPowerRestorePolicyEntry (
  IN       EFI_PEI_FILE_HANDLE  FileHandle,
  IN CONST EFI_PEI_SERVICES     **PeiServices
  );

/**
  Verifies that the BMC will not be updated if the local UEFI Config value is invalid

  @param[in]  Context    not used

  @retval  UNIT_TEST_PASSED             The Unit test has completed and the test
                                        case was successful.
  @retval  UNIT_TEST_ERROR_TEST_FAILED  A test case assertion has failed.
**/
UNIT_TEST_STATUS
EFIAPI
TestNoUpdateToBmcWhenLocalInvalid (
  IN UNIT_TEST_CONTEXT  Context
  )
{
  EFI_PEI_FILE_HANDLE  FileHandle;
  EFI_PEI_SERVICES     *PeiServices;

  ZeroMem (
    &FileHandle,
    sizeof (FileHandle)
    );

  // IpmiGetChassisStatus returns current power state, restore power policy is bits [6:5] in current power state
  will_return (IpmiGetChassisStatus, POWER_RESTORE_POLICY_POWER_ON << 5);
  will_return (IpmiGetChassisStatus, EFI_DEVICE_ERROR);

  will_return (GetPowerRestorePolicy, POWER_RESTORE_POLICY_POWER_ON);
  will_return (GetPowerRestorePolicy, EFI_DEVICE_ERROR); // fail to retrieve local value will prevent update

  // Would be nice to explicitly check that a function isn't called,  test framework does not allow a value of zero
  // However, not specifying an expected value will fail if it is called.
  // expect_function_calls (IpmiSetPowerRestorePolicy, 0);

  EFI_STATUS  status = IpmiPowerRestorePolicyEntry (
                         FileHandle,
                         (CONST EFI_PEI_SERVICES **)&PeiServices
                         );

  UT_ASSERT_EQUAL (status, EFI_DEVICE_ERROR);

  return UNIT_TEST_PASSED;
}

/**
  Verifies that the BMC will not be updated if the local UEFI Config value is the same as the BMC
  Pick two different power policy values

  @param[in]  Context    not used

  @retval  UNIT_TEST_PASSED             The Unit test has completed and the test
                                        case was successful.
  @retval  UNIT_TEST_ERROR_TEST_FAILED  A test case assertion has failed.
**/
UNIT_TEST_STATUS
EFIAPI
TestNoUpdateToBmcWhenInSync (
  IN UNIT_TEST_CONTEXT  Context
  )
{
  EFI_PEI_FILE_HANDLE  FileHandle;
  EFI_PEI_SERVICES     *PeiServices;

  ZeroMem (
    &FileHandle,
    sizeof (FileHandle)
    );

  // Would be nice to explicitly check that a function isn't called,  test framework does not allow a value of zero
  // However, not specifying an expected value will fail if it is called.
  // expect_function_calls (IpmiSetPowerRestorePolicy, 0);

  // IpmiGetChassisStatus returns current power state, restore power policy is bits [6:5] in current power state
  will_return (IpmiGetChassisStatus, POWER_RESTORE_POLICY_POWER_OFF << 5);
  will_return (IpmiGetChassisStatus, EFI_SUCCESS);

  will_return (GetPowerRestorePolicy, POWER_RESTORE_POLICY_POWER_OFF);
  will_return (GetPowerRestorePolicy, EFI_SUCCESS); // fail to retrieve local value will prevent update

  EFI_STATUS  status = IpmiPowerRestorePolicyEntry (
                         FileHandle,
                         (CONST EFI_PEI_SERVICES **)&PeiServices
                         );

  UT_ASSERT_EQUAL (status, EFI_SUCCESS);

  will_return (IpmiGetChassisStatus, POWER_RESTORE_POLICY_LAST_STATE << 5);
  will_return (IpmiGetChassisStatus, EFI_SUCCESS);

  will_return (GetPowerRestorePolicy, POWER_RESTORE_POLICY_LAST_STATE);
  will_return (GetPowerRestorePolicy, EFI_SUCCESS); // fail to retrieve local value will prevent update

  status = IpmiPowerRestorePolicyEntry (
             FileHandle,
             (CONST EFI_PEI_SERVICES **)&PeiServices
             );

  UT_ASSERT_EQUAL (status, EFI_SUCCESS);

  return UNIT_TEST_PASSED;
}

/**
  Verifies that the BMC will not be updated if the local UEFI Config value is POWER_RESTORE_POLICY_NO_CHANGE

  @param[in]  Context    not used

  @retval  UNIT_TEST_PASSED             The Unit test has completed and the test
                                        case was successful.
  @retval  UNIT_TEST_ERROR_TEST_FAILED  A test case assertion has failed.
**/
UNIT_TEST_STATUS
EFIAPI
TestNoUpdateToBmcWhenLocalValueIsNoChange (
  IN UNIT_TEST_CONTEXT  Context
  )
{
  EFI_PEI_FILE_HANDLE  FileHandle;
  EFI_PEI_SERVICES     *PeiServices;

  ZeroMem (
    &FileHandle,
    sizeof (FileHandle)
    );

  // Would be nice to explicitly check that a function isn't called,  test framework does not allow a value of zero
  // However, not specifying an expected value will fail if it is called.
  // expect_function_calls (IpmiSetPowerRestorePolicy, 0);

  // IpmiGetChassisStatus returns current power state, restore power policy is bits [6:5] in current power state
  will_return (IpmiGetChassisStatus, POWER_RESTORE_POLICY_POWER_OFF << 5);
  will_return (IpmiGetChassisStatus, EFI_SUCCESS);

  will_return (GetPowerRestorePolicy, POWER_RESTORE_POLICY_NO_CHANGE);
  will_return (GetPowerRestorePolicy, EFI_SUCCESS); // fail to retrieve local value will prevent update

  EFI_STATUS  status = IpmiPowerRestorePolicyEntry (
                         FileHandle,
                         (CONST EFI_PEI_SERVICES **)&PeiServices
                         );

  UT_ASSERT_EQUAL (status, EFI_SUCCESS);

  return UNIT_TEST_PASSED;
}

/**
  Verifies that the BMC will not be updated if the local UEFI Config value is POWER_RESTORE_POLICY_NO_CHANGE

  @param[in]  Context    not used

  @retval  UNIT_TEST_PASSED             The Unit test has completed and the test
                                        case was successful.
  @retval  UNIT_TEST_ERROR_TEST_FAILED  A test case assertion has failed.
**/
UNIT_TEST_STATUS
EFIAPI
TestFailReturnedIfBmcUpdateFails (
  IN UNIT_TEST_CONTEXT  Context
  )
{
  EFI_PEI_FILE_HANDLE  FileHandle;
  EFI_PEI_SERVICES     *PeiServices;

  ZeroMem (
    &FileHandle,
    sizeof (FileHandle)
    );

  // IpmiGetChassisStatus returns current power state, restore power policy is bits [6:5] in current power state
  will_return (IpmiGetChassisStatus, POWER_RESTORE_POLICY_POWER_OFF << 5);
  will_return (IpmiGetChassisStatus, EFI_SUCCESS);

  will_return (GetPowerRestorePolicy, POWER_RESTORE_POLICY_POWER_ON);
  will_return (GetPowerRestorePolicy, EFI_SUCCESS); // fail to retrieve local value will prevent update

  expect_function_calls (IpmiSetPowerRestorePolicy, 1);
  expect_value (IpmiSetPowerRestorePolicy, ChassisControlRequest->PowerRestorePolicy.Uint8, POWER_RESTORE_POLICY_POWER_ON);

  will_return (IpmiSetPowerRestorePolicy, 0);                     // completion code success
  will_return (IpmiSetPowerRestorePolicy, EFI_INVALID_PARAMETER); //

  EFI_STATUS  status = IpmiPowerRestorePolicyEntry (
                         FileHandle,
                         (CONST EFI_PEI_SERVICES **)&PeiServices
                         );

  UT_ASSERT_EQUAL (status, EFI_INVALID_PARAMETER);

  // test that if IpmiSetPowerRestorePolicy returns success, but completion code is not, then will also fail
  will_return (IpmiGetChassisStatus, POWER_RESTORE_POLICY_POWER_ON << 5);
  will_return (IpmiGetChassisStatus, EFI_SUCCESS);

  will_return (GetPowerRestorePolicy, POWER_RESTORE_POLICY_POWER_OFF);
  will_return (GetPowerRestorePolicy, EFI_SUCCESS); // fail to retrieve local value will prevent update

  expect_function_calls (IpmiSetPowerRestorePolicy, 1);
  expect_value (IpmiSetPowerRestorePolicy, ChassisControlRequest->PowerRestorePolicy.Uint8, POWER_RESTORE_POLICY_POWER_OFF);

  will_return (IpmiSetPowerRestorePolicy, 2);           // completion code fail
  will_return (IpmiSetPowerRestorePolicy, EFI_SUCCESS); // fail to retrieve local value will prevent update

  status = IpmiPowerRestorePolicyEntry (
             FileHandle,
             (CONST EFI_PEI_SERVICES **)&PeiServices
             );

  UT_ASSERT_EQUAL (status, EFI_UNSUPPORTED);

  return UNIT_TEST_PASSED;
}

/**
  Verifies that the BMC is updated and success is returned

  @param[in]  Context    not used

  @retval  UNIT_TEST_PASSED             The Unit test has completed and the test
                                        case was successful.
  @retval  UNIT_TEST_ERROR_TEST_FAILED  A test case assertion has failed.
**/
UNIT_TEST_STATUS
EFIAPI
TestValidBmcUpdate (
  IN UNIT_TEST_CONTEXT  Context
  )
{
  EFI_PEI_FILE_HANDLE  FileHandle;
  EFI_PEI_SERVICES     *PeiServices;

  ZeroMem (
    &FileHandle,
    sizeof (FileHandle)
    );

  // test that if IpmiSetPowerRestorePolicy returns success, but completion code is not, then will also fail
  will_return (IpmiGetChassisStatus, POWER_RESTORE_POLICY_POWER_ON << 5);
  will_return (IpmiGetChassisStatus, EFI_SUCCESS);

  will_return (GetPowerRestorePolicy, POWER_RESTORE_POLICY_POWER_OFF);
  will_return (GetPowerRestorePolicy, EFI_SUCCESS); // fail to retrieve local value will prevent update

  expect_function_calls (IpmiSetPowerRestorePolicy, 1);
  expect_value (IpmiSetPowerRestorePolicy, ChassisControlRequest->PowerRestorePolicy.Uint8, POWER_RESTORE_POLICY_POWER_OFF);

  will_return (IpmiSetPowerRestorePolicy, 0);           // completion code success
  will_return (IpmiSetPowerRestorePolicy, EFI_SUCCESS); // fail to retrieve local value will prevent update

  EFI_STATUS  status = IpmiPowerRestorePolicyEntry (
                         FileHandle,
                         (CONST EFI_PEI_SERVICES **)&PeiServices
                         );

  UT_ASSERT_EQUAL (status, EFI_SUCCESS);

  return UNIT_TEST_PASSED;
}

/**
Initialize the unit test framework, suites and run test

 @retval  EFI_SUCCESS           All test cases were dispatched.
 @retval  EFI_OUT_OF_RESOURCES  There are not enough resources available to
                                initialize the unit tests.
**/
EFI_STATUS
EFIAPI
UefiTestMain (
  VOID
  )
{
  EFI_STATUS                  Status;
  UNIT_TEST_FRAMEWORK_HANDLE  Framework;
  UNIT_TEST_SUITE_HANDLE      TestIpmiPowerRestorePolicyHostTests;

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
  // Populate the TestIpmiPowerRestorePolicyHostTests Unit Test Suite.
  //
  Status = CreateUnitTestSuite (&TestIpmiPowerRestorePolicyHostTests, Framework, "TestIpmiPowerRestorePolicyHost Tests", "Simple.Math", NULL, NULL);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "Failed in CreateUnitTestSuite for TestIpmiPowerRestorePolicyHostTests\n"));
    Status = EFI_OUT_OF_RESOURCES;
    goto EXIT;
  }

  AddTestCase (TestIpmiPowerRestorePolicyHostTests, "Don't Update BMC when local config is invalid", "NoBmcUpdateInvalid", TestNoUpdateToBmcWhenLocalInvalid, NULL, NULL, NULL);
  AddTestCase (TestIpmiPowerRestorePolicyHostTests, "Don't Need to update BMC when already the same", "NoBmcUpdateInSync", TestNoUpdateToBmcWhenInSync, NULL, NULL, NULL);
  AddTestCase (TestIpmiPowerRestorePolicyHostTests, "Don't Need to update BMC when local value is no update", "NoBmcUpdateNoChange", TestNoUpdateToBmcWhenLocalValueIsNoChange, NULL, NULL, NULL);
  AddTestCase (TestIpmiPowerRestorePolicyHostTests, "Verify fail returned if BMC Update fails", "FailIfBmcUpdateFail", TestFailReturnedIfBmcUpdateFails, NULL, NULL, NULL);
  AddTestCase (TestIpmiPowerRestorePolicyHostTests, "Verify successful Bmc Update", "ValidBmcUpdate", TestValidBmcUpdate, NULL, NULL, NULL);

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

/**
  Standard POSIX C entry point for host based unit test execution.
**/
int
main (
  int   argc,
  char  *argv[]
  )
{
  return UefiTestMain ();
}
