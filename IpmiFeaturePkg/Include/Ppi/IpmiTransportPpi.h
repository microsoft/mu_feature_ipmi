/** @file
  IPMI Ttransport PPI Header File.

  @copyright
  Copyright 2014 - 2021 Intel Corporation. <BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#ifndef _IPMI_TRANSPORT_PPI_H_
#define _IPMI_TRANSPORT_PPI_H_

#include <IpmiInterface.h>

typedef struct _IPMI_TRANSPORT PEI_IPMI_TRANSPORT_PPI;

#define PEI_IPMI_TRANSPORT_PPI_GUID \
  { \
    0x7bf5fecc, 0xc5b5, 0x4b25, 0x81, 0x1b, 0xb4, 0xb5, 0xb, 0x28, 0x79, 0xf7 \
  }

extern EFI_GUID  gPeiIpmiTransportPpiGuid;

#endif
