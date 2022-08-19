/** @file
  IPMITtransport Protocol Header File.

  @copyright
  Copyright 2011 - 2021 Intel Corporation. <BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#ifndef _IPMI_TRANSPORT_PROTO_H_
#define _IPMI_TRANSPORT_PROTO_H_

#include <IpmiInterface.h>

#define IPMI_TRANSPORT_PROTOCOL_GUID \
  { \
    0x6bb945e8, 0x3743, 0x433e, 0xb9, 0xe, 0x29, 0xb3, 0xd, 0x5d, 0xc6, 0x30 \
  }

#define SMM_IPMI_TRANSPORT_PROTOCOL_GUID \
{ \
  0x8bb070f1, 0xa8f3, 0x471d, 0x86, 0x16, 0x77, 0x4b, 0xa3, 0xf4, 0x30, 0xa0 \
}

extern EFI_GUID  gIpmiTransportProtocolGuid;
extern EFI_GUID  gSmmIpmiTransportProtocolGuid;

#endif
