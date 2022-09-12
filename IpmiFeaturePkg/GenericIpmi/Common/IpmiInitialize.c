/** @file
  Module for initializing the IPMI communication with the BMC.

  Copyright (c) Microsoft Corporation
  Copyright 1999 - 2021 Intel Corporation. <BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Uefi.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/PcdLib.h>
#include <Library/TimerLib.h>
#include <Library/ReportStatusCodeLib.h>
#include <Library/IpmiPlatformLib.h>

#include <IndustryStandard/Ipmi.h>
#include <SmStatusCodes.h>

#include <GenericIpmi.h>

EFI_STATUS
GetSelfTest (
  IN      IPMI_BMC_INSTANCE_DATA  *IpmiInstance,
  IN      EFI_STATUS_CODE_VALUE   StatusCodeValue[],
  IN OUT  UINT8                   *ErrorCount
  )

/*++

Routine Description:

  Execute the Get Self Test results command to determine whether or not the BMC self tests
  have passed

Arguments:

  IpmiInstance    - Data structure describing BMC variables and used for sending commands
  StatusCodeValue - An array used to accumulate error codes for later reporting.
  ErrorCount      - Counter used to keep track of error codes in StatusCodeValue

Returns:

  EFI_SUCCESS       - BMC Self test results are retrieved and saved into BmcStatus
  EFI_DEVICE_ERROR  - BMC failed to return self test results.

--*/
{
  EFI_STATUS                      Status;
  UINT32                          DataSize;
  UINT8                           Index;
  UINT8                           *TempPtr;
  UINT32                          Retries;
  BOOLEAN                         bResultFlag = FALSE;
  IPMI_SELF_TEST_RESULT_RESPONSE  *SelfTestResult;

  SelfTestResult = (VOID *)&IpmiInstance->TempData[0];

  //
  // Get the SELF TEST Results.
  //
  //
  // Note: If BMC PcdIpmiBmcReadyDelayTimer < BMC_IPMI_TIMEOUT, it need set Retries as 1. Otherwise it will make SELT failure, caused by below condition (EFI_ERROR(Status) || Retries == 0)
  //
  if (PcdGet8 (PcdIpmiBmcReadyDelayTimer) < PcdGet8 (PcdBmcTimeoutSeconds)) {
    Retries = 1;
  } else {
    Retries = PcdGet8 (PcdIpmiBmcReadyDelayTimer);
  }

  DataSize = sizeof (IpmiInstance->TempData);

  IpmiInstance->TempData[1] = 0;

  do {
    Status = IpmiSendCommand (
               &IpmiInstance->IpmiTransport,
               IPMI_NETFN_APP,
               0,
               IPMI_APP_GET_SELFTEST_RESULTS,
               NULL,
               0,
               IpmiInstance->TempData,
               &DataSize
               );

    if (Status == EFI_SUCCESS) {
      switch (SelfTestResult->Result) {
        case IPMI_APP_SELFTEST_NO_ERROR:
        case IPMI_APP_SELFTEST_NOT_IMPLEMENTED:
        case IPMI_APP_SELFTEST_ERROR:
        case IPMI_APP_SELFTEST_FATAL_HW_ERROR:
          bResultFlag = TRUE;
          break;

        default:
          break;
      } // switch

      if (bResultFlag) {
        break;
      }
    }

    MicroSecondDelay (500 * 1000);
  } while (--Retries > 0);

  //
  // If Status indicates a Device error, then the BMC is not responding, so send an error.
  //
  if (EFI_ERROR (Status) || (Retries == 0)) {
    DEBUG ((DEBUG_ERROR, "\n[IPMI]  BMC self-test does not respond (status: %r)!\n\n", Status));
    if (*ErrorCount < MAX_SOFT_COUNT) {
      StatusCodeValue[*ErrorCount] = EFI_COMPUTING_UNIT_FIRMWARE_PROCESSOR | EFI_CU_FP_EC_COMM_ERROR;
      (*ErrorCount)++;
    }

    IpmiInstance->BmcStatus = BMC_HARDFAIL;
    return Status;
  } else {
    DEBUG ((DEBUG_INFO, "[IPMI] BMC self-test result: %02X-%02X\n", SelfTestResult->Result, SelfTestResult->Param));
    //
    // Copy the Self test results to Error Status.  Data will be copied as long as it
    // does not exceed the size of the ErrorStatus variable.
    //
    for (Index = 0, TempPtr = (UINT8 *)&IpmiInstance->ErrorStatus;
         (Index < DataSize) && (Index < sizeof (IpmiInstance->ErrorStatus));
         Index++, TempPtr++
         )
    {
      *TempPtr = IpmiInstance->TempData[Index];
    }

    //
    // Check the IPMI defined self test results.
    // Additional Cases are device specific test results.
    //
    switch (SelfTestResult->Result) {
      case IPMI_APP_SELFTEST_NO_ERROR:
      case IPMI_APP_SELFTEST_NOT_IMPLEMENTED:
        IpmiInstance->BmcStatus = BMC_OK;
        break;

      case IPMI_APP_SELFTEST_ERROR:
        //
        // Three of the possible errors result in BMC hard failure; FRU Corruption,
        // BootBlock Firmware corruption, and Operational Firmware Corruption.  All
        // other errors are BMC soft failures.
        //
        if ((IpmiInstance->TempData[1] & (IPMI_APP_SELFTEST_FRU_CORRUPT | IPMI_APP_SELFTEST_FW_BOOTBLOCK_CORRUPT | IPMI_APP_SELFTEST_FW_CORRUPT)) != 0) {
          IpmiInstance->BmcStatus = BMC_HARDFAIL;
        } else {
          IpmiInstance->BmcStatus = BMC_SOFTFAIL;
        }

        //
        // Check if SDR repository is empty and report it if it is.
        //
        if ((IpmiInstance->TempData[1] & IPMI_APP_SELFTEST_SDR_REPOSITORY_EMPTY) != 0) {
          if (*ErrorCount < MAX_SOFT_COUNT) {
            StatusCodeValue[*ErrorCount] = EFI_COMPUTING_UNIT_FIRMWARE_PROCESSOR | CU_FP_EC_SDR_EMPTY;
            (*ErrorCount)++;
          }
        }

        break;

      case IPMI_APP_SELFTEST_FATAL_HW_ERROR:
        IpmiInstance->BmcStatus = BMC_HARDFAIL;
        break;

      default:
        //
        // Call routine to check device specific failures.
        //
        GetDeviceSpecificTestResults (SelfTestResult, &IpmiInstance->BmcStatus);
    }

    if (IpmiInstance->BmcStatus == BMC_HARDFAIL) {
      if (*ErrorCount < MAX_SOFT_COUNT) {
        StatusCodeValue[*ErrorCount] = EFI_COMPUTING_UNIT_FIRMWARE_PROCESSOR | EFI_CU_FP_EC_HARD_FAIL;
        (*ErrorCount)++;
      }
    } else if (IpmiInstance->BmcStatus == BMC_SOFTFAIL) {
      if (*ErrorCount < MAX_SOFT_COUNT) {
        StatusCodeValue[*ErrorCount] = EFI_COMPUTING_UNIT_FIRMWARE_PROCESSOR | EFI_CU_FP_EC_SOFT_FAIL;
        (*ErrorCount)++;
      }
    }
  }

  return EFI_SUCCESS;
} // GetSelfTest()

