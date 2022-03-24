/** @file
  Contains definitions for the SMBus System Inferfaces (SSIF) transport for
  IPMI.

  Copyright (c) Microsoft Corporation
  PDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef _SSIF_BMC_H
#define _SSIF_BMC_H

#define SSIF_MAX_WRITE_SIZE  (32)
#define SSIF_MAX_READ_SIZE   (32)

#define SSIF_MAX_WRITE_BUFFER_SIZE  (64)
#define SSIF_MAX_READ_BUFFER_SIZE   (64)

#define SMBUS_RW_READ   (0)
#define SMBUS_RW_WRITE  (1)

typedef struct _SMBUS_HEADER {
  UINT8    SlaveAddress : 7;
  UINT8    RW           : 1;
  UINT8    SMBusCmd;
} SMBUS_HEADER;

typedef struct _SSIF_WRITE_START {
  SMBUS_HEADER    SMBusHeader;
  UINT8           Length;
  UINT8           NetFN : 6;
  UINT8           LUN   : 2;
  UINT8           IpmiCmd;
} SSIF_WRITE_START;

typedef struct _SSIF_WRITE_MIDDLE {
  SMBUS_HEADER    SMBusHeader;
  UINT8           Length;
} SSIF_WRITE_MIDDLE;

typedef struct _SSIF_SINGLE_READ {
  UINT8           SMBusCmd;
  SMBUS_HEADER    ResponseSmbusHeader;
  UINT8           Length;
  UINT8           NetFN : 6;
  UINT8           LUN   : 2;
  UINT8           IpmiCmd;
  UINT8           CompletionCode;
} SSIF_SINGLE_READ;

typedef struct _SSIF_MULTI_READ_START {
  UINT8           SMBusCmd;
  SMBUS_HEADER    ResponseSmbusHeader;
  UINT8           Length;
  UINT8           Special0;
  UINT8           Special1;
  UINT8           NetFN : 6;
  UINT8           LUN   : 2;
  UINT8           IpmiCmd;
  UINT8           CompletionCode;
} SSIF_MULTI_READ_START;

typedef struct _SSIF_MULTI_READ_MIDDLE {
  UINT8           SMBusCmd;
  SMBUS_HEADER    ResponseSmbusHeader;
  UINT8           Length;
  UINT8           BlockNumber;
} SSIF_MULTI_READ_MIDDLE;

#define SMBUS_CMD_WRITE              0x2
#define SMBUS_CMD_READ               0x3
#define SMBUS_CMD_MULT_WRITE_START   0x6
#define SMBUS_CMD_MULT_WRITE_MIDDLE  0x7
#define SMBUS_CMD_MULT_WRITE_END     0x8
#define SMBUS_CMD_MULT_READ          0x9
#define SMBUS_CMD_MULT_READ_RETRY    0xA

#define SSIF_READ_END_BLOCK  0xFF

#endif
