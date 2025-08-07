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
  IPMI_BMC_INSTANCE_DATA  *IpmiInstance;
  EFI_PEI_PPI_DESCRIPTOR  *PeiIpmiBmcDataDesc;
  IPMI_BMC_HOB            *BmcHob;

  IpmiInstance = NULL;
  Status       = PeiServicesRegisterForShadow (FileHandle);
  if (!EFI_ERROR (Status)) {
    //
    // Make one allocation for both the PPI descriptor and the Bmc Instance data
    //
    IpmiInstance = AllocateZeroPool (sizeof (IPMI_BMC_INSTANCE_DATA) + sizeof (EFI_PEI_PPI_DESCRIPTOR));
    if (IpmiInstance == NULL) {
      DEBUG ((EFI_D_ERROR, "[IPMI] EFI_OUT_OF_RESOURCES of memory allocation\n"));
      return EFI_OUT_OF_RESOURCES;
    }

    PeiIpmiBmcDataDesc = (EFI_PEI_PPI_DESCRIPTOR *)((UINT8 *)IpmiInstance + sizeof (IPMI_BMC_INSTANCE_DATA));

    //
    // Calibrate TSC Counter.  Stall for 10ms, then multiply the resulting number of
    // ticks in that period by 100 to get the number of ticks in a 1 second timeout
    //
    DEBUG ((DEBUG_INFO, "[IPMI] IPMI STACK Initialization\n"));
    IpmiInstance->IpmiTimeoutPeriod = (PcdGet8 (PcdIpmiCommandTimeoutSeconds) *1000*1000) / IPMI_DELAY_UNIT;
    DEBUG ((DEBUG_INFO, "[IPMI] IpmiTimeoutPeriod = 0x%x\n", IpmiInstance->IpmiTimeoutPeriod));

    //
    // Initialize IPMI IO Base.
    //
    IpmiInstance->Signature                       = SM_IPMI_BMC_SIGNATURE;
    IpmiInstance->SlaveAddress                    = BMC_SLAVE_ADDRESS;
    IpmiInstance->BmcStatus                       = BMC_NOTREADY;
    IpmiInstance->IpmiTransport.IpmiSubmitCommand = IpmiSendCommand;
    IpmiInstance->IpmiTransport.GetBmcStatus      = IpmiGetBmcStatus;

    //
    // Initialize the Ppi descriptor
    //
    PeiIpmiBmcDataDesc->Flags = EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST;
    PeiIpmiBmcDataDesc->Guid  = &gPeiIpmiTransportPpiGuid;
    PeiIpmiBmcDataDesc->Ppi   = &IpmiInstance->IpmiTransport;

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
    Status = IpmiInitializeBmc (IpmiInstance);
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

    BmcHob->BmcStatus = IpmiInstance->BmcStatus;

    //
    // Do not continue initialization if the BMC is in Force Update Mode.
    //
    if ((IpmiInstance->BmcStatus == BMC_UPDATE_IN_PROGRESS) || (IpmiInstance->BmcStatus == BMC_HARDFAIL)) {
      return EFI_UNSUPPORTED;
    }

    //
    // Just produce PPI
    //
    Status = PeiServicesInstallPpi (PeiIpmiBmcDataDesc);
    if (EFI_ERROR (Status)) {
      return Status;
    }
  } else if (Status == EFI_ALREADY_STARTED) {
    // This is the execution of the entrypoint after it was shadowed.

    IPMI_BMC_INSTANCE_DATA  *OldIpmiInstance;
    EFI_PEI_PPI_DESCRIPTOR  *OldPeiIpmiBmcDataDesc;
    EFI_PEI_HOB_POINTERS    Hob;

    // Locate the existing PPI
    Status = PeiServicesLocatePpi (&gPeiIpmiTransportPpiGuid, 0, &OldPeiIpmiBmcDataDesc, (VOID **)&OldIpmiInstance);
    if (!EFI_ERROR (Status)) {
      // Find the existing allocation by searching memory pool hobs
      Hob.Raw = GetHobList ();
      while ((Hob.Raw = GetNextHob (EFI_HOB_TYPE_MEMORY_POOL, Hob.Raw)) != NULL) {
        // First verify that the size of the hob is the exact size that was allocated
        //  above.
        // Calculate the correct pointer to the IPMI instance data
        // The Pool HOB contains the data immediately after the HOB header
        OldIpmiInstance = (IPMI_BMC_INSTANCE_DATA *)(Hob.Pool + 1);

        if ((Hob.Pool->Header.HobLength - sizeof (EFI_HOB_MEMORY_POOL) >= sizeof (UINTN)) &&
            (OldIpmiInstance->Signature == SM_IPMI_BMC_SIGNATURE))
        {
          // Found the allocation, update the pointers to the shadowed memory locations
          IpmiInstance                                  = OldIpmiInstance;
          IpmiInstance->IpmiTransport.IpmiSubmitCommand = IpmiSendCommand;
          IpmiInstance->IpmiTransport.GetBmcStatus      = IpmiGetBmcStatus;

          // The PPI descriptor is located after the IPMI instance data
          PeiIpmiBmcDataDesc      = (EFI_PEI_PPI_DESCRIPTOR *)((UINT8 *)IpmiInstance + sizeof (IPMI_BMC_INSTANCE_DATA));
          PeiIpmiBmcDataDesc->Ppi = (VOID *)&IpmiInstance->IpmiTransport;

          Status = PeiServicesReInstallPpi (OldPeiIpmiBmcDataDesc, PeiIpmiBmcDataDesc);

          DEBUG ((DEBUG_INFO, "%a - Reinstalling gPeiIpmiTransportPpiGuid - %r\n", __func__, Status));
          return EFI_SUCCESS;
        }

        Hob.Raw = GET_NEXT_HOB (Hob);
      }

      DEBUG ((DEBUG_ERROR, "%a - Failed to shadow gPeiIpmiTransportPpiGuid\n", __func__));
    }
  }

  return EFI_SUCCESS;
}
