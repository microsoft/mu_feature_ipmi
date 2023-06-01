# SPMI Table

## Introduction

This driver creates and installs the SPMI table to system via AcpiTableProtocol.

The detail definition of the SPMI table can be found from [IPMI v2.0 SPEC](https://www.intel.com/content/dam/www/public/us/en/documents/product-briefs/ipmi-second-gen-interface-spec-v2-rev1-1.pdf)

## High Level Module Interaction Flow

  The SpmiTableEntryPoint responsible for updating the SPMI table based on PCDs configuration, then install the SPMI table
  by AcpiTableProtocol that will be consumed by OS.

## PCDs

|PCD Name|Descritpion|
|---|---|
|PcdAcpiDefaultOemId|Default OEM ID for ACPI table creation. Default value is derived from MdeModulePkg.dec|
|PcdAcpiDefaultOemTableId|Default OEM Table ID for ACPI table creation. Default value is derived from MdeModulePkg.dec|
|PcdAcpiDefaultOemRevision|Default OEM Revision for ACPI table creation. Default value is derived from MdeModulePkg.dec|
|PcdAcpiDefaultCreatorId|Default Creator ID for ACPI table creation. Default value is derived from MdeModulePkg.dec|
|PcdAcpiDefaultCreatorRevision|Default Creator Revision for ACPI table creation. Default value is derived from MdeModulePkg.dec|
|PcdIpmiInterfaceType|IPMI Interface Type. Default is 0x01 KCS.|
|PcdIpmiIoBaseAddress|IPMI IO Base Address. Default is 0xCA2|
|PcdIpmiGpe|IPMI GPE number. Default is 0x00|
|PcdIpmiPciDeviceFlag|IPMI PCI Device Flag. Default is 0x00|
|PcdIpmiGlobalSystemInterrupt|IPMI Global System Interrupt. Default is 0x00|
|PcdIpmiAddressSpaceId|IPMI GAS Address Space ID. Default is 0x01|
|PcdIpmiRegisterBitWidth|IPMI GAS Register Bit Width. Default is 0x08|
|PcdIpmiRegisterBitOffset|IPMI GAS Register Bit Offset. Default is 0x00|
|PcdIpmiAccessSize|IPMI GAS Register Access Size. Default is 0x1|
|PcdIpmiAddress|IPMI GAS Base Address. Default is 0x00|
|PcdIpmiDeviceId|IPMI Device ID. Default is {0x00, 0x00, 0x00, 0x00}|

## Feature Enablement

To leverage this feature,

1. Add the following to your platform DSC:

    ```ini

    [Components.X64]
    IpmiFeaturePkg/SpmiTable/SpmiTable.inf

    ```

2. Add the following to your platform FDF:

    ```ini

    INF  IpmiFeaturePkg/SpmiTable/SpmiTable.inf

    ```

## Copyright

Copyright (c) Microsoft Corporation.
