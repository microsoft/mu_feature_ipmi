/** @file
  Unit tests for the SpmiTable driver using google tests

  Copyright (c) Microsoft Corporation.
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/
#include <Library/GoogleTestLib.h>
#include <Library/FunctionMockLib.h>

extern "C" {
#include <Uefi.h>
#include <IndustryStandard/ServiceProcessorManagementInterfaceTable.h>
#include <Library/UefiBootServicesTableLib.h>
#include <IpmiInterface.h>
#include <Protocol/AcpiTable.h>

extern EFI_ACPI_SERVICE_PROCESSOR_MANAGEMENT_INTERFACE_TABLE  mSpmiTable;

//
// Interfaces to be tested
//
EFI_STATUS
EFIAPI
SpmiTableEntryPoint (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
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
struct MockIpmiTransport {
  MOCK_INTERFACE_DECLARATION (MockIpmiTransport);

  MOCK_FUNCTION_DECLARATION (
    EFI_STATUS,
    IpmiSendCommand,
    (IN IPMI_TRANSPORT  *This,
     IN UINT8           NetFunction,
     IN UINT8           Lun,
     IN UINT8           Command,
     IN UINT8           *CommandData,
     IN UINT32          CommandDataSize,
     OUT UINT8          *ResponseData,
     OUT UINT32         *ResponseDataSize)
    );
};

MOCK_INTERFACE_DEFINITION (MockIpmiTransport);
MOCK_FUNCTION_DEFINITION (MockIpmiTransport, IpmiSendCommand, 8, EFIAPI);

//
// Declarations to handle usage of the EFI_ACPI_TABLE_PROTOCOL by creating mock
//
struct MockEfiAcpiTableProtocol {
  MOCK_INTERFACE_DECLARATION (MockEfiAcpiTableProtocol);

  MOCK_FUNCTION_DECLARATION (
    EFI_STATUS,
    InstallAcpiTable,
    (IN   EFI_ACPI_TABLE_PROTOCOL       *This,
     IN   VOID                          *AcpiTableBuffer,
     IN   UINTN                         AcpiTableBufferSize,
     OUT  UINTN                         *TableKey)
    );
};

MOCK_INTERFACE_DEFINITION (MockEfiAcpiTableProtocol);
MOCK_FUNCTION_DEFINITION (MockEfiAcpiTableProtocol, InstallAcpiTable, 4, EFIAPI);

static EFI_ACPI_TABLE_PROTOCOL  LocalEfiAcpiTableProtocol;

class MockSpmiTable : public  Test {
protected:
BOOLEAN ReturnValue;
EFI_STATUS Status;
MockUefiBootServicesTableLib UefiBootServicesTableLib;
MockIpmiTransport IPMI_TRANSPORT;
MockEfiAcpiTableProtocol EFI_ACPI_TABLE_PROTOCOL;
virtual void
SetUp (
  )
{
  LocalBs.LocateProtocol                     = gBS_LocateProtocol;
  LocalIpmiTransport.IpmiSubmitCommand       = IpmiSendCommand;
  LocalEfiAcpiTableProtocol.InstallAcpiTable = InstallAcpiTable;
}
};

TEST_F (MockSpmiTable, VerifySpmiTableEntryPointPositiveTestCase) {
  EXPECT_CALL (UefiBootServicesTableLib, gBS_LocateProtocol (_, _, _))
    .Times (2)
    .WillOnce (
       DoAll (
         SetArgPointee<2>(&LocalEfiAcpiTableProtocol),
         Return (EFI_SUCCESS)
         )
       )
    .WillOnce (
       DoAll (
         SetArgPointee<2>(&LocalIpmiTransport),
         Return (EFI_SUCCESS)
         )
       );

  EXPECT_CALL (IPMI_TRANSPORT, IpmiSendCommand)
    .WillOnce (Return (EFI_SUCCESS));

  EXPECT_CALL (EFI_ACPI_TABLE_PROTOCOL, InstallAcpiTable)
    .WillOnce (Return (EFI_SUCCESS));

  Status = SpmiTableEntryPoint (NULL, NULL);
  EXPECT_EQ (Status, EFI_SUCCESS);
  EXPECT_EQ (mSpmiTable.Header.OemTableId, FixedPcdGet64 (PcdAcpiDefaultOemTableId));
  EXPECT_EQ (mSpmiTable.Header.OemRevision, FixedPcdGet32 (PcdAcpiDefaultOemRevision));
  EXPECT_EQ (mSpmiTable.Header.CreatorId, FixedPcdGet32 (PcdAcpiDefaultCreatorId));
  EXPECT_EQ (mSpmiTable.Header.CreatorRevision, FixedPcdGet32 (PcdAcpiDefaultCreatorRevision));
  EXPECT_EQ (mSpmiTable.InterfaceType, FixedPcdGet8 (PcdIpmiInterfaceType));
  EXPECT_EQ (mSpmiTable.InterruptType, FixedPcdGet8 (PcdIpmiInterruptType));
  EXPECT_EQ (mSpmiTable.Gpe, FixedPcdGet8 (PcdIpmiGpe));
  EXPECT_EQ (mSpmiTable.PciDeviceFlag, FixedPcdGet8 (PcdIpmiPciDeviceFlag));
  EXPECT_EQ (mSpmiTable.GlobalSystemInterrupt, FixedPcdGet32 (PcdIpmiGlobalSystemInterrupt));
  EXPECT_EQ (mSpmiTable.BaseAddress.AddressSpaceId, FixedPcdGet8 (PcdIpmiAddressSpaceId));
  EXPECT_EQ (mSpmiTable.BaseAddress.RegisterBitWidth, FixedPcdGet8 (PcdIpmiRegisterBitWidth));
  EXPECT_EQ (mSpmiTable.BaseAddress.RegisterBitOffset, FixedPcdGet8 (PcdIpmiRegisterBitOffset));
  EXPECT_EQ (mSpmiTable.BaseAddress.Reserved, FixedPcdGet8 (PcdIpmiAccessSize));
}

TEST_F (MockSpmiTable, VerifyIpmiSendCommandFailed) {
  EXPECT_CALL (UefiBootServicesTableLib, gBS_LocateProtocol (_, _, _))
    .Times (2)
    .WillOnce (
       DoAll (
         SetArgPointee<2>(&LocalEfiAcpiTableProtocol),
         Return (EFI_SUCCESS)
         )
       )
    .WillOnce (
       DoAll (
         SetArgPointee<2>(&LocalIpmiTransport),
         Return (EFI_SUCCESS)
         )
       );

  EXPECT_CALL (IPMI_TRANSPORT, IpmiSendCommand)
    .WillOnce (Return (EFI_UNSUPPORTED));

  Status = SpmiTableEntryPoint (NULL, NULL);
  EXPECT_EQ (Status, EFI_UNSUPPORTED);
}

TEST_F (MockSpmiTable, VerifyInstallAcpiTableFailed) {
  EXPECT_CALL (UefiBootServicesTableLib, gBS_LocateProtocol (_, _, _))
    .Times (2)
    .WillOnce (
       DoAll (
         SetArgPointee<2>(&LocalEfiAcpiTableProtocol),
         Return (EFI_SUCCESS)
         )
       )
    .WillOnce (
       DoAll (
         SetArgPointee<2>(&LocalIpmiTransport),
         Return (EFI_SUCCESS)
         )
       );

  EXPECT_CALL (IPMI_TRANSPORT, IpmiSendCommand)
    .WillOnce (Return (EFI_SUCCESS));

  EXPECT_CALL (EFI_ACPI_TABLE_PROTOCOL, InstallAcpiTable)
    .WillOnce (Return (EFI_UNSUPPORTED));

  Status = SpmiTableEntryPoint (NULL, NULL);
  EXPECT_EQ (Status, EFI_UNSUPPORTED);
}

TEST_F (MockSpmiTable, VerifyLocateProtocolNotFound) {
  EXPECT_CALL (UefiBootServicesTableLib, gBS_LocateProtocol)
    .WillOnce (Return (EFI_NOT_FOUND));

  Status = SpmiTableEntryPoint (NULL, NULL);
  EXPECT_EQ (Status, EFI_NOT_FOUND);

  EXPECT_CALL (UefiBootServicesTableLib, gBS_LocateProtocol (_, _, _))
    .Times (2)
    .WillOnce (
       DoAll (
         SetArgPointee<2>(&LocalEfiAcpiTableProtocol),
         Return (EFI_SUCCESS)
         )
       )
    .WillOnce (
       DoAll (
         SetArgPointee<2>(&LocalIpmiTransport),
         Return (EFI_NOT_FOUND)
         )
       );

  Status = SpmiTableEntryPoint (NULL, NULL);
  EXPECT_EQ (Status, EFI_NOT_FOUND);
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
