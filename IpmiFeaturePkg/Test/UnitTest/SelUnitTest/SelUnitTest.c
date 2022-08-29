/** @file
  Host based unit tests for the SSIF transport library.

  Copyright (c) Microsoft Corporation
  PDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Uefi.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/UnitTestLib.h>
#include <Library/IpmiSelLib.h>
#include <IndustryStandard/Ipmi.h>

#define UNIT_TEST_NAME     "SEL Unit Test"
#define UNIT_TEST_VERSION  "1.0"

/**
  Tests retrieving the SEL information.

  @param[in]  Context             UNUSED

  @retval  UNIT_TEST_PASSED             The Unit test has completed and the test
                                        case was successful.
  @retval  UNIT_TEST_ERROR_TEST_FAILED  A test case assertion has failed.
**/
UNIT_TEST_STATUS
EFIAPI
TestSelGetInfo (
  IN UNIT_TEST_CONTEXT  Context
  )

{
  SEL_INFO    SelInfo = { 0 };
  EFI_STATUS  Status;

  Status = SelGetInfo (&SelInfo);
  UT_ASSERT_STATUS_EQUAL (Status, EFI_SUCCESS);
  UT_ASSERT_TRUE (SelInfo.Version > 0);

  return UNIT_TEST_PASSED;
}

/**
  Tests adding a system SEL entry.

  @param[in]  Context             UNUSED

  @retval  UNIT_TEST_PASSED             The Unit test has completed and the test
                                        case was successful.
  @retval  UNIT_TEST_ERROR_TEST_FAILED  A test case assertion has failed.
**/
UNIT_TEST_STATUS
EFIAPI
TestSelAddSystemEntry (
  IN UNIT_TEST_CONTEXT  Context
  )

{
  EFI_STATUS  Status;
  SEL_RECORD  Record;
  UINT16      RecordId;

  Status = SelAddSystemEntry (&RecordId, 1, 2, 3, 4, 5, 6);
  UT_ASSERT_STATUS_EQUAL (Status, EFI_SUCCESS);

  Status = SelGetEntry (RecordId, &Record, NULL);
  UT_ASSERT_STATUS_EQUAL (Status, EFI_SUCCESS);
  UT_ASSERT_EQUAL (Record.RecordId, RecordId);
  UT_ASSERT_EQUAL (Record.RecordType, IPMI_SEL_SYSTEM_RECORD);
  UT_ASSERT_EQUAL (Record.Record.System.SensorType, 1);
  UT_ASSERT_EQUAL (Record.Record.System.SensorNumber, 2);
  UT_ASSERT_EQUAL (Record.Record.System.EventDirType, 3);
  UT_ASSERT_EQUAL (Record.Record.System.Data[0], 4);
  UT_ASSERT_EQUAL (Record.Record.System.Data[1], 5);
  UT_ASSERT_EQUAL (Record.Record.System.Data[2], 6);

  return UNIT_TEST_PASSED;
}

/**
  Tests adding a OEM SEL entry.

  @param[in]  Context             UNUSED

  @retval  UNIT_TEST_PASSED             The Unit test has completed and the test
                                        case was successful.
  @retval  UNIT_TEST_ERROR_TEST_FAILED  A test case assertion has failed.
**/
UNIT_TEST_STATUS
EFIAPI
TestSelAddOemEntry (
  IN UNIT_TEST_CONTEXT  Context
  )

{
  EFI_STATUS  Status;
  SEL_RECORD  Record;
  UINT16      RecordId;
  UINT8       Data[6] = { 0x10, 0x11, 0x12, 0x13, 0x14, 0x15 };

  Status = SelAddOemEntry (NULL, IPMI_SEL_SYSTEM_RECORD, Data);
  UT_ASSERT_STATUS_EQUAL (Status, EFI_INVALID_PARAMETER);

  Status = SelAddOemEntry (&RecordId, IPMI_SEL_OEM_TIME_STAMP_RECORD_START, Data);
  UT_ASSERT_STATUS_EQUAL (Status, EFI_SUCCESS);

  Status = SelGetEntry (RecordId, &Record, NULL);
  UT_ASSERT_STATUS_EQUAL (Status, EFI_SUCCESS);
  UT_ASSERT_EQUAL (Record.RecordId, RecordId);
  UT_ASSERT_EQUAL (Record.RecordType, IPMI_SEL_OEM_TIME_STAMP_RECORD_START);
  UT_ASSERT_MEM_EQUAL (Data, Record.Record.Oem.Data, sizeof (Data));

  return UNIT_TEST_PASSED;
}

