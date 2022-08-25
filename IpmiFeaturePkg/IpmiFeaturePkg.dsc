## @file
# This package provides advanced feature functionality to enable the
# Intelligent Platform Management Interface (IPMI).
# This package should only depend on EDK II Core packages, IntelSiliconPkg, and MinPlatformPkg.
#
# The DEC files are used by the utilities that parse DSC and
# INF files to generate AutoGen.c and AutoGen.h files
# for the build infrastructure.
#
# Copyright (c) 2019, Intel Corporation. All rights reserved.<BR>
#
# SPDX-License-Identifier: BSD-2-Clause-Patent
#
##

[Defines]
  PLATFORM_NAME                  = IpmiFeaturePkg
  PLATFORM_GUID                  = 7B2E74D7-9538-4818-B51A-492F3D33BD20
  PLATFORM_VERSION               = 0.1
  DSC_SPECIFICATION              = 0x00010005
  OUTPUT_DIRECTORY               = Build/$(PLATFORM_NAME)
  SUPPORTED_ARCHITECTURES        = IA32|X64|AARCH64
  BUILD_TARGETS                  = DEBUG|RELEASE|NOOPT
  SKUID_IDENTIFIER               = DEFAULT

!include MdePkg/MdeLibs.dsc.inc
[LibraryClasses]
  #######################################
  # BaseCore Packages
  #######################################
  BaseLib|MdePkg/Library/BaseLib/BaseLib.inf
  BaseMemoryLib|MdePkg/Library/BaseMemoryLib/BaseMemoryLib.inf
  DebugLib|MdePkg/Library/BaseDebugLibNull/BaseDebugLibNull.inf
  PrintLib|MdePkg/Library/BasePrintLib/BasePrintLib.inf
  TimerLib|MdePkg/Library/BaseTimerLibNullTemplate/BaseTimerLibNullTemplate.inf
  IoLib|MdePkg/Library/BaseIoLibIntrinsic/BaseIoLibIntrinsic.inf
  OemHookStatusCodeLib|MdeModulePkg/Library/OemHookStatusCodeLibNull/OemHookStatusCodeLibNull.inf
  UefiBootServicesTableLib|MdePkg/Library/UefiBootServicesTableLib/UefiBootServicesTableLib.inf
  SmmServicesTableLib|MdePkg/Library/SmmServicesTableLib/SmmServicesTableLib.inf
  UefiDriverEntryPoint|MdePkg/Library/UefiDriverEntryPoint/UefiDriverEntryPoint.inf
  PcdLib|MdePkg/Library/BasePcdLibNull/BasePcdLibNull.inf
  DxeServicesTableLib|MdePkg/Library/DxeServicesTableLib/DxeServicesTableLib.inf

  #####################################
  # IPMI Feature Package
  #####################################
  IpmiCommandLib|IpmiFeaturePkg/Library/IpmiCommandLib/IpmiCommandLib.inf
  IpmiPlatformHookLib|IpmiFeaturePkg/Library/IpmiPlatformHookLibNull/IpmiPlatformHookLibNull.inf
  IpmiTransportLib|IpmiFeaturePkg/Library/IpmiTransportLibNull/IpmiTransportLibNull.inf
  BmcSelfTestPlatformLib|IpmiFeaturePkg/Library/BmcSelfTestPlatformLibNull/BmcSelfTestPlatformLibNull.inf
  IpmiSelLib|IpmiFeaturePkg/Library/IpmiSelLib/IpmiSelLib.inf

[LibraryClasses.common.PEI_CORE,LibraryClasses.common.PEIM]
  #######################################
  # BaseCore Packages
  #######################################
  HobLib|MdePkg/Library/PeiHobLib/PeiHobLib.inf
  MemoryAllocationLib|MdePkg/Library/PeiMemoryAllocationLib/PeiMemoryAllocationLib.inf
  PcdLib|MdePkg/Library/PeiPcdLib/PeiPcdLib.inf
  PeimEntryPoint|MdePkg/Library/PeimEntryPoint/PeimEntryPoint.inf
  PeiServicesLib|MdePkg/Library/PeiServicesLib/PeiServicesLib.inf
  PeiServicesTablePointerLib|MdePkg/Library/PeiServicesTablePointerLib/PeiServicesTablePointerLib.inf
  ReportStatusCodeLib|MdeModulePkg/Library/PeiReportStatusCodeLib/PeiReportStatusCodeLib.inf

  #####################################
  # IPMI Feature Package
  #####################################
  IpmiBaseLib|IpmiFeaturePkg/Library/IpmiBaseLibPei/IpmiBaseLibPei.inf

