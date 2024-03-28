/** @file
  Generic IPMI stack during PEI phase.

  @copyright
  Copyright 2017 - 2021 Intel Corporation. <BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#include <PiPei.h>
#include <Uefi.h>

#include <Ppi/IpmiTransportPpi.h>

#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/IoLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/PcdLib.h>
#include <Library/PeiServicesLib.h>
#include <Library/PeiServicesTablePointerLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/TimerLib.h>
#include <Library/HobLib.h>
#include <Library/ReportStatusCodeLib.h>
#include <Library/IpmiPlatformLib.h>

#include <IndustryStandard/Ipmi.h>

#include <GenericIpmi.h>

//
// Static definitions for the IPMI PEIM
//

/**
  The entry point of the Ipmi PEIM. Installs Ipmi PPI interface.

  @param[in]  FileHandle    Handle of the file being invoked.
  @param[in]  PeiServices   Describes the list of possible PEI Services.

  @retval EFI_SUCCESS       Indicates that Ipmi initialization completed
                            successfully.
**/
EFI_STATUS
EFIAPI
PeimIpmiInterfaceInit (
  IN       EFI_PEI_FILE_HANDLE  FileHandle,
  IN CONST EFI_PEI_SERVICES     **PeiServices
  )
{
  EFI_STATUS              Status;
  IPMI_BMC_INSTANCE_DATA  *mIpmiInstance;
  EFI_PEI_PPI_DESCRIPTOR  *mPeiIpmiBmcDataDesc;
  IPMI_BMC_HOB            *BmcHob;

  mIpmiInstance = NULL;

  //
  // Make one allocation for both the PPI descriptor and the Bmc Instance data
  //
  mIpmiInstance = AllocateZeroPool (sizeof (IPMI_BMC_INSTANCE_DATA) + sizeof (EFI_PEI_PPI_DESCRIPTOR));
  if (mIpmiInstance == NULL) {
    DEBUG ((EFI_D_ERROR, "[IPMI] EFI_OUT_OF_RESOURCES of memory allocation\n"));
    return EFI_OUT_OF_RESOURCES;
  }

  mPeiIpmiBmcDataDesc = (EFI_PEI_PPI_DESCRIPTOR *)((UINT8 *)mIpmiInstance + sizeof (IPMI_BMC_INSTANCE_DATA));

  //
  // Calibrate TSC Counter.  Stall for 10ms, then multiply the resulting number of
  // ticks in that period by 100 to get the number of ticks in a 1 second timeout
  //
  DEBUG ((DEBUG_INFO, "[IPMI] IPMI STACK Initialization\n"));
  mIpmiInstance->IpmiTimeoutPeriod = (PcdGet8 (PcdIpmiCommandTimeoutSeconds) *1000*1000) / IPMI_DELAY_UNIT;
  DEBUG ((DEBUG_INFO, "[IPMI] IpmiTimeoutPeriod = 0x%x\n", mIpmiInstance->IpmiTimeoutPeriod));

  //
  // Initialize IPMI IO Base.
  //
  mIpmiInstance->Signature                       = SM_IPMI_BMC_SIGNATURE;
  mIpmiInstance->SlaveAddress                    = BMC_SLAVE_ADDRESS;
  mIpmiInstance->BmcStatus                       = BMC_NOTREADY;
  mIpmiInstance->IpmiTransport.IpmiSubmitCommand = IpmiSendCommand;
  mIpmiInstance->IpmiTransport.GetBmcStatus      = IpmiGetBmcStatus;

  //
  // Initialize the Ppi descriptor
  //
  mPeiIpmiBmcDataDesc->Flags = EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST;
  mPeiIpmiBmcDataDesc->Guid  = &gPeiIpmiTransportPpiGuid;
  mPeiIpmiBmcDataDesc->Ppi   = &mIpmiInstance->IpmiTransport;

  //
  // Initialize the transport layer.
  //

  Status = InitializeIpmiTransportHardware ();
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "[IPMI] InitializeIpmiTransportHardware failed - %r!\n", Status));
    return Status;
  }

  //
  // Initialize the BMC state.
  //
  Status = IpmiInitializeBmc (mIpmiInstance);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "[IPMI] Failed to initialize BMC state. %r\n", Status));
  }

  //
  // Initialize the HOB for the DXE phase.
  //

  BmcHob = BuildGuidHob (&gIpmiBmcHobGuid, sizeof (*BmcHob));
  if (BmcHob == NULL) {
    DEBUG ((DEBUG_ERROR, "[IPMI] Failed to create BMC hob!\n"));
    return EFI_OUT_OF_RESOURCES;
  }

  BmcHob->BmcStatus = mIpmiInstance->BmcStatus;

  //
  // Do not continue initialization if the BMC is in Force Update Mode.
  //
  if ((mIpmiInstance->BmcStatus == BMC_UPDATE_IN_PROGRESS) || (mIpmiInstance->BmcStatus == BMC_HARDFAIL)) {
    return EFI_UNSUPPORTED;
  }

  //
  // Just produce PPI
  //
  Status = PeiServicesInstallPpi (mPeiIpmiBmcDataDesc);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  return EFI_SUCCESS;
}
