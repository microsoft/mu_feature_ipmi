/** @file
  Definitions for the SSIF transport unit tests.

  Copyright (c) Microsoft Corporation
  PDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef _SSIF_UNIT_TEST_H
#define _SSIF_UNIT_TEST_H

#include <Uefi.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/TimerLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/UnitTestLib.h>
#include <Library/BmcSmbusLib.h>
#include <Library/IpmiTransportLib.h>
#include <Ssif.h>

//
// SMBus test library functions.
//

#define TEST_BUFFER_SIZE  (1024)

VOID
SmbusTestLibReset (
  VOID
  );

BOOLEAN
CheckTxBuffer (
  UINT8   *Data,
  UINT32  DataSize
  );

VOID
SetRxBuffer (
  UINT8   *Data,
  UINT32  DataSize
  );

#endif
