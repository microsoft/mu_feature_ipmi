/** @file
  Mock implementation for IPMI SEL functions.

  Copyright (c) Microsoft Corporation
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include "MockIpmi.h"

#pragma pack(1)

typedef struct {
  UINT16    RecordId;
  UINT8     RecordType;
  UINT32    TimeStamp;
  UINT8     Data[9];
} SEL_GENERIC_EVENT;

STATIC_ASSERT (sizeof (SEL_GENERIC_EVENT) == 16, "Bad generic SEL event size");

#pragma pack()

//
// Global state for mocking SEL functions.
//

#define SEL_COUNT  (100)
STATIC SEL_GENERIC_EVENT  mSel[SEL_COUNT];
STATIC UINT16             mNextRecordId = 0;
STATIC UINT32             mSelTime      = 0;

#define CURRENT_SEL_TIME  (++mSelTime)

/**
  Mocks the result of IPMI_STORAGE_GET_SEL_INFO.

  @param[in]       Data           The IPMI request data.
  @param[in]       DataSize       The size of the IPMI request data.
  @param[out]      Response       The response data buffer.
  @param[in, out]  ResponseSize   On input, the available size of buffer.
                                  On output, the size of written data in the buffer.
**/
VOID
MockIpmiSelGetInfo (
  IN VOID       *Data,
  IN UINT8      DataSize,
  OUT VOID      *Response,
  IN OUT UINT8  *ResponseSize
  )
{
  IPMI_GET_SEL_INFO_RESPONSE  *SelInfo;

  ASSERT (*ResponseSize >= sizeof (IPMI_GET_SEL_INFO_RESPONSE));

  SelInfo                       = Response;
  SelInfo->CompletionCode       = IPMI_COMP_CODE_NORMAL;
  SelInfo->Version              = 0x51; // Per IPMI v2
  SelInfo->NoOfEntries          = mNextRecordId;
  SelInfo->FreeSpace            = (UINT16)(sizeof (mSel) - (sizeof (mSel[0]) * mNextRecordId));
  SelInfo->RecentAddTimeStamp   = 0;
  SelInfo->RecentEraseTimeStamp = 0;
  SelInfo->OperationSupport     = 0;

  if (mNextRecordId >= SEL_COUNT) {
    SelInfo->OperationSupport |= IPMI_GET_SEL_INFO_OPERATION_SUPPORT_OVERFLOW_FLAG;
  }

  *ResponseSize = sizeof (IPMI_GET_SEL_INFO_RESPONSE);
}

/**
  Mocks the result of IPMI_STORAGE_ADD_SEL_ENTRY.

  @param[in]       Data           The IPMI request data.
  @param[in]       DataSize       The size of the IPMI request data.
  @param[out]      Response       The response data buffer.
  @param[in, out]  ResponseSize   On input, the available size of buffer.
                                  On output, the size of written data in the buffer.
**/
VOID
MockIpmiSelAddEntry (
  IN VOID       *Data,
  IN UINT8      DataSize,
  OUT VOID      *Response,
  IN OUT UINT8  *ResponseSize
  )
{
  IPMI_ADD_SEL_ENTRY_REQUEST   *SelEntry;
  IPMI_ADD_SEL_ENTRY_RESPONSE  *SelResponse;

  ASSERT (DataSize >= sizeof (IPMI_ADD_SEL_ENTRY_REQUEST));
  ASSERT (*ResponseSize >= sizeof (IPMI_ADD_SEL_ENTRY_RESPONSE));

  SelEntry = Data;
  DEBUG ((DEBUG_INFO, "[SEL ENTRY] RecordType: 0x%x\n", SelEntry->RecordData.RecordType));

  SelResponse = Response;
  if (mNextRecordId < SEL_COUNT) {
    CopyMem (&mSel[mNextRecordId], &SelEntry->RecordData, sizeof (SEL_GENERIC_EVENT));
    mSel[mNextRecordId].RecordId = mNextRecordId;
    SelResponse->RecordId        = mNextRecordId;
    if (mSel[mNextRecordId].RecordType < IPMI_SEL_OEM_NO_TIME_STAMP_RECORD_START) {
      mSel[mNextRecordId].TimeStamp = CURRENT_SEL_TIME;
    }

    mNextRecordId++;
    SelResponse->CompletionCode = IPMI_COMP_CODE_NORMAL;
  } else {
    DEBUG ((DEBUG_ERROR, "Mock SEL is full!\n"));
    SelResponse->RecordId       = 0;
    SelResponse->CompletionCode = IPMI_COMP_CODE_OUT_OF_SPACE;
  }

  *ResponseSize = sizeof (IPMI_ADD_SEL_ENTRY_RESPONSE);
}

/**
  Mocks the result of IPMI_STORAGE_GET_SEL_TIME.

  @param[in]       Data           The IPMI request data.
  @param[in]       DataSize       The size of the IPMI request data.
  @param[out]      Response       The response data buffer.
  @param[in, out]  ResponseSize   On input, the available size of buffer.
                                  On output, the size of written data in the buffer.
**/
VOID
MockIpmiSelGetTime (
  IN VOID       *Data,
  IN UINT8      DataSize,
  OUT VOID      *Response,
  IN OUT UINT8  *ResponseSize
  )
{
  IPMI_GET_SEL_TIME_RESPONSE  *TimeResponse;

  ASSERT (*ResponseSize >= sizeof (IPMI_GET_SEL_TIME_RESPONSE));

  TimeResponse                 = Response;
  TimeResponse->CompletionCode = IPMI_COMP_CODE_NORMAL;
  TimeResponse->Timestamp      = CURRENT_SEL_TIME;

  *ResponseSize = sizeof (IPMI_GET_SEL_TIME_RESPONSE);
}

