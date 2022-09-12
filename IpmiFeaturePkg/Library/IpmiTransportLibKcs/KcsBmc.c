/** @file
  KCS Transport Hook.

  @copyright
  Copyright 1999 - 2021 Intel Corporation. <BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#include "KcsBmc.h"

/**
  Initializing hardware for the IPMI transport.

  @retval   EFI_SUCCESS     Hardware was successfully initialized.
  @retval   Other           An error was returned from PlatformIpmiIoRangeSet.
**/
EFI_STATUS
InitializeIpmiTransportHardware (
  VOID
  )

{
  EFI_STATUS  Status;

  //
  // Enable OEM specific southbridge SIO KCS I/O address range 0xCA0 to 0xCAF at here
  // if the the I/O address range has not been enabled.
  //
  Status = PlatformIpmiIoRangeSet (PcdGet16 (PcdIpmiIoBaseAddress));
  DEBUG ((DEBUG_INFO, "IPMI: PlatformIpmiIoRangeSet - %r!\n", Status));
  return Status;
}

/**
  The constructor function initializing global state for the KCS library.

  @retval EFI_SUCCESS   The constructor always returns EFI_SUCCESS.

**/
EFI_STATUS
EFIAPI
BmcKcsConstructor (
  VOID
  )
{
  return EFI_SUCCESS;
}

EFI_STATUS
KcsErrorExit (
  UINT64  IpmiTimeoutPeriod
  )

/**

Routine Description:

  Check the KCS error status

Arguments:

  IpmiTimeoutPeriod     - The period wait before timeout

Returns:

  @retval EFI_DEVICE_ERROR      - The device error happened
  @retval EFI_SUCCESS           - Successfully check the KCS error status

**/
{
  EFI_STATUS  Status;
  UINT8       KcsData;
  KCS_STATUS  KcsStatus;
  UINT16      KcsPort;
  UINT8       RetryCount;
  UINT64      TimeOut;

  KcsPort    = PcdGet16 (PcdIpmiIoBaseAddress);
  TimeOut    = 0;
  RetryCount = 0;
  while (RetryCount < KCS_ABORT_RETRY_COUNT) {
    TimeOut = 0;
    do {
      MicroSecondDelay (IPMI_DELAY_UNIT);
      KcsStatus.RawData = IoRead8 (KcsPort + 1);
      if ((KcsStatus.RawData == 0xFF) || (TimeOut >= IpmiTimeoutPeriod)) {
        RetryCount = KCS_ABORT_RETRY_COUNT;
        break;
      }

      TimeOut++;
    } while (KcsStatus.Status.Ibf);

    if (RetryCount >= KCS_ABORT_RETRY_COUNT) {
      break;
    }

    KcsData = KCS_ABORT;
    IoWrite8 ((KcsPort + 1), KcsData);

    TimeOut = 0;
    do {
      MicroSecondDelay (IPMI_DELAY_UNIT);
      KcsStatus.RawData = IoRead8 (KcsPort + 1);
      if ((KcsStatus.RawData == 0xFF) || (TimeOut >= IpmiTimeoutPeriod)) {
        Status = EFI_DEVICE_ERROR;
        goto LabelError;
      }

      TimeOut++;
    } while (KcsStatus.Status.Ibf);

    KcsData = IoRead8 (KcsPort);

    KcsData = 0x0;
    IoWrite8 (KcsPort, KcsData);

    TimeOut = 0;
    do {
      MicroSecondDelay (IPMI_DELAY_UNIT);
      KcsStatus.RawData = IoRead8 (KcsPort + 1);
      if ((KcsStatus.RawData == 0xFF) || (TimeOut >= IpmiTimeoutPeriod)) {
        Status = EFI_DEVICE_ERROR;
        goto LabelError;
      }

      TimeOut++;
    } while (KcsStatus.Status.Ibf);

    if (KcsStatus.Status.State == KcsReadState) {
      TimeOut = 0;
      do {
        MicroSecondDelay (IPMI_DELAY_UNIT);
        KcsStatus.RawData = IoRead8 (KcsPort + 1);
        if ((KcsStatus.RawData == 0xFF) || (TimeOut >= IpmiTimeoutPeriod)) {
          Status = EFI_DEVICE_ERROR;
          goto LabelError;
        }

        TimeOut++;
      } while (!KcsStatus.Status.Obf);

      // BmcStatus = IoRead8 (KcsPort);   // MU_CHANGE
      IoRead8 (KcsPort);                  // MU_CHANGE

      KcsData = KCS_READ;
      IoWrite8 (KcsPort, KcsData);

      TimeOut = 0;
      do {
        MicroSecondDelay (IPMI_DELAY_UNIT);
        KcsStatus.RawData = IoRead8 (KcsPort + 1);
        if ((KcsStatus.RawData == 0xFF) || (TimeOut >= IpmiTimeoutPeriod)) {
          Status = EFI_DEVICE_ERROR;
          goto LabelError;
        }

        TimeOut++;
      } while (KcsStatus.Status.Ibf);

      if (KcsStatus.Status.State == KcsIdleState) {
        TimeOut = 0;
        do {
          MicroSecondDelay (IPMI_DELAY_UNIT);
          KcsStatus.RawData = IoRead8 (KcsPort + 1);
          if ((KcsStatus.RawData == 0xFF) || (TimeOut >= IpmiTimeoutPeriod)) {
            Status = EFI_DEVICE_ERROR;
            goto LabelError;
          }

          TimeOut++;
        } while (!KcsStatus.Status.Obf);

        KcsData = IoRead8 (KcsPort);
        break;
      } else {
        RetryCount++;
        continue;
      }
    } else {
      RetryCount++;
      continue;
    }
  }

  if (RetryCount >= KCS_ABORT_RETRY_COUNT) {
    Status = EFI_DEVICE_ERROR;
    goto LabelError;
  }

  return EFI_SUCCESS;

LabelError:
  return Status;
}

