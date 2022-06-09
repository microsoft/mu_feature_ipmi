/** @file
  Implements a test version of the SSIF SMBus library.

  Copyright (c) Microsoft Corporation
  PDX-License-Identifier: BSD-2-Clause-Patent

**/

#include "SsifUnitTest.h"

UINT8   RxBuffer[TEST_BUFFER_SIZE];
UINT8   TxBuffer[TEST_BUFFER_SIZE];
UINT32  RxOffset = 0;
UINT32  RxSize   = 0;
UINT32  TxSize   = 0;
UINT8   RxBlock  = 0xFF;

/**
  Opens the SMBus connection if needed.

  @retval   EFI_SUCCESS           Them SMBus connection was successfully opened.
**/
EFI_STATUS
BmcSmbusOpen (
  VOID
  )
{
  return EFI_SUCCESS;
}

/**
  Opens the SMBus connection if needed.

  @retval   EFI_SUCCESS           Them SMBus connection was successfully closed.
**/
EFI_STATUS
BmcSmbusClose (
  VOID
  )
{
  return EFI_SUCCESS;
}

/**
  Reset state in the test SMBus simple lib.
**/
VOID
SmbusTestLibReset (
  VOID
  )
{
  RxOffset = 0;
  RxSize   = 0;
  TxSize   = 0;
  RxBlock  = 0xFF;
  ZeroMem (&RxBuffer[0], TEST_BUFFER_SIZE);
  ZeroMem (&TxBuffer[0], TEST_BUFFER_SIZE);
}

/**
  Writes the provided SMBus message data to the test buffer.

  @param[in]  Command             The SMBus command value.
  @param[in]  WriteBlock          The message data block.
  @param[in]  BlockLength         The length of the message data.
  @param[in]  BlockLength         The length of the message data.

  @retval   EFI_SUCCESS           The message was successfully stored.
  @retval   EFI_INVALID_PARAMETER Unexpected write command.
  @retval   EFI_ALREADY_STARTED   Unexpected first write command.
  @retval   EFI_NOT_STARTED       Unexpected middle or last write.
**/
EFI_STATUS
BmcSmbusBlockWrite (
  UINT8  Command,
  UINT8  *WriteBlock,
  UINT8  BlockLength
  )
{
  UINT8  WriteSize;

  WriteSize = BlockLength < SSIF_MAX_WRITE_SIZE ? BlockLength : SSIF_MAX_WRITE_SIZE;

  if ((Command != SMBUS_CMD_WRITE) &&
      (Command != SMBUS_CMD_MULT_WRITE_START) &&
      (Command != SMBUS_CMD_MULT_WRITE_MIDDLE) &&
      (Command != SMBUS_CMD_MULT_WRITE_END))
  {
    return EFI_INVALID_PARAMETER;
  }

  if (((Command == SMBUS_CMD_WRITE) ||
       (Command == SMBUS_CMD_MULT_WRITE_START)) &&
      (TxSize != 0))
  {
    return EFI_ALREADY_STARTED;
  }

  if (((Command == SMBUS_CMD_MULT_WRITE_MIDDLE) ||
       (Command == SMBUS_CMD_MULT_WRITE_END)) &&
      (TxSize == 0))
  {
    return EFI_NOT_STARTED;
  }

  CopyMem (&TxBuffer[TxSize], WriteBlock, WriteSize);
  TxSize += WriteSize;

  return EFI_SUCCESS;
}

/**
  Reads message data from the test buffer in the correct SMBus format.

  @param[in]      Command           The SMBus read command.
  @param[out]     ReadBlock         The message data block.
  @param[in,out]  BlockLength       Input specifies the buffer size.
                                    Output specifies the read size.

  @retval   EFI_SUCCESS           The message was successfully sent.
  @retval   EFI_NOT_FOUND         No  message was found.
  @retval   EFI_ALREADY_STARTED   Unexpected first read.
  @retval   EFI_NOT_STARTED       Unexpected middle read.
  @retval   EFI_INVALID_PARAMETER Unexpected SMBus command.
  @retval   EFI_BUFFER_TOO_SMALL  The provided buffer was not large enough for
                                  the received message.
**/
EFI_STATUS
BmcSmbusBlockRead (
  UINT8  Command,
  UINT8  *ReadBlock,
  UINT8  *BlockLength
  )
{
  UINT8  ReadSize;

  if (RxSize == 0) {
    return EFI_NOT_FOUND;
  }

  if (Command == SMBUS_CMD_READ) {
    // Make sure this is a clean state.
    if (RxOffset != 0) {
      return EFI_ALREADY_STARTED;
    }

    // Check for a multi-read;
    if (RxSize > SSIF_MAX_READ_SIZE) {
      ReadBlock[0] = 0;
      ReadBlock[1] = 1;
      CopyMem (&ReadBlock[2], &RxBuffer[RxOffset], SSIF_MAX_READ_SIZE - 2);
      RxOffset    += SSIF_MAX_READ_SIZE - 2;
      RxBlock      = 1;
      *BlockLength = SSIF_MAX_READ_SIZE;
    } else {
      CopyMem (&ReadBlock[0], &RxBuffer[RxOffset], RxSize);
      RxOffset    += RxSize;
      *BlockLength = (UINT8)RxSize;
    }
  } else if (Command == SMBUS_CMD_MULT_READ) {
    // Make sure this is not the first read.
    if (RxOffset == 0) {
      return EFI_NOT_STARTED;
    }

    ReadSize = ((RxSize - RxOffset) < (SSIF_MAX_READ_SIZE - 1)) ?
               (UINT8)(RxSize - RxOffset) : (SSIF_MAX_READ_SIZE - 1);

    ReadBlock[0] = RxBlock;
    CopyMem (&ReadBlock[1], &RxBuffer[RxOffset], ReadSize);
    RxBlock     += 1;
    RxOffset    += ReadSize;
    *BlockLength = ReadSize + 1;
  } else {
    return EFI_INVALID_PARAMETER;
  }

  return EFI_SUCCESS;
}

/**
  Compares the Tx buffer to the provided buffer.

  @param[in]  Data        The buffer to compare the to Tx buffer.
  @param[in]  DataSize    The size of the Data buffer.

  @retval   TRUE          Buffers match.
  @retval   FALSE         Buffers do not match.
**/
BOOLEAN
CheckTxBuffer (
  UINT8   *Data,
  UINT32  DataSize
  )
{
  if (DataSize != TxSize) {
    return FALSE;
  }

  return (CompareMem (Data, &TxBuffer[0], DataSize) == 0);
}

/**
  Copy the provided data into the Rx buffer.

  @param[in]  Data        The buffer to set to the Rx buffer.
  @param[in]  DataSize    The size of the Data buffer.

**/
VOID
SetRxBuffer (
  UINT8   *Data,
  UINT32  DataSize
  )
{
  ASSERT (DataSize <= TEST_BUFFER_SIZE);

  CopyMem (&RxBuffer[0], Data, DataSize);
  RxSize   = DataSize;
  RxOffset = 0;
}
