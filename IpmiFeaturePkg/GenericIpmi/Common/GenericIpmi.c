/** @file
  IPMI Transport common layer driver

  @copyright
  Copyright 1999 - 2021 Intel Corporation. <BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#include "GenericIpmi.h"
#include <IndustryStandard/Ipmi.h>

EFI_STATUS
UpdateErrorStatus (
  IN UINT8                BmcError,
  IPMI_BMC_INSTANCE_DATA  *IpmiInstance
  )

/*++

Routine Description:

  Check if the completion code is a Soft Error and increment the count.  The count
  is not updated if the BMC is in Force Update Mode.

Arguments:

  BmcError      - Completion code to check
  IpmiInstance  - BMC instance data

Returns:

  EFI_SUCCESS   - Status

--*/
{
  UINT8   Errors[] = COMPLETION_CODES;
  UINT16  CodeCount;
  UINT8   i;

  CodeCount = sizeof (Errors) / sizeof (Errors[0]);
  for (i = 0; i < CodeCount; i++) {
    if (BmcError == Errors[i]) {
      //
      // Don't change Bmc Status flag if the BMC is in Force Update Mode.
      //
      if (IpmiInstance->BmcStatus != BMC_UPDATE_IN_PROGRESS) {
        IpmiInstance->BmcStatus = BMC_SOFTFAIL;
      }

      IpmiInstance->SoftErrorCount++;
      break;
    }
  }

  return EFI_SUCCESS;
}

/**
  Prints out the IPMI command and it's data for debugging purposes.

  @param[in]  Response          Indicates this is a response.
  @param[in]  NetFunction       The net function of the command.
  @param[in]  Command           The command being sent.
  @param[in]  CompletionCode    The completion code when this is a response.
  @param[in]  Data              The pointer to the data being sent.
  @param[in]  DataSize          The size of the data.
**/
VOID
EFIAPI
IpmiPrintCommand (
  IN CONST BOOLEAN  Response,
  IN CONST UINT8    NetFunction,
  IN CONST UINT8    Command,
  IN CONST UINT8    CompletionCode,
  IN CONST UINT8    *Data,
  IN CONST UINT8    DataSize
  )
{
  UINT8   Index;
  CHAR16  *TypeString;

  TypeString = Response ? L"Response" : L"Command";

  DEBUG ((DEBUG_VERBOSE, "*************** IPMI %ls Start ***************\n", TypeString));
  DEBUG ((DEBUG_VERBOSE, "NetFunction:       0x%02x\n", NetFunction));
  DEBUG ((DEBUG_VERBOSE, "Command:           0x%02x\n", Command));
  if (Response) {
    DEBUG ((DEBUG_VERBOSE, "Completion Code:   0x%02x\n", CompletionCode));
  }

  DEBUG ((DEBUG_VERBOSE, "DataSize:          0x%02x\n", DataSize));
  for (Index = 0; Index < DataSize; Index++) {
    if (Index % 16 == 0) {
      DEBUG ((DEBUG_VERBOSE, "\n"));
    }

    DEBUG ((DEBUG_VERBOSE, "%02x ", (Data == NULL) ? 0 : Data[Index]));
  }

  DEBUG ((DEBUG_VERBOSE, "\n"));
  DEBUG ((DEBUG_VERBOSE, "**************** IPMI %ls End ****************\n", TypeString));
}

