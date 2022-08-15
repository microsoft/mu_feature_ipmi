/** @file
  Generic IPMI stack head file during PEI phase

  @copyright
  Copyright 2017 - 2021 Intel Corporation. <BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#ifndef _PEI_IPMI_INIT_H_
#define _PEI_IPMI_INIT_H_

#include <PiPei.h>
#include <Uefi.h>

#include <Ppi/IpmiTransportPpi.h>

#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/IoLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/PcdLib.h>
#include <Library/PeiServicesLib.h>
#include <Library/PeiServicesTablePointerLib.h>
#include <Library/TimerLib.h>

#include "IpmiHooks.h"
#include "GenericIpmiCommon.h"
#include "GenericIpmi.h"

//
// Prototypes
//
#define MBXDAT_B              0x0B
#define BMC_IPMI_TIMEOUT_PEI  5                 // [s] Single IPMI request timeout
#define IPMI_DELAY_UNIT_PEI   1000              // [s] Each KSC IO delay
#define IPMI_DEFAULT_IO_BASE  0xCA2

//
// Internal(hook) function list
//
EFI_STATUS
SendPreBootSignaltoBmc (
  IN CONST EFI_PEI_SERVICES  **PeiServices
  )

/*++

Routine Description:
Send Pre-Boot signal to BMC

Arguments:
PeiServices           - General purpose services available to every PEIM.

Returns:
EFI_SUCCESS           - Success
--*/
;

//
// internal function list
//
EFI_STATUS
GetDeviceId (
  IN      IPMI_BMC_INSTANCE_DATA  *mIpmiInstance
  )

/*++

Routine Description:
  Execute the Get Device ID command to determine whether or not the BMC is in Force Update
  Mode.  If it is, then report it to the error manager.

Arguments:
  mIpmiInstance   - Data structure describing BMC variables and used for sending commands
  StatusCodeValue - An array used to accumulate error codes for later reporting.
  ErrorCount      - Counter used to keep track of error codes in StatusCodeValue

Returns:
  Status

--*/
;

#endif //_PEI_IPMI_INIT_H_
