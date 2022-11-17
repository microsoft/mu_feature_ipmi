/** @file
  Ipmi structure defintions that are either not defined in MdePkg's Ipmi header files
  or are extensions of existing function headers.

  Copyright (c) Microsoft Corporation
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef IPMI_FEATURE_H__
#define IPMI_FEATURE_H__

#include <IndustryStandard/Ipmi.h>

#pragma pack(1)

//
// In IPMI, GUID are stored least-significant byte first.
// The order of fields are the reverse of convention described in [RFC4122].
// Note: This definition is used for both the IPMI_APP_GET_SYSTEM_GUID
// and IPMI_APP_GET_DEVICE_GUID.
//
typedef struct {
  UINT8     Data4[8];
  UINT16    Data3;
  UINT16    Data2;
  UINT32    Data1;
} IPMI_GUID;

//
// Structure definition for NetApp function IPMI_APP_GET_SYSTEM_GUID
//
typedef struct {
  UINT8        CompletionCode;
  IPMI_GUID    Guid;
} IPMI_GET_SYSTEM_GUID_RESPONSE;

//
// Structure definitions for NetApp Function IPMI_APP_GET_SYSTEM_INTERFACE_CAPABILITIES
//
typedef enum {
  GetSystemInterfaceTypeSsif = 0,
  GetSystemInterfaceTypeKcs,
  GetSystemInterfaceTypeSmic,
  GetSystemInterfaceTypeMax
} GET_SYSTEM_INTEFACE_INTERFACE_TYPE;

typedef struct {
  UINT8    SystemInterfaceType : 4;     /// GET_SYSTEM_INTERFACE_INTERFACE_TYPE
  UINT8    Reserved            : 4;     ///< Reserved.
} IPMI_GET_SYSTEM_INTERFACE_CAPABILITY_REQUEST;

typedef struct {
  UINT8    CompletionCode;            ///< Completion code
  UINT8    Reserved;                  ///< Reserved (returned as 0x00)
  UINT8    SsifVersion : 3;           ///< System Interface Version (000b is version 1)
  UINT8    Reserved1   : 5;           ///< Reserved
  UINT8    InputMessageSize;          ///< Maximum input Message Size that BMC can accept (in byte)
  UINT8    MaxMessageSize;            ///< Max Message Size (0xFF is 255 bytes)
} IPMI_GET_SYSTEM_INTERFACE_CAPABILITY_RESPONSE_SSIF;

typedef struct {
  UINT8    CompletionCode;             ///< Completion code
  UINT8    Reserved;                   ///< Reserved (returned as 0x00)
  UINT8    SystemInterfaceVersion : 3; ///< System Interface Version (000b is version 1)
  UINT8    Reserved1              : 5; ///< Reserved
  UINT8    MaxMessageSize;             ///< Max Message Size (0xFF is 255 bytes)
} IPMI_GET_SYSTEM_INTERFACE_CAPABILITY_RESPONSE_KCS;

typedef struct {
  UINT8    CompletionCode;             ///< Completion code
  UINT8    Reserved;                   ///< Reserved (returned as 0x00)
  UINT8    SystemInterfaceVersion : 3; ///< System Interface Version (000b is version 1)
  UINT8    Reserved1              : 5; ///< Reserved
  UINT8    MaxMessageSize;             ///< Max Message Size (0xFF is 255 bytes)
} IPMI_GET_SYSTEM_INTERFACE_CAPABILITY_RESPONSE_SMIC;

typedef union {
  IPMI_GET_SYSTEM_INTERFACE_CAPABILITY_RESPONSE_SSIF    SsifInterface;
  IPMI_GET_SYSTEM_INTERFACE_CAPABILITY_RESPONSE_KCS     KcsInterface;
  IPMI_GET_SYSTEM_INTERFACE_CAPABILITY_RESPONSE_SMIC    SmicInterface;
} IPMI_GET_SYSTEM_INTERFACE_CAPABILITY_RESPONSE;

#pragma pack()
#endif
