/** @file
  Implements the null version of the IPMI transport library.

  Copyright (c) Microsoft Corporation
  PDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Uefi.h>

/**
  Null implementation of SendDataToBmcPort.

  @param[in]  IpmiTimeoutPeriod     UNUSED.
  @param[in]  Context               UNUSED.
  @param[in]  Command               UNUSED.
  @param[in]  DataSize              UNUSED.

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
  return EFI_SUCCESS;
}

/**
  Null implementation of ReceiveBmcDataFromPort.

  @param[in]  IpmiTimeoutPeriod     UNUSED.
  @param[in]  Context               UNUSED.
  @param[out] Response              UNUSED.
  @param[out] DataSize              UNUSED.

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
  return EFI_SUCCESS;
}

/**
  Null implementation of InitializeIpmiTransportHardware.

  @retval   EFI_SUCCESS     Always.
**/
EFI_STATUS
InitializeIpmiTransportHardware (
  VOID
  )
{
  return EFI_SUCCESS;
}
