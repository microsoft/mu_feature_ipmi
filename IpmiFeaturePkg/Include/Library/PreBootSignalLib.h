/** @file PreBootSignalLib.h
  Copyright (C) Microsoft Corporation. All rights reserved.
**/

#ifndef PRE_BOOT_SIGNAL_LIB_H_
#define PRE_BOOT_SIGNAL_LIB_H_

/**
  Sends a pre-boot signal to the BMC. Signal will be sent from IPMI Generic
  before the system attempts to communiate to the BMC.

  Note: requires PCD PcdSignalPreBootToBmc to be set to true

  @param[in]  PeiServices     The PEI services structure.

  @retval     EFI_SUCCESS     Indicates that the preboot signal succeeded
  @retval         other                   Indicates that the preboot signal failed, and
                                                                no futher communication should be attempted.
**/
EFI_STATUS
EFIAPI
SendPreBootSignaltoBmc (
  IN CONST EFI_PEI_SERVICES  **PeiServices
  );

#endif
