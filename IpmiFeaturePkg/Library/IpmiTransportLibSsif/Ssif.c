/** @file
  Implements the SMBus System Interface (SSIF) transport code for IPMI.

  Copyright (c) Microsoft Corporation
  PDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/TimerLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/BmcSmbusLib.h>
#include <Ssif.h>

/**
  Sends an IPMI command message to the BMC over the SSIF transport.

  @param[in]  IpmiTimeoutPeriod     The timeout of the IPMI send.
  @param[in]  Command               The IMPI command to be sent.
  @param[in]  DataSize              The size of the data in the IPMI command.

  @retval   EFI_SUCCESS             The message was successfully sent.
  @retval   EFI_INVALID_PARAMETER   The command was invalid.
  @retval   Other errors            And error was returned by the SMBus stack.
**/
EFI_STATUS
SendDataToBmcPort (
  UINT64  IpmiTimeoutPeriod,
  UINT8   *Data,
  UINT8   DataSize
  )
{
  UINT8       RemainingSize;
  UINT8       DataOffset;
  UINT8       SMBusCmd;
  UINT8       WriteSize;
  EFI_STATUS  Status;

  if (DataSize == 0) {
    return EFI_INVALID_PARAMETER;
  }

  //
  // Determine if this message will require multiple packets or not. If so,
  // start with the multi write command.
  //

  if (DataSize > SSIF_MAX_WRITE_SIZE) {
    SMBusCmd = SMBUS_CMD_MULT_WRITE_START;
  } else {
    SMBusCmd = SMBUS_CMD_WRITE;
  }

  DEBUG ((
    DEBUG_VERBOSE,
    "[SSIF] Sending IPMI Message - \n"
    "[SSIF]     Send type: %a\n"
    "[SSIF]     DataSize: %d\n",
    (SMBusCmd == SMBUS_CMD_MULT_WRITE_START) ? "MULTI-WRITE" : "WRITE",
    DataSize
    ));

  Status = BmcSmbusOpen ();
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "[SSIF] Failed to open SMBus connection. %r\n", Status));
    return Status;
  }

  DataOffset    = 0;
  RemainingSize = DataSize;

  //
  // Continue sending packets so long as more data remains.
  //

  while (RemainingSize > 0) {
    WriteSize = (RemainingSize < SSIF_MAX_WRITE_SIZE) ? RemainingSize : SSIF_MAX_WRITE_SIZE;

    DEBUG ((DEBUG_VERBOSE, "[SSIF]     Sending 0x%x bytes. Cmd: 0x%x\n", WriteSize, SMBusCmd));
    Status = BmcSmbusBlockWrite (SMBusCmd, &Data[DataOffset], WriteSize);
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_ERROR, "[SSIF] Failed to write SMBus block. Cmd: 0x%x Size: 0x%x (%r)\n", SMBusCmd, WriteSize, Status));
      goto Exit;
    }

    DataOffset   += WriteSize;
    RemainingSize = DataSize - DataOffset;
    if (SMBusCmd == SMBUS_CMD_MULT_WRITE_START) {
      //
      // After the first write, switch to the middle commands. This may later be
      // changed to a end packet if needed.
      //

      ASSERT (RemainingSize > 0);
      SMBusCmd = SMBUS_CMD_MULT_WRITE_MIDDLE;
    }

    if ((SMBusCmd == SMBUS_CMD_MULT_WRITE_MIDDLE) && (RemainingSize <= SSIF_MAX_WRITE_SIZE)) {
      SMBusCmd = SMBUS_CMD_MULT_WRITE_END;
    }
  }

Exit:
  BmcSmbusClose ();
  return Status;
}

