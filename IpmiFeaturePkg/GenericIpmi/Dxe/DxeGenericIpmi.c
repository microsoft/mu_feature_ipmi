/** @file
  Generic IPMI DXE protocol driver. This modules checks basic information from
  the BMC and initializes the IPMI stack for DXE.

  @copyright
  Copyright 1999 - 2021 Intel Corporation. <BR>
  Copyright (c) Microsoft Corporation
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#include <Library/UefiLib.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/UefiDriverEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/PcdLib.h>
#include <Library/TimerLib.h>
#include <Library/ReportStatusCodeLib.h>
#include <Library/HobLib.h>

#include <IndustryStandard/Ipmi.h>
#include <SmStatusCodes.h>

#include <GenericIpmi.h>
#include <Library/IpmiPlatformLib.h>

//
// Global state for the IPMI instance.
//

IPMI_BMC_INSTANCE_DATA  *mIpmiInstance = NULL;

/**
 @brief
  This is entry point for IPMI service for DXE. Initializes the BMC information
  and prepares transport and protocol for use. Depending on the configuration,
  this routine may also validate the BMC is ready for use.

 @param[in] ImageHandle  A handle to driver image.
 @param[in] SystemTable  A pointer to system table.

 @retval EFI_SUCCESS      The function completed successfully.
 @retval EFI_UNSUPPORTED  IPMI is not available.
**/
EFI_STATUS
EFIAPI
DxeGenericIpmiEntry (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS         Status;
  EFI_HANDLE         Handle;
  IPMI_BMC_HOB       *BmcHob;
  EFI_HOB_GUID_TYPE  *GuidHob;

  mIpmiInstance = AllocateRuntimeZeroPool (sizeof (*mIpmiInstance));
  if (mIpmiInstance == NULL) {
    DEBUG ((DEBUG_ERROR, "[IPMI] Failed to allocate IPMI instance data!\n"));
    ASSERT_EFI_ERROR (EFI_OUT_OF_RESOURCES);
    return EFI_OUT_OF_RESOURCES;
  }

  //
  // Initialize the transaction timeout. Convert the seconds to microseconds
  // then divide by the stall period.
  //

  mIpmiInstance->IpmiTimeoutPeriod =
    (FixedPcdGet8 (PcdIpmiCommandTimeoutSeconds) * (1000*1000)) / IPMI_DELAY_UNIT;

  DEBUG ((DEBUG_INFO, "[IPMI] mIpmiInstance->IpmiTimeoutPeriod: 0x%llx\n", mIpmiInstance->IpmiTimeoutPeriod));

  //
  // Initialize IPMI IO Base.
  //
  mIpmiInstance->Signature                       = SM_IPMI_BMC_SIGNATURE;
  mIpmiInstance->SlaveAddress                    = BMC_SLAVE_ADDRESS;
  mIpmiInstance->BmcStatus                       = BMC_NOTREADY;
  mIpmiInstance->IpmiTransport.IpmiSubmitCommand = IpmiSendCommand;
  mIpmiInstance->IpmiTransport.GetBmcStatus      = IpmiGetBmcStatus;

  //
  // Initialize the transport layer.
  //

  Status = InitializeIpmiTransportHardware ();
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "[IPMI] InitializeIpmiTransportHardware failed - %r!\n", Status));
    return Status;
  }

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

  //
  // Now install the Protocol if the BMC is not in a HardFail State and not in Force Update mode
  //
  if ((mIpmiInstance->BmcStatus != BMC_HARDFAIL) &&
      (mIpmiInstance->BmcStatus != BMC_UPDATE_IN_PROGRESS))
  {
    Handle = NULL;
    DEBUG ((DEBUG_INFO, "[IPMI] Installing DXE protocol!\n"));
    Status = gBS->InstallProtocolInterface (
                    &Handle,
                    &gIpmiTransportProtocolGuid,
                    EFI_NATIVE_INTERFACE,
                    &mIpmiInstance->IpmiTransport
                    );
    ASSERT_EFI_ERROR (Status);
  }

  return EFI_SUCCESS;
}
