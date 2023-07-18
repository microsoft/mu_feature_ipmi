# IpmiPowerRestorePolicy

## Introduction

The IpmiPowerRestorePolicy configure the Power Restore Policy by IPMI Chassis Command, For command detail, please refer
to [IPMI Specification 2nd Generation v2.0](https://www.intel.com/content/dam/www/public/us/en/documents/product-briefs/ipmi-second-gen-interface-spec-v2-rev1-1.pdf)

## High Level Module Interaction Flow

The IpmiPowerRestorePolicy Dxe driver get current power restore policy and platform power restore policy setting first, if
the setting is the same then just return EFI_SUCCESS, if it is mismatch, then it will configure the power restore policy
according to Platform setting via IPMI Chassis command.

## Feature Enablement

To leverage this feature,

1. Add the following to your platform DSC's PEI section:

    ```ini

    IpmiFeaturePkg/IpmiPowerRestorePolicy/IpmiPowerRestorePolicy.inf

    ```

2. Add the following to your platform FDF's PEI section:

    ```ini

    INF  IpmiFeaturePkg/IpmiPowerRestorePolicy/IpmiPowerRestorePolicy.inf

    ```

## Platform Specific Library APIs

- Implement `PlatformPowerRestorePolicyConfigurationLib` library as per interfaces defined in [PlatformPowerRestorePolicyConfigurationLib.h](../Include/Library/PlatformPowerRestorePolicyConfigurationLib.h)

## Copyright

Copyright (c) Microsoft Corporation.