EFI_STATUS
GetDeviceId (
  IN      IPMI_BMC_INSTANCE_DATA  *IpmiInstance,
  IN      EFI_STATUS_CODE_VALUE   StatusCodeValue[],
  IN OUT  UINT8                   *ErrorCount
  )

/*++

Routine Description:
  Execute the Get Device ID command to determine whether or not the BMC is in Force Update
  Mode.  If it is, then report it to the error manager.

Arguments:
  IpmiInstance    - Data structure describing BMC variables and used for sending commands
  StatusCodeValue - An array used to accumulate error codes for later reporting.
  ErrorCount      - Counter used to keep track of error codes in StatusCodeValue

Returns:
  Status

--*/
{
  EFI_STATUS                 Status;
  UINT32                     DataSize;
  SM_CTRL_INFO               *pBmcInfo;
  IPMI_MSG_GET_BMC_EXEC_RSP  *pBmcExecContext;
  UINT32                     Retries;

  //
  // Set up a loop to retry for up to PcdIpmiBmcReadyDelayTimer seconds. Calculate retries not timeout
  // so that in case KCS is not enabled and IpmiSendCommand() returns
  // immediately we will not wait all the PcdIpmiBmcReadyDelayTimer seconds.
  //
  Retries = PcdGet8 (PcdIpmiBmcReadyDelayTimer);
  //
  // Get the device ID information for the BMC.
  //
  DataSize = sizeof (IpmiInstance->TempData);
  DEBUG ((DEBUG_INFO, "[IPMI] Getting BMC Device ID. DataSize: 0x%x Retries: %d\n", DataSize, Retries));
  while (EFI_ERROR (
           Status = IpmiSendCommand (
                      &IpmiInstance->IpmiTransport,
                      IPMI_NETFN_APP,
                      0,
                      IPMI_APP_GET_DEVICE_ID,
                      NULL,
                      0,
                      IpmiInstance->TempData,
                      &DataSize
                      )
           )
         )
  {
    DEBUG ((
      DEBUG_ERROR,
      "[IPMI] BMC does not respond by Get BMC DID (status: %r), %d retries left.\n",
      Status,
      Retries
      ));

    if (Retries-- == 0) {
      IpmiInstance->BmcStatus = BMC_HARDFAIL;
      return Status;
    }

    //
    // Handle the case that BMC FW still not enable KCS channel after AC cycle. just stall 1 second
    //
    MicroSecondDelay (1*1000*1000);
  }

  pBmcInfo = (SM_CTRL_INFO *)&IpmiInstance->TempData[0];
  DEBUG ((DEBUG_INFO, "[IPMI] BMC Device ID: 0x%02X, firmware version: %d.%02X UpdateMode:%x\n", pBmcInfo->DeviceId, pBmcInfo->MajorFirmwareRev, pBmcInfo->MinorFirmwareRev, pBmcInfo->UpdateMode));
  //
  // In OpenBMC, UpdateMode: the bit 7 of byte 4 in get device id command is used for the BMC status:
  // 0 means BMC is ready, 1 means BMC is not ready.
  // At the very beginning of BMC power on, the status is 1 means BMC is in booting process and not ready. It is not the flag for force update mode.
  //
  if (pBmcInfo->UpdateMode == BMC_READY) {
    IpmiInstance->BmcStatus = BMC_OK;
    return EFI_SUCCESS;
  } else {
    Status = IpmiSendCommand (
               &IpmiInstance->IpmiTransport,
               IPMI_NETFN_FIRMWARE,
               0,
               IPMI_GET_BMC_EXECUTION_CONTEXT,
               NULL,
               0,
               IpmiInstance->TempData,
               &DataSize
               );

    pBmcExecContext = (IPMI_MSG_GET_BMC_EXEC_RSP *)&IpmiInstance->TempData[0];
    DEBUG ((DEBUG_INFO, "[IPMI] Operational status of BMC: 0x%x\n", pBmcExecContext->CurrentExecutionContext));
    if (!EFI_ERROR (Status) &&
        (pBmcExecContext->CurrentExecutionContext == IPMI_BMC_IN_FORCED_UPDATE_MODE))
    {
      DEBUG ((DEBUG_WARN, "[IPMI] BMC in Forced Update mode, skip waiting for BMC_READY.\n"));
      IpmiInstance->BmcStatus = BMC_UPDATE_IN_PROGRESS;
    } else {
      //
      // Updatemode = 1 mean BMC is not ready, continue waiting.
      //
      Retries = PcdGet8 (PcdIpmiBmcReadyDelayTimer);
      while (Retries-- != 0) {
        MicroSecondDelay (1*1000*1000); // delay 1 seconds
        DEBUG ((DEBUG_ERROR, "[IPMI] UpdateMode Retries: %d \n", Retries));
        Status = IpmiSendCommand (
                   &IpmiInstance->IpmiTransport,
                   IPMI_NETFN_APP,
                   0,
                   IPMI_APP_GET_DEVICE_ID,
                   NULL,
                   0,
                   IpmiInstance->TempData,
                   &DataSize
                   );

        if (!EFI_ERROR (Status)) {
          pBmcInfo = (SM_CTRL_INFO *)&IpmiInstance->TempData[0];
          DEBUG ((EFI_D_ERROR, "[IPMI] UpdateMode Retries: %d   pBmcInfo->UpdateMode:%x, Status: %r, Response Data: 0x%lx\n", Retries, pBmcInfo->UpdateMode, Status, IpmiInstance->TempData));
          if (pBmcInfo->UpdateMode == BMC_READY) {
            IpmiInstance->BmcStatus = BMC_OK;
            return EFI_SUCCESS;
          }
        }
      }

      IpmiInstance->BmcStatus = BMC_HARDFAIL;
    }
  }

  return Status;
}

