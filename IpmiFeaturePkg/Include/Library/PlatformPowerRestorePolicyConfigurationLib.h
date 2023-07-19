/** @file

    PlatformPowerRestorePolicyConfigurationLib.h

    Copyright (c) Microsoft Corporation.
    SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#ifndef PLATFORM_POWER_RESTORE_POLICY_CONFIGURATION_LIB_H_
#define PLATFORM_POWER_RESTORE_POLICY_CONFIGURATION_LIB_H_

//
// Power Restore Policy configuration.
//
#define  POWER_RESTORE_POLICY_POWER_OFF   0x00
#define  POWER_RESTORE_POLICY_LAST_STATE  0x01
#define  POWER_RESTORE_POLICY_POWER_ON    0x02
#define  POWER_RESTORE_POLICY_NO_CHANGE   0x03  // "Set Power Restore Policy"
#define  POWER_RESTORE_POLICY_UNKNOWN     0x03  // "Get Chassis Status"

/**
  Get PowerRestorePolicy

  @param[out] PowerRestorePolicy  The PowerRestorePolicy setting of the platform

  @retval EFI_INVALID_PARAMETER      The input parameter is invalid.
  @retval EFI_SUCCESS                Get PowerRestorePolicy successfully.
**/
EFI_STATUS
EFIAPI
GetPowerRestorePolicy (
  OUT UINT8  *PowerRestorePolicy
  );

#endif //#ifdef PLATFORM_POWER_RESTORE_POLICY_CONFIGURATION_LIB_H_
