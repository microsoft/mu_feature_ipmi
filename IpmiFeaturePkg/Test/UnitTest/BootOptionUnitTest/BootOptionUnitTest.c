/** @file
  Host based unit tests for the Boot Option library.

  Copyright (c) Microsoft Corporation
  PDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Uefi.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/UnitTestLib.h>
#include <IndustryStandard/Ipmi.h>
#include <Library/IpmiBootOptionLib.h>

#define UNIT_TEST_NAME     "Boot Option Unit Test"
#define UNIT_TEST_VERSION  "1.0"

//
// Useful definitions for the test.
//

#define INVALID_SELECTOR  (0xFFFF)
#define BIOS_ACKED_VALUE  (0xFF & ~BOOT_OPTION_HANDLED_BY_BIOS)

//
// Hooks into the mock library for testing.
//

extern IPMI_BOOT_OPTIONS_RESPONSE_PARAMETER_5  mBootFlags;
extern UINT8                                   mBootOptionAcks;

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
  ZeroMem (&mBootFlags, sizeof (mBootFlags));
  mBootOptionAcks = 0xFF;
}

/**
  Tests retrieving the IPMI boot options for a clear boot flags entry.

  @param[in]  Context             UNUSED

  @retval  UNIT_TEST_PASSED             The Unit test has completed and the test
                                        case was successful.
  @retval  UNIT_TEST_ERROR_TEST_FAILED  A test case assertion has failed.
**/
UNIT_TEST_STATUS
EFIAPI
TestGetBootOptionNone (
  IN UNIT_TEST_CONTEXT  Context
  )

{
  IPMI_BOOT_OPTION_SELECTOR  Selector;
  EFI_STATUS                 Status;

  Selector = INVALID_SELECTOR;
  Status   = IpmiGetBootOption (&Selector);
  UT_ASSERT_STATUS_EQUAL (Status, EFI_SUCCESS);
  UT_ASSERT_EQUAL (Selector, BootNone);
  UT_ASSERT_EQUAL (mBootOptionAcks, BIOS_ACKED_VALUE);

  return UNIT_TEST_PASSED;
}

/**
  Tests retrieving the IPMI boot options for a invalid boot flags entry.

  @param[in]  Context             UNUSED

  @retval  UNIT_TEST_PASSED             The Unit test has completed and the test
                                        case was successful.
  @retval  UNIT_TEST_ERROR_TEST_FAILED  A test case assertion has failed.
**/
UNIT_TEST_STATUS
EFIAPI
TestGetBootOptionInvalid (
  IN UNIT_TEST_CONTEXT  Context
  )

{
  IPMI_BOOT_OPTION_SELECTOR  Selector;
  EFI_STATUS                 Status;

  mBootFlags.Data1.Bits.BootFlagValid      = 0;
  mBootFlags.Data2.Bits.BootDeviceSelector = BootCd;
  Selector                                 = INVALID_SELECTOR;
  Status                                   = IpmiGetBootOption (&Selector);
  UT_ASSERT_STATUS_EQUAL (Status, EFI_SUCCESS);
  UT_ASSERT_EQUAL (Selector, BootNone);
  UT_ASSERT_EQUAL (mBootOptionAcks, BIOS_ACKED_VALUE);

  return UNIT_TEST_PASSED;
}

/**
  Tests retrieving the IPMI boot options for a valid boot flags entry.

  @param[in]  Context             UNUSED

  @retval  UNIT_TEST_PASSED             The Unit test has completed and the test
                                        case was successful.
  @retval  UNIT_TEST_ERROR_TEST_FAILED  A test case assertion has failed.
**/
UNIT_TEST_STATUS
EFIAPI
TestGetBootOptionNoPersistance (
  IN UNIT_TEST_CONTEXT  Context
  )

{
  IPMI_BOOT_OPTION_SELECTOR  Selector;
  EFI_STATUS                 Status;

  mBootFlags.Data1.Bits.BootFlagValid      = 1;
  mBootFlags.Data2.Bits.BootDeviceSelector = BootCd;
  Selector                                 = INVALID_SELECTOR;
  Status                                   = IpmiGetBootOption (&Selector);
  UT_ASSERT_STATUS_EQUAL (Status, EFI_SUCCESS);
  UT_ASSERT_EQUAL (Selector, BootCd);

  //
  // Check that the value was cleared.
  //

  Status = IpmiGetBootOption (&Selector);
  UT_ASSERT_STATUS_EQUAL (Status, EFI_SUCCESS);
  UT_ASSERT_EQUAL (Selector, BootNone);
  UT_ASSERT_EQUAL (mBootOptionAcks, BIOS_ACKED_VALUE);

  return UNIT_TEST_PASSED;
}

/**
  Tests retrieving the IPMI boot options with various persistence settings.

  @param[in]  Context             UNUSED

  @retval  UNIT_TEST_PASSED             The Unit test has completed and the test
                                        case was successful.
  @retval  UNIT_TEST_ERROR_TEST_FAILED  A test case assertion has failed.
**/
UNIT_TEST_STATUS
EFIAPI
TestGetBootOptionPersistance (
  IN UNIT_TEST_CONTEXT  Context
  )

{
  IPMI_BOOT_OPTION_SELECTOR  Selector;
  EFI_STATUS                 Status;
  UINT32                     Index;

  mBootFlags.Data1.Bits.BootFlagValid      = 1;
  mBootFlags.Data1.Bits.PersistentOptions  = 1;
  mBootFlags.Data2.Bits.BootDeviceSelector = BootCd;

  //
  // Try many times, should not be cleared.
  //

  for (Index = 0; Index < 5; Index++) {
    Selector = INVALID_SELECTOR;
    Status   = IpmiGetBootOption (&Selector);
    UT_ASSERT_STATUS_EQUAL (Status, EFI_SUCCESS);
    UT_ASSERT_EQUAL (Selector, BootCd);
  }

  UT_ASSERT_EQUAL (mBootOptionAcks, BIOS_ACKED_VALUE);
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
BootOptionTestMain (
  VOID
  )
{
  EFI_STATUS                  Status;
  UNIT_TEST_FRAMEWORK_HANDLE  Framework;
  UNIT_TEST_SUITE_HANDLE      BootOptionTests;

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
  Status = CreateUnitTestSuite (&BootOptionTests, Framework, "Boot Option Library Tests", "IPMI.BOOT_OPTION", NULL, NULL);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "Failed in CreateUnitTestSuite for BootOptionTests\n"));
    Status = EFI_OUT_OF_RESOURCES;
    goto EXIT;
  }

  AddTestCase (BootOptionTests, "Tests retrieving empty boot flags", "TestGetBootOptionNone", TestGetBootOptionNone, NULL, ResetTestState, NULL);
  AddTestCase (BootOptionTests, "Tests retrieving invalid boot flags", "TestGetBootOptionInvalid", TestGetBootOptionInvalid, NULL, ResetTestState, NULL);
  AddTestCase (BootOptionTests, "Tests retrieving boot flags", "TestGetBootOptionNoPersistance", TestGetBootOptionNoPersistance, NULL, ResetTestState, NULL);
  AddTestCase (BootOptionTests, "Tests retrieving persistent boot flags", "TestGetBootOptionPersistance", TestGetBootOptionPersistance, NULL, ResetTestState, NULL);

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
  return BootOptionTestMain ();
}
