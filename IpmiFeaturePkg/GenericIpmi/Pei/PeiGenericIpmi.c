/** @file
  Generic IPMI stack during PEI phase.

  @copyright
  Copyright 2017 - 2021 Intel Corporation. <BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#include "PeiGenericIpmi.h"
#include <IndustryStandard/Ipmi.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/HobLib.h>
#include <Library/ReportStatusCodeLib.h>
#include <Library/IpmiPlatformLib.h>

///////////////////////////////////////////////////////////////////////////////
// Function Implementations
//

/*****************************************************************************
 @brief
  Internal function

 @param[in] PeiServices          General purpose services available to every PEIM.

 @retval EFI_SUCCESS             Always return EFI_SUCCESS
**/
EFI_STATUS
EFIAPI
PeiInitializeIpmiPhysicalLayer (
  IN CONST EFI_PEI_SERVICES  **PeiServices
  )
{
  EFI_STATUS              Status;
  IPMI_BMC_INSTANCE_DATA  *mIpmiInstance;
  EFI_PEI_PPI_DESCRIPTOR  *mPeiIpmiBmcDataDesc;
  IPMI_BMC_HOB            *BmcHob;

  mIpmiInstance = NULL;

  //
  // Send Pre-Boot signal to BMC
  //
  if (PcdGetBool (PcdSignalPreBootToBmc)) {
    Status = SendPreBootSignaltoBmc (PeiServices);
    if (EFI_ERROR (Status)) {
      return Status;
    }
  }

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
  mIpmiInstance->IpmiTimeoutPeriod = (BMC_IPMI_TIMEOUT_PEI *1000*1000) / IPMI_DELAY_UNIT_PEI;
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
    return Status;
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

/*****************************************************************************
 @bref
  PRE-BOOT signal will be sent in very early PEI phase, to enable necessary KCS access for host boot.

  @param[in] PeiServices          General purpose services available to every PEIM.

  @retval EFI_SUCCESS   Indicates that the signal is sent successfully.
**/
EFI_STATUS
SendPreBootSignaltoBmc (
  IN CONST EFI_PEI_SERVICES  **PeiServices
  )
{
  EFI_STATUS          Status;
  EFI_PEI_CPU_IO_PPI  *CpuIoPpi;
  UINT32              ProvisionPort = 0;
  UINT8               PreBoot       = 0;

  //
  // Locate CpuIo service
  //
  CpuIoPpi      = (**PeiServices).CpuIo;
  ProvisionPort = PcdGet32 (PcdSioMailboxBaseAddress) + MBXDAT_B;
  PreBoot       = 0x01;// PRE-BOOT

  Status = CpuIoPpi->Io.Write (
                          PeiServices,
                          CpuIoPpi,
                          EfiPeiCpuIoWidthUint8,
                          ProvisionPort,
                          1,
                          &PreBoot
                          );
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "SendPreBootSignaltoBmc () Write PRE-BOOT Status=%r\n", Status));
    return Status;
  }

  return EFI_SUCCESS;
}

/*****************************************************************************
 @bref
  The entry point of the Ipmi PEIM. Instals Ipmi PPI interface.

  @param  FileHandle  Handle of the file being invoked.
  @param  PeiServices Describes the list of possible PEI Services.

  @retval EFI_SUCCESS   Indicates that Ipmi initialization completed successfully.
**/
EFI_STATUS
EFIAPI
PeimIpmiInterfaceInit (
  IN       EFI_PEI_FILE_HANDLE  FileHandle,
  IN CONST EFI_PEI_SERVICES     **PeiServices
  )
{
  // EFI_STATUS  Status;    // MU_CHANGE - Unused.

  //
  // Performing Ipmi physical layer initialization
  //
  PeiInitializeIpmiPhysicalLayer (PeiServices);                // MU_CHANGE

  return EFI_SUCCESS;
} // PeimIpmiInterfaceInit()
