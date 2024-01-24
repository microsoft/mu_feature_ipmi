/** @file
  Functional shell test for the IPMI Sel Protocol.

  Copyright (c) Microsoft Corporation.
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Uefi.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/UnitTestLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Protocol/IpmiSelProtocol.h>

#define UNIT_TEST_NAME     "SEL Protocol Test"
#define UNIT_TEST_VERSION  "1.0"

//
// Command Tests
//

/**
  Tests creating a SEL entry

  @param[in]  Context             UNUSED

  @retval  UNIT_TEST_PASSED             The Unit test has completed and the test
                                        case was successful.
  @retval  UNIT_TEST_ERROR_TEST_FAILED  A test case assertion has failed.
**/
UNIT_TEST_STATUS
EFIAPI
SelAddEntryTestBad (
  IN UNIT_TEST_CONTEXT  Context
  )
{
  IPMI_SEL_PROTOCOL  *SelProtocol;
  EFI_STATUS         Status;
  UINT8              ManId[] = { 'a', 'b', 'c' };
  UINT8              Data[]  = { 0, 1, 2, 3, 4, 5 };

  Status = gBS->LocateProtocol (
                  &gIpmiSelProtocolGuid,
                  NULL,
                  (VOID **)&SelProtocol
                  );

  UT_ASSERT_NOT_EFI_ERROR (Status);

  // Test bad record IDs.
  Status = SelProtocol->AddRecordEntry (NULL, 0, ManId, Data);
  UT_ASSERT_STATUS_EQUAL (Status, EFI_INVALID_PARAMETER);
  Status = SelProtocol->AddRecordEntry (NULL, 0xBF, ManId, Data);
  UT_ASSERT_STATUS_EQUAL (Status, EFI_INVALID_PARAMETER);
  Status = SelProtocol->AddRecordEntry (NULL, 0xE0, ManId, Data);
  UT_ASSERT_STATUS_EQUAL (Status, EFI_INVALID_PARAMETER);
  Status = SelProtocol->AddRecordEntry (NULL, 0xFF, ManId, Data);
  UT_ASSERT_STATUS_EQUAL (Status, EFI_INVALID_PARAMETER);

  // Test NULL pointers.
  Status = SelProtocol->AddRecordEntry (NULL, 0xC0, NULL, Data);
  UT_ASSERT_STATUS_EQUAL (Status, EFI_INVALID_PARAMETER);
  Status = SelProtocol->AddRecordEntry (NULL, 0xC0, ManId, NULL);
  UT_ASSERT_STATUS_EQUAL (Status, EFI_INVALID_PARAMETER);

  return UNIT_TEST_PASSED;
}

#define NUM_SEL_ENTRIES  (10)

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
  IPMI_SEL_PROTOCOL  *SelProtocol;
  EFI_STATUS         Status;
  UINT8              ManId[]                    = { 'a', 'b', 'c' };
  UINT8              Data[]                     = { 's', 'e', 'l', 't', '-', 0xFF };
  UINT16             RecordIds[NUM_SEL_ENTRIES] = { 0 };
  UINT16             NextId;
  UINT8              Index;
  SEL_RECORD         Record;

  Status = gBS->LocateProtocol (
                  &gIpmiSelProtocolGuid,
                  NULL,
                  (VOID **)&SelProtocol
                  );

  UT_ASSERT_NOT_EFI_ERROR (Status);

  // Add all the entries.
  for (Index = 0; Index < NUM_SEL_ENTRIES; Index++) {
    Data[5] = Index;
    Status  = SelProtocol->AddRecordEntry (&RecordIds[Index], 0xC0 + Index, ManId, Data);
    UT_ASSERT_NOT_EFI_ERROR (Status);
  }

  // Make sure the entries can be retrieved
  for (Index = 0; Index < NUM_SEL_ENTRIES; Index++) {
    Data[5] = Index;
    Status  = SelProtocol->GetRecordEntry (RecordIds[Index], &Record, &NextId);
    UT_ASSERT_NOT_EFI_ERROR (Status);
    UT_ASSERT_EQUAL (Record.RecordType, 0xC0 + Index);
    UT_ASSERT_NOT_EQUAL (NextId, RecordIds[Index]);
    UT_ASSERT_MEM_EQUAL ((VOID *)&Record.Record.Oem.ManufacturerId[0], (VOID *)&ManId[0], 3);
    Data[5] = Index;
    UT_ASSERT_MEM_EQUAL ((VOID *)&Record.Record.Oem.Data[0], (VOID *)&Data[0], 6);
  }

  return UNIT_TEST_PASSED;
}

//
// Test Orchestration
//

EFI_STATUS
EFIAPI
SelProtocolTestEntryPoint (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS                  Status;
  UNIT_TEST_FRAMEWORK_HANDLE  Framework;
  UNIT_TEST_SUITE_HANDLE      Suite;

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
  Status = CreateUnitTestSuite (&Suite, Framework, "IPMI Command Tests", "IPMI.Commands", NULL, NULL);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "Failed in CreateUnitTestSuite.\n"));
    Status = EFI_OUT_OF_RESOURCES;
    goto EXIT;
  }

  AddTestCase (Suite, "", "SelAddEntryTestBad", SelAddEntryTestBad, NULL, NULL, NULL);
  AddTestCase (Suite, "", "SelAddEntryTest", SelAddEntryTest, NULL, NULL, NULL);

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