/**
  Receives an IPMI command message from the BMC over the SSIF transport.

  @param[in]  IpmiTimeoutPeriod     The timeout of the IPMI receive.
  @param[out] Response              The IMPI response received.
  @param[out] DataSize              The size of the data in the IPMI response.

  @retval   EFI_SUCCESS             The message was successfully sent.
  @retval   EFI_DEVICE_ERROR        Received an unexpected message from BMC.
  @retval   EFI_BUFFER_TOO_SMALL    The provided buffer could not fit the full
                                    message
  @retval   Other errors            And error was returned by the SMBus stack.
**/
EFI_STATUS
ReceiveBmcDataFromPort (
  UINT64  IpmiTimeoutPeriod,
  UINT8   *Data,
  UINT8   *DataSize
  )
{
  UINT8       DataOffset;
  UINT8       SMBusCmd;
  UINT8       BlockBuffer[SSIF_MAX_READ_BUFFER_SIZE];
  UINT8       BlockDataOffset;
  UINT8       BlockDataSize;
  UINT8       BlockNumber;
  EFI_STATUS  Status;
  UINT8       ReadSize;
  BOOLEAN     Done;

  DataOffset = 0;
  Done       = FALSE;
  SMBusCmd   = SMBUS_CMD_READ;

  DEBUG ((DEBUG_VERBOSE, "[SSIF] Reading IPMI Message - \n"));
  Status = BmcSmbusOpen ();
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "[SSIF] Failed to open SMBus connection. %r\n", Status));
    return Status;
  }

  while (!Done) {
    ReadSize = SSIF_MAX_READ_BUFFER_SIZE;
    Status   = BmcSmbusBlockRead (SMBusCmd, &BlockBuffer[0], &ReadSize);
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_ERROR, "[SSIF] Failed to read SMBus block. Cmd: 0x%x Offset: 0x%x (%r)\n", SMBusCmd, BlockDataOffset, Status));
      goto Exit;
    }

    DEBUG ((DEBUG_VERBOSE, "[SSIF]     Read 0x%x bytes.\n", ReadSize));
    if (SMBusCmd == SMBUS_CMD_READ) {
      //
      // If the first two bytes of the data are 0 and 1 this indicates it is the
      // beginning of a multi-part message.
      //

      if ((BlockBuffer[0] == 0) && (BlockBuffer[1] == 1)) {
        DEBUG ((DEBUG_VERBOSE, "[SSIF]     Multi-read detected.\n", ReadSize));
        //
        // Trim off the indicator and copy the data over. Set the multi-command
        // for the next loop.
        //
        ASSERT (DataOffset == 0);
        SMBusCmd        = SMBUS_CMD_MULT_READ;
        BlockNumber     = 0;
        BlockDataOffset = 2;
      } else {
        BlockDataOffset = 0;
        Done            = TRUE;
      }
    } else {
      //
      // In multi-part read the first byte will be the block number. 0xFF
      // indicates this is the final block.
      //

      BlockDataOffset = 1;
      if (BlockBuffer[0] == 0xFF) {
        Done = TRUE;
      } else {
        if (BlockBuffer[0] != BlockNumber + 1) {
          DEBUG ((DEBUG_ERROR, "[SSIF] SMBus block out of order! Expected %d, Received %d.\n", BlockNumber + 1, BlockBuffer[0]));
          Status = EFI_DEVICE_ERROR;
          goto Exit;
        }

        BlockNumber = BlockBuffer[0];
      }
    }

    BlockDataSize = ReadSize - BlockDataOffset;
    if (DataOffset + BlockDataSize > *DataSize) {
      DEBUG ((DEBUG_ERROR, "[SSIF] Buffer too small! Buffer size: 0x%x\n", *DataSize));
      Status = EFI_BUFFER_TOO_SMALL;
      goto Exit;
    }

    CopyMem (&Data[DataOffset], &BlockBuffer[BlockDataOffset], BlockDataSize);
    DataOffset += BlockDataSize;
  }

  *DataSize = DataOffset;
Exit:
  BmcSmbusClose ();
  return Status;
}

/**
  Initializing hardware for the IPMI transport.

  @retval   EFI_SUCCESS     Always.
**/
EFI_STATUS
InitializeIpmiTransportHardware (
  VOID
  )
{
  return EFI_SUCCESS;
}
