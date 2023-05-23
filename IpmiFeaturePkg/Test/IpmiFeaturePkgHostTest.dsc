## @file
# IpmiFeaturePkg DSC file used to build host-based unit tests.
#
# Copyright (c) Microsoft Corporation.<BR>
# SPDX-License-Identifier: BSD-2-Clause-Patent
#
##

[Defines]
  PLATFORM_NAME           = IpmiFeaturePkgHostTest
  PLATFORM_GUID           = 0CDC468B-0C10-4BD0-834C-E732AFB87E64
  PLATFORM_VERSION        = 0.1
  DSC_SPECIFICATION       = 0x00010005
  OUTPUT_DIRECTORY        = Build/IpmiFeaturePkg/HostTest
  SUPPORTED_ARCHITECTURES = IA32|X64
  BUILD_TARGETS           = NOOPT
  SKUID_IDENTIFIER        = DEFAULT

!include UnitTestFrameworkPkg/UnitTestFrameworkPkgHost.dsc.inc

[LibraryClasses]
  BmcSmbusLib|IpmiFeaturePkg/Test/UnitTest/SsifUnitTest/BmcSmbusLibTest.inf
  IpmiSelLib|IpmiFeaturePkg/Library/IpmiSelLib/IpmiSelLib.inf
  TimerLib|MdePkg/Library/BaseTimerLibNullTemplate/BaseTimerLibNullTemplate.inf
  ReportStatusCodeLib|MdePkg/Library/BaseReportStatusCodeLibNull/BaseReportStatusCodeLibNull.inf
  IpmiTransportLib|IpmiFeaturePkg/Library/MockIpmi/IpmiTransportLibMock.inf
  IpmiPlatformLib|IpmiFeaturePkg/Library/IpmiPlatformLibNull/IpmiPlatformLibNull.inf
  IpmiCommandLib|IpmiFeaturePkg/Library/IpmiCommandLib/IpmiCommandLib.inf
  IpmiBaseLib|IpmiFeaturePkg/Library/MockIpmi/IpmiBaseLibMock.inf
  IpmiWatchdogLib|IpmiFeaturePkg/Library/IpmiWatchdogLib/IpmiWatchdogLib.inf
  IpmiBootOptionLib|IpmiFeaturePkg/Library/IpmiBootOptionLib/IpmiBootOptionLib.inf

[PcdsFixedAtBuild]
  gIpmiFeaturePkgTokenSpaceGuid.PcdIpmiCheckSelfTestResults|TRUE

[Components]
  IpmiFeaturePkg/Test/UnitTest/SsifUnitTest/BmcSmbusLibTest.inf
  IpmiFeaturePkg/Test/UnitTest/SsifUnitTest/SsifUnitTestHost.inf {
    <LibraryClasses>
      IpmiTransportLib|IpmiFeaturePkg/Library/IpmiTransportLibSsif/SsifIpmiTransportLib.inf
  }

  IpmiFeaturePkg/Test/UnitTest/SelUnitTest/SelUnitTest.inf
  IpmiFeaturePkg/GenericIpmi/Test/GenericIpmiUnitTest.inf
  IpmiFeaturePkg/Test/UnitTest/WatchdogUnitTest/WatchdogUnitTest.inf
  IpmiFeaturePkg/Test/UnitTest/BootOptionUnitTest/BootOptionUnitTest.inf

  #
  # Build HOST_APPLICATION Libraries
  #

  MdePkg/Library/BaseLib/UnitTestHostBaseLib.inf

[BuildOptions]
# Need to use floats in this library. Got rid of -mgeneral-regs-only to do so.
  GCC:*_*_AARCH64_CC_XIPFLAGS == -mstrict-align