/**
  Send IPMI command to BMC

  @param[in]      This              Pointer to IPMI protocol instance.
  @param[in]      NetFunction       Net Function of command to send.
  @param[in]      Lun               LUN of command to send.
  @param[in]      Command           IPMI command to send.
  @param[in]      CommandData       Pointer to command data buffer, if needed.
  @param[in]      CommandDataSize   Size of command data buffer.
  @param[in,out]  ResponseData      Pointer to response data buffer. Optional depending on command being sent.
  @param[in,out]  ResponseDataSize  Pointer to response data buffer size. Optional depending on command being sent.

  @retval   EFI_INVALID_PARAMETER   One of the input values is bad.
  @retval   EFI_DEVICE_ERROR        IPMI command failed.
  @retval   EFI_BUFFER_TOO_SMALL    Response buffer is too small.
  @retval   EFI_SUCCESS             Command completed successfully.
**/
EFI_STATUS
EFIAPI
IpmiSendCommandInternal (
  IN      IPMI_TRANSPORT  *This,
  IN      UINT8           NetFunction,
  IN      UINT8           Lun,
  IN      UINT8           Command,
  IN      UINT8           *CommandData,
  IN      UINT8           CommandDataSize,
  IN OUT  UINT8           *ResponseData OPTIONAL,
  IN OUT  UINT8           *ResponseDataSize OPTIONAL
  )
{
  IPMI_BMC_INSTANCE_DATA  *IpmiInstance;
  UINT8                   DataSize;
  EFI_STATUS              Status;
  IPMI_COMMAND            *IpmiCommand;
  IPMI_RESPONSE           *IpmiResponse;
  UINT8                   RetryCnt;
  UINT8                   Index;

  if ((CommandDataSize > 0) && (CommandData == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  //
  // ResponseData and ResponseDataSize cannot be mismatched.
  // Either both NULL are NULL or both are Non-Null, depending
  // on the command being sent.
  //
  if ((ResponseData != NULL) != (ResponseDataSize != NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  RetryCnt     = FixedPcdGet8 (PcdIpmiCommandMaxReties);
  IpmiInstance = INSTANCE_FROM_SM_IPMI_BMC_THIS (This);

  //
  // Print out the command being sent for debugging.
  //

  IpmiPrintCommand (FALSE, NetFunction, Command, 0, CommandData, CommandDataSize);

  while (RetryCnt--) {
    //
    // The TempData buffer is used for both sending command data and receiving
    // response data.  Since the command format is different from the response
    // format, the buffer is cast to both structure definitions.
    //
    IpmiCommand  = (IPMI_COMMAND *)IpmiInstance->TempData;
    IpmiResponse = (IPMI_RESPONSE *)IpmiInstance->TempData;

    //
    // Send IPMI command to BMC
    //
    IpmiCommand->Lun         = Lun;
    IpmiCommand->NetFunction = NetFunction;
    IpmiCommand->Command     = Command;

    //
    // Ensure that the buffer is valid before attempting to copy the command data
    // buffer into the IpmiCommand structure.
    //
    if (CommandDataSize > 0) {
      CopyMem (
        IpmiCommand->CommandData,
        CommandData,
        CommandDataSize
        );
    }

    Status = SendDataToBmcPort (
               IpmiInstance->IpmiTimeoutPeriod,
               (UINT8 *)IpmiCommand,
               (CommandDataSize + IPMI_COMMAND_HEADER_SIZE)
               );

    if (Status != EFI_SUCCESS) {
      DEBUG ((DEBUG_ERROR, "[IPMI] Generic - Softfail! (%r)\n", Status));
      IpmiInstance->BmcStatus = BMC_SOFTFAIL;
      IpmiInstance->SoftErrorCount++;
      return Status;
    }

    //
    // Get Response to IPMI Command from BMC.
    // Subtract 1 from DataSize so memory past the end of the buffer can't be written
    //
    DataSize = MAX_TEMP_DATA - 1;
    Status   = ReceiveBmcDataFromPort (
                 IpmiInstance->IpmiTimeoutPeriod,
                 (UINT8 *)IpmiResponse,
                 &DataSize
                 );

    if (Status != EFI_SUCCESS) {
      DEBUG ((DEBUG_ERROR, "[IPMI] Generic - Softfail! (%r)\n", Status));
      IpmiInstance->BmcStatus = BMC_SOFTFAIL;
      IpmiInstance->SoftErrorCount++;
      return Status;
    }

    //
    // If we got this far without any error codes, but the DataSize less than IPMI_RESPONSE_HEADER_SIZE, then the
    // command response failed, so do not continue.
    //
    if (DataSize < IPMI_RESPONSE_HEADER_SIZE) {
      DEBUG ((DEBUG_ERROR, "[IPMI] Generic - DataSize too small! (%d)\n", DataSize));
      return EFI_DEVICE_ERROR;
    }

    //
    // Print out the response for debugging purposes.
    //

    IpmiPrintCommand (
      TRUE,
      IpmiResponse->NetFunction,
      IpmiResponse->Command,
      IpmiResponse->CompletionCode,
      IpmiResponse->ResponseData,
      DataSize - IPMI_RESPONSE_HEADER_SIZE
      );

    if ((IpmiResponse->CompletionCode != IPMI_COMP_CODE_NORMAL) &&
        (IpmiInstance->BmcStatus == BMC_UPDATE_IN_PROGRESS))
    {
      //
      // If the completion code is not normal and the BMC is in Force Update
      // mode, then update the error status and return EFI_UNSUPPORTED.
      //
      UpdateErrorStatus (
        IpmiResponse->CompletionCode,
        IpmiInstance
        );
      return EFI_UNSUPPORTED;
    } else if (IpmiResponse->CompletionCode != IPMI_COMP_CODE_NORMAL) {
      //
      // Otherwise if the BMC is in normal mode, but the completion code
      // is not normal, then update the error status and return device error.
      //
      UpdateErrorStatus (
        IpmiResponse->CompletionCode,
        IpmiInstance
        );
      //
      // Intel Server System Integrated Baseboard Management Controller (BMC) Firmware v0.62
      // D4h C Insufficient privilege, in KCS channel this indicates KCS Policy Control Mode is Deny All.
      // In authenticated channels this indicates invalid authentication/privilege.
      //
      if (IpmiResponse->CompletionCode == IPMI_COMP_CODE_INSUFFICIENT_PRIVILEGE) {
        return EFI_SECURITY_VIOLATION;
      } else {
        return EFI_DEVICE_ERROR;
      }
    }

    //
    // If we got this far without any error codes, but the DataSize is 0 then the
    // command response failed, so do not continue.
    //
    if (DataSize < IPMI_RESPONSE_HEADER_SIZE) {
      return EFI_DEVICE_ERROR;
    }

    //
    // Verify the response data buffer passed in is big enough.
    //
    if ((ResponseDataSize != NULL) && ((DataSize - IPMI_RESPONSE_HEADER_SIZE) > *ResponseDataSize)) {
      //
      // Verify the response data matched with the cmd sent.
      //
      if ((IpmiResponse->NetFunction != (NetFunction | 0x1)) || (IpmiResponse->Command != Command)) {
        if (0 == RetryCnt) {
          return EFI_DEVICE_ERROR;
        } else {
          continue;
        }
      }

      //
      // return the required size. Total size - header size  + 1 byte for
      // completion code.
      //

      *ResponseDataSize = DataSize - IPMI_RESPONSE_HEADER_SIZE + 1;
      return EFI_BUFFER_TOO_SMALL;
    }

    break;
  }

  if ((ResponseData != NULL) && (ResponseDataSize != NULL)) {
    //
    // Copy data over to the response data buffer.
    //
    *ResponseDataSize = DataSize - IPMI_RESPONSE_HEADER_SIZE;
    CopyMem (
      ResponseData,
      IpmiResponse->ResponseData,
      *ResponseDataSize
      );

    //
    // Add completion code in response data to meet the requirement of IPMI spec 2.0
    //
    *ResponseDataSize += 1;   // Add one byte for Completion Code
    for (Index = 1; Index < *ResponseDataSize; Index++) {
      ResponseData[*ResponseDataSize - Index] = ResponseData[*ResponseDataSize - (Index + 1)];
    }

    ResponseData[0] = IpmiResponse->CompletionCode;
  }

  IpmiInstance->BmcStatus = BMC_OK;
  return EFI_SUCCESS;
}

/**
  Updates the BMC status and returns the Com Address.

  @param[in,out]  <ParameterName>       <Description>
  @param[in]  This            Pointer to IPMI protocol instance
  @param[out] BmcStatus       BMC status
  @param[out] ComAddress      Com Address

  @retval     EFI_SUCCESS     Success
**/
EFI_STATUS
EFIAPI
IpmiBmcStatus (
  IN  IPMI_TRANSPORT  *This,
  OUT BMC_STATUS      *BmcStatus,
  OUT SM_COM_ADDRESS  *ComAddress
  )
{
  IPMI_BMC_INSTANCE_DATA  *IpmiInstance;

  IpmiInstance = INSTANCE_FROM_SM_IPMI_BMC_THIS (This);

  if ((IpmiInstance->BmcStatus == BMC_SOFTFAIL) && (IpmiInstance->SoftErrorCount >= MAX_SOFT_COUNT)) {
    IpmiInstance->BmcStatus = BMC_HARDFAIL;
  }

  *BmcStatus                                    = IpmiInstance->BmcStatus;
  ComAddress->ChannelType                       = SmBmc;
  ComAddress->Address.BmcAddress.LunAddress     = 0x0;
  ComAddress->Address.BmcAddress.SlaveAddress   = IpmiInstance->SlaveAddress;
  ComAddress->Address.BmcAddress.ChannelAddress = 0x0;

  return EFI_SUCCESS;
}
