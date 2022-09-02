/** @file
  Generic IPMI stack during PEI phase.

  @copyright
  Copyright 2017 - 2021 Intel Corporation. <BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#include "PeiGenericIpmi.h"
#include <IndustryStandard/Ipmi.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/ReportStatusCodeLib.h>
#include <Library/IpmiPlatformHookLib.h>

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
    DEBUG ((EFI_D_ERROR, "IPMI Peim:EFI_OUT_OF_RESOURCES of memory allocation\n"));
    return EFI_OUT_OF_RESOURCES;
  }

  mPeiIpmiBmcDataDesc = (EFI_PEI_PPI_DESCRIPTOR *)((UINT8 *)mIpmiInstance + sizeof (IPMI_BMC_INSTANCE_DATA));

  //
  // Calibrate TSC Counter.  Stall for 10ms, then multiply the resulting number of
  // ticks in that period by 100 to get the number of ticks in a 1 second timeout
  //
  DEBUG ((DEBUG_INFO, "IPMI Peim:IPMI STACK Initialization\n"));
  mIpmiInstance->IpmiTimeoutPeriod = (BMC_IPMI_TIMEOUT_PEI *1000*1000) / IPMI_DELAY_UNIT_PEI;
  DEBUG ((EFI_D_INFO, "IPMI Peim:IpmiTimeoutPeriod = 0x%x\n", mIpmiInstance->IpmiTimeoutPeriod));

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
    DEBUG ((DEBUG_ERROR, "IPMI: InitializeIpmiTransportHardware failed - %r!\n", Status));
    return Status;
  }

  //
  // Get the Device ID and check if the system is in Force Update mode.
  //
  Status = GetDeviceId (mIpmiInstance);
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "IPMI Peim:Get BMC Device Id Failed. Status=%r\n", Status));
    return Status;
  }

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

EFI_STATUS
GetDeviceId (
  IN      IPMI_BMC_INSTANCE_DATA  *mIpmiInstance
  )

/*++

Routine Description:
  Execute the Get Device ID command to determine whether or not the BMC is in Force Update
  Mode.  If it is, then report it to the error manager.

Arguments:
  mIpmiInstance   - Data structure describing BMC variables and used for sending commands
  StatusCodeValue - An array used to accumulate error codes for later reporting.
  ErrorCount      - Counter used to keep track of error codes in StatusCodeValue

Returns:
  Status

--*/
{
  EFI_STATUS    Status;
  UINT32        DataSize;
  SM_CTRL_INFO  *pBmcInfo;
  UINTN         Retries;

  //
  // Set up a loop to retry for up to PcdIpmiBmcReadyDelayTimer seconds. Calculate retries not timeout
  // so that in case KCS is not enabled and IpmiSendCommand() returns
  // immediately we will not wait all the PcdIpmiBmcReadyDelayTimer seconds.
  //
  Retries = PcdGet8 (PcdIpmiBmcReadyDelayTimer);
  //
  // Get the device ID information for the BMC.
  //
  DataSize = sizeof (mIpmiInstance->TempData);
  while (EFI_ERROR (
           Status = IpmiSendCommand (
                      &mIpmiInstance->IpmiTransport,
                      IPMI_NETFN_APP,
                      0,
                      IPMI_APP_GET_DEVICE_ID,
                      NULL,
                      0,
                      mIpmiInstance->TempData,
                      &DataSize
                      )
           ))
  {
    DEBUG ((
      EFI_D_ERROR,
      "[IPMI] BMC does not respond (status: %r), %d retries left\n",
      Status,
      Retries
      ));

    if (Retries-- == 0) {
      ReportStatusCode (EFI_ERROR_CODE | EFI_ERROR_MAJOR, EFI_COMPUTING_UNIT_FIRMWARE_PROCESSOR | EFI_CU_FP_EC_COMM_ERROR);
      mIpmiInstance->BmcStatus = BMC_HARDFAIL;
      return Status;
    }

    //
    // Handle the case that BMC FW still not enable KCS channel after AC cycle. just stall 1 second
    //
    MicroSecondDelay (1*1000*1000);
  }

  pBmcInfo = (SM_CTRL_INFO *)&mIpmiInstance->TempData[0];
  DEBUG ((
    DEBUG_INFO,
    "[IPMI PEI] BMC Device ID: 0x%02X, firmware version: %d.%02X UpdateMode:%x\n",
    pBmcInfo->DeviceId,
    pBmcInfo->MajorFirmwareRev,
    pBmcInfo->MinorFirmwareRev,
    pBmcInfo->UpdateMode
    ));
  //
  // In OpenBMC, UpdateMode: the bit 7 of byte 4 in get device id command is used for the BMC status:
  // 0 means BMC is ready, 1 means BMC is not ready.
  // At the very beginning of BMC power on, the status is 1 means BMC is in booting process and not ready. It is not the flag for force update mode.
  //
  if (pBmcInfo->UpdateMode == BMC_READY) {
    mIpmiInstance->BmcStatus = BMC_OK;
    return EFI_SUCCESS;
  } else {
    //
    // Updatemode = 1 mean BMC is not ready, continue waiting.
    //
    while (Retries-- != 0) {
      MicroSecondDelay (1*1000*1000); // delay 1 seconds
      DEBUG ((DEBUG_INFO, "[IPMI PEI] UpdateMode Retries:%x \n", Retries));
      Status = IpmiSendCommand (
                 &mIpmiInstance->IpmiTransport,
                 IPMI_NETFN_APP,
                 0,
                 IPMI_APP_GET_DEVICE_ID,
                 NULL,
                 0,
                 mIpmiInstance->TempData,
                 &DataSize
                 );
      if (!EFI_ERROR (Status)) {
        pBmcInfo = (SM_CTRL_INFO *)&mIpmiInstance->TempData[0];
        DEBUG ((DEBUG_INFO, "[IPMI PEI] UpdateMode Retries:%x   pBmcInfo->UpdateMode:%x\n", Retries, pBmcInfo->UpdateMode));
        if (pBmcInfo->UpdateMode == BMC_READY) {
          mIpmiInstance->BmcStatus = BMC_OK;
          return EFI_SUCCESS;
        }
      }
    }
  }

  mIpmiInstance->BmcStatus = BMC_HARDFAIL;
  return Status;
} // GetDeviceId()