/**
  Mocks the result of IPMI_STORAGE_SET_SEL_TIME.

  @param[in]       Data           The IPMI request data.
  @param[in]       DataSize       The size of the IPMI request data.
  @param[out]      Response       The response data buffer.
  @param[in, out]  ResponseSize   On input, the available size of buffer.
                                  On output, the size of written data in the buffer.
**/
VOID
MockIpmiSelSetTime (
  IN VOID       *Data,
  IN UINT8      DataSize,
  OUT VOID      *Response,
  IN OUT UINT8  *ResponseSize
  )
{
  IPMI_SET_SEL_TIME_REQUEST  *SetTime;
  UINT8                      *CompletionCode;

  ASSERT (DataSize >= sizeof (IPMI_SET_SEL_TIME_REQUEST));
  ASSERT (*ResponseSize >= sizeof (*CompletionCode));

  SetTime  = Data;
  mSelTime = SetTime->Timestamp;

  CompletionCode  = Response;
  *CompletionCode = IPMI_COMP_CODE_NORMAL;

  *ResponseSize = sizeof (*CompletionCode);
}

/**
  Mocks the result of IPMI_STORAGE_CLEAR_SEL.

  @param[in]       Data           The IPMI request data.
  @param[in]       DataSize       The size of the IPMI request data.
  @param[out]      Response       The response data buffer.
  @param[in, out]  ResponseSize   On input, the available size of buffer.
                                  On output, the size of written data in the buffer.
**/
VOID
MockIpmiSelClear (
  IN VOID       *Data,
  IN UINT8      DataSize,
  OUT VOID      *Response,
  IN OUT UINT8  *ResponseSize
  )
{
  IPMI_CLEAR_SEL_REQUEST   *ClearRequest;
  IPMI_CLEAR_SEL_RESPONSE  *ClearResponse;

  ASSERT (DataSize >= sizeof (IPMI_CLEAR_SEL_REQUEST));
  ASSERT (*ResponseSize >= sizeof (IPMI_CLEAR_SEL_RESPONSE));

  ClearRequest = Data;
  ASSERT (ClearRequest->AscC == 'C');
  ASSERT (ClearRequest->AscL == 'L');
  ASSERT (ClearRequest->AscR == 'R');

  ClearResponse                 = Response;
  ClearResponse->CompletionCode = IPMI_COMP_CODE_NORMAL;
  mNextRecordId                 = 0;
  if (ClearRequest->Erase == IPMI_CLEAR_SEL_REQUEST_INITIALIZE_ERASE) {
    ClearResponse->ErasureProgress = IPMI_CLEAR_SEL_RESPONSE_ERASURE_IN_PROGRESS;
  } else if (ClearRequest->Erase == IPMI_CLEAR_SEL_REQUEST_GET_ERASE_STATUS) {
    ClearResponse->ErasureProgress = IPMI_CLEAR_SEL_RESPONSE_ERASURE_COMPLETED;
  } else {
    ASSERT (FALSE);
  }

  *ResponseSize = sizeof (IPMI_CLEAR_SEL_RESPONSE);
}

/**
  Mocks the result of IPMI_STORAGE_GET_SEL_ENTRY.

  @param[in]       Data           The IPMI request data.
  @param[in]       DataSize       The size of the IPMI request data.
  @param[out]      Response       The response data buffer.
  @param[in, out]  ResponseSize   On input, the available size of buffer.
                                  On output, the size of written data in the buffer.
**/
VOID
MockIpmiSelGetEntry (
  IN VOID       *Data,
  IN UINT8      DataSize,
  OUT VOID      *Response,
  IN OUT UINT8  *ResponseSize
  )
{
  IPMI_GET_SEL_ENTRY_REQUEST   *GetRequest;
  IPMI_GET_SEL_ENTRY_RESPONSE  *GetResponse;
  UINT16                       RecordId;

  ASSERT (DataSize >= sizeof (IPMI_GET_SEL_ENTRY_REQUEST));
  ASSERT (*ResponseSize >= sizeof (IPMI_GET_SEL_ENTRY_RESPONSE));

  GetRequest  = Data;
  GetResponse = Response;
  ZeroMem (GetResponse, sizeof (IPMI_GET_SEL_ENTRY_RESPONSE));
  *ResponseSize = sizeof (IPMI_GET_SEL_ENTRY_RESPONSE);

  if ((GetRequest->ReserveId[0] != 0) ||
      (GetRequest->ReserveId[1] != 0) ||
      (GetRequest->Offset != 0) ||
      (GetRequest->BytesToRead != 0xFF))
  {
    DEBUG ((DEBUG_ERROR, "%a: Mock SEL does not support partial entry reads!\n", __FUNCTION__));
    DEBUG ((DEBUG_INFO, "0x%x 0x%x 0%x 0x%x\n", GetRequest->ReserveId[0], GetRequest->ReserveId[1], GetRequest->Offset, GetRequest->BytesToRead != 0xFF));
    ASSERT (FALSE);
    GetResponse->CompletionCode = IPMI_COMP_CODE_INVALID_DATA_FIELD;
    return;
  }

  RecordId = GetRequest->SelRecID[0] | (GetRequest->SelRecID[1] << 8);
  if (RecordId >= mNextRecordId) {
    GetResponse->CompletionCode = IPMI_COMP_CODE_NOT_PRESENT;
    return;
  }

  if (RecordId + 1 < mNextRecordId) {
    GetResponse->NextSelRecordId = RecordId + 1;
  } else {
    GetResponse->NextSelRecordId = 0xFFFF;
  }

  CopyMem (&GetResponse->RecordData, &mSel[RecordId], sizeof (mSel[0]));
  GetResponse->CompletionCode = IPMI_COMP_CODE_NORMAL;
}