EFI_STATUS
EFIAPI
IpmiInitializeBmc (
  IN IPMI_BMC_INSTANCE_DATA  *IpmiInstance
  )

{
  EFI_STATUS             Status;
  UINT8                  Index;
  UINT8                  ErrorCount;
  EFI_STATUS_CODE_VALUE  StatusCodeValue[MAX_SOFT_COUNT];

  ErrorCount = 0;

  //
  // Get the Device ID and check if the system is in Force Update mode.
  //

  Status = GetDeviceId (
             IpmiInstance,
             StatusCodeValue,
             &ErrorCount
             );

  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a: Failed to get device ID. %r\n", __FUNCTION__, Status));
    return Status;
  }

  //
  // Do not continue initialization if the BMC is in Force Update Mode.
  //
  if (PcdGetBool (PcdIpmiCheckSelfTestResults) &&
      (IpmiInstance->BmcStatus != BMC_UPDATE_IN_PROGRESS) &&
      (IpmiInstance->BmcStatus != BMC_HARDFAIL))
  {
    //
    // Get the SELF TEST Results.
    //
    Status = GetSelfTest (
               IpmiInstance,
               StatusCodeValue,
               &ErrorCount
               );

    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_ERROR, "%a: Failed to get self test results. %r\n", __FUNCTION__, Status));
      return Status;
    }
  }

  //
  // Iterate through the errors reporting them to the error manager.
  //

  for (Index = 0; Index < ErrorCount; Index++) {
    ReportStatusCode (
      EFI_ERROR_CODE | EFI_ERROR_MAJOR,
      StatusCodeValue[Index]
      );
  }

  return Status;
}
