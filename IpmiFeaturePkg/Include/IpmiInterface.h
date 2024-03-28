/** @file
  Generic definitions for the common IPMI interfaces.

  Copyright 2014 - 2021 Intel Corporation. <BR>
  Copyright (c) Microsoft Corporation
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef _IPMI_INTERFACE_H_
#define _IPMI_INTERFACE_H_

#include <ServerManagement.h>

typedef struct _IPMI_TRANSPORT IPMI_TRANSPORT;

//
// BMC status bit definitions.
//
typedef UINT32 BMC_STATUS;

#define BMC_OK                  0
#define BMC_SOFTFAIL            1
#define BMC_HARDFAIL            2
#define BMC_UPDATE_IN_PROGRESS  3
#define BMC_NOTREADY            4

//
// Structure to communicate BMC state from PEI to DXE.
//

#pragma pack(1)

typedef struct _IPMI_BMC_HOB {
  BMC_STATUS    BmcStatus;
} IPMI_BMC_HOB;

#pragma pack()

//
//  IPMI Function Prototypes
//
typedef
EFI_STATUS
(EFIAPI *IPMI_SEND_COMMAND)(
  IN IPMI_TRANSPORT                    *This,
  IN UINT8                             NetFunction,
  IN UINT8                             Lun,
  IN UINT8                             Command,
  IN UINT8                             *CommandData,
  IN UINT32                            CommandDataSize,
  OUT UINT8                            *ResponseData,
  OUT UINT32                           *ResponseDataSize
  );

typedef
EFI_STATUS
(EFIAPI *IPMI_GET_CHANNEL_STATUS)(
  IN IPMI_TRANSPORT                   *This,
  OUT BMC_STATUS                      *BmcStatus,
  OUT SM_COM_ADDRESS                  *ComAddress
  );

//
// IPMI TRANSPORT
//
struct _IPMI_TRANSPORT {
  UINT64                     Revision;
  IPMI_SEND_COMMAND          IpmiSubmitCommand;
  IPMI_GET_CHANNEL_STATUS    GetBmcStatus;
};

#endif
