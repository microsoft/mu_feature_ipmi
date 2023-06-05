/** @file IpmiSmbios.c
    Create IPMI SMBIOS in ReadyToBoot event.

    Copyright (c) Microsoft Corporation.
    SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <PiDxe.h>
#include <IndustryStandard/SmBios.h>
#include <Protocol/Smbios.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/IpmiCommandLib.h>
#include <ServerManagement.h>

#define IPMI_ACCESS_TYPE_MMIO  0
#define IPMI_ACCESS_TYPE_IO    1

#pragma pack(1)
struct {
  SMBIOS_TABLE_TYPE38    Type38;
  UINT8                  TrailingZero[2]; // SMBIOS protocol expects two zero bytes behind added record
} mSmbiosTableType38 = {
  {
    {
      EFI_SMBIOS_TYPE_IPMI_DEVICE_INFORMATION,   // Type
      sizeof (SMBIOS_TABLE_TYPE38),              // Length
      0,                                         // Handle, will be programmed at runtime
    },
    FixedPcdGet8 (PcdIpmiInterfaceType),                        // InterfaceType
    0x20,                                                       // IPMISpecificationRevision, will be programmed at runtime
    FixedPcdGet8 (PcdSmbiosTablesIpmiI2CSlaveAddress),          // I2CSlaveAddress
    FixedPcdGet8 (PcdSmbiosTablesIpmiNVStorageDeviceAddress),   // NVStorageDeviceAddress
    0,                                                          // BaseAddress, will be programmed at runtime
    0,                                                          // BaseAddressModifier_InterruptInfo, will be programmed at runtime
    FixedPcdGet8 (PcdSmbiosTablesIpmiInterruptNumber),          // InterruptNumber
  },
  {
    0,
    0,
  }
};
#pragma pack()

/**
    Create SMBIOS Type 38.

    @retval EFI_SUCCESS         Success.
    @retval Others              Failed.
**/
EFI_STATUS
EFIAPI
CreateIpmiSmbiosType38 (
  VOID
  )
{
  EFI_STATUS                   Status;
  EFI_SMBIOS_PROTOCOL          *SmbiosProtocol;
  EFI_SMBIOS_HANDLE            SmbiosHandle;
  IPMI_GET_DEVICE_ID_RESPONSE  DeviceId;
  UINT8                        IPMISpecificationRevision;

  Status = gBS->LocateProtocol (
                  &gEfiSmbiosProtocolGuid,
                  NULL,
                  (VOID **)&SmbiosProtocol
                  );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a - LocateProtocol(gEfiSmbiosProtocolGuid): %r\n", __FUNCTION__, Status));
    return Status;
  }

  Status = IpmiGetDeviceId (&DeviceId);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_INFO, "%a IpmiGetDeviceId Failed, %r\n", __FUNCTION__, Status));
    return Status;
  }

  IPMISpecificationRevision                           = (((DeviceId.SpecificationVersion & 0xF0)>>4) | ((DeviceId.SpecificationVersion & 0x0F)<<4));
  mSmbiosTableType38.Type38.IPMISpecificationRevision = IPMISpecificationRevision;

  if (PcdGet8 (PcdSmbiosTablesIpmiAccessType) == IPMI_ACCESS_TYPE_IO) {
    mSmbiosTableType38.Type38.BaseAddress                       = (PcdGet16 (PcdIpmiIoBaseAddress) | BIT0);
    mSmbiosTableType38.Type38.BaseAddressModifier_InterruptInfo = ((PcdGet16 (PcdIpmiIoBaseAddress) & BIT0) << 4);
  } else {
    mSmbiosTableType38.Type38.BaseAddress                       = (PcdGet64 (PcdIpmiAddress) & ~BIT0);
    mSmbiosTableType38.Type38.BaseAddressModifier_InterruptInfo = ((PcdGet64 (PcdIpmiAddress) & BIT0) << 4);
  }

  mSmbiosTableType38.Type38.BaseAddressModifier_InterruptInfo |= ((PcdGet8 (PcdSmbiosTablesIpmiRegisterSpacing) & 0x3) << 6) |
                                                                 ((PcdGet8 (PcdSmbiosTablesIpmiInterruptInfo) & 1) << 3) |
                                                                 ((PcdGet8 (PcdSmbiosTablesIpmiInterruptPolarity) & 1) << 1) |
                                                                 (PcdGet8 (PcdSmbiosTablesIpmiInterruptTriggerMode) & 1);

  SmbiosHandle = SMBIOS_HANDLE_PI_RESERVED;

  Status = SmbiosProtocol->Add (SmbiosProtocol, NULL, &SmbiosHandle, &mSmbiosTableType38.Type38.Hdr);
  DEBUG ((DEBUG_INFO, "%a Add IPMI SMBIOS TYPE38 %r\n", __FUNCTION__, Status));

  return Status;
}

/**
    Create IpmiSmbios on ReadyToBoot.

    @param Event            Event which caused this handler
    @param *Context         Context passed during Event Handler registration

    @retval EFI_SUCCESS     Successful
**/
VOID
EFIAPI
IpmiSmbiosCreateOnReadyToBoot (
  IN EFI_EVENT  Event,
  IN VOID       *Context
  )
{
  gBS->CloseEvent (Event);

  CreateIpmiSmbiosType38 ();
}

/**
    This is the standard EFI driver point.

    @param ImageHandle      Handle for the image of this driver.
    @param SystemTable      Pointer to the EFI System Table.

    @retval EFI_SUCCESS     Action is performed successfully.
    @retval Other           Error occurred during execution.
**/
EFI_STATUS
EFIAPI
IpmiSmbiosEntry (
  IN  EFI_HANDLE        ImageHandle,
  IN  EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS  Status;
  EFI_EVENT   ReadyToBootEvent;

  Status = gBS->CreateEventEx (
                  EVT_NOTIFY_SIGNAL,
                  TPL_CALLBACK,
                  IpmiSmbiosCreateOnReadyToBoot,
                  NULL,
                  &gEfiEventReadyToBootGuid,
                  &ReadyToBootEvent
                  );
  DEBUG ((DEBUG_ERROR, "%a - CreateEventEx(%g): %r\n", __FUNCTION__, gEfiEventReadyToBootGuid, Status));

  return Status;
}
