/** @file
  Host based unit tests for the Generic IPMI modules.

  Copyright (c) Microsoft Corporation
  PDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Uefi.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/UnitTestLib.h>
#include <IndustryStandard/Ipmi.h>

#include <GenericIpmi.h>

#define UNIT_TEST_NAME     "Generic IPMI Unit Test"
#define UNIT_TEST_VERSION  "1.0"

IPMI_BMC_INSTANCE_DATA  mIpmiInstance;

/**
  Tests initializing the IPMI stack.

  @param[in]  Context             UNUSED

  @retval  UNIT_TEST_PASSED             The Unit test has completed and the test
                                        case was successful.
  @retval  UNIT_TEST_ERROR_TEST_FAILED  A test case assertion has failed.
**/
UNIT_TEST_STATUS
EFIAPI
TestIpmiInit (
  IN UNIT_TEST_CONTEXT  Context
  )

{
  EFI_STATUS  Status;

  ZeroMem (&mIpmiInstance, sizeof (mIpmiInstance));
  mIpmiInstance.Signature                       = SM_IPMI_BMC_SIGNATURE;
  mIpmiInstance.SlaveAddress                    = BMC_SLAVE_ADDRESS;
  mIpmiInstance.BmcStatus                       = BMC_NOTREADY;
  mIpmiInstance.IpmiTransport.IpmiSubmitCommand = IpmiSendCommand;
  mIpmiInstance.IpmiTransport.GetBmcStatus      = IpmiGetBmcStatus;

  Status = IpmiInitializeBmc (&mIpmiInstance);
  UT_ASSERT_STATUS_EQUAL (Status, EFI_SUCCESS);
  UT_ASSERT_EQUAL (mIpmiInstance.BmcStatus, BMC_OK);
  UT_ASSERT_EQUAL (mIpmiInstance.SoftErrorCount, 0);

  return UNIT_TEST_PASSED;
}

/**
  Initializes and configures the generic IPMI module tests.

  @retval  EFI_SUCCESS           All test cases were dispatched.
  @retval  EFI_OUT_OF_RESOURCES  There are not enough resources available to
                                 initialize the unit tests.
**/
EFI_STATUS
EFIAPI
GenericIpmiTestMain (
  VOID
  )
{
  EFI_STATUS                  Status;
  UNIT_TEST_FRAMEWORK_HANDLE  Framework;
  UNIT_TEST_SUITE_HANDLE      IpmiTests;

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
  Status = CreateUnitTestSuite (&IpmiTests, Framework, "Generic IPMI Module Tests", "IPMI.GENERIC", NULL, NULL);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "Failed in CreateUnitTestSuite for IpmiTests\n"));
    Status = EFI_OUT_OF_RESOURCES;
    goto EXIT;
  }

  AddTestCase (IpmiTests, "Tests initializing IPMI stack", "TestIpmiInit", TestIpmiInit, NULL, NULL, NULL);

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
  return GenericIpmiTestMain ();
}
