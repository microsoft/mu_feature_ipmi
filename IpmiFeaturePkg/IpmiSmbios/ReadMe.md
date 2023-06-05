# IpmiSmbios

## Introduction

The IpmiSmbios fulfills the SMBIOS Type38 requirement of [IPMI Specification 2nd Generation v2.0](https://www.intel.com/content/dam/www/public/us/en/documents/product-briefs/ipmi-second-gen-interface-spec-v2-rev1-1.pdf)

## High Level Module Interaction Flow

The IpmiSmbios Dxe driver entry point creates IpmiSmbiosCreateOnReadyToBoot event, which creates the SMBIOS Type38
according to PCDs configuration.

## Feature Enablement

To leverage this feature,

1. Add the following to your platform DSC:

    ```ini

    [Components.X64]
    IpmiFeaturePkg/IpmiSmbios/IpmiSmbios.inf

    ```

2. Add the following to your platform FDF FVMAIN:

    ```ini

    INF  IpmiFeaturePkg/IpmiSmbios/IpmiSmbios.inf

    ```

## PCDs

|PCD Name|Descritpion|
|---|---|
|PcdSmbiosTablesIpmiAccessType|Default value is 0x01. 0x00 MMIO, 0x01 IO
|PcdIpmiIoBaseAddress|Default value is 0xCA2. only available when PcdSmbiosTablesIpmiAccessType == 0x01
|PcdIpmiAddress|Default value is 0. only available when PcdSmbiosTablesIpmiAccessType == 0x00
|PcdIpmiInterfaceType|Default value is 0x1. 0x00 Uknown, 0x01 KCS, 0x02 SMIC, 0x3 BT, 0x4 SSIF.
|PcdSmbiosTablesIpmiRegisterSpacing|Default value is 0x0. 0x0 successive byte, 0x1 32bit, 0x2 16bit boundaries.
|PcdSmbiosTablesIpmiInterruptInfo|Default value is 0x0. 0x0 not specified, 0x1 specified.
|PcdSmbiosTablesIpmiInterruptPolarity|Default value is 0x0. 0x0 active low, 0x1 active high.
|PcdSmbiosTablesIpmiInterruptTriggerMode|Default value is 0x0. 0x0 edge, 0x1 level.
|PcdSmbiosTablesIpmiInterruptNumber|Default value is 0x0. 0x0 unspecified/unsupported.
|PcdSmbiosTablesIpmiI2CSlaveAddress|Default value is 0x20.
|PcdSmbiosTablesIpmiNVStorageDeviceAddress|Default value is 0xFF.


## Copyright

Copyright (c) Microsoft Corporation. All rights reserved.