[LibraryClasses.common.DXE_DRIVER,LibraryClasses.common.UEFI_DRIVER]
  #######################################
  # BaseCore Packages
  #######################################
  DevicePathLib|MdePkg/Library/UefiDevicePathLib/UefiDevicePathLib.inf
  MemoryAllocationLib|MdePkg/Library/UefiMemoryAllocationLib/UefiMemoryAllocationLib.inf
  PcdLib|MdePkg/Library/DxePcdLib/DxePcdLib.inf
  UefiBootServicesTableLib|MdePkg/Library/UefiBootServicesTableLib/UefiBootServicesTableLib.inf
  UefiDriverEntryPoint|MdePkg/Library/UefiDriverEntryPoint/UefiDriverEntryPoint.inf
  UefiLib|MdePkg/Library/UefiLib/UefiLib.inf
  UefiRuntimeServicesTableLib|MdePkg/Library/UefiRuntimeServicesTableLib/UefiRuntimeServicesTableLib.inf

  #####################################
  # IPMI Feature Package
  #####################################
  IpmiBaseLib|IpmiFeaturePkg/Library/IpmiBaseLibDxe/IpmiBaseLibDxe.inf

[LibraryClasses.common.DXE_DRIVER]
  ReportStatusCodeLib|MdeModulePkg/Library/DxeReportStatusCodeLib/DxeReportStatusCodeLib.inf

[LibraryClasses.common.DXE_SMM_DRIVER]
  MemoryAllocationLib|MdePkg/Library/SmmMemoryAllocationLib/SmmMemoryAllocationLib.inf
  ReportStatusCodeLib|MdeModulePkg/Library/SmmReportStatusCodeLib/SmmReportStatusCodeLib.inf

[Components]
  IpmiFeaturePkg/Library/IpmiCommandLib/IpmiCommandLib.inf
  IpmiFeaturePkg/Library/IpmiBaseLibNull/IpmiBaseLibNull.inf
  IpmiFeaturePkg/Library/IpmiBaseLibDxe/IpmiBaseLibDxe.inf
  IpmiFeaturePkg/Library/IpmiBaseLibPei/IpmiBaseLibPei.inf
  IpmiFeaturePkg/Library/IpmiBaseLibSmm/IpmiBaseLibSmm.inf
  IpmiFeaturePkg/Library/BmcSmbusLibNull/BmcSmbusLibNull.inf
  IpmiFeaturePkg/Library/IpmiPlatformHookLibNull/IpmiPlatformHookLibNull.inf
  IpmiFeaturePkg/GenericIpmi/Pei/PeiGenericIpmi.inf
  IpmiFeaturePkg/Frb/FrbPei.inf
  IpmiFeaturePkg/GenericIpmi/Dxe/DxeGenericIpmi.inf
  IpmiFeaturePkg/BmcAcpi/BmcAcpi.inf
  IpmiFeaturePkg/BmcElog/BmcElog.inf
  IpmiFeaturePkg/Frb/FrbDxe.inf
  IpmiFeaturePkg/IpmiFru/IpmiFru.inf
  IpmiFeaturePkg/OsWdt/OsWdt.inf
  IpmiFeaturePkg/SolStatus/SolStatus.inf
  IpmiFeaturePkg/Library/BmcSelfTestPlatformLibNull/BmcSelfTestPlatformLibNull.inf
  IpmiFeaturePkg/Library/IpmiSelLib/IpmiSelLib.inf

  # Transport Libraries
  IpmiFeaturePkg/Library/IpmiTransportLibNull/IpmiTransportLibNull.inf
  IpmiFeaturePkg/Library/IpmiTransportLibKcs/KcsIpmiTransportLib.inf
  IpmiFeaturePkg/Library/IpmiTransportLibSsif/SsifIpmiTransportLib.inf {
    <LibraryClasses>
      BmcSmbusLib|IpmiFeaturePkg/Library/BmcSmbusLibNull/BmcSmbusLibNull.inf
  }

  # Mock Libraries
  IpmiFeaturePkg/Library/MockIpmi/IpmiTransportLibMock.inf
  IpmiFeaturePkg/Library/MockIpmi/IpmiBaseLibMock.inf

  # Functional Tests
  IpmiFeaturePkg/Test/FunctionalTest/IpmiShellTest/IpmiShellTest.inf {
    <LibraryClasses>
      UefiApplicationEntryPoint|MdePkg/Library/UefiApplicationEntryPoint/UefiApplicationEntryPoint.inf
      UnitTestLib|UnitTestFrameworkPkg/Library/UnitTestLib/UnitTestLib.inf
      UnitTestPersistenceLib|UnitTestFrameworkPkg/Library/UnitTestPersistenceLibNull/UnitTestPersistenceLibNull.inf
      UnitTestResultReportLib|UnitTestFrameworkPkg/Library/UnitTestResultReportLib/UnitTestResultReportLibDebugLib.inf
      IpmiBaseLib|IpmiFeaturePkg/Library/IpmiBaseLibDxe/IpmiBaseLibDxe.inf
      MemoryAllocationLib|MdePkg/Library/UefiMemoryAllocationLib/UefiMemoryAllocationLib.inf
  }

[Components.IA32, Components.X64]
  IpmiFeaturePkg/GenericIpmi/Smm/SmmGenericIpmi.inf

[LibraryClasses.ARM, LibraryClasses.AARCH64]
  NULL|MdePkg/Library/CompilerIntrinsicsLib/ArmCompilerIntrinsicsLib.inf
  NULL|MdePkg/Library/BaseStackCheckLib/BaseStackCheckLib.inf
