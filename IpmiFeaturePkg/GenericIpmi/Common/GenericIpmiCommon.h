/** @file
  IPMI Transport common layer driver common head file

  @copyright
  Copyright 1999 - 2021 Intel Corporation. <BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#ifndef _IPMI_COMMON_BMC_H_
#define _IPMI_COMMON_BMC_H_

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
  IN OUT  UINT8           *ResponseDataSize,
  IN      VOID            *Context
  )

/*++

Routine Description:

  Send IPMI command to BMC

Arguments:

  This              - Pointer to IPMI protocol instance
  NetFunction       - Net Function of command to send
  Lun               - LUN of command to send
  Command           - IPMI command to send
  CommandData       - Pointer to command data buffer, if needed
  CommandDataSize   - Size of command data buffer
  ResponseData      - Pointer to response data buffer
  ResponseDataSize  - Pointer to response data buffer size
  Context           - Context

Returns:

  EFI_INVALID_PARAMETER - One of the input values is bad
  EFI_DEVICE_ERROR      - IPMI command failed
  EFI_BUFFER_TOO_SMALL  - Response buffer is too small
  EFI_SUCCESS           - Command completed successfully

--*/
;

EFI_STATUS
EFIAPI
IpmiBmcStatus (
  IN  IPMI_TRANSPORT  *This,
  OUT BMC_STATUS      *BmcStatus,
  OUT SM_COM_ADDRESS  *ComAddress,
  IN  VOID            *Context
  )

/*++

Routine Description:

  Updates the BMC status and returns the Com Address

Arguments:

  This        - Pointer to IPMI protocol instance
  BmcStatus   - BMC status
  ComAddress  - Com Address
  Context     - Context

Returns:

  EFI_SUCCESS - Success

--*/
;

#endif
