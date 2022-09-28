# IPMI Boot Options

The IPMI boot option commands allows the system to query the BMC for the desired
boot device and configuration. This is described in sections 28.11 and 28.12 of
the the [IPMI Specification 2nd Generation v2.0](https://www.intel.com/content/dam/www/public/us/en/documents/product-briefs/ipmi-second-gen-interface-spec-v2-rev1-1.pdf).
Much of functionality of these parameters are no longer relevant or are not used
in practice.

## IPMI Boot Option Library

The IPMI feature package provides the [IPMI Boot Option Library](../Include/Library/IpmiBootOptionLib.h)
for easily querying the IPMI boot option information. Currently this is abstracted
as only retrieving the desired boot device. The library will do the following to
determine this device.

1. Query the boot option parameter 5, checking that the parameter and boot flags
are valid.
1. Clear the boot flags if the persistance bit is not set.
1. Send and acknowledgement to the BMC that the boot option has been handled by BIOS.
1. Return the queried boot device.

The caller is then responsible for ensuring the platform is configured to boot
to the proper device for the returned value. Because the IPMI option is abstract,
for example saying _BootDefaultHardDrive_, the caller should determine which
specific device path this should correspond to. Because this library potentially
clears the boot flags, this library should only be called once per boot.

## Implementing OEM Boot Options

The IPMI feature package only implements standard IPMI boot option parameters
with some minor industry updates since the IPMI specification does not reflect
more modern devices. Any additional OEM defined parameters can be implemented
on top of the base or command libraries and can be used instead of or in
conjunction with the standard boot option library.
