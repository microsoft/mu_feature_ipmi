/** @file
  IPMI Transport common layer driver head file

  @copyright
  Copyright 1999 - 2021 Intel Corporation. <BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#ifndef GENERIC_IPMI_H_
#define GENERIC_IPMI_H_

#include <Uefi.h>
#include <IndustryStandard/Ipmi.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/IpmiTransportLib.h>
#include <IpmiInterface.h>

#include <IpmiHooks.h>

#define IPMI_DELAY_UNIT    50   // Unit is microseconds.
#define MAX_TEMP_DATA      255
#define BMC_SLAVE_ADDRESS  0x20
#define MAX_SOFT_COUNT     10

#define COMPLETION_CODES \
  { \
    IPMI_COMP_CODE_NODE_BUSY, IPMI_COMP_CODE_TIMEOUT, IPMI_COMP_CODE_OUT_OF_SPACE, \
    IPMI_COMP_CODE_OUT_OF_RANGE, IPMI_COMP_CODE_CMD_RESP_NOT_PROVIDED, \
    IPMI_COMP_CODE_FAIL_DUP_REQUEST, IPMI_COMP_CODE_SDR_REP_IN_UPDATE_MODE, \
    IPMI_COMP_CODE_DEV_IN_FW_UPDATE_MODE, IPMI_COMP_CODE_BMC_INIT_IN_PROGRESS, \
    IPMI_COMP_CODE_INSUFFICIENT_PRIVILEGE, IPMI_COMP_CODE_UNSPECIFIED \
  }

//
// IPMI Instance signature
//
#define SM_IPMI_BMC_SIGNATURE  SIGNATURE_32 ('i', 'p', 'm', 'i')
#define INSTANCE_FROM_SM_IPMI_BMC_THIS(a) \
  CR ( \
  a, \
  IPMI_BMC_INSTANCE_DATA, \
  IpmiTransport, \
  SM_IPMI_BMC_SIGNATURE \
  )

//
// Dxe Ipmi instance data
//
typedef struct {
  UINTN             Signature;
  UINT64            IpmiTimeoutPeriod;
  UINT8             SlaveAddress;
  UINT8             TempData[MAX_TEMP_DATA];
  BMC_STATUS        BmcStatus;
  UINT64            ErrorStatus;
  UINT8             SoftErrorCount;
  IPMI_TRANSPORT    IpmiTransport;
} IPMI_BMC_INSTANCE_DATA;

#pragma pack(1)

//
// Structure of IPMI Command buffer
//
#define IPMI_COMMAND_HEADER_SIZE  2

typedef struct {
  UINT8    Lun         : 2;
  UINT8    NetFunction : 6;
  UINT8    Command;
  UINT8    CommandData[MAX_TEMP_DATA - IPMI_COMMAND_HEADER_SIZE];
} IPMI_COMMAND;

//
// Structure of IPMI Command response buffer
//
#define IPMI_RESPONSE_HEADER_SIZE  3

typedef struct {
  UINT8    Lun         : 2;
  UINT8    NetFunction : 6;
  UINT8    Command;
  UINT8    CompletionCode;
  UINT8    ResponseData[MAX_TEMP_DATA - IPMI_RESPONSE_HEADER_SIZE];
} IPMI_RESPONSE;

//
// Structure to communicate BMC state from PEI to DXE.
//

typedef struct _IPMI_BMC_HOB {
  BMC_STATUS    BmcStatus;
} IPMI_BMC_HOB;

#pragma pack()

/**
  Initializes the IPMI state for the BMC. This includes performs platform
  specific logic, getting the device ID, and checking self-test results.

  @param[in,out]  IpmiInstance    The IPMI instance being initialized.

  @retval         EFI_SUCCESS     The BMC state was successfully initialized.
  @retval         Other          An error was returned by a subroutine.
**/
EFI_STATUS
EFIAPI
IpmiInitializeBmc (
  IN IPMI_BMC_INSTANCE_DATA  *IpmiInstance
  );

/**
  Send IPMI command to BMC

  @param[in]      This              Pointer to IPMI protocol instance.
  @param[in]      NetFunction       Net Function of command to send.
  @param[in]      Lun               LUN of command to send.
  @param[in]      Command           IPMI command to send.
  @param[in]      CommandData       Pointer to command data buffer, if needed.
  @param[in]      CommandDataSize   Size of command data buffer.
  @param[in,out]  ResponseData      Pointer to response data buffer.
  @param[in,out]  ResponseDataSize  Pointer to response data buffer size.

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
  IN OUT  UINT8           *ResponseData,
  IN OUT  UINT8           *ResponseDataSize
  );

/**
  Updates the BMC status and returns the Com Address.

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
  );

#endif