/**
  Tests adding a OEM non-timestamped SEL entry.

  @param[in]  Context             UNUSED

  @retval  UNIT_TEST_PASSED             The Unit test has completed and the test
                                        case was successful.
  @retval  UNIT_TEST_ERROR_TEST_FAILED  A test case assertion has failed.
**/
UNIT_TEST_STATUS
EFIAPI
TestSelAddOemNoTimestampEntry (
  IN UNIT_TEST_CONTEXT  Context
  )

{
  EFI_STATUS  Status;
  SEL_RECORD  Record;
  UINT16      RecordId;
  UINT8       Data[13] = {
    0x10, 0x11, 0x12, 0x13, 0x14,
    0x15, 0x16, 0x17, 0x18, 0x19,
    0x1A, 0x1B, 0x1C
  };

  Status = SelAddOemEntryNoTimestamp (NULL, IPMI_SEL_SYSTEM_RECORD, Data);
  UT_ASSERT_STATUS_EQUAL (Status, EFI_INVALID_PARAMETER);

  Status = SelAddOemEntryNoTimestamp (&RecordId, IPMI_SEL_OEM_NO_TIME_STAMP_RECORD_START, Data);
  UT_ASSERT_STATUS_EQUAL (Status, EFI_SUCCESS);

  Status = SelGetEntry (RecordId, &Record, NULL);
  UT_ASSERT_STATUS_EQUAL (Status, EFI_SUCCESS);
  UT_ASSERT_EQUAL (Record.RecordId, RecordId);
  UT_ASSERT_EQUAL (Record.RecordType, IPMI_SEL_OEM_NO_TIME_STAMP_RECORD_START);
  UT_ASSERT_MEM_EQUAL (Data, Record.Record.OemNonTimestamped.Data, sizeof (Data));

  return UNIT_TEST_PASSED;
}

/**
  Tests getting/setting SEL time.

  @param[in]  Context             UNUSED

  @retval  UNIT_TEST_PASSED             The Unit test has completed and the test
                                        case was successful.
  @retval  UNIT_TEST_ERROR_TEST_FAILED  A test case assertion has failed.
**/
UNIT_TEST_STATUS
EFIAPI
TestSelTime (
  IN UNIT_TEST_CONTEXT  Context
  )

{
  EFI_STATUS  Status;
  UINT32      Time;

  Time   = 100;
  Status = SelSetTime (Time);
  UT_ASSERT_STATUS_EQUAL (Status, EFI_SUCCESS);

  Time   = 0;
  Status = SelGetTime (&Time);
  UT_ASSERT_STATUS_EQUAL (Status, EFI_SUCCESS);
  UT_ASSERT_TRUE (Time >= 100);

  return UNIT_TEST_PASSED;
}

/**
  Initializes and configures the SEL library tests.

  @retval  EFI_SUCCESS           All test cases were dispatched.
  @retval  EFI_OUT_OF_RESOURCES  There are not enough resources available to
                                 initialize the unit tests.
**/
EFI_STATUS
EFIAPI
SelTestMain (
  VOID
  )
{
  EFI_STATUS                  Status;
  UNIT_TEST_FRAMEWORK_HANDLE  Framework;
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
  // Populate the SEL Unit Test Suite.
  //
  Status = CreateUnitTestSuite (&SelTests, Framework, "SEL Library Tests", "IPMI.SEL", NULL, NULL);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "Failed in CreateUnitTestSuite for SelTests\n"));
    Status = EFI_OUT_OF_RESOURCES;
    goto EXIT;
  }

  AddTestCase (SelTests, "Tests retrieving the SEL information", "TestSelGetInfo", TestSelGetInfo, NULL, NULL, NULL);
  AddTestCase (SelTests, "Tests adding a system event to the SEL", "TestSelAddSystemEntry", TestSelAddSystemEntry, NULL, NULL, NULL);
  AddTestCase (SelTests, "Tests adding an OEM event to the SEL", "TestSelAddOemEntry", TestSelAddOemEntry, NULL, NULL, NULL);
  AddTestCase (SelTests, "Tests adding an OEM non-timestamped event to the SEL", "TestSelAddOemNoTimestampEntry", TestSelAddOemNoTimestampEntry, NULL, NULL, NULL);
  AddTestCase (SelTests, "Tests setting/getting SEL time", "TestSelTime", TestSelTime, NULL, NULL, NULL);

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
  return SelTestMain ();
}
