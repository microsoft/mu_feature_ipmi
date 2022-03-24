/** @file
  Host based unit tests for the SSIF transport library.

  Copyright (c) Microsoft Corporation
  PDX-License-Identifier: BSD-2-Clause-Patent

**/

#include "SsifUnitTest.h"

#define UNIT_TEST_NAME     "SSIF Unit Test"
#define UNIT_TEST_VERSION  "1.0"

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
  SmbusTestLibReset ();
}

/**
  Patterns a provided memory block for testing.

  @param[in]  Buffer    A pointer to the buffer to be patterned.
  @param[in]  Size      The size of ht buffer in  bytes.
**/
VOID
PatternBuffer (
  IN UINT8   *Buffer,
  IN UINT32  Size
  )
{
  UINT32         Index;
  static UINT32  Mod = 0;

  for (Index = 0; Index < Size; Index += 1) {
    Buffer[Index] = (UINT8)(Mod + Index);
  }

  Mod += 1;
}

/**
  Tests several sized messages write through the SSIF interface.

  @param[in]  Context             UNUSED

  @retval  UNIT_TEST_PASSED             The Unit test has completed and the test
                                        case was successful.
  @retval  UNIT_TEST_ERROR_TEST_FAILED  A test case assertion has failed.
**/
UNIT_TEST_STATUS
EFIAPI
TestSsifSimpleWrite (
  IN UNIT_TEST_CONTEXT  Context
  )

{
  UINT8       TestData[250];
  EFI_STATUS  Status;
  UINT32      Index;
  UINT8       TestSizes[] = {
    8,
    SSIF_MAX_WRITE_SIZE,
    SSIF_MAX_WRITE_SIZE + 1,
    250
  };

  for (Index = 0; Index < (sizeof (TestSizes) / sizeof (TestSizes[0])); Index += 1) {
    PatternBuffer (&TestData[0], TestSizes[Index]);
    Status = SendDataToBmcPort (0, NULL, (UINT8 *)&TestData[0], TestSizes[Index]);
    UT_ASSERT_STATUS_EQUAL (Status, EFI_SUCCESS);
    UT_ASSERT_TRUE (CheckTxBuffer ((UINT8 *)&TestData[0], TestSizes[Index]));
    SmbusTestLibReset ();
  }

  return UNIT_TEST_PASSED;
}

/**
  Tests several sized messages read through the SSIF interface.

  @param[in]  Context             UNUSED

  @retval  UNIT_TEST_PASSED             The Unit test has completed and the test
                                        case was successful.
  @retval  UNIT_TEST_ERROR_TEST_FAILED  A test case assertion has failed.
**/
UNIT_TEST_STATUS
EFIAPI
TestSsifSimpleRead (
  IN UNIT_TEST_CONTEXT  Context
  )

{
  UINT8       TestData[250];
  UINT8       ReadData[250];
  UINT8       ReadSize;
  EFI_STATUS  Status;
  UINT32      Index;
  UINT8       TestSizes[] = {
    8,
    SSIF_MAX_READ_SIZE,
    SSIF_MAX_READ_SIZE + 1,
    250
  };

  for (Index = 0; Index < (sizeof (TestSizes) / sizeof (TestSizes[0])); Index += 1) {
    PatternBuffer (&TestData[0], TestSizes[Index]);
    SetRxBuffer (&TestData[0], TestSizes[Index]);
    ReadSize = sizeof (ReadData);
    Status   = ReceiveBmcDataFromPort (0, NULL, (UINT8 *)&ReadData[0], &ReadSize);
    UT_ASSERT_STATUS_EQUAL (Status, EFI_SUCCESS);
    UT_ASSERT_EQUAL (ReadSize, TestSizes[Index]);
    UT_ASSERT_MEM_EQUAL (&TestData[0], &ReadData[0], ReadSize);
    SmbusTestLibReset ();
  }

  return UNIT_TEST_PASSED;
}

/**
  Initializes and configures the SSIF transport tests.

  @retval  EFI_SUCCESS           All test cases were dispatched.
  @retval  EFI_OUT_OF_RESOURCES  There are not enough resources available to
                                 initialize the unit tests.
**/
EFI_STATUS
EFIAPI
SsifTestMain (
  VOID
  )
{
  EFI_STATUS                  Status;
  UNIT_TEST_FRAMEWORK_HANDLE  Framework;
  UNIT_TEST_SUITE_HANDLE      SsifTests;

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
  // Populate the SimpleMathTests Unit Test Suite.
  //
  Status = CreateUnitTestSuite (&SsifTests, Framework, "SSIF Transport Tests", "IPMI.SSIF", NULL, NULL);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "Failed in CreateUnitTestSuite for SsifTests\n"));
    Status = EFI_OUT_OF_RESOURCES;
    goto EXIT;
  }

  AddTestCase (SsifTests, "Tests writing messages of different sizes through SSIF", "TestSsifSimpleWrite", TestSsifSimpleWrite, NULL, ResetTestState, NULL);
  AddTestCase (SsifTests, "Tests reading messages of different sizes through SSIF", "TestSsifSimpleRead", TestSsifSimpleRead, NULL, ResetTestState, NULL);

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
  return SsifTestMain ();
}
