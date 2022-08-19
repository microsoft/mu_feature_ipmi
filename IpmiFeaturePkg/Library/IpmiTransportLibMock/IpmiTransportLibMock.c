/** @file
  Implements the transport library implementation for the mock BMC.

  Copyright (c) Microsoft Corporation
  PDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Uefi.h>
#include "MockIpmi.h"

/**
  Mock implementation of SendDataToBmcPort.

  @param[in]  IpmiTimeoutPeriod     UNUSED.
  @param[in]  Context               UNUSED.
  @param[in]  Command               The data to send to the mock BMC.
  @param[in]  DataSize              The size of the data.

  @retval   EFI_SUCCESS             Always.
**/
EFI_STATUS
SendDataToBmcPort (
  UINT64  IpmiTimeoutPeriod,
  VOID    *Context,
  UINT8   *Data,
  UINT8   DataSize
  )
{
  return MockIpmiCommand ((IPMI_COMMAND *)Data, DataSize);
}

/**
  Mock implementation of ReceiveBmcDataFromPort.

  @param[in]  IpmiTimeoutPeriod     UNUSED.
  @param[in]  Context               UNUSED.
  @param[out] Response              The data received from the mock BMC.
  @param[out] DataSize              The size of the data.

  @retval   EFI_SUCCESS             Always.
**/
EFI_STATUS
ReceiveBmcDataFromPort (
  UINT64  IpmiTimeoutPeriod,
  VOID    *Context,
  UINT8   *Data,
  UINT8   *DataSize
  )
{
  return MockIpmiResponse ((IPMI_RESPONSE *)Data, DataSize);
}

/**
  Mock implementation of InitializeIpmiTransportHardware.

  @retval   EFI_SUCCESS     Always.
**/
EFI_STATUS
InitializeIpmiTransportHardware (
  VOID
  )
{
  return EFI_SUCCESS;
}
