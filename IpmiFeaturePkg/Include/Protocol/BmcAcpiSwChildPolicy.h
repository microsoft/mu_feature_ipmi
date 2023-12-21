/** @file
  This protocol produces BmcAcpiSwChildPolicy Protocol.

Copyright (c) 2023, Intel Corporation. All rights reserved.<BR>
SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef _BMC_ACPI_SW_CHILD_POLICY_H_
#define _BMC_ACPI_SW_CHILD_POLICY_H_

typedef struct _EFI_BMC_ACPI_SW_CHILD_POLICY_PROTOCOL EFI_BMC_ACPI_SW_CHILD_POLICY_PROTOCOL;

#define EFI_BMC_ACPI_SW_CHILD_POLICY_PROTOCOL_GUID \
  { 0x89843c0b, 0x5701, 0x4ff6, 0xa4, 0x73, 0x65, 0x75, 0x99, 0x04, 0xf7, 0x35 }

typedef
EFI_STATUS
(EFIAPI *EFI_SET_ACPI_POWER_STATE_IN_BMC)(
  IN EFI_BMC_ACPI_SW_CHILD_POLICY_PROTOCOL   *This,
  IN UINT8                                   PowerState,
  IN UINT8                                   DeviceState
  );

struct _EFI_BMC_ACPI_SW_CHILD_POLICY_PROTOCOL {
  EFI_SET_ACPI_POWER_STATE_IN_BMC    SetACPIPowerStateInBMC;
};

extern EFI_GUID  gEfiBmcAcpiSwChildPolicyProtocolGuid;

#endif
