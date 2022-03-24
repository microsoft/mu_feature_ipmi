/** @file
  NULL implementation of the SSIF SMBus library.

  Copyright (c) Microsoft Corporation
  PDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/BmcSmbusLib.h>

/**
  NULL implementation of the SMBus write routine for BMC SMBus.

  @param[in]  Command             The SMBus command value.
  @param[in]  WriteBlock          The message data block.
  @param[in]  BlockLength         The length of the message data.
  @param[in]  BlockLength         The length of the message data.
  @param[in]  IncludePEC          Specifies if a PEC should be used in the message.

  @retval   EFI_SUCCESS           Always
**/
EFI_STATUS
BmcSmbusBlockWrite (
  UINT8    Command,
  UINT8    *WriteBlock,
  UINT8    BlockLength,
  BOOLEAN  IncludePEC
  )
{
  return EFI_SUCCESS;
}

/**
  NULL implementation of the SMBus read routine.

  @param[in]      Command           The SMBus read command.
  @param[out]     ReadBlock         The message data block.
  @param[in,out]  BlockLength       Input specifies the buffer size.
                                    Output specifies the read size.
  @param[in]      IncludePEC        Specifies if a PEC should be used in the message.

  @retval   EFI_SUCCESS           Always.
**/
EFI_STATUS
BmcSmbusBlockRead (
  UINT8    Command,
  UINT8    *ReadBlock,
  UINT8    *BlockLength,
  BOOLEAN  IncludePEC
  )
{
  *BlockLength = 0;
  return EFI_SUCCESS;
}
