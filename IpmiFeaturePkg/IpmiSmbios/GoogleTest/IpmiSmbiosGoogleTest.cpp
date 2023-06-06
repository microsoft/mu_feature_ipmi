/** @file
  Unit tests for the IpmiSmbios driver using google tests

  Copyright (c) Microsoft Corporation.
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/
#include <Library/GoogleTestLib.h>
#include <Library/FunctionMockLib.h>

extern "C" {
#include <Uefi.h>
#include <IndustryStandard/SmBios.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Protocol/Smbios.h>
#include <IpmiInterface.h>

extern SMBIOS_TABLE_TYPE38  mSmbiosTableType38;

//
// Interfaces to be tested
//
EFI_STATUS
EFIAPI
CreateIpmiSmbiosType38 (
  VOID
  );
}

using namespace testing;

//
// Declarations to handle usage of the UefiBootServiceTableLib by creating mock
//
struct MockUefiBootServicesTableLib {
  MOCK_INTERFACE_DECLARATION (MockUefiBootServicesTableLib);

  MOCK_FUNCTION_DECLARATION (
    EFI_STATUS,
    gBS_LocateProtocol,
    (IN  EFI_GUID  *Protocol,
     IN  VOID      *Registration  OPTIONAL,
     OUT VOID      **Interface)
    );
};

MOCK_INTERFACE_DEFINITION (MockUefiBootServicesTableLib);
MOCK_FUNCTION_DEFINITION (MockUefiBootServicesTableLib, gBS_LocateProtocol, 3, EFIAPI);

static EFI_BOOT_SERVICES  LocalBs;
EFI_BOOT_SERVICES         *gBS = &LocalBs;

//
// Declarations to handle usage of the IPMI_TRANSPORT by creating mock
//
struct MockIpmiBaseLib {
  MOCK_INTERFACE_DECLARATION (MockIpmiBaseLib);

  MOCK_FUNCTION_DECLARATION (
    EFI_STATUS,
    IpmiSubmitCommand,
    (IN UINT8    NetFunction,
    IN UINT8    Command,
    IN UINT8    *CommandData,
    IN UINT32   CommandDataSize,
    OUT UINT8   *ResponseData,
    OUT UINT32  *ResponseDataSize)
    );
};

MOCK_INTERFACE_DEFINITION (MockIpmiBaseLib);
MOCK_FUNCTION_DEFINITION (MockIpmiBaseLib, IpmiSubmitCommand, 6, EFIAPI);

//
// Declarations to handle usage of the EFI_SMBIOS_PROTOCOL by creating mock
//
struct MockSmbiosProtocol {
  MOCK_INTERFACE_DECLARATION (MockSmbiosProtocol);

  MOCK_FUNCTION_DECLARATION (
    EFI_STATUS,
    MockSmbiosAdd,
    (IN CONST      EFI_SMBIOS_PROTOCOL     *This,
     IN            EFI_HANDLE              ProducerHandle OPTIONAL,
     IN OUT        EFI_SMBIOS_HANDLE       *SmbiosHandle,
     IN            EFI_SMBIOS_TABLE_HEADER *Record)
    );
};

MOCK_INTERFACE_DEFINITION (MockSmbiosProtocol);
MOCK_FUNCTION_DEFINITION (MockSmbiosProtocol, MockSmbiosAdd, 4, EFIAPI);

static EFI_SMBIOS_PROTOCOL  LocalSmbiosProtocol;

class MockIpmiSmbios : public  Test {
protected:
BOOLEAN ReturnValue;
EFI_STATUS Status;
MockUefiBootServicesTableLib UefiBootServicesTableLib;
MockIpmiBaseLib IpmiBaseLib;
MockSmbiosProtocol EFI_SMBIOS_PROTOCOL;
virtual void
SetUp (
  )
{
  LocalBs.LocateProtocol               = gBS_LocateProtocol;
  LocalSmbiosProtocol.Add              = MockSmbiosAdd;
}
};

TEST_F (MockIpmiSmbios, VerifyCreateIpmiSmbiosType38TestCase) {
  EXPECT_CALL (UefiBootServicesTableLib, gBS_LocateProtocol (_, _, _))
    .WillOnce (
       DoAll (
         SetArgPointee<2>(&LocalSmbiosProtocol),
         Return (EFI_SUCCESS)
         )
       );
  EXPECT_CALL (IpmiBaseLib, IpmiSubmitCommand)
    .WillOnce (Return (EFI_SUCCESS));
  EXPECT_CALL (EFI_SMBIOS_PROTOCOL, MockSmbiosAdd)
    .WillOnce (Return (EFI_SUCCESS));

  Status = CreateIpmiSmbiosType38 ();
  EXPECT_EQ (Status, EFI_SUCCESS);
  EXPECT_EQ (mSmbiosTableType38.Hdr.Type, EFI_SMBIOS_TYPE_IPMI_DEVICE_INFORMATION);
  EXPECT_EQ (mSmbiosTableType38.Hdr.Length, sizeof (SMBIOS_TABLE_TYPE38));
  EXPECT_EQ (mSmbiosTableType38.InterfaceType, FixedPcdGet8 (PcdIpmiInterfaceType));
  EXPECT_EQ (mSmbiosTableType38.I2CSlaveAddress, FixedPcdGet8 (PcdSmbiosTablesIpmiI2CSlaveAddress));
  EXPECT_EQ (mSmbiosTableType38.NVStorageDeviceAddress, FixedPcdGet8 (PcdSmbiosTablesIpmiNVStorageDeviceAddress));
  EXPECT_EQ (mSmbiosTableType38.InterruptNumber, FixedPcdGet8 (PcdSmbiosTablesIpmiInterruptNumber));
  EXPECT_EQ (mSmbiosTableType38.InterfaceType, FixedPcdGet8 (PcdIpmiInterfaceType));
  // Mock IPMI Base Address 0xCA2 IO Space.
  EXPECT_EQ (mSmbiosTableType38.BaseAddress, FixedPcdGet16 (PcdIpmiIoBaseAddress) | BIT0);
  EXPECT_EQ (
    mSmbiosTableType38.BaseAddressModifier_InterruptInfo,
    ((PcdGet16 (PcdIpmiIoBaseAddress) & BIT0) << 4) |
    ((PcdGet8 (PcdSmbiosTablesIpmiInterruptInfo) & 1) << 3) |
    ((PcdGet8 (PcdSmbiosTablesIpmiInterruptPolarity) & 1) << 1) |
    (PcdGet8 (PcdSmbiosTablesIpmiInterruptTriggerMode) & 1)
  );
}

TEST_F (MockIpmiSmbios, VerifyCreateIpmiSmbiosType38LocateProtocolFailTestCase) {
  EXPECT_CALL (UefiBootServicesTableLib, gBS_LocateProtocol)
    .WillOnce (Return (EFI_NOT_FOUND));

  Status = CreateIpmiSmbiosType38 ();
  EXPECT_EQ (Status, EFI_NOT_FOUND);
}

TEST_F (MockIpmiSmbios, VerifyCreateIpmiSmbiosType38IpmiSubmitCommandFailTestCase) {
  EXPECT_CALL (UefiBootServicesTableLib, gBS_LocateProtocol (_, _, _))
    .WillOnce (
       DoAll (
         SetArgPointee<2>(&LocalSmbiosProtocol),
         Return (EFI_SUCCESS)
         )
       );
  EXPECT_CALL (IpmiBaseLib, IpmiSubmitCommand)
    .WillOnce (Return (EFI_NOT_READY));

  Status = CreateIpmiSmbiosType38 ();
  EXPECT_EQ (Status, EFI_NOT_READY);
}

TEST_F (MockIpmiSmbios, VerifyCreateIpmiSmbiosType38SmbiosAddFailTestCase) {
  EXPECT_CALL (UefiBootServicesTableLib, gBS_LocateProtocol (_, _, _))
    .WillOnce (
       DoAll (
         SetArgPointee<2>(&LocalSmbiosProtocol),
         Return (EFI_SUCCESS)
         )
       );
  EXPECT_CALL (IpmiBaseLib, IpmiSubmitCommand)
    .WillOnce (Return (EFI_SUCCESS));
  EXPECT_CALL (EFI_SMBIOS_PROTOCOL, MockSmbiosAdd)
    .WillOnce (Return (EFI_INVALID_PARAMETER));

  Status = CreateIpmiSmbiosType38 ();
  EXPECT_EQ (Status, EFI_INVALID_PARAMETER);
}

// Trigger all google tests to run
int
main (
  int   argc,
  char  *argv[]
  )
{
  testing::InitGoogleTest (&argc, argv);
  return RUN_ALL_TESTS ();
}
