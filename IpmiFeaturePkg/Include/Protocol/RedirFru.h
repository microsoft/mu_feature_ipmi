/** @file
  This code abstracts the generic FRU Protocol.

Copyright (c) 2023, Intel Corporation. All rights reserved.<BR>
SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef _REDIR_FRU_H_
#define _REDIR_FRU_H_

typedef struct _EFI_SM_FRU_REDIR_PROTOCOL EFI_SM_FRU_REDIR_PROTOCOL;

#define EFI_SM_FRU_REDIR_PROTOCOL_GUID \
  { \
    0x28638cfa, 0xea88, 0x456c, 0x92, 0xa5, 0xf2, 0x49, 0xca, 0x48, 0x85, 0x35 \
  }

// {41F49AE4-7FB0-4c54-994E-EA199171B0AC}
#define EFI_PRE_FRU_SMBIOS_DATA_GUID \
  { \
    0x41f49ae4, 0x7fb0, 0x4c54, 0x99, 0x4e, 0xea, 0x19, 0x91, 0x71, 0xb0, 0xac \
  }

#define EFI_SM_FRU_REDIR_SIGNATURE  SIGNATURE_32 ('f', 'r', 'r', 'x')

//
//  Redir FRU Function Prototypes
//
typedef
EFI_STATUS
(EFIAPI *EFI_GET_FRU_REDIR_INFO)(
  IN EFI_SM_FRU_REDIR_PROTOCOL            *This,
  IN  UINTN                               FruSlotNumber,
  OUT EFI_GUID                            *FruFormatGuid,
  OUT UINTN                               *DataAccessGranularity,
  OUT CHAR16                              **FruInformationString
  );

typedef
EFI_STATUS
(EFIAPI *EFI_GET_FRU_SLOT_INFO)(
  IN EFI_SM_FRU_REDIR_PROTOCOL            *This,
  OUT EFI_GUID                            *FruTypeGuid,
  OUT UINTN                               *StartFruSlotNumber,
  OUT UINTN                               *NumSlots
  );

typedef
EFI_STATUS
(EFIAPI *EFI_GET_FRU_REDIR_DATA)(
  IN EFI_SM_FRU_REDIR_PROTOCOL            *This,
  IN  UINTN                               FruSlotNumber,
  IN  UINTN                               FruDataOffset,
  IN  UINTN                               FruDataSize,
  IN  UINT8                               *FruData
  );

typedef
EFI_STATUS
(EFIAPI *EFI_SET_FRU_REDIR_DATA)(
  IN EFI_SM_FRU_REDIR_PROTOCOL            *This,
  IN  UINTN                               FruSlotNumber,
  IN  UINTN                               FruDataOffset,
  IN  UINTN                               FruDataSize,
  IN  UINT8                               *FruData
  );

//
// REDIR FRU PROTOCOL
//
struct _EFI_SM_FRU_REDIR_PROTOCOL {
  EFI_GET_FRU_REDIR_INFO    GetFruRedirInfo;
  EFI_GET_FRU_SLOT_INFO     GetFruSlotInfo;
  EFI_GET_FRU_REDIR_DATA    GetFruRedirData;
  EFI_SET_FRU_REDIR_DATA    SetFruRedirData;
};

extern EFI_GUID  gEfiRedirFruProtocolGuid;

#endif
