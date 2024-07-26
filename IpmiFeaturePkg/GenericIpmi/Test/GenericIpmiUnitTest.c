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
  Tests getting the BMC status.

  @param[in]  Context             UNUSED

  @retval  UNIT_TEST_PASSED             The Unit test has completed and the test
                                        case was successful.
  @retval  UNIT_TEST_ERROR_TEST_FAILED  A test case assertion has failed.
**/
UNIT_TEST_STATUS
EFIAPI
TestIpmiGetBmcStatus (
  IN UNIT_TEST_CONTEXT  Context
  )

{
  EFI_STATUS      Status;
  BMC_STATUS      BmcStatus;
  SM_COM_ADDRESS  ComAddress;

  BmcStatus = BMC_HARDFAIL;
  ZeroMem (&ComAddress, sizeof (ComAddress));

  Status = mIpmiInstance.IpmiTransport.GetBmcStatus (
                                         &mIpmiInstance.IpmiTransport,
                                         &BmcStatus,
                                         &ComAddress
                                         );

  UT_ASSERT_STATUS_EQUAL (Status, EFI_SUCCESS);
  UT_ASSERT_EQUAL (BmcStatus, BMC_OK);
  UT_ASSERT_EQUAL (ComAddress.Address.BmcAddress.SlaveAddress, BMC_SLAVE_ADDRESS);

  return UNIT_TEST_PASSED;
}

/**
  Tests send a the IPMI command and getting a response.

  @param[in]  Context             UNUSED

  @retval  UNIT_TEST_PASSED             The Unit test has completed and the test
                                        case was successful.
  @retval  UNIT_TEST_ERROR_TEST_FAILED  A test case assertion has failed.
**/
UNIT_TEST_STATUS
EFIAPI
TestIpmiCommand (
  IN UNIT_TEST_CONTEXT  Context
  )

{
  EFI_STATUS               Status;
  IPMI_CLEAR_SEL_REQUEST   Request;
  IPMI_CLEAR_SEL_RESPONSE  Response;
  UINT32                   DataSize;

  //
  // Use the Clear SEL command since it takes both an input and output.
  //

  ZeroMem (&Request, sizeof (Request));
  ZeroMem (&Response, sizeof (Response));
  Request.AscC  = 'C';
  Request.AscL  = 'L';
  Request.AscR  = 'R';
  Request.Erase = IPMI_CLEAR_SEL_REQUEST_INITIALIZE_ERASE;

  ZeroMem (&Response, sizeof (Response));
  DataSize = sizeof (Response);

  Status = mIpmiInstance.IpmiTransport.IpmiSubmitCommand (
                                         &mIpmiInstance.IpmiTransport,
                                         IPMI_NETFN_STORAGE,
                                         0,
                                         IPMI_STORAGE_CLEAR_SEL,
                                         (UINT8 *)&Request,
                                         sizeof (Request),
                                         (UINT8 *)&Response,
                                         &DataSize
                                         );

  UT_ASSERT_STATUS_EQUAL (Status, EFI_SUCCESS);
  UT_ASSERT_EQUAL (DataSize, sizeof (Response));
  UT_ASSERT_EQUAL (Response.CompletionCode, IPMI_COMP_CODE_NORMAL);

  return UNIT_TEST_PASSED;
}

/**
  Tests send a the IPMI command with too small of a response buffer.

  @param[in]  Context             UNUSED

  @retval  UNIT_TEST_PASSED             The Unit test has completed and the test
                                        case was successful.
  @retval  UNIT_TEST_ERROR_TEST_FAILED  A test case assertion has failed.
**/
UNIT_TEST_STATUS
EFIAPI
TestIpmiBufferTooSmall (
  IN UNIT_TEST_CONTEXT  Context
  )

{
  EFI_STATUS                   Status;
  IPMI_GET_DEVICE_ID_RESPONSE  ZeroResponse;
  IPMI_GET_DEVICE_ID_RESPONSE  Response;
  UINT32                       DataSize;

  //
  // Test a 0 length buffer while passing a Non-Null Response Buffer
  //

  DataSize = 0;
  ZeroMem (&Response, sizeof (Response));

  Status = mIpmiInstance.IpmiTransport.IpmiSubmitCommand (
                                         &mIpmiInstance.IpmiTransport,
                                         IPMI_NETFN_APP,
                                         0,
                                         IPMI_APP_GET_DEVICE_ID,
                                         NULL,
                                         0,
                                         (UINT8 *)&Response,
                                         &DataSize
                                         );

  UT_ASSERT_STATUS_EQUAL (Status, EFI_BUFFER_TOO_SMALL);

  //
  // Test a 4 byte length buffer while passing NULL
  //

  DataSize = 4;

  Status = mIpmiInstance.IpmiTransport.IpmiSubmitCommand (
                                         &mIpmiInstance.IpmiTransport,
                                         IPMI_NETFN_APP,
                                         0,
                                         IPMI_APP_GET_DEVICE_ID,
                                         NULL,
                                         0,
                                         NULL,
                                         &DataSize
                                         );

  UT_ASSERT_STATUS_EQUAL (Status, EFI_INVALID_PARAMETER);

  //
  // Test an inadequate buffer size.
  //

  ZeroMem (&ZeroResponse, sizeof (ZeroResponse));
  ZeroMem (&Response, sizeof (Response));
  DataSize = 4;
  UT_ASSERT_TRUE (DataSize < sizeof (IPMI_GET_DEVICE_ID_RESPONSE))

  Status = mIpmiInstance.IpmiTransport.IpmiSubmitCommand (
                                         &mIpmiInstance.IpmiTransport,
                                         IPMI_NETFN_APP,
                                         0,
                                         IPMI_APP_GET_DEVICE_ID,
                                         NULL,
                                         0,
                                         (UINT8 *)&Response,
                                         &DataSize
                                         );

  UT_ASSERT_STATUS_EQUAL (Status, EFI_BUFFER_TOO_SMALL);
  UT_ASSERT_EQUAL (DataSize, sizeof (IPMI_GET_DEVICE_ID_RESPONSE));
  UT_ASSERT_MEM_EQUAL (&Response, &ZeroResponse, sizeof (Response));

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
  AddTestCase (IpmiTests, "Tests getting the BMC status", "TestIpmiGetBmcStatus", TestIpmiGetBmcStatus, NULL, NULL, NULL);
  AddTestCase (IpmiTests, "Tests sending and IPMI command", "TestIpmiCommand", TestIpmiCommand, NULL, NULL, NULL);
  AddTestCase (IpmiTests, "Tests sending a command with a undersized response buffer", "TestIpmiBufferTooSmall", TestIpmiBufferTooSmall, NULL, NULL, NULL);

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
