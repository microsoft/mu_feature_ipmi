## @file
# This include provides the core libraries from the IPMI package that are used
# internally and/or are not usually overriden.
#
# Copyright (c) Microsoft Corporation
#
# SPDX-License-Identifier: BSD-2-Clause-Patent
#
##

[LibraryClasses]
  IpmiCommandLib|IpmiFeaturePkg/Library/IpmiCommandLib/IpmiCommandLib.inf
  IpmiSelLib|IpmiFeaturePkg/Library/IpmiSelLib/IpmiSelLib.inf
  IpmiWatchdogLib|IpmiFeaturePkg/Library/IpmiWatchdogLib/IpmiWatchdogLib.inf
  IpmiBootOptionLib|IpmiFeaturePkg/Library/IpmiBootOptionLib/IpmiBootOptionLib.inf

[LibraryClasses.common.PEI_CORE,LibraryClasses.common.PEIM]
  IpmiBaseLib|IpmiFeaturePkg/Library/IpmiBaseLibPei/IpmiBaseLibPei.inf

[LibraryClasses.common.DXE_DRIVER,LibraryClasses.common.UEFI_DRIVER]
  IpmiBaseLib|IpmiFeaturePkg/Library/IpmiBaseLibDxe/IpmiBaseLibDxe.inf

[LibraryClasses.common.DXE_SMM_DRIVER]
  IpmiBaseLib|IpmiFeaturePkg/Library/IpmiBaseLibSmm/IpmiBaseLibSmm.inf