EFI_STATUS
KcsCheckStatus (
  UINT64     IpmiTimeoutPeriod,
  KCS_STATE  KcsState,
  BOOLEAN    *Idle
  )

/**

Routine Description:

  Ckeck KCS status

Arguments:

  IpmiTimeoutPeriod  - The period to wait before timeout
  KcsState           - The state of KCS to be checked
  Idle               - If the KCS is idle

Returns:

  @retval EFI_SUCCESS   - Checked the KCS status successfully

**/
{
  EFI_STATUS  Status;
  KCS_STATUS  KcsStatus;
  UINT16      KcsPort;
  UINT64      TimeOut;

  if (Idle == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  *Idle = FALSE;

  TimeOut = 0;
  KcsPort = PcdGet16 (PcdIpmiIoBaseAddress);
  do {
    MicroSecondDelay (IPMI_DELAY_UNIT);
    KcsStatus.RawData = IoRead8 (KcsPort + 1);
    if ((KcsStatus.RawData == 0xFF) || (TimeOut >= IpmiTimeoutPeriod)) {
      Status = EFI_DEVICE_ERROR;
      goto LabelError;
    }

    TimeOut++;
  } while (KcsStatus.Status.Ibf);

  if (KcsState == KcsWriteState) {
    // KcsData = IoRead8 (KcsPort);   // MU_CHANGE
    IoRead8 (KcsPort);                // MU_CHANGE
  }

  if (KcsStatus.Status.State != KcsState) {
    if ((KcsStatus.Status.State == KcsIdleState) && (KcsState == KcsReadState)) {
      *Idle = TRUE;
    } else {
      Status = KcsErrorExit (IpmiTimeoutPeriod);
      goto LabelError;
    }
  }

  if (KcsState == KcsReadState) {
    TimeOut = 0;
    do {
      MicroSecondDelay (IPMI_DELAY_UNIT);
      KcsStatus.RawData = IoRead8 (KcsPort + 1);
      if ((KcsStatus.RawData == 0xFF) || (TimeOut >= IpmiTimeoutPeriod)) {
        Status = EFI_DEVICE_ERROR;
        goto LabelError;
      }

      TimeOut++;
    } while (!KcsStatus.Status.Obf);
  }

  if ((KcsState == KcsWriteState) || (*Idle == TRUE)) {
    // KcsData = IoRead8 (KcsPort);   // MU_CHANGE
    IoRead8 (KcsPort);                // MU_CHANGE
  }

  return EFI_SUCCESS;

LabelError:
  return Status;
}

EFI_STATUS
SendDataToBmc (
  UINT64  IpmiTimeoutPeriod,
  UINT8   *Data,
  UINT8   DataSize
  )

/**

Routine Description:

  Send data to BMC

Arguments:

  IpmiTimeoutPeriod  - The period to wait before timeout
  Data               - The data pointer to be sent
  DataSize           - The data size

Returns:

  @retval EFI_SUCCESS   - Send out the data successfully

**/
{
  KCS_STATUS  KcsStatus;
  UINT8       KcsData;
  UINT16      KcsIoBase;
  EFI_STATUS  Status;
  UINT8       i;
  BOOLEAN     Idle;
  UINT64      TimeOut;

  KcsIoBase = PcdGet16 (PcdIpmiIoBaseAddress);

  TimeOut = 0;

  do {
    MicroSecondDelay (IPMI_DELAY_UNIT);
    KcsStatus.RawData = IoRead8 (KcsIoBase + 1);
    if ((KcsStatus.RawData == 0xFF) || (TimeOut >= IpmiTimeoutPeriod)) {
      if ((Status = KcsErrorExit (IpmiTimeoutPeriod)) != EFI_SUCCESS) {
        return Status;
      }
    }

    TimeOut++;
  } while (KcsStatus.Status.Ibf);

  KcsData = KCS_WRITE_START;
  IoWrite8 ((KcsIoBase + 1), KcsData);
  if ((Status = KcsCheckStatus (IpmiTimeoutPeriod, KcsWriteState, &Idle)) != EFI_SUCCESS) {
    return Status;
  }

  for (i = 0; i < DataSize; i++) {
    if (i == (DataSize - 1)) {
      if ((Status = KcsCheckStatus (IpmiTimeoutPeriod, KcsWriteState, &Idle)) != EFI_SUCCESS) {
        return Status;
      }

      KcsData = KCS_WRITE_END;
      IoWrite8 ((KcsIoBase + 1), KcsData);
    }

    Status = KcsCheckStatus (IpmiTimeoutPeriod, KcsWriteState, &Idle);
    if (EFI_ERROR (Status)) {
      return Status;
    }

    IoWrite8 (KcsIoBase, Data[i]);
  }

  return EFI_SUCCESS;
}

EFI_STATUS
ReceiveBmcData (
  UINT64  IpmiTimeoutPeriod,
  UINT8   *Data,
  UINT8   *DataSize
  )

/**

Routine Description:

  Routine Description:

  Receive data from BMC

Arguments:

  IpmiTimeoutPeriod  - The period to wait before timeout
  Data               - The buffer pointer
  DataSize           - The buffer size

Returns:

  @retval EFI_SUCCESS   - Received data successfully

**/
{
  UINT8       KcsData;
  UINT16      KcsIoBase;
  EFI_STATUS  Status;
  BOOLEAN     Idle;
  UINT8       Count;

  Count     = 0;
  KcsIoBase = PcdGet16 (PcdIpmiIoBaseAddress);

  while (TRUE) {
    if ((Status = KcsCheckStatus (IpmiTimeoutPeriod, KcsReadState, &Idle)) != EFI_SUCCESS) {
      return Status;
    }

    if (Idle) {
      *DataSize = Count;
      break;
    }

    //
    // Need to check Data Size -1 to account for array access
    //
    if (Count >= *DataSize) {
      return EFI_DEVICE_ERROR;
    }

    Data[Count] = IoRead8 (KcsIoBase);

    Count++;

    KcsData = KCS_READ;
    IoWrite8 (KcsIoBase, KcsData);
  }

  return EFI_SUCCESS;
}

EFI_STATUS
ReceiveBmcDataFromPort (
  UINT64  IpmiTimeoutPeriod,
  UINT8   *Data,
  UINT8   *DataSize
  )

/**

Routine Description:

  Receive data from BMC

Arguments:

  IpmiTimeoutPeriod  - The period to wait before timeout
  Data               - The buffer pointer to receive data
  DataSize           - The buffer size

Returns:

  @retval EFI_SUCCESS   - Received the data successfully

**/
{
  EFI_STATUS  Status;
  UINT8       i;
  UINT8       MyDataSize;

  MyDataSize = *DataSize;

  for (i = 0; i < KCS_ABORT_RETRY_COUNT; i++) {
    Status = ReceiveBmcData (IpmiTimeoutPeriod, Data, DataSize);
    if (EFI_ERROR (Status)) {
      if ((Status = KcsErrorExit (IpmiTimeoutPeriod)) != EFI_SUCCESS) {
        return Status;
      }

      *DataSize = MyDataSize;
    } else {
      return Status;
    }
  }

  return EFI_DEVICE_ERROR;
}

EFI_STATUS
SendDataToBmcPort (
  UINT64  IpmiTimeoutPeriod,
  UINT8   *Data,
  UINT8   DataSize
  )

/**

Routine Description:

  Send data to BMC

Arguments:

  IpmiTimeoutPeriod  - The period to wait before timeout
  Data               - The data pointer to be sent
  DataSize           - The data size

Returns:

  @retval EFI_SUCCESS   - Send out the data successfully

**/
{
  EFI_STATUS  Status;
  UINT8       i;

  for (i = 0; i < KCS_ABORT_RETRY_COUNT; i++) {
    Status = SendDataToBmc (IpmiTimeoutPeriod, Data, DataSize);
    if (EFI_ERROR (Status)) {
      if ((Status = KcsErrorExit (IpmiTimeoutPeriod)) != EFI_SUCCESS) {
        return Status;
      }
    } else {
      return Status;
    }
  }

  return EFI_DEVICE_ERROR;
}
