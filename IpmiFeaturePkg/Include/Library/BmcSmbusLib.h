/** @file
  Header definitions for the SMBus platform library for SSIF to be implemented
  by the platform.

  Copyright (c) Microsoft Corporation
  PDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef _BMC_SMBUS_LIB_H
#define _BMC_SMBUS_LIB_H

#include <Uefi.h>

/*------------- Functions ----------------*/

// NOTE: This API is tentative and may be subject to change!

/**
  Writes the provided SMBus message to the BMC.

  @param[in]  Command             The SMBus command value.
  @param[in]  WriteBlock          The message data block.
  @param[in]  BlockLength         The length of the message data.
  @param[in]  BlockLength         The length of the message data.
  @param[in]  IncludePEC          Specifies if a PEC should be used in the message.

  @retval   EFI_SUCCESS           The message was successfully sent.
**/
EFI_STATUS
BmcSmbusBlockWrite (
  UINT8    Command,
  UINT8    *WriteBlock,
  UINT8    BlockLength,
  BOOLEAN  IncludePEC
  );

/**
  Reads a message from the specified SMBus device.

  @param[in]      Command           The SMBus read command.
  @param[out]     ReadBlock         The message data block.
  @param[in,out]  BlockLength       Input specifies the buffer size.
                                    Output specifies the read size.
  @param[in]      IncludePEC        Specifies if a PEC should be used in the message.

  @retval   EFI_SUCCESS           The message was successfully sent.
  @retval   EFI_BUFFER_TOO_SMALL  The provided buffer was not large enough for
                                  the received message.
**/
EFI_STATUS
BmcSmbusBlockRead (
  UINT8    Command,
  UINT8    *ReadBlock,
  UINT8    *BlockLength,
  BOOLEAN  IncludePEC
  );

#endif
