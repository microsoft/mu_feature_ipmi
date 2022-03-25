/** @file
  External functions and definitions for the IPMI transport layer libraries.

  Copyright 1999 - 2021 Intel Corporation.
  Copyright (c) Microsoft Corporation
  PDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef _IPMI_TRANSPORT_H
#define _IPMI_TRANSPORT_H

/**
  Send data to BMC over the IPMI transport.

  @param[in]  IpmiTimeoutPeriod   The timeout for the transaction.

  @retval   EFI_SUCCESS                 <Description>
**/
EFI_STATUS
SendDataToBmcPort (
  UINT64  IpmiTimeoutPeriod,
  VOID    *Context,
  UINT8   *Data,
  UINT8   DataSize
  )

/*++

Routine Description:

  Send data to BMC

Arguments:

  IpmiInstance  - The pointer of IPMI_BMC_INSTANCE_DATA
  Context       - The context of this operation
  Data          - The data pointer to be sent
  DataSize      - The data size

Returns:

  EFI_SUCCESS   - Send out the data successfully

--*/
;

EFI_STATUS
ReceiveBmcDataFromPort (
  UINT64  IpmiTimeoutPeriod,
  VOID    *Context,
  UINT8   *Data,
  UINT8   *DataSize
  )

/*++

Routine Description:

  Routine Description:

  Receive data from BMC

Arguments:

  IpmiInstance  - The pointer of IPMI_BMC_INSTANCE_DATA
  Context       - The context of this operation
  Data          - The buffer pointer
  DataSize      - The buffer size

Returns:

  EFI_SUCCESS   - Received data successfully

--*/
;

/**
  Initializing hardware for the IPMI transport.

  @retval   EFI_SUCCESS     Hardware was successfully initialized.
  @retval   Other           There was a hardware specific failure.
**/
EFI_STATUS
InitializeIpmiTransportHardware (
  VOID
  );

#endif
