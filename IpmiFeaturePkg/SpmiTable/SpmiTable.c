/** @file
  SPMI Table installation.

    Copyright (c) Microsoft Corporation. All rights reserved.
    SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <PiDxe.h>

#include <IndustryStandard/Acpi.h>
#include <IndustryStandard/ServiceProcessorManagementInterfaceTable.h>
#include <IndustryStandard/IpmiNetFnApp.h>
#include <Protocol/AcpiSystemDescriptionTable.h>
#include <Protocol/AcpiTable.h>
#include <Protocol/IpmiTransportProtocol.h>

#include <Library/BaseLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/DebugLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/UefiLib.h>
#include <Library/PcdLib.h>

// Service Processor Management Interface Table definition
EFI_ACPI_SERVICE_PROCESSOR_MANAGEMENT_INTERFACE_TABLE  gSpmiTable = {
  {
    EFI_ACPI_2_0_SERVER_PLATFORM_MANAGEMENT_INTERFACE_SIGNATURE,
    sizeof (EFI_ACPI_SERVICE_PROCESSOR_MANAGEMENT_INTERFACE_TABLE),
    0x05,                                          // Revision
    0x00,                                          // Checksum will be updated at runtime
    { ' ', ' ', ' ', ' ', ' ', ' ' },              // OEM ID. It is expected that these values will be programmed at runtime
    FixedPcdGet64 (PcdAcpiDefaultOemTableId),      // OEM Table ID
    FixedPcdGet32 (PcdAcpiDefaultOemRevision),     // OEM Revision
    FixedPcdGet32 (PcdAcpiDefaultCreatorId),       // Creator ID
    FixedPcdGet32 (PcdAcpiDefaultCreatorRevision), // Creator Revision
  },
  // Beginning of SPMI specific fields
  FixedPcdGet8 (PcdIpmiInterfaceType),          // Interface Type
  0x01,                                         // Reserved should be 01h
  0,                                            // IPMI Specification Version, will be programmed at runtime
  FixedPcdGet8 (PcdIpmiInterruptType),          // SPMI INTERRUPT TYPE
  FixedPcdGet8 (PcdIpmiGpe),                    // SPMI GPE
  0,                                            // Reserved
  FixedPcdGet8 (PcdIpmiPciDeviceFlag),          // PCI DEVICE FLAG
  FixedPcdGet32 (PcdIpmiGlobalSystemInterrupt), // GLOBAL SYSTEM INTERRUPT
  {                                          // Base Address of the Interface register set described using the GAS
    FixedPcdGet8 (PcdIpmiAddressSpaceId),    // AddressSpaceId
    FixedPcdGet8 (PcdIpmiRegisterBitWidth),  // RegisterBitWidth
    FixedPcdGet8 (PcdIpmiRegisterBitOffset), // RegisterBitOffset
    FixedPcdGet8 (PcdIpmiAccessSize),        // AccessSize
    0,                                       // Address, will be programmed at runtime
  },
  {   // Device identification information.
    {
      0,
      0,
      0,
      0,
    },
  },
  0                                  // Reserved
};

/**

  Update SPMI Table.

  @retval EFI_SUCCESS           -  Success.
  @retval EFI_OUT_OF_RESOURCES  -  Out of resources.

**/
EFI_STATUS
EFIAPI
UpdateAcpiSpmiTables (
  VOID
  )
{
  EFI_STATUS      Status;
  IPMI_TRANSPORT  *IpmiTransport = NULL;
  SM_CTRL_INFO    *SmCtrlInfo    = NULL;
  UINT16          IpmiSpecRevision;
  UINT32          ResponseDataSize;

  SmCtrlInfo = AllocateZeroPool (sizeof (SM_CTRL_INFO));
  if (SmCtrlInfo == NULL) {
    DEBUG ((DEBUG_ERROR, "[%a] Allocate resources failed!\n", __FUNCTION__));
    return EFI_OUT_OF_RESOURCES;
  }

  // OEM ID
  CopyMem (
    gSpmiTable.Header.OemId,
    PcdGetPtr (PcdAcpiDefaultOemId),
    sizeof (gSpmiTable.Header.OemId)
    );

  ResponseDataSize = sizeof (SM_CTRL_INFO);
  Status           = gBS->LocateProtocol (
                            &gIpmiTransportProtocolGuid,
                            NULL,
                            (VOID **)&IpmiTransport
                            );
  if (EFI_ERROR (Status)) {
    FreePool (SmCtrlInfo);
    return Status;
  }

  Status = IpmiTransport->IpmiSubmitCommand (
                            IpmiTransport,
                            IPMI_NETFN_APP,
                            0,
                            IPMI_APP_GET_DEVICE_ID,
                            NULL,
                            0,
                            (UINT8 *)SmCtrlInfo,
                            &ResponseDataSize
                            );
  if (EFI_ERROR (Status)) {
    FreePool (SmCtrlInfo);
    return Status;
  }

  IpmiSpecRevision  = (UINT16)(SmCtrlInfo->SpecificationVersion & 0xF0);
  IpmiSpecRevision |= (UINT16)((SmCtrlInfo->SpecificationVersion & 0x0F) << 8);
  // IPMI Specification Version
  gSpmiTable.SpecificationRevision = IpmiSpecRevision;

  // Address
  if (PcdGet8 (PcdIpmiAddressSpaceId) == EFI_ACPI_5_0_SYSTEM_IO) {
    gSpmiTable.BaseAddress.Address = PcdGet16 (PcdIpmiIoBaseAddress);
  } else {
    gSpmiTable.BaseAddress.Address = PcdGet64 (PcdIpmiAddress);
  }

  // DeviceId
  CopyMem (
    &gSpmiTable.DeviceId,
    PcdGetPtr (PcdIpmiDeviceId),
    sizeof (gSpmiTable.DeviceId)
    );

  FreePool (SmCtrlInfo);
  return EFI_SUCCESS;
}

/**

  Entry point for SPMI Table driver.

  @param ImageHandle  -  A handle for the image that is initializing this driver.
  @param SystemTable  -  A pointer to the EFI system table.

  @retval EFI_SUCCESS           -  Driver initialized successfully.
  @retval EFI_LOAD_ERROR        -  Failed to Initialize or has been loaded.
  @retval EFI_OUT_OF_RESOURCES  -  Could not allocate needed resources.

**/
EFI_STATUS
EFIAPI
SpmiTableEntryPoint (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS               Status;
  UINTN                    TableHandle = 0;
  EFI_ACPI_TABLE_PROTOCOL  *AcpiTable;

  //
  // Find the AcpiTable protocol
  //
  Status = gBS->LocateProtocol (&gEfiAcpiTableProtocolGuid, NULL, (VOID **)&AcpiTable);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "Locate ACPI Table protocol Status: %r \n", Status));
    return Status;
  }

  // Update SPMI Table
  Status = UpdateAcpiSpmiTables ();
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "UpdateAcpiSpmiTables Status: %r \n", Status));
    return Status;
  }

  // Add Spmi table.
  TableHandle = 0;
  Status      = AcpiTable->InstallAcpiTable (
                             AcpiTable,
                             &gSpmiTable,
                             sizeof (EFI_ACPI_SERVICE_PROCESSOR_MANAGEMENT_INTERFACE_TABLE),
                             &TableHandle
                             );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "AcpiTable->InstallAcpiTable(SPMI) = %r\n", Status));
    ASSERT_EFI_ERROR (Status);
    return Status;
  }

  return EFI_SUCCESS;
}
