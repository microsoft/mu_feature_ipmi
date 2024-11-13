/** @file
  Generic Standalone MM IPMI stack driver

  @copyright
  Copyright 1999 - 2021 Intel Corporation. <BR>
  Copyright (c) Microsoft Corporation.
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

//
// Statements that include other files
//
#include <Uefi.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MmServicesTableLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/IpmiBaseLib.h>
#include <Library/TimerLib.h>
#include <Library/HobLib.h>

#include <IndustryStandard/Ipmi.h>
#include <SmStatusCodes.h>

#include <GenericIpmi.h>

IPMI_BMC_INSTANCE_DATA  *mIpmiInstance;
EFI_HANDLE              mImageHandle;

/**
  Setup and initialize the BMC for the Standalone MM phase.  In order to verify the BMC is functioning
  as expected, the BMC Selftest is performed.  The results are then checked and any errors are
  reported to the error manager.  Errors are collected throughout this routine and reported
  just prior to installing the driver.  If there are more errors than MAX_SOFT_COUNT, then they
  will be ignored.

  @param[in]  ImageHandle   A handle to driver image.
  @param[in]  SystemTable   A pointer to system table.

  @retval     EFI_SUCCESS   Successful driver initialization.
**/
EFI_STATUS
EFIAPI
InitializeStandaloneMmGenericIpmi (
  IN EFI_HANDLE           ImageHandle,
  IN EFI_MM_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS         Status;
  EFI_HANDLE         Handle;
  IPMI_BMC_HOB       *BmcHob;
  EFI_HOB_GUID_TYPE  *GuidHob;

  DEBUG ((DEBUG_INFO, "InitializeStandaloneMmGenericIpmi entry\n"));
  mImageHandle = ImageHandle;

  mIpmiInstance = AllocateZeroPool (sizeof (IPMI_BMC_INSTANCE_DATA));
  ASSERT (mIpmiInstance != NULL);
  if (mIpmiInstance == NULL) {
    DEBUG ((DEBUG_ERROR, "ERROR!! Null Pointer returned by AllocateZeroPool ()\n"));
    ASSERT_EFI_ERROR (EFI_OUT_OF_RESOURCES);
    return EFI_OUT_OF_RESOURCES;
  }

  //
  // Initialize the KCS transaction timeout. Assume delay unit is 1000 us.
  //
  mIpmiInstance->IpmiTimeoutPeriod =
    (PcdGet8 (PcdIpmiCommandTimeoutSeconds) * 1000 * 1000) / IPMI_DELAY_UNIT;

  //
  // Initialize IPMI IO Base, we still use SMS IO base to get device ID and
  // Self-test result since MM IF may have different cmds supported.
  //

  mIpmiInstance->Signature                       = SM_IPMI_BMC_SIGNATURE;
  mIpmiInstance->SlaveAddress                    = BMC_SLAVE_ADDRESS;
  mIpmiInstance->BmcStatus                       = BMC_NOTREADY;
  mIpmiInstance->IpmiTransport.IpmiSubmitCommand = IpmiSendCommand;
  mIpmiInstance->IpmiTransport.GetBmcStatus      = IpmiGetBmcStatus;

  //
  // Check if PEI already initialized the BMC connection.
  //

  GuidHob = GetFirstGuidHob (&gIpmiBmcHobGuid);
  if (GuidHob == NULL) {
    //
    // PEI did not create a BMC HOB, initialize the BMC now.
    //

    Status = IpmiInitializeBmc (mIpmiInstance);
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_ERROR, "[IPMI] Failed to initialize BMC. %r\n", Status));
      return Status;
    }
  } else {
    BmcHob                   = (IPMI_BMC_HOB *)GET_GUID_HOB_DATA (GuidHob);
    mIpmiInstance->BmcStatus = BmcHob->BmcStatus;
    DEBUG ((DEBUG_INFO, "[IPMI] Found IPMI BMC HOB. BMC Status = 0x%d\n", BmcHob->BmcStatus));
  }

  Handle = NULL;
  DEBUG ((DEBUG_INFO, "[IPMI] Installing MM protocol!\n"));
  Status = gMmst->MmInstallProtocolInterface (
                    &Handle,
                    &gSmmIpmiTransportProtocolGuid,
                    EFI_NATIVE_INTERFACE,
                    &mIpmiInstance->IpmiTransport
                    );
  ASSERT_EFI_ERROR (Status);

  return Status;
}
