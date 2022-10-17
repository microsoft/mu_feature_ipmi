# IpmiFeaturePkg

This package contains core code and features for leveraging IPMI in UEFI. For
details on the design of the IPMI package see the [IPMI feature documentation](./Docs/Ipmi_Feature.md).

## Configuring the IPMI package

The IPMI feature package is designed to be customizable to fit the needs of the
platform. This can be done through a combination of platform libraries and PCD
settings.

### Important PCDs

For the full list of PCDs, see the [package DEC file](./IpmiFeaturePkg.dec).

- Generic IPMI
  - PcdIpmiCheckSelfTestResults - Indicates the self-test command should be queried for IPMI initialization.
  - PcdIpmiCommandTimeoutSeconds - Timeout for IPMI command response.
  - PcdBmcTimeoutSeconds - Timeout for initial BMC initialization.
- IPMI Watchdog
  - PcdFrb2EnabledFlag - Enables use of the FRB2 watchdog for UEFI boot.
  - PcdFrb2TimeoutSeconds - FRB2 timeout in seconds.
  - PcdFrb2TimeoutAction - Action taken on FRB2 timeout.
  - PcdOsWatchdogEnabled - Enables the OS watchdog at exit boot services.
  - PcdOsWatchdogTimeoutSeconds - The timeout for the OS watchdog in seconds.
  - PcdOsWatchdogAction - Action taken on OS watchdog timeout.
- SEL Library
  - PcdIpmiSelOemManufacturerId - The manufacturer ID used in OEM SEL events.

### Platform Libraries

The primary location for platform customization of the generic IPMI code is in
the IPMI Platform library. If the system requires special initialization logic,
self-test handling, IO port configuration, etc, then it should be done by
implementing this library.

Additionally the platform may choice to implement their own IPMI transport
library for a non-standard communication method wth the BMC.
