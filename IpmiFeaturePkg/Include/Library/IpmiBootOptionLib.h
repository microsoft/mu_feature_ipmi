/** @file
  Definitions for the IPMI boot options library.

  Copyright (c) Microsoft Corporation
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef IPMI_BOOT_OPTION_LIB_H_
#define IPMI_BOOT_OPTION_LIB_H_

typedef enum _IPMI_BOOT_OPTION_SELECTOR {
  BootNone                 = 0x0,
  BootPxe                  = 0x1,
  BootHardDrive            = 0x2,
  BootHardDriveSafeMode    = 0x3,
  BootDiagnosticsPartition = 0x4,
  BootCd                   = 0x5,
  BootBiosSetup            = 0x6,
  BootRemoteFloppy         = 0x7,
  BootRemoteCd             = 0x8,
  BootPrimaryRemoteMedia   = 0x9,
  BootRemoteHardDrive      = 0xB,
  BootHttp                 = 0xC,
  BootHttps                = 0xD,
  BootFloppy               = 0xF, // Max value, only 4 bits allocated.
} IPMI_BOOT_OPTION_SELECTOR;

/**
  Gets the boot device override provided by the BMC. This function will clear
  the IPMI flags on calling.

  @param[out]   Selector        The boot option device specified by BMC. BootNone
                                will be returned if no valid override exists.

  @retval       EFI_SUCCESS         The boot options were successfully queried.
  @retval       EFI_PROTOCOL_ERROR  A failing IPMI completion code was returned.
  @retval       Other               A failure was returned by the IPMI stack.
**/
EFI_STATUS
EFIAPI
IpmiGetBootOption (
  OUT IPMI_BOOT_OPTION_SELECTOR  *Selector
  );

#endif
