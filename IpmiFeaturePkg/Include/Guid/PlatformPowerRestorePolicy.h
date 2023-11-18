/** @file PlatformPowerRestorePolicy.h

    Platform Power Restore Policy Definition

    Copyright (c) Microsoft Corporation.
    SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#ifndef PLATFORM_POWER_RESTORE_POLICY_H_
#define PLATFORM_POWER_RESTORE_POLICY_H_

#define PLATFORM_POWER_RESTORE_POLICY_GUID {0x85bcbff7, 0x8f9d, 0x4997, {0xac, 0x46, 0x5b, 0x36, 0x70, 0x0b, 0x0b, 0x85}}

//
// Power Restore Policy definition from IPMI Spec.
//
typedef enum {
 PowerRestorePolicyPowerOff  = 0x00,
 PowerRestorePolicyLastState = 0x01,
 PowerRestorePolicyPowerOn   = 0x02,
 PowerRestorePolicyNoChange  = 0x03,
} PLATFORM_POWER_RESTORE_POLICY_TYPE;

#pragma pack(1)

typedef struct _PLATFORM_POWER_RESTORE_POLICY
{
  PLATFORM_POWER_RESTORE_POLICY_TYPE PolicyValue;
} PLATFORM_POWER_RESTORE_POLICY;

#pragma pack()

#endif //#ifdef PLATFORM_POWER_RESTORE_POLICY_H_
