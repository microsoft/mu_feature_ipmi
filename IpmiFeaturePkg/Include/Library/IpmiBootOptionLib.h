/** @file
  Definitions for the IPMI boot options library.

  Copyright (c) Microsoft Corporation
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef IPMI_BOOT_OPTION_LIB_H_
#define IPMI_BOOT_OPTION_LIB_H_

//
// Boot options as defined by the IPMI spec. Consuming platforms may choice to
// interpret these differently from the spec.
//

typedef enum _IPMI_BOOT_OPTION_SELECTOR {
  BootNone               = 0x0,
  BootPxe                = 0x1,
  BootHardDrive          = 0x2,
  BootHardDriveSafeMode  = 0x3,
  Boot4                  = 0x4,
  BootCd                 = 0x5,
  BootBiosSetup          = 0x6,
  BootRemoteFloppy       = 0x7,
  BootRemoteCd           = 0x8,
  BootPrimaryRemoteMedia = 0x9,
  BootA                  = 0xA,
  BootRemoteHardDrive    = 0xB,
  BootC                  = 0xC,
  BootD                  = 0xD,
  BootE                  = 0xE,
  BootFloppy             = 0xF, // Max value, only 4 bits allocated.
} IPMI_BOOT_OPTION_SELECTOR;

/**
  Gets the boot device override provided by the BMC. This function will clear
  the IPMI flags on calling.

  @param[out]   Selector        The boot option device specified by BMC. BootNone
                                will be returned if no valid override exists.

  @retval       EFI_SUCCESS             The boot options were successfully queried.
  @retval       EFI_INVALID_PARAMETER   Selector is NULL.
  @retval       EFI_PROTOCOL_ERROR      A failing IPMI completion code was returned.
  @retval       Other                   A failure was returned by the IPMI stack.
**/
EFI_STATUS
EFIAPI
IpmiGetBootDevice (
  OUT IPMI_BOOT_OPTION_SELECTOR  *Selector
  );

/**
  Checks if the CMOS clear bit is set in the IPMI boot options.

  @param[out]  ClearCmos    TRUE if the CMOS clear bit is set, FALSE otherwise.

  @retval     EFI_SUCCESS   The CMOS bit was successfully checked.
  @retval     EFI_NOT_READY The boot option set in progress bit is set.
  @retval     Other         An error was returned by a subroutine.
**/
EFI_STATUS
EFIAPI
IpmiGetCmosClearOption (
  OUT BOOLEAN  *ClearCmos
  );

#endif
