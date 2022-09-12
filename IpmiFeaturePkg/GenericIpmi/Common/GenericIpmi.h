/** @file
  IPMI Transport common layer driver head file

  @copyright
  Copyright 1999 - 2021 Intel Corporation. <BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#ifndef _GENERIC_IPMI_H_
#define _GENERIC_IPMI_H_

#include <Uefi.h>
#include <IndustryStandard/Ipmi.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/IpmiTransportLib.h>
#include <IpmiInterface.h>

#include <IpmiHooks.h>
#include <GenericIpmiCommon.h>

#define IPMI_DELAY_UNIT  50   // Unit is microseconds.

//
// IPMI Instance signature
//
#define SM_IPMI_BMC_SIGNATURE  SIGNATURE_32 ('i', 'p', 'm', 'i')
#define INSTANCE_FROM_SM_IPMI_BMC_THIS(a) \
  CR ( \
  a, \
  IPMI_BMC_INSTANCE_DATA, \
  IpmiTransport, \
  SM_IPMI_BMC_SIGNATURE \
  )

#pragma pack(1)

//
// Structure to communicate BMC state from PEI to DXE.
//

typedef struct _IPMI_BMC_HOB {
  BMC_STATUS    BmcStatus;
} IPMI_BMC_HOB;

#pragma pack()

EFI_STATUS
EFIAPI
IpmiInitializeBmc (
  IN IPMI_BMC_INSTANCE_DATA  *IpmiInstance
  );

#endif
